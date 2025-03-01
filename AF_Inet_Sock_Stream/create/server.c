#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in serv,client;
    int servlen = sizeof(serv);
    char buffer[1024];
    char message[100] = "Привет, клиент!";
    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Настраиваем адрес сокета
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY; // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);
    // strncpy(serv.sun_path, SOCKET_PATH, sizeof(serv.sun_path) - 1);

    bind(server_fd, (struct sockaddr *)&serv, sizeof(serv));

    // Слушаем входящие соединения
    listen(server_fd, 1);
    printf("Сервер слушает на порте %d\n", PORT);

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


    return 0;
}