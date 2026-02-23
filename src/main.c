#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include "socket_utils.h"
#include "config_parser.h"
#include "http_parser.h"
#include "response_handler.h"

typedef struct {
    int clientfd;
    server_config *config;
} thread_args;

void *wrapper_handle_response(void *ptr) {
    thread_args *args = (thread_args *)ptr;
    char buffer[8192];

    // Recibir petición
    int bytes_read = recv(args->clientfd, buffer, sizeof(buffer) -1, 0);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        http_request request;

        // Parsear y responder
        int header_len = parse_request(buffer, bytes_read, &request);
        if (header_len >= 0) {
            const char *body = buffer + header_len;
            size_t body_len = bytes_read - header_len;

            handle_response(args->clientfd, &request, args->config. body, body_len);

        } else {
            char *bad_req = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
            send(args->clientfd, bad_req, strlen(bad_req), 0);
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

    // Cargar configuración del servidor
    if (load_config("server.conf", &config) != 0) {
        perror("Error al leer server.conf.");
        exit(1);
    }

    printf("Iniciando %s...\n", config.server_signature);
    
    int sockfd = init_server(config);
    if (sockfd < 0) {
        perror("Error al iniciar socket.");
        exit(1);
    }

    printf("Escuchando en puerto %ld...\n", config.listen_port);

    while (1) {
        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        // Aceptar conexiones
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
        printf("Cliente conectado desde %s\n", inet_ntoa(client_addr.sin_addr));

        if (clientfd > 0) {
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

    close(sockfd);
    return 0;
}
