#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
    //флаг закрытия
    int flag_while = 0;

    //проверка возвращаемых функций
    int status;
    ssize_t amount_byte;

    //буфер  записи
    int message;
    char buffer[200];

    fd_serv = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

        // Настраиваем адрес сокета
        // Заполнение структуры адреса назначения
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1"); // Принимать соединения на все интерфейсы
    serv.sin_port = htons(PORT);
    
    while(flag_while!=1){
         memset(buffer, 0, sizeof(buffer));

        //указатель на заголовок
        header_udp = (struct head_udp*)buffer;
        //переместимся к данным за заголовком
        mover = buffer + sizeof(header_udp);

        header_udp->source_port = ntohs(7564);
        header_udp->destination_port = ntohs(PORT);
        header_udp->length_message = ntohs(sizeof(header_udp) + sizeof(int));
        header_udp->checksum = 0;
        
        printf("Введите сообщение:");
        scanf("%d", &message);
        *((int*)mover) = message;
        // message=htonl(message);
        // fgets(mover, 100, stdin);
        // mover[strcspn(mover, "\n")]='\0';
        // memcpy(buffer, &header_udp, sizeof(header_udp));
        // memcpy(buffer + sizeof(header_udp), &message, sizeof(message));
        // mover=buffer;
        // printf("\nДо sendto\n");
            // for(int i=0; i<100; i++){
                // printf("%d) %02X\n",i,*(unsigned char *)mover);
                // mover=mover+1;
            // }
        status = sendto(fd_serv, (void *)&buffer, sizeof(struct head_udp) + sizeof(int), 
        0, (struct sockaddr*)&serv, len_sock);
             error_func(status, "sendto");
         //указатель на заголовок III уровня (не забываем о том, что на IV добавляесят заголовок)
        header_udp = (struct head_udp*)(buffer + 20);
        
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