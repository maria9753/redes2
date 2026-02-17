#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "response_handler.h"

void handle_response(int clientfd, http_request *request, server_config *config) {
    // Verbo OPTIONS
    if (strcmp(request->method, "OPTIONS") == 0) {
        char response[512];
        sprintf(response, "HTTP/1.1 200 OK\r|n"
                           "Allow: GET, POST, OPTIONS\r\n"
                           "Server: %s\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n", config->server_signature);
        send(clientfd, response, strlen(response), 0);
        return;
    }

    // Verbo GET
    if (strcmp(request->method, "GET") == 0) {
        char path[512];

        // Construir ruta: htmlfiles/ + index.html
        sprintf(path, "%s%s", config->server_root, request->url);

        if (strstr(path, ".py") || strstr(path, ".php")) {
            return;
        }

        // Abrir archivo
        FILE *f = fopen(path, "rb");
        if (!f) {
            char *error404 = "HTTP/1.1 404 Not Found\r\n\r\nArchivo no enocontrado.";
            send(clientfd, error404, strlen(error404), 0);
            return;
        }

        // Enviar cabecera
        char *header = "HTTP/1.1 200 OK\r\n\r\nContent-Type: text/html\r\n\r\n";
        send(clientfd, header, strlen(header), 0);

        // Leer archivo y enviar al socket
        char file_buffer[1024];
        size_t bytes;
        while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), f)) > 0) {
            send(clientfd, file_buffer, bytes, 0);
        }

        fclose(f);
        return;
    }

    // Verbo no soportado
    char *error501 = "HTTP/1.1 501 Not Implemented\r\n\r\nMétodo no soportado.";
    send(clientfd, error501, strlen(error501), 0);   
}