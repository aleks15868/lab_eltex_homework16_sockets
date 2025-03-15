#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
int flag_while;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Получен сигнал SIGINT\n");
        flag_while = 0;
    }
}

#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
    struct sockaddr_in serv, client;
    int fd_sock;
    flag_while = 1;
    int status;
    int fd_clients;
    int buffer;
    socklen_t len_sock = sizeof(client);

    struct sigaction act;
    act.sa_handler = handle_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    status = sigaction(SIGINT, &act, NULL);
    error_func(status, "when installing a signal processor SIGINT");

    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    error_func(fd_sock, "socket");

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("224.0.0.2"); // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);

    status = bind(fd_sock, (struct sockaddr *)&serv, sizeof(serv));
    error_func(status, "bind");

    printf("Сервер создан\n");

    while (flag_while) {
        status = recvfrom(fd_sock, (void *)&buffer, sizeof(buffer), 0, 
                          (struct sockaddr *)&client, &len_sock);
        if (status > 0) {
            printf("Что то присылает\n");
        }
        if (status < 0) {
            if (errno != EINTR) {
                close(fd_sock);
                error_func(status, "recvfrom");
            } else {
                break;
            }
        }

        printf("От клиента:%d\n", buffer);

        buffer = buffer * buffer;
        printf("Сервер отправил:%d\n", buffer);
        status = sendto(fd_sock, (void *)&buffer, sizeof(buffer), 0, 
                        (struct sockaddr *)&client, len_sock);
        error_func(status, "sendto");
    }

    close(fd_sock);

    return 0;
}