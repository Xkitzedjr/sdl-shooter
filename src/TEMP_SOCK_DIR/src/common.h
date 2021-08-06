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

#define SERVER_PORT 18000
#define MAXLINE 4096
#define SA struct sockaddr

#endif
