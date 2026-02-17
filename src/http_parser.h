#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "picohttpparser.h"

typedef struct {
    char method[10];
    char url[255];
    int version;
} http_request;

int parse_request(const char *buffer, int buflen, http_request *request);

#endif