#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/udp.h>


#define PORT 8080

struct head_udp {
    short source_port;
    short destination_port;
    short length_message;
    short checksum;
};
#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)
int main() {
     // Удаляем сокет, если он уже существует
    // unlink(SOCKET_PATH_SERVER);
    
    struct sockaddr_in serv, client;

    //дескриптор сокета
    int fd_serv, fd_client,cout;
    char *mover;
    //размер
    socklen_t len_sock = sizeof(serv);

    //заголовок udp с данными
    struct head_udp *header_udp;

    //заголовок ip
    struct ip *head_ip;

    //флаг закрытия
    int flag_while = 0;

    //проверка возвращаемых функций
    int status;
    ssize_t amount_byte;

    //буфер  записи
    int message;
    char buffer[200];
    //включенная опция
    int opt = 1;

    fd_serv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    status = setsockopt(fd_serv, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
        error_func(status,"setsockopt");
        // Настраиваем адрес сокета
        // Заполнение структуры адреса назначения
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1"); // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);
    
    while(flag_while!=1){
        memset(buffer, 0, sizeof(buffer));

        // printf("Отклоенение ip %ld %p %p\n", sizeof(struct ip), header_udp,buffer);
        //переместимся к данным за заголовком
        mover = buffer + sizeof(struct head_udp)+sizeof(struct ip);
        head_ip = (struct ip*)buffer;

        //заполняем ip заголовок
        head_ip->ip_hl = 5;
        head_ip->ip_v = 4;
        head_ip->ip_tos = 1;
        head_ip->ip_id = htons(1);
        head_ip->ip_off = 0;
        head_ip->ip_ttl = 10;
        head_ip->ip_p = IPPROTO_UDP;
        head_ip->ip_src.s_addr = inet_addr("127.0.0.1");
        head_ip->ip_dst.s_addr = inet_addr("127.0.0.1");

        //указатель на заголовок
        header_udp = (struct head_udp*)(buffer+sizeof(struct ip));

        header_udp->source_port = ntohs(7564);
        header_udp->destination_port = ntohs(PORT);
        header_udp->length_message = ntohs(sizeof(header_udp) + sizeof(int));
        header_udp->checksum = 0;

        printf("Введите сообщение:");
        scanf("%d", &message);
        *((int*)mover) = message;

        status = sendto(fd_serv, (void *)&buffer, sizeof(struct head_udp)+sizeof(struct ip) + sizeof(int), 
        0, (struct sockaddr*)&serv, len_sock);
             error_func(status, "sendto");
         //указатель на заголовок III уровня (не забываем о том, что на IV добавляесят заголовок)
        
        while (1) {
            memset(buffer, 0, sizeof(buffer));
             
            status = recvfrom(fd_serv, (void *)&buffer, sizeof(buffer), 0,
                             (struct sockaddr*)&serv, &len_sock);
            error_func(status, "recvfrom");

            if (status > 0) {
                mover = (char*)(header_udp + 1); // Перемещаемся к данным после UDP заголовка

                if (ntohs(header_udp->destination_port) == 7564) {
                    printf("Сообщение: %d\n", *(int*)mover);
                    break;
                }
            } else if (status == 0) {
                break;
            }
        }
        printf("Хотите выйти?(1-Yes/0-No):");
        scanf("%d", &flag_while);
    }
    
    
    return 0;
}
