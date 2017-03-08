all: array.o libarray.a client server

array.o:
	gcc -Wall -c array.c

libarray.a:
	ar -rv libarray.a array.o

client:
	gcc -Wall client.c -o client libarray.a -lrt

server:
	gcc -Wall server.c -o server -lpthread -lrt

clean:
	rm client client.o server array.o libarray.a

rebuild: clean array.o libarray.a client server