#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
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

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
const char *encryption_key = "encryption_key";

// Function to check if the user exists
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

// Function to register a new user
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

// XOR encryption/decryption function
void xor_encrypt_decrypt(char *data, const char *key) {
    int data_len = strlen(data);
    int key_len = strlen(key);

    for (int i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}

void broadcast_message(char *message, int sender_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->sockfd != sender_sockfd) {
            char encrypted_message[BUFFER_SIZE];
            strcpy(encrypted_message, message);
            xor_encrypt_decrypt(encrypted_message, encryption_key);
            send(clients[i]->sockfd, encrypted_message, strlen(encrypted_message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    int nbytes;

    // Authentication process
    while (1) {
        // Receive authentication data
        if ((nbytes = read(cli->sockfd, buffer, sizeof(buffer))) <= 0) {
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

    // Handle client communication
    while ((nbytes = read(cli->sockfd, buffer, sizeof(buffer))) > 0) {
        buffer[nbytes] = '\0';
        xor_encrypt_decrypt(buffer, encryption_key);  // Decrypt the received message

        printf("%s: %s", cli->username, buffer);
        broadcast_message(buffer, cli->sockfd);  // Broadcast to other clients
    }

    // Client disconnected
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

int main() {
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
