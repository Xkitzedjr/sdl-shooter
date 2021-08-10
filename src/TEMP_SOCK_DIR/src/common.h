#ifndef _COMMON_H_
#define _COMMON_H_

//network sockets
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//variadic functions
#include <stdarg.h> 
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stddef.h>

#define PORT 18000
#define IP "127.0.1.1"
#define MAXLINE 4096
#define SA struct sockaddr

typedef enum {
  SAY_HELLO,
  SAY_GOODBYE,
  SAY_MY_NAME
} COMMAND;

typedef struct {
    COMMAND command;
    char name[30];
} Command;

#endif
