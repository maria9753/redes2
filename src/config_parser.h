#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

typedef struct {
    char *server_root;
    int max_clients;
    int listen_port;
    char *server_signature;
} server_config;

int load_config(const char *file, server_config *config);

#endif