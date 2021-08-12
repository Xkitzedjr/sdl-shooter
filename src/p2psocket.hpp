#ifndef P2PSOCKET_H_
#define P2PSOCKET_H_

#include "common.h"
#include "common_multiplayer.hpp"
#include <array>

static void *startUpdater(void *args);
static void *runServer(void *args);
static void initServer(int *sock_id, struct sockaddr_in *server_address, const int port);
static void initUpdater(int *sock_id, struct sockaddr_in *server_address, const int port, const char *ip);

static Message *toBeSent;
static Message *lastRecieved;
static pthread_t server_thread;
static pthread_t updater_thread;

#endif // P2PSOCKET_H_
