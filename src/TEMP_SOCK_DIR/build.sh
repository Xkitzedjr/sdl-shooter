gcc -g -c src/client.c -o bin/client.o
gcc -g -c src/server.c -o bin/server.o

gcc bin/client.o bin/err_n_die.o -o client
gcc bin/server.o bin/err_n_die.o -o server
