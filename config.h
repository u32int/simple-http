#pragma once

/* port that the server will listen on */
#define PORT "8080"
/* number of backlog connections allowed by listen() */
#define BACKLOG 10
/* max data size per recv call */
#define DATA_CAP 4096

/* the server name that will be sent with each response */
#define SERVER_NAME "simple-http"

extern char *ROOT_DIR;
