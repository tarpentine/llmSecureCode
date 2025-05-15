#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4444
#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

SSL_CTX *create_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int main() {
    int server_fd, client_fd, max_fd, activity, i;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    fd_set readfds;
    SSL_CTX *ctx;
    SSL *clients[MAX_CLIENTS] = {0};

    SSL_library_init();
    ctx = create_context();
    configure_context(ctx);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, MAX_CLIENTS);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i]) {
                FD_SET(SSL_get_fd(clients[i]), &readfds);
                if (SSL_get_fd(clients[i]) > max_fd)
                    max_fd = SSL_get_fd(clients[i]);
            }
        }

        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, client_fd);
            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                close(client_fd);
                SSL_free(ssl);
            } else {
                for (i = 0; i < MAX_CLIENTS; ++i) {
                    if (!clients[i]) {
                        clients[i] = ssl;
                        break;
                    }
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; ++i) {
            SSL *ssl = clients[i];
            if (ssl && FD_ISSET(SSL_get_fd(ssl), &readfds)) {
                char buffer[BUFFER_SIZE];
                int bytes = SSL_read(ssl, buffer, sizeof(buffer));
                if (bytes <= 0) {
                    SSL_shutdown(ssl);
                    close(SSL_get_fd(ssl));
                    SSL_free(ssl);
                    clients[i] = NULL;
                } else {
                    buffer[bytes] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        if (clients[j] && j != i) {
                            SSL_write(clients[j], buffer, bytes);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    return 0;
}

