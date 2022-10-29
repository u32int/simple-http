#pragma once

/* port that the server will listen on */
#define PORT "8080"
/* number of backlog connections allowed by listen() */
#define BACKLOG 10
/* max data size per recv'ed packet */
#define DATA_CAP 1024
