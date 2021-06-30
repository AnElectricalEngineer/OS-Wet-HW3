#ifndef _SERVER_H
#define _SERVER_H


#define MAX_PCKT_LEN 516 // Max packet length in bytes of data packet

int serverLoop(int sockFd, struct sockaddr_in clntAddr, unsigned int
        cliAddrLen);

#endif //_SERVER_H
