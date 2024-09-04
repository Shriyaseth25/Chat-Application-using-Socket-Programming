#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "encryption.h"
#include "client_header.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_LEN 100

const char *encryption_key = "your_key";


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
