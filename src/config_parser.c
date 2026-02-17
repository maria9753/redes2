#include <confuse.h>
#include <string.h>
#include <stdlib.h>
#include "config_parser.h"

int load_config(const char *file, server_config *config) {
    cfg_opt_t opts[] = {
        CFG_STR("server_root", "html/", CFGF_NONE),
        CFG_INT("max_clients", 10, CFGF_NONE),
        CFG_INT("listen_port", 8080, CFGF_NONE),
        CFG_STR("server_signature", "Servidor Redes 2", CFGF_NONE),
        CFG_END()
    };

    cfg_t *cfg;
    cfg = cfg_init(opts, CFGF_NONE);

    if (cfg_parse(cfg, archivo) == CFG_FILE_ERROR) {
        perror("Error al abrir server.conf");
        return -1;
    }

    // Pasar los datos
    config->server_root = strdup(cfg_getstr(cfg, "server_root"));
    config->max_clients = cfg_getint(cfg, "max_clients");
    config->listen_port = cfg_getint(cfg, "listen_port");
    config->server_signature = strdup(cfg_getstr(cfg, "server_signature"));

    cfg_free(cfg);
    return 0;
}