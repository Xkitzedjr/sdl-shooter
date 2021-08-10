#ifndef P2PSOCKET_H_
#define P2PSOCKET_H_

#include "common.h"

#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stddef.h>

#define DEFAULT_PORT 18023
#define DEFAULT_IP "127.0.1.22"

extern void err_n_die(const char *fmt, ...);

static void *runServer(void *args);
static void *sendUpdate(void *args);
static void initServer(int *sock_id, struct sockaddr_in *server_address, const int port);
static void initUpdater(int *sock_id, struct sockaddr_in *server_address, const int port, const char *ip);

//TEMPORARY PLACEHOLDER STRUCT
typedef struct {
    int x;
    int y;
} Placeholder;

typedef enum {UPDATE} OP_CODE;

#endif // P2PSOCKET_H_
