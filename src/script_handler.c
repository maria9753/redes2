#include "script_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>

void run_script(int clientfd, const char *path, const char *args, server_config *config) {
    // Tubería para leer salida de script
    int pipe_out[2]; 
    // Tubería para enviarle los argumentos al script
    int pipe_in[2]; 

    pipe(pipe_out);
    pipe(pipe_in);

    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo
        
        // Redirigir la entrada estándar para que lea del pipe_in
        dup2(pipe_in[0], STDIN_FILENO);
        // Redirigir la salida estándar para que escirba en el pipe_out
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[1]);
        close(pipe_out[0]);
        close(clientfd);

        const char *extension = strrchr(path, '.');

        if (strcmp(extension, ".py")) {
            execlp("python3", "python3", path, NULL);
        } else if (strcmp(extension, ".php")) {
            execlp("php", "php", path, NULL);
        }

        // Error
        exit(1);

    } else {
        // Proceso padre

        close(pipe_in[0]);
        close(pipe_out[1]);

        // Pasar argumentos al script
        if (args) {
            write(pipe_in[1], args, strlen(args));
        }
        close(pipe_in[1]);

        // Leer salida del script y enviarla al cliente
        char buffer[1024];
        ssize_t bytes_read;
        int first_chunk = 1;

        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            if (first_chunk) {
                char header[512];
                time_t now = time(NULL);
                struct tm *tm_info = gmtime(&now);
                char date_buf[100];
                strftime(date_buf, sizeof(date_buf), "%a, %d %b %Y %H:%M:%S GMT", tm_info);

                sprintf(header, "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n", date_buf, config->server_signature);
                
                send(clientfd, header, strlen(header), 0);
                first_chunk = 0;
            }
            send(clientfd, buffer, bytes_read, 0);
        }

        close(pipe_out[0]);
        waitpid(pid, NULL, 0);
    }