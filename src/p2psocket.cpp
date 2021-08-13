#include "p2psocket.hpp"

// TODO
// [X] server needs access to p2 ship struct
// [X] updater needs to be able to send p1 ship struct
//     - used message struct instead
// [X] make update init run in a while loop and use updater to change the message it sends
// [~] add ablility to specify port and ip
// [X] fix very erratic messages

void initConnection(void) {
    toBeSent = new Message();
    lastRecieved = new Message();

    printf("starting server\n");
    pthread_create(&server_thread, NULL, runServer, NULL);

    printf("starting updater\n");
    pthread_create(&updater_thread, NULL, startUpdater, NULL);

    //pthread_join(socket_id, NULL);
}

void updateToBeSent(int x, int y, OP_CODE opCode) {
    toBeSent->x = x;
    toBeSent->y = y;
    toBeSent->message = opCode;

    // we have new info, send it
    sendMessage = true;
}

void getLastRecieved(std::unique_ptr<Message> &returnMessage) {
    *returnMessage = Message(*lastRecieved);

    // just got last message, ready for another
    listenForMessage = true;
}

void killConnection(void) {
    pthread_join(server_thread, NULL);
    pthread_join(updater_thread, NULL);
    delete toBeSent;
    delete lastRecieved;
}

// the server will be listening for the other players status
static void *runServer(void *args) {
    int listener, reader, talker, n;
    int port = DEFAULT_PORT;
    struct sockaddr_in server_address;

    initServer(&listener, &server_address, port);

    while(1) {
        if (listenForMessage) {
            struct sockaddr_in address;
            socklen_t address_len;

            printf("\nlistening on port:%d\n", port);
            fflush(stdin);
            reader = accept(listener, (struct sockaddr *) NULL , NULL);

            while ( (n = read(reader, lastRecieved, sizeof(Message))) > 0) {
                fprintf(stdin, "\nRecieving Dtata\n");
            }

            printf("\nserver recieved data: %f, %f\n", lastRecieved->x, lastRecieved->y);

            close(reader);

            listenForMessage = false;
        }
    }

    return NULL;
}

// Updater will be sending the local players status to the remote player's server
static void *startUpdater(void *args) {
    int sender, n;
    struct sockaddr_in server_address;
    int port = DEFAULT_PORT;
    std::string ip;

    std::cout << "IP: ";
    std::cin >> ip;

    ip = (ip.empty() ? DEFAULT_IP : ip);

    initUpdater(&sender, &server_address, port, ip.c_str());

    while (1) {
        if (sendMessage) {
            if ( write(sender, toBeSent, sizeof(Message)) != sizeof(Message) ) {
                err_n_die("\nupdate write error\n");
            }
            sendMessage = false;
        }
        //printf("\nClient sucsefully sent data\n");
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

    while ( connect(*sock_id, (struct sockaddr *) server_address, sizeof(*server_address)) < 0 ) {
        //err_n_die("\nupdate connection failed\n");
        sleep(1);
        printf("trying to connect\n");
    }
}
