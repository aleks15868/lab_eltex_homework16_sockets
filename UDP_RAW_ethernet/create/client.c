#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

struct head_udp {
    short source_port;
    short destination_port;
    short length_message;
    short checksum;
};
#define error_func(a, error) do{if(-1 == a){ printf("line:%d %s\n", __LINE__, error); \
                                        perror("error"); exit(EXIT_FAILURE);}} while(0)

unsigned short my_checksum(struct ip *, int);

int main() {
    struct sockaddr_ll serv, client;
    int fd_serv;
    char *mover;
    socklen_t len_sock = sizeof(serv);
    struct head_udp *header_udp;
    struct ip *head_ip;
    struct ether_header *head_ethernet;
    int flag_while = 0;
    int status;
    int message;
    char buffer[200];
    int opt = 1;

    fd_serv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    error_func(fd_serv, "socket");

    const char *interface_name = "lo";
    uint8_t dest_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t src_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    serv.sll_family = AF_PACKET;
    serv.sll_protocol = htons(ETH_P_ALL);
    serv.sll_ifindex = if_nametoindex(interface_name);
    if (0 == serv.sll_ifindex) {
        printf("Неправильный интерфейс\n");
        perror("error");
        exit(EXIT_FAILURE);
    }
    serv.sll_pkttype = PACKET_HOST;
    serv.sll_halen = ETH_ALEN;
    memcpy(serv.sll_addr, dest_mac, ETH_ALEN);

    while (flag_while != 1) {
        memset(buffer, 0, sizeof(buffer));
        head_ethernet = (struct ether_header *)buffer;
        memcpy(head_ethernet->ether_dhost, dest_mac, ETH_ALEN);
        memcpy(head_ethernet->ether_shost, src_mac, ETH_ALEN);
        head_ethernet->ether_type = htons(ETH_P_IP);

        mover = buffer + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct head_udp);
        head_ip = (struct ip *)(buffer + sizeof(struct ether_header));

        head_ip->ip_hl = 5;
        head_ip->ip_v = 4;
        head_ip->ip_tos = 1;
        head_ip->ip_id = htons(3);
        head_ip->ip_off = 0;
        head_ip->ip_ttl = 10;
        head_ip->ip_p = IPPROTO_UDP;
        head_ip->ip_src.s_addr = inet_addr("127.0.0.3");
        head_ip->ip_dst.s_addr = inet_addr("127.0.0.3");
        head_ip->ip_len = htons(sizeof(struct ip) + sizeof(struct head_udp) + sizeof(int));
        head_ip->ip_sum = my_checksum(head_ip, sizeof(struct ip));

        header_udp = (struct head_udp *)(buffer + sizeof(struct ether_header) + sizeof(struct ip));
        header_udp->source_port = ntohs(7564);
        header_udp->destination_port = ntohs(PORT);
        header_udp->length_message = htons(sizeof(struct head_udp) + sizeof(int));
        header_udp->checksum = 0;

        printf("Введите сообщение:");
        scanf("%d", &message);
        *((int *)mover) = message;

        status = sendto(fd_serv, (void *)buffer, sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct head_udp) + sizeof(int), 0, (struct sockaddr *)&serv, len_sock);
        error_func(status, "sendto");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            status = recvfrom(fd_serv, (void *)buffer, sizeof(buffer), 0, (struct sockaddr *)&serv, &len_sock);
            error_func(status, "recvfrom");

            if (status > 0) {
                struct ether_header *eth_header = (struct ether_header *)buffer;
                struct ip *ip_header = (struct ip *)(buffer + sizeof(struct ether_header));
                struct head_udp *udp_header = (struct head_udp *)(buffer + sizeof(struct ether_header) + sizeof(struct ip));
                char *data = (char *)(udp_header + 1);

                if (ntohs(udp_header->destination_port) == 7564) {
                    printf("Сообщение: %d\n", *(int *)data);
                    break;
                }
            } else if (status == 0) {
                break;
            }
        }
        printf("Хотите выйти?(1-Yes/0-No):");
        scanf("%d", &flag_while);
    }

    close(fd_serv);
    return 0;
}

unsigned short my_checksum(struct ip *point_header, int size_head) {
    int all_sum = 0;
    int dop_sum;
    unsigned short checksum;
    unsigned short *ptr = (unsigned short *)point_header;

    for (int i = 0; i < size_head / 2; i++) {
        all_sum += *ptr;
        ptr++;
    }

    if (size_head % 2 != 0) {
        all_sum += *((unsigned char *)ptr);
    }

    dop_sum = all_sum >> 16;
    while (dop_sum > 0) {
        all_sum = (all_sum & 0xFFFF) + dop_sum;
        dop_sum = all_sum >> 16;
    }
    checksum = (unsigned short)~all_sum;

    return checksum;
}