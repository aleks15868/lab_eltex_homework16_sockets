#include "server.h"

void* communicate_with_client_tcp(void* arg) {
    phhread_fd* fd_tcp = (phhread_fd*)arg;
    int status, client_fd_tcp;
    sign buffer;
    socklen_t len = sizeof(fd_tcp->client);
    client_fd_tcp = accept(fd_tcp->fd_serv, (struct sockaddr *)&fd_tcp->client, 
    &len);
        error_func(client_fd_tcp,"accept_tcp");
    while (flag) {
        // Чтение данных от клиента
        status = recv(client_fd_tcp, (void *)&buffer, sizeof(buffer), 0); ///2--4---
        if (status == 0) {
            printf("Клиент закрыл соединение (recv)\n");
            break;
        } else if (status < 0) {
            perror("recv_tcp");
            break;
        }

        printf("Получено от клиента по TCP:%d\n",buffer.message);

        // Обработка данных
        if (buffer.sign == 1) {
            buffer.sign = 2;
        }
        if (buffer.sign == 3) {
            buffer.sign = 4;
        }
        buffer.message = pow3(buffer.message);

        // Отправка данных клиенту
        status = send(client_fd_tcp, (void *)&buffer, sizeof(buffer), 0);///3-----
        if (status == 0) {
            printf("Клиент закрыл соединение (send_tcp)\n");
            break;
        } else if (status < 0) {
            if (errno == EPIPE) {
                printf("Соединение с клиентом разорвано (EPIPE)\n");
            } else if (errno == ECONNRESET) {
                printf("Клиент сбросил соединение (ECONNRESET)\n");
            } else {
                perror("send_tcp");
            }
            break;
        }
        

        if (buffer.sign == 2) {
            break;
        }
    }
    pthread_mutex_lock(&mutex); 
        flag_client++;
        ports_array[fd_tcp->index_array].status = 0;
        
    pthread_mutex_unlock(&mutex); 
    printf("Сессия закрылась\n");
    close(fd_tcp->fd_serv);
    pthread_exit(NULL);
}