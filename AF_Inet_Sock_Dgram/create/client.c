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
    // unlink(SOCKET_PATH_SERVER);
    
     struct sockaddr_in serv, client;

    //дескриптор сокета
    int fd_serv, fd_client;

    //проверка возвращаемых функций
    int status;
    ssize_t amount_byte;

    //буфер  записи
    char buffer[100];

    fd_serv = socket(AF_INET, SOCK_DGRAM, 0);

        // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на все интерфейсы
    client.sin_port = htons(PORT);

    status = connect(fd_serv, (struct sockaddr *)&serv, sizeof(serv));
        error_func(status, "connect");

    strncpy(buffer, "Привет сервер!!!",sizeof(buffer)-1);
    
    amount_byte = send(fd_serv, (void *)&buffer, sizeof(buffer), 0); //отправка сообщение "Привет сервер!!!"
    error_func(amount_byte, "send");

    printf("Серверу:%s\n",buffer);

    amount_byte = recv(fd_serv, (void *)&buffer, sizeof(buffer), 0); //получение сообщения
    error_func(amount_byte, "recv");

    strncpy(buffer, "exit",sizeof(buffer)-1);

    amount_byte = send(fd_serv, (void *)&buffer, sizeof(buffer), 0); //отправка сообщение "Привет сервер!!!"
    error_func(amount_byte, "send");

    amount_byte = recv(fd_serv, (void *)&buffer, sizeof(buffer), 0); //получение сообщения
    error_func(amount_byte, "recv");
    printf("От сервера:%s\n",buffer);
     if(strcmp(buffer, "exit:Yes")==0){ // Псевдо проверка на закрытие дескриптера
         close(fd_client);
        close(fd_serv);
    }
    return 0;
}