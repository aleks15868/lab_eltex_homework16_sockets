#include "server.h"

int flag,flag_tcp,flag_udp;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    int server_fd_tcp,server_fd_udp, client_fd_tcp,status, ret;
    flag_tcp = 1;
    flag_udp = 1;
    fd_set fd_sock;
    pthread_t thread_tcp,thread_udp;
    struct timespec tv = {TIMEOUT, 0};
    struct sockaddr_in serv_tcp,client_tcp; //структура для tcp
    struct sockaddr_in serv_udp,client_udp; //структура для udp
    // Создаем переменную для хранения размера
    socklen_t client_len_tcp = sizeof(client_tcp); 
    //переменная по работе с сигналами
    sigset_t sigmask;
    flag=1;
    //<==========Создание и настройка сигнала=========================>
    struct sigaction act;

    // Настраиваем обработчик
    // Указываем функцию-обработчик
    act.sa_handler = handle_signal;  
    // Очищаем маску сигналов (не блокируем другие сигналы)
    sigemptyset(&act.sa_mask);       
    // Флаги не используются
    act.sa_flags = 0;               
     

    // Устанавливаем обработчик для SIGINT
    status = sigaction(SIGINT, &act, NULL);
        error_func(server_fd_tcp,"when installing a signal processor SIGINT");
    //<==========Создание и настройка сигнала=========================>

    int servlen_tcp = sizeof(serv_tcp);
    int buffer;
    // Создаем сокет
    server_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
        error_func(server_fd_tcp,"socket_tcp");
    server_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(server_fd_udp,"socket_udp");

    // Настраиваем адрес  и поключаем TCP сокет 
    serv_tcp.sin_family = AF_INET;
    serv_tcp.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_tcp.sin_port = htons(PORT_TCP);

    status = bind(server_fd_tcp, (struct sockaddr *)&serv_tcp, sizeof(serv_tcp));
        error_func(status, "bind_tcp");

    //очередь для подключения клиентов
    status = listen(server_fd_tcp, 1);
        error_func(status,  "listen");

     // Настраиваем адрес TCP сокета
    serv_udp.sin_family = AF_INET;
    serv_udp.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_udp.sin_port = htons(PORT_UDP);

    status = bind(server_fd_udp, (struct sockaddr *)&serv_udp, sizeof(serv_udp));
        error_func(status,  "bind_udp");
    // strncpy(serv.sun_path, SOCKET_PATH, sizeof(serv.sun_path) - 1);
    // обнуляем структуры
    FD_ZERO(&fd_sock);

    // Будем мониторить события о входящих данных для server_fd_tcp и server_fd_udp
    FD_SET(server_fd_tcp, &fd_sock);
    FD_SET(server_fd_udp, &fd_sock);
    // Будем мониторить события об исходящих данных для server_fd_tcp
    
    // Определим сокет с максимальным числовым значением (select требует это значение)
    int largest_sock = server_fd_tcp > server_fd_udp ? server_fd_tcp : server_fd_udp;

    printf("Сервер создан и готов к работе\n");
    while (flag)
    {
        // Инициализируем набор файловых дескрипторов перед каждым вызовом select
        FD_ZERO(&fd_sock);
        FD_SET(server_fd_tcp, &fd_sock);
        FD_SET(server_fd_udp, &fd_sock);
        // Вызываем pselect
        ret = pselect(largest_sock + 1, &fd_sock, NULL, NULL, &tv, &sigmask);
        if (ret == -1) {
            if (errno == EINTR) {
                // Системный вызов был прерван сигналом (например, SIGINT)
                continue; // Перезапускаем select
            }else{
                error_func(ret,"select");
            }
        }
        // Проверяем успешность вызова
        // событие на tcp сервер
        if ( FD_ISSET(server_fd_tcp, &fd_sock) && flag_tcp>0)
        {
            client_fd_tcp = accept(server_fd_tcp, (struct sockaddr *)&client_tcp, 
            &client_len_tcp);
                error_func(client_fd_tcp,"accept_tcp");
            status=pthread_create(&thread_tcp, NULL, communicate_with_client_tcp, 
            (void*)&client_fd_tcp);
                error_no_zero_func(status,"pthread_create_tcp");
            flag_tcp--;
        };
        // Проверяем успешность вызова
        // событие на udp сервер
        if ( FD_ISSET(server_fd_udp, &fd_sock) && flag_udp>0)
        {
            phhread_fd ph_udp = {server_fd_udp, client_udp};
            status=pthread_create(&thread_udp, NULL, communicate_with_client_udp, 
            (void*)&ph_udp);
                error_no_zero_func(status,"pthread_create_udp");
            flag_udp--;
        };

    }
    // Ожидаем завершения потока
    pthread_join(thread_tcp, NULL);
    pthread_join(thread_udp, NULL);
    // pthread_join(thread_output, NULL);
    // Закрываем соединения
    close(client_fd_tcp);
    close(server_fd_tcp);
    return 0;
}