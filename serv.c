#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

int num_clients = 0;
int num_pieces = 5;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    char buffer[1024] = {0};
    sprintf(buffer, "%d", num_pieces);
    send(client_fd, buffer, strlen(buffer), 0);
    while (1) {
        memset(buffer, 0, 1024);
        int valread = read(client_fd, buffer, 1024);
        if (valread == 0) {
            break;
        }
        if (strcmp(buffer, "eat") == 0) {
            pthread_mutex_lock(&mutex);
            if (num_pieces == 0) {
                sprintf(buffer, "Waking up the cook to refill the pot...\nYou ate a piece of meat. 4 pieces left.");
                num_pieces = 5;
                printf("\nA savage asked to refill the pot. Pot was refilled.\n");
                fflush(stdout);
            }
            else{
            sprintf(buffer, "You ate a piece of meat. %d pieces left.", num_pieces-1);
            }
                num_pieces--;
                send(client_fd, buffer, strlen(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }
    }
    close(client_fd);
    num_clients--;
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t threads[4];
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 4) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is active and working!");
    fflush(stdout);
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        if (num_clients < 4) {
            pthread_create(&threads[num_clients], NULL, handle_client, &new_socket);
            num_clients++;
        } else {
            char *msg = "Maximum number of clients reached.";
            send(new_socket, msg, strlen(msg), 0);
            close(new_socket);
        }
    }
    return 0;
}

