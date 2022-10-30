#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "httpdef.h"
#include "request_parser.h"
#include "response.h"

#pragma GCC diagnostic ignored "-Wformat-truncation"

#define RESPONSE_HTTP_VERSION "HTTP/1.1"
#define RESPONSE_BUFF_SIZE 4096

#define RESPONSE_HTML_BEGIN "<!DOCTYPE html><html><body style=\"text-align: center\"><h1>"
#define RESPONSE_HTML_END   "</h1></body></html>"

/* response_fromstr - generate generic response given a string
   with format "<CODE> <PHRASE>"
   eg. "400 Not Found"
*/ 
char *response_fromstr(char *str)
{
    char date_buff[128];
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    strftime(date_buff, sizeof(date_buff), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char *ret = (char*)calloc(RESPONSE_BUFF_SIZE, 1);
    if (ret == NULL) {
        perror("malloc");
        exit(1);
    }

    snprintf(ret, RESPONSE_BUFF_SIZE,
             "%s %s\r\n"
             "Date: %s\r\n"
             "Server: %s\r\n\r\n"
             "%s%s%s",
             RESPONSE_HTTP_VERSION, str,
             date_buff,
             SERVER_NAME,
             RESPONSE_HTML_BEGIN, str, RESPONSE_HTML_END);

    return ret;
}

/*  generate_response - parse and generate a http response given a request string 
    RETURN VALUE
    pointer to malloc'd char *response
*/ 
char *generate_response(char *str_request)
{
    struct httprequest req;
    if (request_parse(str_request, &req) < 0) {
        return response_fromstr("400 Bad Request");
    }
    
    switch (req.method) {
    case GET: {
        FILE *f;
        char path[256];
        if (req.url[0] == '/' && req.url[1] == '\0') {
            snprintf(path, 256, "%s/index.html", ROOT_DIR);
        } else {
            snprintf(path, 256, "%s%s", ROOT_DIR, req.url);
        }

        f = fopen(path, "rb");
        if (f == NULL) {
            return response_fromstr("404 Not Found");
        }

        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buff = (char *)calloc(RESPONSE_BUFF_SIZE, 1);

        snprintf(buff, RESPONSE_BUFF_SIZE,
                 "HTTP/1.1 200 OK\r\n\r\n");

        size_t bytes_read = fread(buff+strlen(buff), 1, file_size, f);
        fclose(f);

        buff[bytes_read+strlen(buff)] = '\0';

        return buff;
    }
    default:
        fprintf(stderr,
                "generate_response: http method unimplemented (%d)", req.method);
        return response_fromstr("500 Internal Server Error");
    }

    return response_fromstr("500 Internal Server Error");
}
