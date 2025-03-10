#include "server.h"
/**
 * @brief Функция для перехвата сигнала SIGINT
 * 
 * @param sig [in] Номер сигнала
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Получен сигнал SIGINT\n");
        flag = 0;
    }
}