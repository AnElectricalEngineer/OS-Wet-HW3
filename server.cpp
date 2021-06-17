#include <cstdlib>
#include <ctime>
#include <cstdio>
#include "server.h"
#include "ttftps.h"

void serverLoop(int sockFd)
{
    const int WAIT_FOR_PACKET_TIMEOUT = 3;
    const int NUMBER_OF_FAILURES = 7;

    fd_set readFds;
    struct timeval timeout{0};
    timeout.tv_sec = WAIT_FOR_PACKET_TIMEOUT;
    int readyToRead;

    do
    {
        do
        {
            do
            {
                // Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
                // for us at the socket (we are waiting for DATA)
                FD_ZERO(&readFds);
                FD_SET(sockFd, &readFds);

                readyToRead = select(sockFd + 1, &readFds, nullptr, nullptr,
                                     &timeout);

                // TODO ask lior if need to check sys call here because it
                //  wasn't in sheled
                SYS_CALL_CHECK(readyToRead);

                // If there was something at the socket and we are here not
                // because of a timeout
                if(readyToRead > 0)
                {
                    // Read the DATA packet from the socket (at least we hope
                    // this is a DATA packet)

                }

                //Time out expired while waiting for data to appear at the
                // socket
                if(readyToRead == 0)
                {
                //TODO: Send another ACK for the last packet
                    timeoutExpiredCount++;
                }
                if (timeoutExpiredCount>= NUMBER_OF_FAILURES)
                {
// FATAL ERROR BAIL OUT
                }
            }while (...) // TODO: Continue while some socket was ready
// but recvfrom failed to read the data (ret 0)
            if (...) // TODO: We got something else but DATA
            {
// FATAL ERROR BAIL OUT
            } i
            f (...) // TODO: The incoming block number is not what we have
// expected, i.e. this is a DATA pkt but the block number
// in DATA was wrong (not last ACK’s block number + 1)
            {
// FATAL ERROR BAIL OUT
            }
        }while (FALSE);
        timeoutExpiredCount = 0;
        lastWriteSize = fwrite(...); // write next bulk of data
// TODO: send ACK packet to the client
    }while (...); // Have blocks left to be read from client (not end of transmission)
}