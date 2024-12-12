#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define handle_error(vl, msg) \
    if(vl == -1) { perror(msg); return EXIT_FAILURE; }
#define handle_error_socket(vl, msg, server_sock_fd) \
    if(vl == -1) { perror(msg); if (close(server_sock_fd) == -1) perror("close socket"); return EXIT_FAILURE; }

#define LISTENING_ADDRESS "::" // The listening ip address IN6ADDR_ANY


void close_sockets(int client_fd, int server_fd);
/**
 * Params: argv[0] <port>
 */
int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]); // Can be converted to strtol to better implem (using errno etc) but is ok bc return 0 on error and 0 is not an accepted value.
    if(port < 1 || port > 65535) { fprintf(stderr, "Port should be an int between 1 and 65535.\n"); return EXIT_FAILURE; }

    struct sockaddr_in6 sa;
    sa.sin6_port = htons(port);
    sa.sin6_family = AF_INET6;

    int err = inet_pton(AF_INET6, LISTENING_ADDRESS, &sa.sin6_addr); // convert string version of the listening address to a binary form (in network byte order)
    if(err == 0) { fprintf(stderr, "An error occurred while parsing the IP '%s'.\n", LISTENING_ADDRESS); return EXIT_FAILURE; }

    int server_sock_fd = socket(sa.sin6_family, SOCK_STREAM, 0); handle_error(server_sock_fd, "socket");

    struct sockaddr_in6 peer_addr;

    int enable = 1;
    err = setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    handle_error_socket(err, "setsockopt", server_sock_fd);

    err = bind(server_sock_fd, (struct sockaddr *) &sa, sizeof(sa));
    handle_error_socket(err, "bind", server_sock_fd);

    err = listen(server_sock_fd, SOMAXCONN); // Here, `SOMAXCONN` as backlog to use `/proc/sys/net/core/somaxconn` (The system defined backlog value) By default on my system: 4096
    handle_error_socket(err, "listen", server_sock_fd);

    socklen_t peer_addr_size = sizeof(peer_addr);
    int client_socket_fd = accept(server_sock_fd, (struct sockaddr *) &peer_addr, &peer_addr_size);
    handle_error_socket(client_socket_fd, "accept", server_sock_fd);

    char *message = "Ceci n'est pas un message provenant du surfer, ceci est un message de hack, rempli de malice.\n";

    ssize_t byte_sent = send(client_socket_fd, message, strlen(message), 0);
    if(byte_sent == -1) {
        perror("send");
        close_sockets(client_socket_fd, server_sock_fd);
        return EXIT_FAILURE;
    }

    close_sockets(client_socket_fd, server_sock_fd);
    return EXIT_SUCCESS;
}

/**
 * Close every socket
 * Before using close(2), using shutdown(2) to shutdown gracefully the sockets.
 * @param client_fd the client socket file descriptor number
 * @param server_fd the server socket file descriptor number
 */
void close_sockets(int client_fd, int server_fd) {
    if (shutdown(client_fd, SHUT_RDWR) == -1) perror("shutdown client socket ends");
    if (close(client_fd) == -1) perror("close client socket");

    if(shutdown(server_fd, SHUT_RDWR) == -1) perror("shutdown server socket ends");
    if(close(server_fd) == -1) perror("close server socket");
}