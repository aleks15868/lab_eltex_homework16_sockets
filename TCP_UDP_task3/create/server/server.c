#include "server.h"

struct sockaddr_in serv_tcp,client_tcp; //структура для tcp
struct sockaddr_in serv_udp,client_udp; //структура для udp
int server_fd_tcp,server_fd_udp;
int flag,flag_client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
stack_server stack_array[LENGTH_STACK];

int main() {
    int status, ret, index_port,index_thread,client_fd_tcp,cout_stack;
    int serving_server_fd,port_message, cout_thread;
    int thread_args[LENGTH_CLIENT];
    flag_client = LENGTH_CLIENT;
    fd_set fd_sock;
    pthread_t thread_input[LENGTH_CLIENT];
    struct timespec tv = {TIMEOUT, 0};
    pthread_t ph_tcp_udp[LENGTH_CLIENT];
    // Создаем переменную для хранения размера
    socklen_t client_len_tcp = sizeof(client_tcp); 
    socklen_t client_len_udp = sizeof(client_udp); 
    //переменная по работе с сигналами
    sigset_t sigmask;
    flag=1;
    for (index_port = 0; index_port < LENGTH_CLIENT; index_port++)
    {
        stack_array[index_port].fd_client=0;
        stack_array[index_port].status=0;
        stack_array[index_port].status_tcp_udp = 0;
        stack_array[index_port].id= 0;
    }
    
    cout_thread=0;
    cout_stack=0;
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
    sign buffer;
    // Создаем сокет
    server_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
        error_func(server_fd_tcp,"socket_tcp");
    server_fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
        error_func(server_fd_udp,"socket_udp");

    // Настраиваем адрес  и поключаем TCP сокет 
    serv_tcp.sin_family = AF_INET;
    serv_tcp.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_tcp.sin_port = htons(MAIN_PORT_T);

    status = bind(server_fd_tcp, (struct sockaddr *)&serv_tcp, sizeof(serv_tcp));
        error_func(status, "bind_tcp");

    //очередь для подключения клиентов
    status = listen(server_fd_tcp, 1);
        error_func(status,  "listen");

     // Настраиваем адрес TCP сокета
    serv_udp.sin_family = AF_INET;
    serv_udp.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_udp.sin_port = htons(MAIN_PORT_U);

    status = bind(server_fd_udp, (struct sockaddr *)&serv_udp, sizeof(serv_udp));
        error_func(status,  "bind_udp");

    // обнуляем структуры
    FD_ZERO(&fd_sock);

    // Будем мониторить события о входящих данных для server_fd_tcp и server_fd_udp
    FD_SET(server_fd_tcp, &fd_sock);
    FD_SET(server_fd_udp, &fd_sock);
    // Будем мониторить события об исходящих данных для server_fd_tcp
    // Инициализация маски сигналов для pselect
    // sigemptyset(&sigmask);
    // sigaddset(&sigmask, SIGINT);
    // Определим сокет с максимальным числовым значением (select требует это значение)
    int largest_sock = server_fd_tcp > server_fd_udp ? server_fd_tcp : server_fd_udp;
    for (int i = 0; i < LENGTH_CLIENT; i++) {
        thread_args[i] = i;
        status = pthread_create(&thread_input[i], NULL, create_server, (void*)&thread_args[i]);
            error_func(status,  "pthread_create");
    }
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
        if ( FD_ISSET(server_fd_tcp, &fd_sock))
        {
            //обрабатываем общения данных с потоками
            if(flag_client>0){
            pthread_mutex_lock(&mutex); 

            client_fd_tcp = accept(server_fd_tcp, (struct sockaddr *)&client_tcp, 
            &client_len_tcp);
                error_func(client_fd_tcp,"accept_tcp");

            printf("Новый клиент присоединился к серверу tcp\n");

            //выбираем свободный порт
            stack_array[cout_stack].status = 1;
            stack_array[cout_stack].status_tcp_udp = 1;
            stack_array[cout_stack].fd_client=client_fd_tcp;

            cout_stack++;
            flag_client--;
            pthread_mutex_unlock(&mutex); 
            }
            else{
                pthread_mutex_lock(&mutex); 

                client_fd_tcp = accept(server_fd_tcp, (struct sockaddr *)&client_tcp, 
                &client_len_tcp);
                    error_func(client_fd_tcp,"accept_tcp");
                buffer.sign=6;
                status = send(client_fd_tcp, (void *)&buffer, sizeof(buffer), 0);
                error_func(status,"send_tcp"); ///1-----
                pthread_mutex_unlock(&mutex); 
            }
            
        };
        // Проверяем успешность вызова
        // событие на udp сервер
        if ( FD_ISSET(server_fd_udp, &fd_sock))
        {
            if(flag_client>0){
                pthread_mutex_lock(&mutex); 
                recvfrom(server_fd_udp, (void *)&buffer, sizeof(buffer), 0, 
                (void *)&client_udp,&client_len_udp);

                printf("Новый клиент присоединился к серверу udp\n");
                
                //выбираем свободный порт
                stack_array[cout_stack].status = 1;
                stack_array[cout_stack].status_tcp_udp = 2;
                
                cout_stack++;
                flag_client--;
                
                pthread_mutex_unlock(&mutex);
            }
            else{
                pthread_mutex_lock(&mutex); 
                recvfrom(server_fd_udp, (void *)&buffer, sizeof(buffer), 0, 
                (void *)&client_udp,&client_len_udp);
                buffer.sign=6;
                sendto(server_fd_udp, (void *)&buffer, sizeof(buffer), 0, 
                (void *)&client_udp,client_len_udp);
                pthread_mutex_unlock(&mutex); 
            }
             
        };
        pthread_mutex_lock(&mutex);
        while (stack_array[0].status==2)
        {
            for (int i_stack = 0; i_stack < LENGTH_STACK - 1; i_stack++) {
                stack_array[i_stack] = stack_array[i_stack + 1];
            }
            stack_array[LENGTH_STACK-1].fd_client = 0; // Затираем последний элемент
            stack_array[LENGTH_STACK-1].status = 0;
            stack_array[LENGTH_STACK-1].status_tcp_udp = 0;
            stack_array[LENGTH_STACK-1].id= 0;
            cout_stack--;
        }
        pthread_mutex_unlock(&mutex); 
        

    }
    // Ожидаем завершения потока
    for (int index_port = 0; index_port < LENGTH_CLIENT; index_port++)
    {
        pthread_join(thread_input[index_port], NULL);
    }
       // Закрываем соединения
    close(client_fd_tcp);
    close(server_fd_tcp);
    close(server_fd_udp);
    close(serving_server_fd);
    return 0;
}