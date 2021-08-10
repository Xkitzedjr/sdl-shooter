#include "p2psocket.h"

void initConnection(void) {
   pthread_t server_thread;

   printf("starting server\n");
   pthread_create(&server_thread, NULL, runServer, NULL);

   pthread_t updater_thread;
   printf("sending update\n");
   pthread_create(&updater_thread, NULL, sendUpdate, NULL);

   //pthread_join(socket_id, NULL);
}

static void *runServer(void *args) {
    int listener, reader, talker, n;
    int port = DEFAULT_PORT;
    char ip[] = DEFAULT_IP;
    struct sockaddr_in server_address;

    initServer(&listener, &server_address, port);

    while(1) {
        struct sockaddr_in address;
        socklen_t address_len;

        Placeholder *ph = (Placeholder *)malloc(sizeof(Placeholder));
        memset(ph, 0, sizeof(Placeholder));

        printf("\nlistening on %s:%d\n", ip, port);
        fflush(stdout);
        reader = accept(listener, (struct sockaddr *) NULL , NULL);

        while ( (n = read(reader, ph, sizeof(Placeholder))) > 0) {
            fprintf(stdout, "\nRecieving Dtata\n");
        }

        printf("\nserver recieved data: %d, %d\n", ph->x, ph->y);

        close(reader);

        free(ph);
    }

    return NULL;
}

static void *sendUpdate(void *args) {
    int sender, n;
    struct sockaddr_in server_address;
    char ip[] = DEFAULT_IP;
    int port = DEFAULT_PORT;

    initUpdater(&sender, &server_address, port, ip);

    while(1) {
        sleep(3);

        Placeholder *ph = (Placeholder *)malloc(sizeof(Placeholder));
        memset(ph, 0, sizeof(Placeholder));
        ph->x = 1; ph->y = 2;

        if ( write(sender, ph, sizeof(Placeholder)) != sizeof(Placeholder) ) {
            err_n_die("\nupdate write error\n");
        }

        printf("\nClient sucsefully sent data\n");

        free(ph);
    }


    return NULL;
}

static void initServer(int *sock_id, struct sockaddr_in *server_address, const int port) {
    memset(server_address, 0, sizeof(*server_address));
    server_address->sin_family = AF_INET;                  // set to ICP
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);   // let os chooses port
    server_address->sin_port = htons(port);                // set port

    if ( (*sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        err_n_die("server socket error\n");
    }

    // The following two calls are needed to for reusing ports
    int enable = 1;
    if ( setsockopt(*sock_id, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 ) {
        err_n_die("setsockopt(SO_REUSEADDR) failed");
    }

    if ( setsockopt(*sock_id, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0 ) {
        err_n_die("setsockopt(SO_REUSEPORT) failed");
    }

    if ( (bind(*sock_id, (struct sockaddr *) server_address, sizeof(*server_address))) < 0) {
        err_n_die("server bind error\n");
    }

    if ( listen(*sock_id, 10) < 0 ) {
        err_n_die("listen error\n");
    }
}

static void initUpdater(int *sock_id, struct sockaddr_in *server_address, const int port, const char *ip) {
    if ( (*sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        err_n_die("update socker error\n");
    }

    memset(server_address, 0, sizeof(*server_address));
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);

    if ( inet_pton(AF_INET, ip, &server_address->sin_addr) <= 0 ) {
        err_n_die("client inet_prton error for %s\n", port);
    }

    if ( connect(*sock_id, (struct sockaddr *) server_address, sizeof(*server_address)) < 0 ) {
        err_n_die("\nupdate connection failed\n");
    }
}
