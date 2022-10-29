#pragma once

#define URL_FIELD_SIZE 256
#define VERSION_FIELD_SIZE 8
#define HEADER_FIELD_NUM 16
#define HEADER_KEY_SIZE 32
#define HEADER_VALUE_SIZE 128

// ref: https://datatracker.ietf.org/doc/html/rfc9110#section-9
enum httpmethod {
    GET,
    HEAD,
    /* all other methods are optional */
    INVALID
};

struct httpheaderfield {
    char key[32];
    char value[128];
};

struct httprequest {
    enum httpmethod method;
    char url[256];
    char version[8];

    struct httpheaderfield fields[HEADER_FIELD_NUM];
};

int request_parse(const char *req, struct httprequest *ret);
