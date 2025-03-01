#include "server.h"

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Получен сигнал SIGINT\n");
        flag = 0;
    }
}