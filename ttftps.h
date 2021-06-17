#ifndef _TTFTPS_H
#define _TTFTPS_H

#include <string>

#define MTU 1500

// TODO check
// Macro to check system call success
#define SYS_CALL_CHECK(f) do {          \
    int e = f;                          \
    if (e < 0) {                        \
        perror("TTFTP_ERROR");          \
        exit(-1);                       \
    }                                   \
} while (0)

// TODO check if works
// TODO ask lior why it's important
typedef struct wrq
{
    std::string fileName;
    std::string mode;
} __attribute__((packed)) WRQ;


#endif //_TTFTPS_H
