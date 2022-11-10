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
    char key[HEADER_KEY_SIZE];
    char value[HEADER_VALUE_SIZE];
};

struct httprequest {
    enum httpmethod method;
    char url[URL_FIELD_SIZE];
    char version[VERSION_FIELD_SIZE];

    struct httpheaderfield fields[HEADER_FIELD_NUM];
    struct httpheaderfield get_params[HEADER_FIELD_NUM];
};
