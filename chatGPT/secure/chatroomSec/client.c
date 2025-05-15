#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

#define PORT 4444
#define BUFFER_SIZE 2048

SSL_CTX *create_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void *receive_handler(void *arg) {
    SSL *ssl = (SSL *)arg;
    char buffer[BUFFER_SIZE];
    int bytes;

    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("\n[Server]: %s\n", buffer);
        fflush(stdout);
    }

    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    SSL_CTX *ctx;
    SSL *ssl;
    int sock;
    pthread_t tid;

    SSL_library_init();
    ctx = create_context();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    pthread_create(&tid, NULL, receive_handler, ssl);

    char msg[BUFFER_SIZE];
    while (1) {
        printf("You: ");
        fgets(msg, sizeof(msg), stdin);
        SSL_write(ssl, msg, strlen(msg));
    }

    SSL_shutdown(ssl);
    close(sock);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}

