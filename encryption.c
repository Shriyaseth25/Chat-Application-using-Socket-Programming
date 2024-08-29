// libraries for input/output, mememory allocation and string manipulation
// unistd.h provides access to POSIX OS API( for closing and opening sockets// arpa/inet.h defines internet operations
// pthread.h supports multi threading
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// defines port number, memory of buffer, the max number of clients that can connect simultaneously and lengths for username and password
#define PORT 8080
#define BUFFER_SIZE 3000
#define MAX_CLIENTS 10
#define USERNAME_LEN 50
#define PASSWORD_LEN 50



void xor_encrypt_decrypt(char *data, const char *key) {
    int data_len = strlen(data);
    int key_len = strlen(key);

    for (int i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}
