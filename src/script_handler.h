#ifndef SCRIPT_HANDLER_H
#define SCRIPT_HANDLER_H

#include "config_parser.h"

void run_script(int clientfd, const char *path, const char *args, server_config *config);

#endif