#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>

#define PORT 7777
#define IP "224.0.0.5"

#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    // unlink(SOCKET_PATH_SERVER);
    
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(client);
    //дескриптор сокета
    int fd_network;
    int broadcastEnable=1;
    //проверка возвращаемых функций
    int status;
    ssize_t amount_byte;

    //буфер  записи
    char buffer[100];
    struct ip_mreqn multicust;
    multicust.imr_address.s_addr = INADDR_ANY;
    multicust.imr_multiaddr.s_addr = inet_addr(IP);

    //используем ноль из-за локальности
    multicust.imr_ifindex = 0;

    fd_network = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(fd_network, "socket");
    
    //позволяем сокету делать широковещательную рассылку;
    status = setsockopt(fd_network, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicust, sizeof(multicust));
        error_func(status, "setsockopt");

        // Настраиваем адрес сокета
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(IP); // Принимать соединения
    client.sin_port = htons(PORT);

    status = bind(fd_network, (struct sockaddr *)&client, sizeof(client));
        error_func(status, "connect");

  
    
    status = recv(fd_network, (void *)&buffer, sizeof(buffer), 0); //получение сообщения
        error_func(status, "recv");
    
    printf("От сервера:%s\n",buffer);
    
    shutdown(fd_network, SHUT_RDWR);
    close(fd_network);
    return 0;
}