CC=g++
CFLAGS=-g
LDFLAGS=-lsqlite3
proggy=sqlexe

all: proggy

proggy: sql.cpp
	$(CC) $(CFLAGS) $< -o $(proggy) $(LDFLAGS)

clean:
	$(RM) -rf $(proggy) highscores.db
