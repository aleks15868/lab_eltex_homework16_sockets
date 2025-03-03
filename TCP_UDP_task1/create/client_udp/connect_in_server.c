#include "client_udp.h"


int connect_in_server(int* socket_fd, sign* buffer,struct sockaddr_in* sock){
    int status, new_port;
    int index;
    struct timeval timeout;
    timeout.tv_sec = 10;  // 10 секунд
    timeout.tv_usec = 0;
    char notification[4][17] = {
    "Connecting",       // 10 characters + '\0' = 11
    "Trying again",     // 12 characters + '\0' = 13
    "Trying once more", // 16 characters + '\0' = 17 (exceeds the 16-character limit)
    "Control"           // 7 characters + '\0' = 8
};
    status = setsockopt(*socket_fd, SOL_SOCKET, SO_RCVTIMEO, 
    (const char *)&timeout, sizeof(timeout));
        error_func(status, "setsockopt");
    buffer->sign=3;
    status = send(*socket_fd, (void *)buffer, sizeof(sign), 0);///1-----
        error_func(*socket_fd, "recv");
    status = recv(*socket_fd, (void *)buffer, sizeof(sign), 0);///1-----
        error_func(*socket_fd, "recv");
    if(buffer->sign==6){
        perror("Limit");
        return 1;
    }
    shutdown(*socket_fd, SHUT_RDWR);
    close(*socket_fd);
    
    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(*socket_fd, "socket");

    sock->sin_family = AF_INET;
    sock->sin_addr.s_addr = inet_addr("127.0.0.1"); 
    sock->sin_port = htons(buffer->message);
    printf("Port:%d\n",buffer->message);
    status=connect(*socket_fd, (struct sockaddr *)sock, sizeof(*sock));
        error_func(status, "connect");
    status = setsockopt(*socket_fd, SOL_SOCKET, SO_RCVTIMEO, 
    (const char *)&timeout, sizeof(timeout));
        error_func(status, "setsockopt");
    if(buffer->sign==5){
        buffer->sign=3;
        buffer->message=0;
    }
    status = send(*socket_fd, (void *)buffer, sizeof(sign), 0);///2---5 0--
        error_func(status, "send");
    for (index = 0; index < 4; index++)
    {
        printf("%s\n", notification[index]);
        status = recv(*socket_fd, (void *)buffer, sizeof(sign), 0);///3-----
        if (status > 0) {
            if(buffer->sign==5){
                buffer->sign=3;
                buffer->message=0;
            }
       
                    
            if(buffer->sign == 4){
                buffer->sign=0;
                return 0;
            }else{
                 status = send(*socket_fd, (void *)buffer, sizeof(sign), 0);///4-----
                    error_func(status, "send");
            }
        } else if (status == 0) {
            // Соединение закрыто
            printf("Соединение закрыто\n");
        } else {
            // Ошибка или таймаут
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("recv");
                break;
            }
        }
    }
    printf("Не удалось подключится\n");
    return 1;
}