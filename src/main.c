#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "socket_utils.h"
#include "config_parser.h"
#include "http_parser.h"

typedef struct {
    int clientfd;
    server_config *config;
} thread_args;

void *wrapper_handle_response(void *ptr) {
    thread_args *args = (thread_args *)ptr;
    char buffer[2048];

    // Recibir petición
    int bytes_read = recv(args->clientfd, buffer, sizeof(buffer) -1, 0);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        http_request request;

        // Parsear y responder
        if (parse_request(buffer, bytes_read, &request) == 0) {
            handle_response(args->clientfd, &request, args->config);
        }
    }

    // Limpiar y cerrar
    close(args->clientfd);
    free(args);
    pthread_detach(pthread_self());
    return NULL;
}

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

        if (client > 0) {
            thread_args *args = malloc(sizeof(thread_args));
            args->clientfd = clientfd;
            args->config = &config;

            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, wrapper_handle_response, (void *)args) != 0) {
                perror("Error al crear un hilo");
                close(clientfd);
                free(args);
            }
        }
    }

    return 0;
}
