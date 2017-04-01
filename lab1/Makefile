BIN_FILES  = server test

CC = gcc


CCGLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: CFLAGS=$(CCGLAGS)
all: $(BIN_FILES)
.PHONY : all

server: server.o read_line.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

#user_list: user_list.o read_line.o
#	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: test.o read_line.o user_list.o msg_list.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
