RPC_PATH = ./rpc_store_service

SERVER  = server

CC = gcc


CCGLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: CFLAGS=$(CCGLAGS)
all: $(SERVER)
.PHONY : all

server: server.o read_line.o user_list.o msg_list.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(SERVER) *.o

.SUFFIXES:
.PHONY : clean
