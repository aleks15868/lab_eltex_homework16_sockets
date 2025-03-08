#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
int flag_while;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Получен сигнал SIGINT\n");
        flag_while = 0;
    }
}

#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    struct sockaddr_in serv,client;

    //дескриптор сокета
    int fd_sock;

    //флаг закрытия
    flag_while =1;
    //для проверки правильности завершения программы
    int status;

    //дескрипторы клиентов
    int fd_clients;
    int buffer;

    //размер
    socklen_t len_sock = sizeof(client);

    //<==========Создание и настройка сигнала=========================>
    struct sigaction act;

    // Настраиваем обработчик
    // Указываем функцию-обработчик
    act.sa_handler = handle_signal;  
    // Очищаем маску сигналов (не блокируем другие сигналы)
    sigemptyset(&act.sa_mask);       
    // Флаги не используются
    act.sa_flags = 0;               
     

    // Устанавливаем обработчик для SIGINT
    status = sigaction(SIGINT, &act, NULL);
        error_func(status,"when installing a signal processor SIGINT");
    //<==========Создание и настройка сигнала=========================>

    //cоздаем сокет
    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(fd_sock, "socket");

    // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1"); // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);

    status = bind(fd_sock, (struct sockaddr *)&serv, sizeof(serv));
        error_func(status, "bind");
    
    printf("Сервер создан\n");

    while (flag_while)
    {
        status = recvfrom(fd_sock, (void *)&buffer, sizeof(buffer), 0, 
        (struct sockaddr *)&client, &len_sock);
        if (status<0)
        {
        if (errno != EINTR) {
            close(fd_sock);
            error_func(status,"recvfrom");
        } else {
            break;   
        }
        }
             
        printf("От клиента:%d\n",buffer);


        buffer=buffer*buffer;
        printf("Сервер отправил:%d\n",buffer);
        status = sendto(fd_sock, (void *)&buffer, sizeof(buffer), 0, 
        (struct sockaddr *)&client, len_sock);
            error_func(status, "sendto");
    }
    
    close(fd_sock);

    return 0;
}
