CC=gcc
CFLAGS=-Wall -Wextra

all: server.c
	$(CC) $(CFLAGS) server.c -o server

clean:
	rm server
