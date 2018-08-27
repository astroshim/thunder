#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "pthread.h"

#define MESSAGE_BUFFER 500
#define CLIENT_ADDRESS_LENGTH 100

// Start server
void * start_server(int socket_fd, struct sockaddr_in *address) {
    // bind(socket_fd, (struct sockaddr *) address, sizeof *address);
    bind(socket_fd, (struct sockaddr *) address, sizeof(struct sockaddr_in));
    printf("Waiting for connection...\n");
    listen(socket_fd, 10);
}

// Get message from stdin and send to client
void * send_message(int new_socket_fd, struct sockaddr *cl_addr) {
    char message[MESSAGE_BUFFER];
    while (fgets(message, MESSAGE_BUFFER, stdin) != NULL) {
        if (strncmp(message, "/quit", 5) == 0) {
            printf("Closing connection...\n");
            exit(0);
        }
        sendto(new_socket_fd, message, MESSAGE_BUFFER, 0, (struct sockaddr *) &cl_addr, sizeof cl_addr);
    }
}

void * receive(void * socket) {
    int socket_fd, response;
    char message[MESSAGE_BUFFER];
    memset(message, 0, MESSAGE_BUFFER); // Clear message buffer
    socket_fd = (int) socket;

    // Print received message
    while(true) {
        response = recvfrom(socket_fd, message, MESSAGE_BUFFER, 0, NULL, NULL);
        if (response) {
            printf("%s", message);
        }
    }
}

int main(int argc, char**argv) {
    struct sockaddr_in address, cl_addr;
    int socket_fd, length, response, new_socket_fd;
    char client_address[CLIENT_ADDRESS_LENGTH];
    pthread_t thread;

    if (argc < 2) {
        printf("Usage: server port_number\n");
        exit(1);
    }
    long port = strtol(argv[1], NULL, 10);

    printf("port = (%d)\n", port);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    start_server(socket_fd, &address);

    // Accept connection
    length = sizeof(cl_addr);
    new_socket_fd = accept(socket_fd, (struct sockaddr *) &cl_addr, &length);
    if (new_socket_fd < 0) {
        printf("Failed to connect\n");
        exit(1);
    }

    inet_ntop(AF_INET, &(cl_addr.sin_addr), client_address, CLIENT_ADDRESS_LENGTH);
    printf("Connected: %s\n", client_address);

    // Create new thread to receive messages
    pthread_create(&thread, NULL, receive, (void *) new_socket_fd);

    // Send message
    send_message(new_socket_fd, &cl_addr);

    // Close sockets and kill thread
    close(new_socket_fd);
    close(socket_fd);
    pthread_exit(NULL);
    return 0;
}

