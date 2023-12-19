#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl_tool.h"

int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	    perror("Fail to create socket");
	    exit(1);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	    perror("Fail to bind");
	    exit(1);
    }

    if (listen(s, 1) < 0) {
	    perror("Fail to listen");
	    exit(1);
    }

    return s;
}



int main(int argc, char** argv)
{
    int sock;
    SSL_CTX *ctx;

    ssl_init();
    ctx = ssl_server_setup("/etc/pki/tls/certs/postfix.pem", "/etc/pki/tls/private/postfix.key");

    sock = create_socket(4433);

    /* Handle connections */
    while(1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        const char reply1[] = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:4\r\n\r\nTEST";
        const char reply2[] = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n";

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }


        //ssl = ssl_server_socket(client, "/etc/pki/tls/certs/postfix.pem", "/etc/pki/tls/private/postfix.key");
        ssl = ssl_server_socket(client, ctx);

        /*
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        */

        int ret;
        do {
            ret = SSL_accept(ssl);
            ret = SSL_get_error(ssl, ret);
        } while (ret==SSL_ERROR_WANT_READ || ret==SSL_ERROR_WANT_WRITE || ret==SSL_ERROR_SYSCALL);

        if (ret==SSL_ERROR_SSL) {
            printf("ERRRORRRRRRRRRRRRRRRRR!!\n");
        }

        if (argc>0) {
            SSL_write(ssl, reply2, strlen(reply2));
            SSL_write(ssl,(const char*) argv[1], strlen(argv[1]));
        }
        else {
            SSL_write(ssl, reply1, strlen(reply1));
        }

/*
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            SSL_write(ssl, reply, strlen(reply));
        }
*/

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }

    close(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
}
