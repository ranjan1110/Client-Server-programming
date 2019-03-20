#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define m_len 1024 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

struct message
{
    uint8_t type;
    uint16_t length;
    char msg[m_len];
};


