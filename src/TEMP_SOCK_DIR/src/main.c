#include "main.h"

extern void *runServer(void *args);
extern void *runClient(void *args);

int main(int argc, char **argv) {
    pthread_t server_thread_id, client_thread_id;

    pthread_create(&server_thread_id, NULL, runServer, NULL);

    runClient(NULL);

    //pthread_create(&client_thread_id, NULL, runClient(argc, argv), NULL);
}
