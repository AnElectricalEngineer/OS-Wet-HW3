#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
#include <climits>

#include "ttftps.h"
#include "server.h"

std::ofstream fileOnServer;

int main(int argc, char* argv[])
{
    int tmpPortNum; // port number that server listens on
    uint16_t portNum;

    // Check number of command arguments
    if(argc != 2)
    {
        std::cerr << "Error: wrong number of arguments. Exiting..." <<
        std::endl;
        exit(-1);
    }

    // Check that portNum is a number
    try
    {
        tmpPortNum = std::stoi(argv[1]);
    }
    catch(std::invalid_argument&)
    {
        std::cerr << "Error: bad port number. Exiting..." << std::endl;
        exit(-1);
    }

    // Check that received port num is valid port num
    if(tmpPortNum < 0 || tmpPortNum > USHRT_MAX)
    {
        std::cerr << "Error: bad port number. Exiting..." << std::endl;
        exit(-1);
    }

    portNum = static_cast<uint16_t>(tmpPortNum);

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
        bool isValidWRQ{false};
        bool isModeOctet{false};

        cliAddrLen = sizeof(clntAddr);

        // Try to receive WRQ from client
        recvMsgSize = recvfrom(sock, buffer, MTU, 0, (struct sockaddr*)
                &clntAddr, &cliAddrLen);

        SYS_CALL_CHECK(recvMsgSize);

        // Check if opcode of message matches WRQ
        uint16_t tmpOpcode;
        memcpy(&tmpOpcode, buffer, sizeof(uint16_t));
        uint16_t opcode = ntohs(tmpOpcode);

        // opcode is WRQ opcode
        if(opcode == WRQ_OPCODE)
        {
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
                //TODO ensure that file is opened in proper place, and
                // deleted in certain cases, eg file doesn't exist on client
                auto fileNameLen = strlen(&buffer[2]);

                // Check that file name is valid string with length > 0
                if(fileNameLen > 0)
                {
                    isValidWRQ = true;

                    // Save file name
                    char fileName[MTU];
                    strcpy(fileName, &buffer[2]);

                    std::cout << "IN:WRQ," << fileName << "," << OCTET <<
                              std::endl;

                    // Check that transmission mode is octet
                    if(!strcmp("octet", &buffer[fileNameLen + 3]))
                    {
                        // Transmission mode is correct - octet
                        isModeOctet = true;

                        // Create file to write client's file content to
                        //TODO CLOSE FILE
                        // TODO maybe we should only open file once one data
                        //  packet has been successfully received
                        fileOnServer.open(fileName, std::ofstream::trunc |
                        std::ofstream::binary);

                        // Create and send ack (0) to ack WRQ
                        ACK ack0;
                        ack0.opcode = htons(ACK_OPCODE);
                        ack0.blockNum = htons(0);

                        auto ackBytesSent = sendto(sock, &ack0, ACK_LENGTH, 0,
                                                   (struct sockaddr*) &clntAddr,
                                                   cliAddrLen);
                        SYS_CALL_CHECK(ackBytesSent);
                        std::cout << "OUT:ACK,0" << std::endl;

                        serverLoop(sock, clntAddr, cliAddrLen);
                        fileOnServer.close();
                    }

                    // Mode is not octet
                    else
                    {
                        isModeOctet = false;
                    }
                }
            }
        }

        //TODO if possible try to test all ifs above - all possible WRQ
        // failures, see if below message was printed.

        // Received message was NOT a valid WRQ for some reason
        if(isValidWRQ == false || isModeOctet == false)
        {
            std::cout << "FLOWERROR:bad packet." << std::endl;
        }
    }
    return 0;
}
