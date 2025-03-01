#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>



#define SOCKET_PATH "/tmp/local_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un serv, client;
    char buffer[1024];
    char message[100] = "Привет, клиент!";
    // Создаем сокет
    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    // Настраиваем адрес сокета
    memset(&serv, 0, sizeof(struct sockaddr_un));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SOCKET_PATH, sizeof(serv.sun_path) - 1);

    bind(server_fd, (struct sockaddr *)&serv, sizeof(struct sockaddr_un));

    // Слушаем входящие соединения
    listen(server_fd, 1);
    printf("Сервер слушает на %s\n", SOCKET_PATH);

    socklen_t client_len = sizeof(client); // Создаем переменную для хранения размера
    // Принимаем соединение от клиента
    client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
    
    // Читаем данные от клиента
    ssize_t num_bytes = recv(client_fd, buffer, sizeof(buffer),0);

    buffer[num_bytes] = '\0';
    printf("Получено от клиента: %s\n", buffer);
     // Отправляем сообщение клиенту
    send(client_fd, message, strlen(message),0);

    num_bytes = recv(client_fd, buffer, sizeof(buffer),0);

    buffer[num_bytes] = '\0';
    printf("Получено от клиента: %s\n", buffer);
    // Отправляем сообщение клиенту, что все хорошо
    strncpy(message, "exit:Yes", sizeof(message) - 1);
    send(client_fd, message, strlen(message),0);
    // Закрываем соединения
    close(client_fd);
    close(server_fd);

    // Удаляем сокет
    unlink(SOCKET_PATH);

    return 0;
}