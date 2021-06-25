#ifndef _SERVER_H
#define _SERVER_H

#include <fstream>

#define MAX_PCKT_LEN 516 // Max packet length in bytes of data packet

void serverLoop(int sockFd, struct sockaddr_in clntAddr, unsigned int
        cliAddrLen, std::ofstream* fileOnServer);

#endif //_SERVER_H
