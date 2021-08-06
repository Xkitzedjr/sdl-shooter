#include "common.h"

#define PORT 8080
#define BACKLOG 3
#define BUFFER_LEN 1024

extern void err_n_die(const char *fmt, ...);

int main(int arc, char **argv) {
  int serverfd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFFER_LEN];
  char *hello = "hello from server!";

  memset(buffer, 0, sizeof(char) * 1024);

  // create socket (domain, type, protocal)
  if (serverfd = socket(AF_INET, SOCK_STREAM, 0) < 0)
    err_n_die("error creating server socket!");

  // set socket (optional)
  if (setsockopt(serverfd, SOL_SOCKET, 
        SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    err_n_die("error setting socket!");

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // bind socket to address and port in address
  if (bind(serverfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    err_n_die("error binding socket!");

  // set socket to listen for requests
  // backlog is the maximum amount of reqs that can
  // be in the queue, sends ECONNREFUESD if max is reached
  if (listen(serverfd, BACKLOG) < 0)
    err_n_die("error listening!");

  // extracts the first req in queue and creates a socket
  // that is connected, returning a file desc refering to
  // that socket
  if ((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
    err_n_die("error accepting");

  valread = read( new_socket, buffer, BUFFER_LEN );
  printf("%s\n", buffer);
  send(new_socket, hello, strlen(hello), 0);
  printf("message sent\n");
  return 0;
}
