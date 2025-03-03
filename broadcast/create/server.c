#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define PORT 7777


#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    struct sockaddr_in serv;
    int broadcastEnable=1;
    //дескриптор сокета
    int fd_sock;

    //для проверки правильности завершения программы
    int status;
    ssize_t amount_byte;

    //дескрипторы клиентов
    int fd_clients;
    char buffer[100];

    //размер
    socklen_t addr_len_serv = sizeof(serv);
    //cоздаем сокет
    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(fd_sock, "socket");

    int ret=setsockopt(fd_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, 
    sizeof(broadcastEnable));
        error_func(ret, "socket");

    // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_BROADCAST);// Принимать соединения на широковещательный аддресс
    serv.sin_port = htons(PORT);
        
    printf("Сервер создан\n");

    strncpy(buffer, "Привет клиент!!!",sizeof(buffer)-1);
    buffer[sizeof(buffer) - 1] = '\0';

    status = sendto(fd_sock, (void *)&buffer, sizeof(buffer), 0, 
    (struct sockaddr *)&serv, addr_len_serv);
        error_func(status, "recvfrom");

    close(fd_sock);

    return 0;
}