#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "socket_utils.h"
#include "config_parser.h"

int main(void) {
    server_config config;

    if (load_config("server.conf", &config) != 0) {
        exit(0);
    }

    printf("Iniciando %s...\n", config.server_signature);
    
    int sockfd = init_server(config.listen_port);
    if (sockfd < 0) {
        exit(1);
    }

    printf("Escuchando en puerto %d...\n", config.listen_port);

    while (1) {
        int clientfd;
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);

        // Aceptar conexiones
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
        printf("Cliente conectado desde %s\n", inet_ntoa(client_addr.sin_addr));

        // Esto para probarlo luego
        char *msg = "HTTP/1.1 200 OK\r\n\r\nHola caracola";
        send(clientfd, msg, 45, 0);

        close(clientfd);
    }

    return 0;
}
