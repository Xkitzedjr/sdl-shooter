#include "common.h"

extern void err_n_die(const char *fmt, ...);

int main(int argc, char **argv) {
  int sockfd, n;
  int sendbytes;
  struct sockaddr_in servaddr;
  char sendline[MAXLINE];
  char recvline[MAXLINE];

  if (argc != 2) 
    err_n_die("usage: %s <server address>", argv[0]);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //internet, stream type, 0=TCP, returns <0 on error
    err_n_die("Error while creating the socket!");

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERVER_PORT); //htons host to network, short

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) //converts string ip to binary ip and stores in address member
    err_n_die("inet_pton error for %s", argv[1]);

  if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)  //connect to the server
    err_n_die("connect failed!");

  sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
  sendbytes = strlen(sendline);

  if (write(sockfd, sendline, sendbytes) != sendbytes)
    err_n_die("write error");

  while ((n = read(sockfd, recvline, MAXLINE-1)) > 0) {
    printf("%s", recvline);
  }

  if (n < 0)
    err_n_die("read error");

  return 0;
}

