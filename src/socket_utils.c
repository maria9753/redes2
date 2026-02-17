// Bibliotecas copiadas de server.c en ejemplos de código
#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>

// Función para inicializar el socket del servidor
int init_server(int port) {
    int sockfd;
    struct sockaddr_in self;

    // Crear socket tipo TCP (SOCK_STREAM para TCP o SOCK_DGRAM para UDP; 
    // 0 para protocolo predeterminado; devuelve descriptor de archivo o -1)
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creando socket");
        return -1;
    }

    // Permitir reutilizar el puerto tras cerrar el servidor
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurar dirección y puerto
    // En ejemplo usan bzero pero resulta que está obsoleta
    memset(&self, 0, sizeof(self));
    self.sin_family = AF_INET;
    self.sin_port = htons(port);
    self.sin_addr.s_addr = INADDR_ANY;

    // Ligar el puerto al socket
    if (bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0) {
        perror("Error ligando el puerto al socket");
        return -1;
    }

    // Escuchar
    if (listen(sockfd, 10) != 0) {
        perror("Error escuchando");
        return -1;
    }

    return sockfd;
}