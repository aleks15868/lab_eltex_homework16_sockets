#include "client_tcp.h"
/**
 * @brief Функция подключается несколько раз к серверу и проверяет, есть ли подключение или нет
 * 
 * @param socket_fd [in] файловый дексриптер
 * @param buffer [in] буфер для отрпавки сообщений серверу
 * @return положение flag для отлкючения цикла в main(1/0) 
 */
int connect_in_server(int socket_fd, sign* buffer){
    int status;
    int index;
    struct timeval timeout;
    timeout.tv_sec = 10;  // 10 секунд
    timeout.tv_usec = 0;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, 
    (const char *)&timeout, sizeof(timeout));
        error_func(status, "setsockopt");
    char notification[4][17] = {
    "Connecting",       // 10 characters + '\0' = 11
    "Trying again",     // 12 characters + '\0' = 13
    "Trying once more", // 16 characters + '\0' = 17 (exceeds the 16-character limit)
    "Control"           // 7 characters + '\0' = 8
};
    buffer->sign =3;
    status = send(socket_fd, (void *)buffer, sizeof(sign), 0);
        error_func(status, "send");
    
    for (index = 0; index < 4; index++)
    {
        printf("%s\n", notification[index]);
        status = recv(socket_fd, (void *)buffer, sizeof(sign), 0);
        if (status > 0) {
            // Данные получены
            if(buffer->sign == 4){
                buffer->sign=0;
                return 0;
            }
        } else if (status == 0) {
            // Соединение закрыто
            printf("Соединение закрыто\n");
            break;
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