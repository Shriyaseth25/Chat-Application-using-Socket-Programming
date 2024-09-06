int user_exists(const char *username, const char *password);

int register_user(const char *username, const char *password);

void broadcast_message(char *message, int sender_sockfd);

void *handle_client(void *arg);
