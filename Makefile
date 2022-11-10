CC=gcc
CFLAGS=-Wall -Wextra

all: server.c
	$(CC) $(CFLAGS) server.c request_parser.c response.c args_parser.c -o simple-http

clean:
	rm server
