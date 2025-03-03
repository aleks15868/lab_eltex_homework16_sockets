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

    fd_network = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(fd_network, "socket");

        // Настраиваем адрес сокета
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_BROADCAST); // Принимать соединения
    client.sin_port = htons(PORT);

    status = bind(fd_network, (struct sockaddr *)&client, sizeof(client));
        error_func(status, "connect");

    // strncpy(buffer, "Привет сервер!!!",sizeof(buffer)-1);
    
    // amount_byte = send(fd_serv, (void *)&buffer, sizeof(buffer), 0); //отправка сообщение "Привет сервер!!!"
    // error_func(amount_byte, "send");

    // printf("Серверу:%s\n",buffer);
   
    
    status = recv(fd_network, (void *)&buffer, sizeof(buffer), 0); //получение сообщения
        error_func(status, "recv");
    
    printf("От сервера:%s\n",buffer);
    // strncpy(buffer, "exit",sizeof(buffer)-1);

    // amount_byte = send(fd_serv, (void *)&buffer, sizeof(buffer), 0); //отправка сообщение "Привет сервер!!!"
    // error_func(amount_byte, "send");

    // amount_byte = recv(fd_serv, (void *)&buffer, sizeof(buffer), 0); //получение сообщения
    // error_func(amount_byte, "recv");
    // printf("От сервера:%s\n",buffer);
    //  if(strcmp(buffer, "exit:Yes")==0){ // Псевдо проверка на закрытие дескриптера
    //      close(fd_client);
    //     close(fd_serv);
    // }
    shutdown(fd_network, SHUT_RDWR);
    close(fd_network);
    return 0;
}