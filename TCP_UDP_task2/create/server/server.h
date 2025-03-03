#ifndef _SERVER_H
#define _SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>  
#include <sys/time.h>  
#include <sys/socket.h>  
#include <unistd.h>      
#include <stdio.h>       
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#define PORT 8081
#define MAIN_PORT_T 8080
#define MAIN_PORT_U 8079
#define TIMEOUT 10
#define LENGTH_CLIENT 2

#define error_func(a, error) do{if(-1 == a){ printf("Error %s\nline:%d\n",error, __LINE__); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)

#define error_no_zero_func(a, error) do{if(0 != a){ printf("Error %s\nline:%d\n",error, __LINE__); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
typedef struct{
    int index_array;
    int fd_serv;
    struct sockaddr_in client;
}phhread_fd;

typedef struct{
    int sign;
    int message;
}sign;

typedef struct{
    int port;
    char status;
    char status_tcp_udp;
}parameters_ports;

extern int flag;
extern int flag_client;
extern pthread_mutex_t mutex;
extern parameters_ports ports_array[LENGTH_CLIENT];

extern struct sockaddr_in serv_tcp,client_tcp; //структура для tcp
extern struct sockaddr_in serv_udp,client_udp; //структура для udp
extern int server_fd_tcp,server_fd_udp;

void handle_signal(int);
void* create_server(void*);

void* communicate_with_client_tcp(void*);
void* communicate_with_client_udp(void*);
int pow3(int);

#endif