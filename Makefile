CC=gcc
CFLAGS=-Wall -Wextra

all: server.c
	$(CC) $(CFLAGS) server.c request_parser.c -o server

clean:
	rm server
