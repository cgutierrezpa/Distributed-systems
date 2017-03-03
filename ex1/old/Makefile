BIN_FILES  = client server array

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

#LDFLAGS = -L$(INSTALL_PATH)/lib/
#LDLIBS = -lpthread


all: $(BIN_FILES)
.PHONY : all


client: client.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@ -lrt

server: server.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@ -lrt

array: array.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@ -lrt

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
