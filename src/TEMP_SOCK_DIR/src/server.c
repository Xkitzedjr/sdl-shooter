#include "common.h"

#define PORT 18000

extern void err_n_die(const char *fmt, ...);
extern char *bin2hex(const unsigned char *input, size_t len);

int parseArgs(const int argc, const char **argv, int *port);

int main(int argc, char **argv) {
  // listenfd will be the socket for listening
  // confd will be the socket for recieving data
  int listenfd, connfd, n;
  // struct for containing server address and port
  struct sockaddr_in servaddr;
  uint8_t buff[MAXLINE+1];
  uint8_t recvline[MAXLINE+1];
  int port;

  if (argc == 2){
    printf("using non-defualt port %s\n", argv[1]);
    port = atoi(argv[1]);
  }
  else
    port = PORT;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_n_die("socket error");

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);
  
  if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
    err_n_die("bind error");

  if ((listen(listenfd, 10)) < 0)
    err_n_die("listen error");

  while(1) {
    struct sockaddr_in addr;
    socklen_t addr_len;

    Command *command = (Command *)malloc(sizeof(Command));
    memset(command, 0, sizeof(Command));

    printf("waiting for a connection on port %d\n", port);
    fflush(stdout);
    connfd = accept(listenfd, (SA *) NULL, NULL);

    memset(recvline, 0, MAXLINE);

    // read from client, stop when no longer recieving data
    //while((n = read(connfd, command, sizeof(Command))) > 0) {
    read(connfd, command, sizeof(Command));
      fprintf(stdout, "\nData Recieved\n");


    switch (command->command) {
      case (SAY_HELLO) : {
        printf("\n\nHello! it worked?\n");
        break;
      }
      case (SAY_MY_NAME) : {
        printf("\n\nMy name is %s\n", command->name);
        break;
      }
      case (SAY_GOODBYE) : {
        printf("\n\nGoodbye!\n");
        break;
      }
    }


    free(command);

    /*
    while((n = read(connfd, recvline, MAXLINE-1)) > 0) {
      fprintf(stdout, "\nData Recieved\nHex:\n%s\nString:\n%s", bin2hex(recvline, n), recvline);

      if (recvline[n-1] == '\n')
        break;

      memset(recvline, 0, MAXLINE);
    }
    */

    if (n < 0)
      err_n_die("read error");

    snprintf((char*)buff, sizeof(buff), "\nOK \r\n\r\nHello From Sever!\n");

    write(connfd, (char*)buff, strlen((char *)buff));
    close(connfd);
  }
}
