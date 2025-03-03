#include "server.h"



void* communicate_with_client_udp(void* arg) {
    struct sockaddr_in connected_client;
    int is_client_connected = 0; // Флаг: 0 - клиент не подключен, 1 - клиент подключен
    phhread_fd* fd_udp = (phhread_fd*)arg;
    struct sockaddr_in client;
    int status;
    sign buffer;
    socklen_t len_client = sizeof(client);

    while (flag) {
        // Чтение данных от клиента
        status = recvfrom(fd_udp->fd_serv, (void *)&buffer, sizeof(buffer), 0, 
                          (struct sockaddr *)&client, &len_client);
        if (status < 0) {
            perror("recv_udp");
            break;
        }

        // Проверка, подключен ли уже клиент
        if (is_client_connected) {
            // Сравниваем адрес нового клиента с адресом подключенного
            if (memcmp(&fd_udp->client, &connected_client, len_client) != 0) {
                continue; // Пропускаем обработку данных от нового клиента
            }
        } else {
            // Подключаем первого клиента
            memcpy(&connected_client, &fd_udp->client, len_client);
            is_client_connected = 1;
            printf("Клиент подключен.\n");
        }

        printf("Получено от клиента по UDP:%d\n", buffer.message);

        // Обработка данных
        if (buffer.sign == 1) {
            buffer.sign = 2;
        }
        if (buffer.sign == 3) {
            buffer.sign = 4;
        }
        buffer.message = pow3(buffer.message);

        // Отправка данных клиенту
        if (buffer.sign != 3) {
            status = sendto(fd_udp->fd_serv, (void *)&buffer, sizeof(buffer), 
                            0, (struct sockaddr *)&client, len_client);
            if (status < 0) {
                perror("send_udp");
                break;
            }
        }

        // Завершение работы, если получен сигнал
        if (buffer.sign == 2) {
            break;
        }
    }

    // Сброс подключения клиента
    is_client_connected = 0;
    pthread_mutex_lock(&mutex); 
        flag_client++;
        ports_array[fd_udp->index_array].status = 0;
    pthread_mutex_unlock(&mutex); 
    close(fd_udp->fd_serv);
    pthread_exit(NULL);
}