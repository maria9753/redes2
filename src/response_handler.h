#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include "config_parser.h"
#include "http_parser.h"

void get_http_date(char *buf, size_t size, time_t);
const char* get_mime_type(const char *path);
void handle_response(int clientfd, http_request *request, server_config *config, const char *body, size_t body_len);

#endif