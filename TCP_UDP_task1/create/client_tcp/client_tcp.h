#ifndef _CLIENT_TCP_H
#define _CLIENT_TCP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>


/**
 * @brief Структура сообщения
 * 
 */
typedef struct{
    int sign;//номер сигнала(протокол)
    int message;//Сообщение
}sign;

#define PORT 8080

#define error_func(a, error) do{if(-1 == a){ printf("Error %s\nline:%d\n",error, __LINE__); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)

int connect_in_server(int*, sign*,struct sockaddr_in*);
#endif