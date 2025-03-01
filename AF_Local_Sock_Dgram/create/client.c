#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH_SERVER "/tmp/local_socket_dgram4"
#define SOCKET_PATH_CLIENT "/tmp/local_socket_client"

#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    // unlink(SOCKET_PATH_SERVER);
    unlink(SOCKET_PATH_CLIENT);
    
     struct sockaddr_un serv, client;

    //дескриптор сокета
    int fd_serv, fd_client;

    //проверка возвращаемых функций
    int status;
    ssize_t amount_byte;

    //буфер  записи
    char buffer[100];

    fd_serv = socket(AF_LOCAL, SOCK_DGRAM, 0);

    serv.sun_family = AF_LOCAL;
    strcpy(serv.sun_path, SOCKET_PATH_SERVER);

    client.sun_family = AF_LOCAL;
    strcpy(client.sun_path, SOCKET_PATH_CLIENT);

    status = bind(fd_serv, (struct sockaddr *)&client, sizeof(client));
        error_func(status, "bind");

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