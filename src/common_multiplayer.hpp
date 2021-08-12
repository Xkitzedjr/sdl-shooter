#ifndef COMMON_MULTIPLAYER_H_
#define COMMON_MULTIPLAYER_H_

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
#define DEFAULT_IP "127.0.1.1"

extern void err_n_die(const char *fmt, ...);

enum OP_CODE{
  UPDATE_POS,
  UPDATE_POS_AND_FIRE,
  DEATH
};

struct Message {
  OP_CODE message = UPDATE_POS;
  float x = 0;
  float y = 0;

  Message(void) {}

  Message(OP_CODE op_code, float x, float y) :
    x(x),
    y(y),
    message(op_code) {}
};

#endif // COMMON_MULTIPLAYER_H_
