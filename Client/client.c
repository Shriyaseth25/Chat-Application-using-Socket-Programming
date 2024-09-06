#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../Encryption/encryption.h"
#include "client_header.h"

#define PORT 8080
#define BUFFER_SIZE 20000
#define MAX_LEN 12000

const char *encryption_key = "your_key";


int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    pthread_t tid;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to the server\n");

    int choice;
    printf("1. Register\n2. Login\nChoose an option: ");
    scanf("%d", &choice);
    getchar();  // To consume the newline after scanf

    if (choice == 1) {
        register_user(sock);
    } else {
        login(sock);
    }

    if (pthread_create(&tid, NULL, receive_messages, (void *)&sock) != 0) {
        perror("Thread creation failed");
        close(sock);
        return -1;
    }

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        xor_encrypt_decrypt(buffer, encryption_key);
        
        // Ensure the message is properly terminated
        strncat(buffer, "\n", BUFFER_SIZE - strlen(buffer) - 1);
        
        size_t len = strlen(buffer);
        if (send(sock, buffer, len, 0) != len) {
            perror("send failed");
        }
        // usleep(50000); 
    }

    pthread_join(tid, NULL);
    close(sock);

    return 0;
}
