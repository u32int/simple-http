CC=gcc
CFLAGS=-Wall -Wextra -g

all:
	$(CC) $(CFLAGS) main.c request_parser.c response.c args_parser.c -o simple-http

clean:
	rm simple-http 
