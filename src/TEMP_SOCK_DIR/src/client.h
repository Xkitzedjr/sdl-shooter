#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include <time.h>

static const char *httpmessage = "GET / HTTP/1.1\r\n\r\n";

extern void err_n_die(const char *fmt, ...);

void parseArgs(const int argc, char **argv, char **ipstr, char **message, int *port);

#endif
