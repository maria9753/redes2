#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "response_handler.h"
#include "script_handler.h"

void get_http_date(char *buf, size_t size, time_t) {
    struct tm *tm_info = gmtime(&t);
    strftime(buf, size, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
}

const char* get_mime_type(const char *path) {
    const char *extension = strrchr(path, '.');

    if (!extension) {
        return "application/octet-stream";
    }

    if (strcmp(extension, ".txt") == 0) {
        return "text/plain";
    }

    if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm")) {
        return "text/html";
    }

    if (strcmp(extension, ".gif") == 0) {
        return "image/gif";
    }

    if (strcmp(extension, ".jpeg") == 0 || strcmp(extension, ".jpg")) {
        return "image/jpeg";
    }

    if (strcmp(extension, ".mpeg") == 0 || strcmp(extension, ".mpg")) {
        return "video/jpeg";
    }

    if (strcmp(extension, ".doc") == 0 || strcmp(extension, ".docx")) {
        return "application/msword";
    }

    if (strcmp(extension, ".pdf") == 0) {
        return "application/pdf";
    }

    return "application/octet-stream";
}

void handle_response(int clientfd, http_request *request, server_config *config, const char *body, size_t body_len) {
    char path[512];
    char date_now[100], date_mod[100];
    char *args = NULL;
    char *temp_url = strdup(request->url);

    // Sacar argumentos en función del verbo
    if (strcmp(request->method, "GET") == 0) {
        // Sacar los argumentos de la url
        char *query = strchr(temp_url, '?');
        if (query) {
            *query = '\0';
            args = query + 1; 
        }

    } else if (strcmp(request->method, "POST") == 0) {
        // Sacar los argumentos del cuerpo
        args = (char *)body;
    }

    // Construir ruta
    if (temp_url[0] == '/') {
        sprintf(path, "%s%s", config->server_root, &temp_url[1]);

    } else {
        sprintf(path, "%s%s", config->server_root, temp_url);
    }

    // Obtener fecha
    get_http_date(date_now, sizeof(date_now), time(NULL));

    if (strcmp(request->method, "OPTIONS") == 0) {
        // Verbo OPTIONS
        char response[1024];
        sprintf(response, "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: %s\r\nAllow: GET, POST, OPTIONS\r\nContent-Length: 0\r\n\r\n", date_now, config->server_signature);
        send(clientfd, response, strlen(response), 0);

    } else if (strstr(path, ".py") || strstr(path, ".php")) {
        // Ejecutar scripts
        run_script(clientfd, path, args, config);
         
    } else if (strcmp(request->method, "GET") == 0) {
        // Verbo GET
        struct stat st;
        if (stat(path, &st) == -1) {
            char *err = "HTTP/1.1 404 Not Found\r\nContent-Length: 22\r\n\r\nArchivo no enocntrado.";
            send(clientfd, err, strlen(err), 0);

        } else {
            get_http_date(date_mod, sizeof(dae_mod), st.st_mtime);

            const char *mime = get_mime_type(path);

            // Enviar cabecera
            char header[1024];
            int hlen = sprintf(header, "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: %s\r\nLast-Modifies: %s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", date_now, config->server_signature, date_mod, mime, (long)st.st_size);
            send(clientfd, header, hlen, 0);

            FILE *f = fopen(path, "rb");
            if (f) {
                char file_buffer[1024];
                size_t bytes;
                while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), f)) > 0) {
                    send(clientfd, file_buffer, bytes, 0);
                }

                fclose(f);
            }
        }

    } else if (strcmp(request->method, "POST") == 0) {
        // Verbo POST
        char *err = "HTTP/1.1 400 Bad Request\r\nContent-Length: 17\r\n\r\nCuerpo no válido.";
        send(clientfd, err, strlen(err), 0);

    } else {
        // Verbo no soportado
        char err[512];
        int len = sprintf(err, "HTTP/1.1 501 Method Not Implemented\r\nDate: %s\r\nServer: %s\r\nContent-Length: 22\r\n\r\nMétodo no soportado.", date_now, config->server_signature);
        send(clientfd, err, len, 0);
    }

    free(temp_url);
}