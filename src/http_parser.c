#include "http_parser.h"
#include <string.h>
#include <stdio.h>

int parse_request(char *buffer, int buflen, http_request *request) {
    const char *method, *path;
    size_t method_len, path_len;
    int minor_version;
    struct phr_header headers[100];
    size_t num_headers = 100;
    int pret;

    pret = phr_parse_request(buffer, buflen, &method, &method_len, &path, &path_len, &minor_version, headers, &num_headers, 0);

    if (pret > 0) {
        // Pasar datos
        snprintf(request->method, sizeof(request->method), "%.*s", (int)method_len, method);
        snprintf(request->url, sizeof(request->url), "%.*s", (int)path_len, path);
        request->version = minor_version;
        return 0;
    }

    // Error
    return pret;
}