#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd;
    struct sockaddr_in serv;
    char message[100] = "Привет, сервер!";
    char buffer[1024];
    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);

    // Подключаемся к серверу
    connect(server_fd, (struct sockaddr *)&serv, sizeof(serv));

    // Отправляем сообщение серверу
    send(server_fd, message, strlen(message),0);

    printf("Сообщение отправлено серверу: %s\n", message);
    
    ssize_t num_bytes = recv(server_fd, buffer, sizeof(buffer),0);

    buffer[num_bytes] = '\0';
    printf("Получено от сервера: %s\n", buffer);

    // Отправляем сообщение серверу о выходе
    strncpy(message, "exit", sizeof(message) - 1);

    send(server_fd, message, strlen(message),0);

    printf("Сообщение отправлено серверу: %s\n", message);

    num_bytes = recv(server_fd, buffer, sizeof(buffer),0);
    buffer[num_bytes] = '\0';
    printf("Получено от сервера: %s\n", buffer);

    if(strcmp(buffer, "exit:Yes")==0){ // Псевдо проверка на закрытие дескриптера
        close(server_fd);
    }

    return 0;
}