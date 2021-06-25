#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fstream>

#include "ttftps.h"
#include "server.h"

std::ofstream fileOnServer;

int main(int argc, char* argv[])
{
    unsigned short portNum{}; // port number that server listens on

    // Check number of command arguments
    if(argc != 2)
    {
        std::cerr << "Error: wrong number of arguments. Exiting..." <<
        std::endl;
        exit(-1);
    }

    // Check that portNum is valid
    try
    {
        portNum = std::stoi(argv[1]);
    }
    catch(std::invalid_argument&)
    {
        std::cerr << "Error: bad port number. Exiting..." << std::endl;
        exit(-1);
    }

    int sock;                        /* Socket */
    struct sockaddr_in servAddr{0};  /* Local address */
    struct sockaddr_in clntAddr{0};  /* Client address */
    unsigned int cliAddrLen;         /* Length of client address */
    char buffer[MTU] = {0};          /* Buffer for packets */
    ssize_t recvMsgSize;             /* Size of received message */

    /* Create socket for sending/receiving datagrams */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    SYS_CALL_CHECK(sock);

    /* Internet address family */
    servAddr.sin_family = AF_INET;

    /* Any incoming interface */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Local port */
    servAddr.sin_port = htons(portNum);

    /* Bind to the local address */
    SYS_CALL_CHECK(bind(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)));

    while(true)
    {
        cliAddrLen = sizeof(clntAddr);

        // Try to receive WRQ from client
        recvMsgSize = recvfrom(sock, buffer, MTU, 0, (struct sockaddr*)
                &clntAddr, &cliAddrLen);

        SYS_CALL_CHECK(recvMsgSize);

        // Check if opcode of message matches WRQ

        // TODO ask lior when to use ntohs. This works, but switching indexes
        //  of buffers and using ntohs also works.
        unsigned short opcode = (((unsigned short)buffer[0]) << 8) | buffer[1];
        //opcode = ntohs(opcode);

        // opcode is WRQ opcode
        // TODO ask what to do if any field of WRQ is incorrect
        if(opcode == 2)
        {
            // TODO delete comment lior said that we need to check that the
            //  structure of WRQ is what we expect it to be.

            // Check that received message contains two null terminators
            int nullTermCount{0};
            for(int i = 2; i < recvMsgSize; ++i)
            {
                if(buffer[i] == '\0')
                {
                    nullTermCount++;
                }
            }

            // Number of null terminators in message received is exactly 2
            if(nullTermCount == 2)
            {
                // Save file name
                auto fileNameLen = strlen(&buffer[2]);
                char fileName[MTU];
                strcpy(fileName, &buffer[2]);


                //TODO ask what to do if message with opcode OTHER than 2 is
                // received here. And what if tm is NOT octet. See הדפסת שגיאות
                // on forum. FLOWERROR?

                // Check that transmission mode is octet
                if(!strcmp("octet", &buffer[fileNameLen + 3]))
                {
                    // Message is a WRQ so print proper message
                    std::cout << "IN:WRQ," << fileName << "," << OCTET <<
                    std::endl;

                    // Transmission mode is correct - octet
                    // Create file to write client's file content to
                    //TODO CLOSE FILE
                    // TODO maybe we should only open file once one data
                    //  packet has been successfully received
                    fileOnServer.open(fileName, std::ofstream::trunc);
                    // TODO ensure that file should be created on server if
                    //  it doesnt already exist, and that truncate should be
                    //  used
                    if(!fileOnServer.is_open())
                    {
                        // TODO check if this sys call check works with
                        //  std::open
                        perror("TTFTP_ERROR");
                        //std::cerr << "TTFTP_ERROR" << std::endl;
                        exit(-1);
                    }

                    // Create and send ack (0) to ack WRQ
                    ACK ack0;
                    ack0.opcode = htons(ACK_OPCODE);
                    ack0.blockNum = htons(0);

                    auto ackBytesSent = sendto(sock, &ack0, ACK_LENGTH, 0,
                                               (struct sockaddr*) &clntAddr,
                                                       cliAddrLen);

                    // Ensure that entire ACK0 for WRQ was successfully sent
                    // TODO check if there is some other error that should be
                    //  printed here instead of just syscall error. Maybe use
                    //  retvalue < 0 to check syscall, and separately check
                    //  num of bytes sent. Check this for ALL ack sends.
                    if(ackBytesSent != ACK_LENGTH)
                    {
                        perror("TTFTP_ERROR");
                        exit(-1);
                    }

                    std::cout << "OUT:ACK,0" << std::endl;
                    serverLoop(sock, clntAddr, cliAddrLen);
                    fileOnServer.close(); //TODO check syscall maybe
                }
            }
        }
    }

    return 0;
}
