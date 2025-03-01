#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define PORT 8080


#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    struct sockaddr_in serv,client;

    //дескриптор сокета
    int fd_sock;

    //для проверки правильности завершения программы
    int status;
    ssize_t amount_byte;

    //дескрипторы клиентов
    int fd_clients;
    char buffer[100];

    //размер
    socklen_t len_sock;
    //cоздаем сокет
    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(fd_sock, "socket");

    // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);

    status = bind(fd_sock, (struct sockaddr *)&serv, sizeof(serv));
        error_func(status, "bind");
    
    printf("Сервер создан\n");

    memset(&client,0,sizeof(client));

    len_sock = sizeof(client);

    amount_byte = recvfrom(fd_sock, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &len_sock);
        error_func(amount_byte, "recvfrom");
    printf("От клиента:%s\n",buffer);

    strncpy(buffer, "Привет клиент!!!",sizeof(buffer)-1);

    amount_byte = sendto(fd_sock, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&client, len_sock);
        error_func(amount_byte, "sendto");
    printf("Клиенту:%s\n",buffer);

    amount_byte = recvfrom(fd_sock, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &len_sock);
        error_func(amount_byte, "recvfrom");
    printf("От клиента:%s\n",buffer);

    if(strcmp(buffer, "exit")==0){ // Псевдо проверка на закрытие дескриптера
    strncpy(buffer, "exit:Yes",sizeof(buffer)-1);

    amount_byte = sendto(fd_sock, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&client, len_sock);
        error_func(amount_byte, "sendto");
    printf("Клиенту:%s\n",buffer);
    // Удаляем сокет
    }
    close(fd_sock);

    return 0;
}