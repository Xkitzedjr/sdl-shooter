CC=gcc
CFLAGS=-g
LDFLAGS=-lsqlite3
proggy=sqlexe

all: proggy

proggy: sql.c
	$(CC) $(CFLAGS) $< -o $(proggy) $(LDFLAGS)

clean:
	$(RM) -rf $(proggy) highscores.db
