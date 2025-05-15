#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(char *message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < client_count; ++i) {
        if (client_sockets[i] != sender_sock) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        broadcast(buffer, client_sock);
    }

    // Remove client
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < client_count; ++i) {
        if (client_sockets[i] == client_sock) {
            for(int j = i; j < client_count - 1; ++j) {
                client_sockets[j] = client_sockets[j + 1];
            }
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&clients_mutex);

    close(client_sock);
    free(arg);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);

    printf("Server started on port 8888\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS) {
            client_sockets[client_count++] = client_sock;

            int *pclient = malloc(sizeof(int));
            *pclient = client_sock;
            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, pclient);
            pthread_detach(tid);
        } else {
            printf("Max clients reached. Connection rejected.\n");
            close(client_sock);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    return 0;
}

