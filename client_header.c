#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "encryption.h"

#define BUFFER_SIZE 3000
#define MAX_LEN 12000
#define MAX_INPUT_LEN 2990

extern const char *encryption_key;

void *receive_messages(void *sock) {
    int sockfd = *((int *)sock);
    char buffer[BUFFER_SIZE];
    int nbytes;

    while (1) {
        memset(buffer, 0, 3000);  // Clear buffer before receiving

        // Receive data from the server
        nbytes = recv(sockfd, buffer, sizeof(buffer) - 2, 0);

        if (nbytes > 0) {
            // Properly terminate the received string
            buffer[nbytes] = '\0';

            // Decrypt the received message
            xor_encrypt_decrypt(buffer, encryption_key);

            // Print the decrypted message
            printf("Client: %s\n", buffer);

        } else if (nbytes == 0) {
            printf("Disconnected from server.\n");
            close(sockfd);
            pthread_exit(NULL);
        } else {
            perror("recv failed");
            close(sockfd);
            pthread_exit(NULL);
        }
    }
}


void login(int sockfd) {
    char username[MAX_INPUT_LEN], password[MAX_INPUT_LEN], buffer[MAX_LEN];

    // Request login credentials
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // Remove newline

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;  // Remove newline

    // Send login details to the server
    snprintf(buffer, sizeof(buffer), "LOGIN %s %s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive server response
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
}

void register_user(int sockfd) {
    char username[MAX_INPUT_LEN], password[MAX_INPUT_LEN], buffer[MAX_LEN];

    // Request registration details
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // Remove newline

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;  // Remove newline

    // Send registration details to the server
    snprintf(buffer, sizeof(buffer), "REGISTER %s %s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive server response
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    printf("Please Login using your credentials:");
    login(sockfd);
}




