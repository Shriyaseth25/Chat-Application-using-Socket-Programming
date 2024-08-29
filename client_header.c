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


// defines port number, memory of buffer, the max number of clients that can connect simultaneously and lengths for username and password
#define PORT 8080
#define BUFFER_SIZE 3000
#define MAX_LEN 6000


extern const char *encryption_key;


// Function to receive messages from the server
void *receive_messages(void *sock) {
    int sockfd = *((int *)sock);
    char buffer[BUFFER_SIZE];

    while (1) {
        int nbytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (nbytes > 0) {
            buffer[nbytes] = '\0';
            xor_encrypt_decrypt(buffer, encryption_key);  // Decrypt the received message
            printf("Server: %s", buffer);
        } else {
            printf("Disconnected from server.\n");
            close(sockfd);
            pthread_exit(NULL);
        }
    }
}



// Function to handle user login
void login(int sockfd) {
    char username[MAX_LEN], password[MAX_LEN], buffer[MAX_LEN];

    // Request login credentials
    printf("Enter username: ");
    fgets(username, MAX_LEN, stdin);
    username[strcspn(username, "\n")] = 0;  // Remove newline

    printf("Enter password: ");
    fgets(password, MAX_LEN, stdin);
    password[strcspn(password, "\n")] = 0;  // Remove newline

    // Send login details to the server
    snprintf(buffer, MAX_LEN, "LOGIN %s %s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive server response
    recv(sockfd, buffer, MAX_LEN, 0);
    printf("%s\n", buffer);
}
