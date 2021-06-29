#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <fstream>

#include "server.h"
#include "ttftps.h"

extern std::ofstream fileOnServer;

void serverLoop(int sockFd, struct sockaddr_in clntAddr, unsigned int
cliAddrLen)
{
    const int WAIT_FOR_PACKET_TIMEOUT = 3;
    const int NUMBER_OF_FAILURES = 7;

    char buffer[MAX_PCKT_LEN] = {0};
    unsigned int timeoutExpiredCount{0};
    uint16_t lastReceivedBlkNum{0};
    uint16_t blockNum;
    uint16_t opcode;
    ssize_t recvMsgSize{0}; /* Size of received message */

    fd_set readFds;
    struct timeval timeout{0};
    int readyToRead;

    do
    {
        do
        {
            do
            {
                recvMsgSize = 0;

                // Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
                // for us at the socket (we are waiting for DATA)
                FD_ZERO(&readFds);
                FD_SET(sockFd, &readFds);

                // Zero the timeout each iteration - critical because select
                // modifies value
                timeout.tv_sec = WAIT_FOR_PACKET_TIMEOUT;

                // Returns 0 if timeout was reached and no message was read
                readyToRead = select(sockFd + 1, &readFds, nullptr,
                                     nullptr, &timeout);

                SYS_CALL_CHECK(readyToRead);

                // If there was something at the socket and we are here not
                // because of a timeout
                if(readyToRead > 0)
                {
                    // Read the DATA packet from the socket (at least we hope
                    // this is a DATA packet)

                    recvMsgSize = recvfrom(sockFd, buffer, MAX_PCKT_LEN, 0,
                                           (struct sockaddr*)&clntAddr,
                                                   &cliAddrLen);
                    SYS_CALL_CHECK(recvMsgSize);

                    // Get opcode from packet
                    uint16_t tmpOpCode;
                    memcpy(&tmpOpCode, buffer, sizeof(uint16_t));
                    opcode = ntohs(tmpOpCode);

                    // Get block number from packet
                    uint16_t tmpBlockNum;
                    memcpy(&tmpBlockNum, buffer + 2, sizeof(uint16_t));
                    blockNum = ntohs(tmpBlockNum);
                }

                //Time out expired while waiting for data to appear at the
                // socket
                if(readyToRead == 0)
                {
                    // Send another ACK for the last packet
                    timeoutExpiredCount++;

                    std::cout << "FLOWERROR:number of timeouts is: "
                              << timeoutExpiredCount << std::endl;

                    // Create and send ack
                    ACK ack;
                    ack.opcode = htons(ACK_OPCODE);
                    ack.blockNum = htons(lastReceivedBlkNum);

                    auto ackBytesSent = sendto(sockFd, &ack, ACK_LENGTH, 0,
                                               (struct sockaddr*) &clntAddr,
                                                       cliAddrLen);
                    SYS_CALL_CHECK(ackBytesSent);
                    std::cout << "OUT:ACK," << lastReceivedBlkNum << std::endl;
                }
                if (timeoutExpiredCount >= NUMBER_OF_FAILURES)
                {
                    // FATAL ERROR BAIL OUT
                    std::cout << "RECVFAIL" << std::endl;
                    std::cout << "FLOWERROR:number of failures has exceeded "
                    << NUMBER_OF_FAILURES << " exiting..." << std::endl;
                    // TODO test that return here works
                    return;
                }
                // check that this (recvMsgSize == 0) is correct
            }while (recvMsgSize == 0); // Continue while some socket was
                // ready but recvfrom failed to read the data (ret 0)

            if (opcode != DATA_OPCODE) // We got something else but DATA
            {
                // FATAL ERROR BAIL OUT
                std::cout << "RECVFAIL" << std::endl;
                std::cout << "FLOWERROR:unexpected packet type received. " <<
                             "Exiting..." << std::endl;
                // TODO test this return
                return;
            }
            if (blockNum != lastReceivedBlkNum + 1) // The incoming block
                // number is not what we have expected, i.e. this is a DATA
                // pkt but the block number in DATA was wrong (not last ACK’s
                // block number + 1)
            {
                // FATAL ERROR BAIL OUT
                std::cout << "RECVFAIL" << std::endl;
                std::cout << "FLOWERROR:block number received does not match "
                             "last ACK's block number + 1. " <<
                          "Exiting..." << std::endl;
                // TODO test this - that server starts waiting again for WRQ
                return;
            }
        }while (false);

        // We treat timeout count as per packet
        timeoutExpiredCount = 0;

        // Packet is correct, Data packet - print message
        lastReceivedBlkNum = blockNum;
        std::cout << "IN:DATA, " << blockNum << "," << recvMsgSize << std::endl;

        // Write packet to file on server and print message
        fileOnServer.write(buffer + 4, recvMsgSize - 4); // TODO check that
        // this syscall succeeded

        // TODO - here we printed number of bytes that we INTENDED to write,
        //  NOT how many were actually written. Ask Lior if ok.
        std::cout << "WRITING: " << recvMsgSize - 4 << std::endl;

        //lastWriteSize = fwrite(...); // write next bulk of data
        // send ACK packet to the client

        // Create and send ack
        ACK ack;
        ack.opcode = htons(ACK_OPCODE);
        ack.blockNum = htons(lastReceivedBlkNum);

        auto ackBytesSent = sendto(sockFd, &ack, ACK_LENGTH, 0,
                                   (struct sockaddr*) &clntAddr,
                                   cliAddrLen);
        SYS_CALL_CHECK(ackBytesSent);
        std::cout << "OUT:ACK," << lastReceivedBlkNum << std::endl;

    }while (recvMsgSize == MAX_PCKT_LEN); // Have blocks left to be read
    // from client (not end of transmission)

    // Successful end of transmission - print message
    std::cout << "RECVOK" << std::endl;
}

//TODO  test each possible
// reason that can cause 'znicha' and ensure that server is able to get next
// request.

// TODO implement + test that if process of sending failed before single char
//  was written - delete file created on server.

// TODO valgrind - I want to - not required