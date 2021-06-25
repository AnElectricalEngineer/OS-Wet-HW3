#ifndef _TTFTPS_H
#define _TTFTPS_H

#include <string>

#define MTU 1500
#define WRQ_OPCODE 2
#define ACK_OPCODE 4
#define DATA_OPCODE 3
#define ACK_LENGTH 4 // length of ack message in bytes
#define OCTET "octet"

// TODO check
// Macro to check system call success
#define SYS_CALL_CHECK(f) do {          \
    auto e = f;                         \
    if (e < 0) {                        \
        perror("TTFTP_ERROR");          \
        exit(-1);                       \
    }                                   \
} while (0)

// TODO check if works
typedef struct ack
{
    unsigned short opcode;
    unsigned short blockNum;
} __attribute__((packed)) ACK;



#endif //_TTFTPS_H
