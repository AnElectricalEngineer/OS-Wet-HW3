#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#include "ttftps.h"
#include "server.h"

int main(int argc, char* argv[])
{
    unsigned short portNum{}; // port number that server listens on

    // Check number of command arguments
    if(argc != 1)
    {
        std::cerr << "Error: wrong number of arguments. Exiting..." <<
        std::endl;
        exit(-1);
    }

    // Check that portNum is valid
    try
    {
        portNum = std::stoi(argv[0]);
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
    char buffer[MTU];                /* Buffer for packets */
    int recvMsgSize;                 /* Size of received message */

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
        recvMsgSize = recvfrom(sock, buffer, MTU, 0, (struct sockaddr*)
                &clntAddr, &cliAddrLen);

        SYS_CALL_CHECK(recvMsgSize);

        if(recvMsgSize >= 2)
        {
            // Check if opcode of message matches WRQ
            // TODO check if this is how messages are arranged
            if(buffer[0] == 0 && buffer[1] == 2)
            {
                //TODO check if we can assume that WRQ are always correct
                int fileNameLen = strlen(&buffer[2]);
                char fileName[fileNameLen + 1];
                strcpy(fileName, &buffer[2]);

                //TODO check if mode is always one byte, or what
                int modeLen = strlen(&buffer[fileNameLen + 3]);
                char mode[modeLen + 1];
                strcpy(mode, &buffer[fileNameLen + 3]);

                std::string fileNameString = fileName;
                std::string modeString = mode;

                WRQ wrqMessage{};
                wrqMessage.fileName = fileNameString;
                wrqMessage.mode = mode;

                std::cout << "IN:WRQ,"
                //TODO stopped here
            }
        }

        serverLoop(sock);
    }

    return 0;
}
