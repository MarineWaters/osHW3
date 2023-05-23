#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int num_pieces = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection fail\n");
        return -1;
    }
    printf("Client is active and working!\n");
    fflush(stdout);
    valread = read(sock, buffer, 1024);
    num_pieces = atoi(buffer);
    printf("There are %d pieces of meat in the pot.\n", num_pieces);
    while (1) {
        printf("Press enter to eat a piece of meat.\n");
        getchar();
        send(sock, "eat", strlen("eat"), 0);
        memset(buffer, 0, 1024);
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);
    }
    return 0;
}
