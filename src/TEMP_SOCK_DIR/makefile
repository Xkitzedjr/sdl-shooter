SERV = server
CLIE = client
CC = gcc

OUT = bin
SEARCHPATH = src
vpath = %.h $(SEARCHPATH)
vpath = %.c $(SEARCHPATH)

_SERV_OBJS += server.o
_SERV_OBJS += err_n_die.o
_CLIE_OBJS += client.o
_CLIE_OBJS += err_n_die.o

SERV_OBJS = $(patsubst %,$(OUT)/%,$(_SERV_OBJS))
CLIE_OBJS = $(patsubst %,$(OUT)/%,$(_CLIE_OBJS))

all: $(SERV) $(CLIE)

$(OUT)/%.o: %.c %.h
	@mkdir -p $(OUT)
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	$(RM) -rf $(OUT) $(SERV) $(CLIE)

CFLAGS = -g

$(SERV): $(SERV_OBJS)
	$(CC) -o $@ $(SERV_OBJS)

$(CLIE): $(CLIE_OBJS)
	$(CC) -o $@ $(CLIE_OBJS)
