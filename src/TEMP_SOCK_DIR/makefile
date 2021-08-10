proggy = thread_server
CC = gcc
CFLAGS = -g #-Wall 
LDFLAGS = -pthread

OUT = bin
SEARCHPATH += src
vpath %.h $(SEARCHPATH)
vpath %.c $(SEARCHPATH)

_OBJS += server.o
_OBJS += err_n_die.o
_OBJS += client.o
_OBJS += main.o

OBJS = $(patsubst %,$(OUT)/%,$(_OBJS))


all: $(proggy)

$(OUT)/%.o: %.c %.h
	@mkdir -p $(OUT)
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	$(RM) -rf $(OUT) $(proggy)

$(proggy): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
