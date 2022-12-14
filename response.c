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
#define RESPONSE_HTML_END "</h1></body></html>"

static const size_t RHTML_PADDING = strlen(RESPONSE_HTML_BEGIN) + strlen(RESPONSE_HTML_END);

/* get_date - get current date in a html standard compliant format
*/
static void get_date(char *date_buff, size_t n)
{
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    strftime(date_buff, n, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

/* response_fromstr - generate generic response given a string
   with format "<CODE> <PHRASE>"
   eg. "400 Not Found"
*/ 
char *response_fromstr(char *str)
{
    char date_buff[128];
    get_date(date_buff, 128);

    char *ret = (char*)malloc(RESPONSE_BUFF_SIZE);
    if (ret == NULL) {
        perror("malloc");
        return NULL;
    }

    snprintf(ret, RESPONSE_BUFF_SIZE,
             "%s %s\r\n"
             "Date: %s\r\n"
             "Server: %s\r\n"
             "Content-Length: %lu\r\n"
             "\r\n%s%s%s",
             RESPONSE_HTTP_VERSION, str,
             date_buff,
             SERVER_NAME,
             strlen(str)+RHTML_PADDING,
             RESPONSE_HTML_BEGIN, str, RESPONSE_HTML_END);

    return ret;
}

/*  generate_response - parse and generate a http response given a request string 
    RETURN VALUE
    pointer to malloc'd char *response
*/ 
char *generate_response(char *str_request, size_t *bytes_total)
{

    struct httprequest req;
    if (request_parse(str_request, &req) < 0) {
        return response_fromstr("400 Bad Request");
    }
    
    switch (req.method) {
    case GET: {
        char file_name[256];
        if (req.url[0] == '/' && req.url[1] == '\0') {
            snprintf(file_name, 256, "%s/index.html", ROOT_DIR);
        } else {
            snprintf(file_name, 256, "%s/%s", ROOT_DIR, req.url);
        }

        FILE *f = fopen(file_name, "rb");
        if (f == NULL) {
            return response_fromstr("404 Not Found");
        }

        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        char date_buff[128];
        get_date(date_buff, 128);

        #define MAX_HEADER_SIZE 512
        char *buff = malloc(file_size+MAX_HEADER_SIZE);
        if (buff == NULL) {
            perror("malloc");
            exit(1);
        }
        snprintf(buff, MAX_HEADER_SIZE,
                "%s HTTP/1.1 200 OK\r\n"
                "Date: %s\r\n"
                "Server: %s\r\n"
                "Content-Length: %lu\r\n"
                "\r\n",
                RESPONSE_HTTP_VERSION,
                date_buff,
                SERVER_NAME, file_size);

        size_t headers_len = strlen(buff);
        fread(buff+headers_len, 1, file_size, f);
        fclose(f);

        *bytes_total = headers_len+file_size;
        buff[*bytes_total] = 0;
        return buff;
    }
    default:
        fprintf(stderr,
                "generate_response: http method unimplemented (%d)", req.method);
        return response_fromstr("500 Internal Server Error");
    }

    return response_fromstr("500 Internal Server Error");
}
