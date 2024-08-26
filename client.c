#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_LEN 100

const char *encryption_key = "your_key";

// XOR encryption/decryption function
void xor_encrypt_decrypt(char *data, const char *key) {
    int data_len = strlen(data);
    int key_len = strlen(key);

    for (int i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

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

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    pthread_t tid;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to the server\n");

    // Handle login
    login(sock);

    // Create a thread to receive messages from the server
    if (pthread_create(&tid, NULL, receive_messages, (void *)&sock) != 0) {
        perror("Thread creation failed");
        close(sock);
        return -1;
    }

    // Send messages to the server
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        xor_encrypt_decrypt(buffer, encryption_key);  // Encrypt the message before sending
        send(sock, buffer, strlen(buffer), 0);
    }

    pthread_join(tid, NULL);
    close(sock);

    return 0;
}
