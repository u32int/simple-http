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

#define RESPONSE_HTML_BEGIN "<!DOCTYPE html><html><body style=\"text-align: center\"><h1>"
#define RESPONSE_HTML_END "</h1></body></html>"

/* response_str - append \r\n\r\n and prepend HTTP version to str, then
   return malloc'd ptr
*/
char *response_fromstr(char *str)
{
    #define RESPONSE_BUFF_SIZE 4096

    char date_buff[128];
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    strftime(date_buff, sizeof(date_buff), "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char *ret = (char*)malloc(RESPONSE_BUFF_SIZE);
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

/*  generate_response - generate a http response given a httprequest object
    RETURN VALUE
     - pointer to malloc'd char * response
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

        const char *prefix = "HTTP/1.1 200 OK\r\n\r\n";
        size_t pref_len = strlen(prefix);

        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buff = (char *)malloc(file_size+pref_len+1);
        strncpy(buff, prefix, pref_len);
        if (buff == NULL) {
            perror("malloc");
            exit(1);
        }

        fread(buff+pref_len, 1, file_size, f);
        buff[file_size+pref_len+1] = '\0';

        fclose(f);
        return buff;
    }
    default:
        fprintf(stderr,
                "generate_response: http method unimplemented (%d)", req.method);
        return response_fromstr("500 Internal Server Error");
    }

    return response_fromstr("500 Internal Server Error");
}
