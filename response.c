#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "response.h"

#pragma GCC diagnostic ignored "-Wformat-truncation"

#define RESPONSE_HTTP_VERSION "HTTP/1.1"


/* response_str - append \r\n\r\n and prepend HTTP version to str, then
   return malloc'd ptr
*/
char *response_str(char *str)
{
    size_t padding = strlen(RESPONSE_HTTP_VERSION) + 4;
    char *ret = (char*)malloc(strlen(str) + padding);
    if (ret == NULL) {
        perror("malloc");
        exit(1);
    }
    sprintf(ret, "%s %s\r\n\r\n", RESPONSE_HTTP_VERSION, str);

    return ret;
}

/*  generate_response - generate a http response given a httprequest object
    RETURN VALUE
     - pointer to malloc'd char * response
*/ 
char *generate_response(struct httprequest *req)
{
    switch (req->method) {
    case GET: {
        FILE *f;
        char path[256];
        if (req->url[0] == '/' && req->url[1] == '\0') {
            snprintf(path, 256, "%s/index.html", ROOT_DIR);
        } else {
            snprintf(path, 256, "%s%s", ROOT_DIR, req->url);
        }

        f = fopen(path, "rb");
        if (f == NULL) {
            return response_str("404 Not Found");
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

        //printf("[GENERATOR] %s\n", buff);

        return buff;
    }
    default:
        assert(0 && "method unimplemented");
        break;
    }

    return response_str("500 Internal Server Error");
}
