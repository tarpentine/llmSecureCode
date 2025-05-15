#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048

int sock;

void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    int len;
    while ((len = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';
        printf("\n%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    pthread_t recv_thread;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    printf("Connected. You can start typing messages.\n");

    while (fgets(message, BUFFER_SIZE, stdin) != NULL) {
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}

