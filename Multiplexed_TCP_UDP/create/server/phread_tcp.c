#include "server.h"

void* communicate_with_client_tcp(void* arg) {
    int* client_fd_tcp = (int*)arg;
    int status;
    sign buffer;

    while (flag) {
        // Чтение данных от клиента
        status = recv(*client_fd_tcp, (void *)&buffer, sizeof(buffer), 0);
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
        if( buffer.sign !=3){
            status = send(*client_fd_tcp, (void *)&buffer, sizeof(buffer), 0);
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
        }
        

        if (buffer.sign == 2) {
            break;
        }
    }

    flag_tcp++;
    close(*client_fd_tcp);
    pthread_exit(NULL);
}