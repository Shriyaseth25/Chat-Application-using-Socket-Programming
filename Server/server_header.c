#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../Encryption/encryption.h"

#define PORT 8080
#define BUFFER_SIZE 20000
#define MAX_CLIENTS 10
#define USERNAME_LEN 50
#define PASSWORD_LEN 50

typedef struct {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} User;

typedef struct {
    int sockfd;
    struct sockaddr_in addr;
    char username[USERNAME_LEN];
} client_t;

extern client_t *clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;
extern const char *encryption_key;

int user_exists(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        return 0;
    }

    User user;
    while (fscanf(file, "%s %s", user.username, user.password) != EOF) {
        if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
            fclose(file);
            return 1;  // User exists
        }
    }

    fclose(file);
    return 0;  // User not found
}

int register_user(const char *username, const char *password) {
    if (user_exists(username, password)) {
        return 0;  // User already exists
    }

    FILE *file = fopen("users.txt", "a");
    if (!file) {
        return 0;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    return 1;  // Registration successful
}

void broadcast_message(char *message, int sender_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->sockfd != sender_sockfd) {
            char encrypted_message[BUFFER_SIZE];
            strcpy(encrypted_message, message);
            xor_encrypt_decrypt(encrypted_message, encryption_key);
            send(clients[i]->sockfd, encrypted_message, strlen(encrypted_message)-1, 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    int nbytes;

    while (1) {
        if ((nbytes = read(cli->sockfd, buffer, 20000)) <= 0) {
            close(cli->sockfd);
            free(cli);
            pthread_exit(NULL);
        }

        buffer[nbytes] = '\0';
        char *command = strtok(buffer, " ");
        char *username = strtok(NULL, " ");
        char *password = strtok(NULL, " ");

        if (strcmp(command, "REGISTER") == 0) {
            if (register_user(username, password)) {
                send(cli->sockfd, "Registration successful\n", 24, 0);
            } else {
                send(cli->sockfd, "User already exists\n", 20, 0);
            }
        } else if (strcmp(command, "LOGIN") == 0) {
            if (user_exists(username, password)) {
                strcpy(cli->username, username);
                send(cli->sockfd, "Login successful\n", 18, 0);
                break;
            } else {
                send(cli->sockfd, "Invalid credentials\n", 20, 0);
            }
        }
    }

    while ((nbytes = read(cli->sockfd, buffer, 20000)) > 0) {
        buffer[nbytes] = '\0';
        printf("Encrypted message from %s: %s\n", cli->username, buffer);

        xor_encrypt_decrypt(buffer, encryption_key);
        // printf("Decrypted message from %s: %s", cli->username, buffer);

        broadcast_message(buffer, cli->sockfd);
    }

    close(cli->sockfd);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->sockfd == cli->sockfd) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    free(cli);
    pthread_exit(NULL);
}
