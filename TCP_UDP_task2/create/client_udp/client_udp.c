#include "client_udp.h"

int main() {
    int network_socket_fd,status;
    int flag_close;
    struct sockaddr_in network_socket;
    int message;
    sign buffer;
    // Создаем сокет
    network_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(network_socket_fd, "socket");
    flag_close=0;
    // Настраиваем адрес сокета
    network_socket.sin_family = AF_INET;
    network_socket.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    network_socket.sin_port = htons(PORT);

    // Подключаемся к серверу
    status=connect(network_socket_fd, (struct sockaddr *)&network_socket, 
    sizeof(network_socket));
        error_func(status, "connect");

    flag_close=connect_in_server(&network_socket_fd, &buffer,&network_socket);
    while (flag_close!=1)
    {
        printf("Введите сообщение:");
        scanf("%d", &message);
        buffer.message = message;
        status=send(network_socket_fd, (void *)&buffer, sizeof(buffer),0);
            error_func(status, "send");
        printf("Сообщение отправлено серверу: %d\n", message);
        status=recv(network_socket_fd, (void *)&buffer, sizeof(buffer),0);
            error_func(status, "recv");
        printf("От сервера получено (%d)^3:%d\n",message,buffer.message);
        printf("Хотите выйти?(1-Yes/0-No):");
        scanf("%d", &flag_close);
        if(flag_close == 1){
            buffer.sign=1;
            send(network_socket_fd, (void *)&buffer, sizeof(buffer),0);
        }
    }
    close(network_socket_fd);

    return 0;
}