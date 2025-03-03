#include "server.h"

void* create_server(void* arg) 
{
    int index = *(int*)arg;
    int serving_server_fd,status,client_fd_tcp;
    sign buffer;
    struct sockaddr_in connected_client;
    struct sockaddr_in client;
    socklen_t len_client = sizeof(client);
    int is_client_connected = 0; // Флаг: 0 - клиент не подключен, 1 - клиент подключен
    while (flag)
    {
        if(ports_array[index].status_tcp_udp == 1)
        {
            //обрабатывающий сокет
            serving_server_fd = socket(AF_INET, SOCK_STREAM, 0);
                error_func(serving_server_fd, "socket_tcp");

            //обрабатывающий сервер    
            serv_tcp.sin_family = AF_INET;
            serv_tcp.sin_port = htons(ports_array[index].port);
            printf("Порт %d\n",ports_array[index].port);
            serv_tcp.sin_addr.s_addr = inet_addr("127.0.0.1");

            // новый сервер, который будет обслуживать клиента
            status = bind(serving_server_fd, (struct sockaddr *)&serv_tcp, 
            sizeof(serv_tcp));
                error_func(status, "bind_tcp");

            //очередь для подключения клиентов
            status = listen(serving_server_fd, 1);
                error_func(status, "listen_tcp"); 

            socklen_t len = sizeof(client_tcp);
            client_fd_tcp = accept(serving_server_fd, (struct sockaddr *)&client_tcp, 
            &len); 

            while (flag) 
            {
                // Чтение данных от клиента
                status = recv(client_fd_tcp, (void *)&buffer, sizeof(buffer), 0); ///2--4---
                if (status == 0) {
                    printf("Клиент закрыл соединение (recv)\n");
                    break;
                } else if (status < 0) {
                    perror("recv_tcp");
                    break;
                }

                printf("Получено от клиента по TCP:%d\n",buffer.message);

                // Обработка данных
                if (buffer.sign == 1) {
                    buffer.sign = 2;
                }
                if (buffer.sign == 3) {
                    buffer.sign = 4;
                }
                buffer.message = pow3(buffer.message);

                // Отправка данных клиенту
                status = send(client_fd_tcp, (void *)&buffer, sizeof(buffer), 0);///3-----
                if (status == 0) {
                    printf("Клиент закрыл соединение (send_tcp)\n");
                    break;
                } else if (status < 0) {
                    if (errno == EPIPE) {
                        printf("Соединение с клиентом разорвано (EPIPE)\n");
                    } else if (errno == ECONNRESET) {
                        printf("Клиент сбросил соединение (ECONNRESET)\n");
                    } else {
                        perror("send_tcp");
                    }
                    break;
                }
                

                if (buffer.sign == 2) {
                    break;
                }
            }
            close(serving_server_fd);
            pthread_mutex_lock(&mutex);
            flag_client++;
            ports_array[index].status_tcp_udp = 0;
            ports_array[index].status = 0;
            pthread_mutex_unlock(&mutex); 
        }

         if(ports_array[index].status_tcp_udp == 2)
        {
            //обрабатывающий сокет
            serving_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
                error_func(serving_server_fd, "socket_udp");
            
            //обрабатывающий сервер    
            serv_udp.sin_family = AF_INET;
            serv_udp.sin_addr.s_addr = inet_addr("127.0.0.1");
            serv_udp.sin_port = htons(ports_array[index].port);
            printf("Порт %d\n",ports_array[index].port);

             // новый сервер, который будет обслуживать клиента
            status = bind(serving_server_fd, (struct sockaddr *)&serv_udp, 
                sizeof(serv_udp));
                    error_func(status, "bind_udp");
            
            while (flag) {
                // Чтение данных от клиента
                status = recvfrom(serving_server_fd, (void *)&buffer, sizeof(buffer), 0, 
                                (struct sockaddr *)&client, &len_client);
                if (status < 0) {
                    perror("recv_udp");
                    break;
                }

                // Проверка, подключен ли уже клиент
                if (is_client_connected) {
                    // Сравниваем адрес нового клиента с адресом подключенного
                    if (memcmp(&client, &connected_client, len_client) != 0) {
                        continue; // Пропускаем обработку данных от нового клиента
                    }
                } else {
                    // Подключаем первого клиента
                    memcpy(&connected_client, &client, len_client);
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
                    status = sendto(serving_server_fd, (void *)&buffer, sizeof(buffer), 
                                    0, (struct sockaddr *)&client, len_client);
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
            close(serving_server_fd);
            pthread_mutex_lock(&mutex);
            flag_client++;
            ports_array[index].status_tcp_udp = 0;
            ports_array[index].status = 0;
            pthread_mutex_unlock(&mutex); 
        }
        
    }
    pthread_exit(NULL);
}
    
    

//             //новый сервер, который будет обслуживать клиента
//             status = bind(serving_server_fd, (struct sockaddr *)&serv_tcp, 
//             sizeof(serv_tcp));
//                 error_func(status, "bind_tcp");

//             //очередь для подключения клиентов
//             status = listen(serving_server_fd, 1);
//                 error_func(status, "listen_tcp");
                      
//             ph_tcp_udp[index_thread].fd_serv = serving_server_fd;
//             ph_tcp_udp[index_thread].client=client_tcp;
//             ph_tcp_udp[index_thread].index_array = index_thread;

    // struct sockaddr_in connected_client;
    // int is_client_connected = 0; // Флаг: 0 - клиент не подключен, 1 - клиент подключен
    // phhread_fd* fd_udp = (phhread_fd*)arg;
    // struct sockaddr_in client;
    // int status;
    // sign buffer;
    // socklen_t len_client = sizeof(client);

    // while (flag) {
    //     // Чтение данных от клиента
    //     status = recvfrom(fd_udp->fd_serv, (void *)&buffer, sizeof(buffer), 0, 
    //                       (struct sockaddr *)&client, &len_client);
    //     if (status < 0) {
    //         perror("recv_udp");
    //         break;
    //     }

    //     // Проверка, подключен ли уже клиент
    //     if (is_client_connected) {
    //         // Сравниваем адрес нового клиента с адресом подключенного
    //         if (memcmp(&fd_udp->client, &connected_client, len_client) != 0) {
    //             continue; // Пропускаем обработку данных от нового клиента
    //         }
    //     } else {
    //         // Подключаем первого клиента
    //         memcpy(&connected_client, &fd_udp->client, len_client);
    //         is_client_connected = 1;
    //         printf("Клиент подключен.\n");
    //     }

    //     printf("Получено от клиента по UDP:%d\n", buffer.message);

    //     // Обработка данных
    //     if (buffer.sign == 1) {
    //         buffer.sign = 2;
    //     }
    //     if (buffer.sign == 3) {
    //         buffer.sign = 4;
    //     }
    //     buffer.message = pow3(buffer.message);

    //     // Отправка данных клиенту
    //     if (buffer.sign != 3) {
    //         status = sendto(fd_udp->fd_serv, (void *)&buffer, sizeof(buffer), 
    //                         0, (struct sockaddr *)&client, len_client);
    //         if (status < 0) {
    //             perror("send_udp");
    //             break;
    //         }
    //     }

    //     // Завершение работы, если получен сигнал
    //     if (buffer.sign == 2) {
    //         break;
    //     }
    // }

    // // Сброс подключения клиента
    // is_client_connected = 0;
    // pthread_mutex_lock(&mutex); 
    //     flag_client++;
    //     ports_array[fd_udp->index_array].status = 0;
    // pthread_mutex_unlock(&mutex); 
    // close(fd_udp->fd_serv);
