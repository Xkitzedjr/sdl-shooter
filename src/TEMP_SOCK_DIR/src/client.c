#include "common.h"
#include <time.h>

static const char *httpmessage = "GET / HTTP/1.1\r\n\r\n";

extern void err_n_die(const char *fmt, ...);

void parseArgs(const int argc, char **argv, char **ipstr, char **message, int *port);

int main(int argc, char **argv) {
  // socket info contained as an int
  int sockfd, n;
  int sendbytes;
  // struct for containing server address and port
  struct sockaddr_in servaddr;
  char sendline[MAXLINE];
  char recvline[MAXLINE];
  char *ipstr = argv[1];
  char *message;
  int port;

  Command *command = (Command *)malloc(sizeof(Command));
  memset(command, 0, sizeof(Command));

  srand(time(0));
  int i = rand() % 3;
  switch (i) {
    case (0) : {
      command->command = SAY_HELLO;
      break;
    }
    case (1) : {
      command->command = SAY_GOODBYE;
      break;
    }
    case (2)  : {
      command->command = SAY_MY_NAME;
      break;
    }
  }
  strcpy(command->name, "chris");

  parseArgs(argc, argv, &ipstr, &message, &port);

  // create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //internet, stream type, 0=TCP, returns <0 on error
    err_n_die("Error while creating the socket!");

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port); //htons host to network, short

  if (inet_pton(AF_INET, ipstr, &servaddr.sin_addr) <= 0) //converts string ip to binary ip and stores in address member
    err_n_die("inet_pton error for %s", argv[1]);

  if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)  //connect to the server
    err_n_die("connect failed!");

  //sprintf(sendline, message);
  //sendbytes = strlen(sendline);

  sendbytes = sizeof(Command);

  if (write(sockfd, command, sizeof(Command)) != sizeof(Command)) {
    err_n_die("write error");
    printf("bytes sent = %d\nsize of Command = %d", sendbytes, sizeof(Command));
  }

  /*
  if (write(sockfd, sendline, sendbytes) != sendbytes)
    err_n_die("write error");
  */
  while ((n = read(sockfd, recvline, MAXLINE-1)) > 0) {
    printf("%s", recvline);
  }

  if (n < 0)
    err_n_die("read error");

  return 0;
}

void parseArgs(const int argc, char **argv, char **ipstr, char **message, int *port) {
   if (argc < 2)
    err_n_die("usage: %s <server address> (optional)<port> (optional)<message>", argv[0]);

  if (argc >= 3) {
    printf("using port %s\n", argv[2]);
    *port = atoi(argv[2]);
  }
  else
    *port = SERVER_PORT;

  if (argc >= 4){
    *message = argv[3];
    strcat(*message, "\r\n\r\n");
  }
  else {
    *message = "Hello from client!\r\n\r\n";
  }

}
