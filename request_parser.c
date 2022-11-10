#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "httpdef.h"
#include "request_parser.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

enum httpmethod httpmethod_fromstr(const char *str)
{
    if (!strncmp(str, "GET", 3)) {
        return GET;
    } else if (!strncmp(str, "HEAD", 4)) {
        return HEAD;
    } else {
        return INVALID;
    }
}

/*  request_parse - parse a http request string

    RETURN VALUE
    -1: ERROR - invalid request
     0: OK
 */ 
int request_parse(const char *req, struct httprequest *ret)
{
    char *token, *i_token;
    char *req_dup = strdup(req);
    char *req_dup_ptr = req_dup; /* needed to free later,
                                    strtok modifies the original pointer */
    int i = -3;
    while ((token = strtok_r(req_dup, "\r\n", &req_dup)) != NULL) {
        if (i < 0) {
            while ((i_token = strtok_r(token, " ", &token)) != NULL) {
                // TODO: sanitize url and version input
                switch (i) {
                case -3: // http method
                    ret->method = httpmethod_fromstr(i_token);
                    if (ret->method == INVALID)
                        return -1; /* invalid http method */
                    break;
                case -2: // url
                    strncpy(ret->url, i_token, URL_FIELD_SIZE);
                    break;
                case -1: // version
                    strncpy(ret->version, i_token, VERSION_FIELD_SIZE);
                    break;
                }
                i++;
            }
        } else {
            /* TODO: Maybe some kind of a hashmap? */
            char *value = strstr(token, ": ");

            if (value == NULL || value-token > HEADER_KEY_SIZE ||
                strlen(value)-2 > HEADER_VALUE_SIZE) {
                return -1;
            }

            strncpy(ret->fields[i].key, token, MIN(value-token, HEADER_KEY_SIZE));
            strncpy(ret->fields[i].value, value+2, HEADER_VALUE_SIZE);
            ret->fields[i].key[value-token] = '\0';
            i++;
        }
    }

    free(req_dup_ptr);
    return 0;
}
