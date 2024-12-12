CC=gcc
CFLAGS=-Wall -std=c99

all: client server

client: prereq src/client/client.c
	$(CC) $(CFLAGS) src/client/client.c -o build/client

server: prereq src/server/server.c
	$(CC) $(CFLAGS) src/server/server.c -o build/server

prereq:
	@mkdir -p build


clean:
	rm -f build/client build/server
