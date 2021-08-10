#ifndef _SERVER_H_
#define _SERVER_H_

#include "common.h"

extern void err_n_die(const char *fmt, ...);
extern char *bin2hex(const unsigned char *input, size_t len);

int parseArgs(const int argc, const char **argv, int *port);

#endif
