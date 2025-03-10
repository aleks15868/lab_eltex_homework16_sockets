#include "server.h"

// Глобальная переменная для хранения адреса подключенного клиента
struct sockaddr_in connected_client;
int is_client_connected = 0; // Флаг: 0 - клиент не подключен, 1 - клиент подключен

/**
 * @brief Функция обработки сервер-клиент через UDP формат
 * 
 * @param arg [in] передача структуры phhread_fd в потоковую функцию
 */
void* communicate_with_client_udp(void* arg) {
    phhread_fd* fd_udp = (phhread_fd*)arg;
    int status;
    sign buffer;
    socklen_t len_client = sizeof(fd_udp->client);

    while (flag) {
        // Чтение данных от клиента
        status = recvfrom(fd_udp->fd_serv, (void *)&buffer, sizeof(buffer), 0, 
                          (struct sockaddr *)&fd_udp->client, &len_client);
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
                            0, (struct sockaddr *)&fd_udp->client, len_client);
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
    flag_udp++;
    pthread_exit(NULL);
}