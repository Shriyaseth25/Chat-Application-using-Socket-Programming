//libraries for input/output, mememory allocation and string manipulation
// unistd.h provides access to POSIX OS API( for closing and opening sockets
// arpa/inet.h defines internet operations
// pthread.h supports multi threading
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "encryption.h"
#include "server_header.h"


// defines port number, memory of buffer, the max number of clients that can connect simultaneously and lengths for username and password
#define PORT 8080
#define BUFFER_SIZE 2000
#define MAX_CLIENTS 10
#define USERNAME_LEN 50
#define PASSWORD_LEN 50

// user structure to store username and password for each user
typedef struct {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} User;

//client structure to store info about connected clients
typedef struct {
    int sockfd;
    struct sockaddr_in addr;
    char username[USERNAME_LEN];
} client_t;


// Global variables
client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
const char *encryption_key = "encryption_key";


int main() {
    // variables to store file descriptor of server socket and client socket, and structure that stores the server's address info.
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept an incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Create a new client structure
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->sockfd = new_socket;
        cli->addr = address;

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!clients[i]) {
                clients[i] = cli;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        // Create a thread to handle the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)cli) != 0) {
            perror("Thread creation failed");
            close(new_socket);
            free(cli);
        }
    }

    return 0;
}
