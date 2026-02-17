#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include "config_parser.h"
#include "http_parser.h"

void handle_response(int clientfd, http_request *request, server_config *config);

#endif