#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/local_socket"

int main() {
    int server_fd;
    struct sockaddr_un serv;
    char message[100] = "Привет, сервер!";
    char buffer[1024];
    // Создаем сокет
    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    // Настраиваем адрес сокета
    memset(&serv, 0, sizeof(struct sockaddr_un));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SOCKET_PATH, sizeof(serv.sun_path) - 1);

    // Подключаемся к серверу
    connect(server_fd, (struct sockaddr *)&serv, sizeof(struct sockaddr_un));

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