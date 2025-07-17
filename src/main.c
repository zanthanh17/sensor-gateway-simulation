#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "log.h"
#include <pthread.h>
#include "shared_data.h"
#include "connection_manager.h"
#include "data_manager.h"
#include "storage_manager.h"
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    // Tạo thư mục logs/ và db/ nếu chưa có
    mkdir("logs", 0777);
    mkdir("db", 0777);
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    if (port <= 0) {
        printf("Invalid port number!\n");
        return 1;
    }
    create_log_fifo(LOG_FIFO);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        return EXIT_FAILURE;
    }
    if (pid == 0)
    {
        execl("./log_process", "log_process", NULL);
        perror("execl failed");
        return EXIT_FAILURE;
    }

    // Khởi tạo shared data
    shared_data_t shared_data;
    shared_data_init(&shared_data);

    // Chuẩn bị args cho connection manager
    connection_manager_args_t conn_args;
    conn_args.shared_data = &shared_data;
    conn_args.port = port;

    // Tạo 3 thread
    pthread_t connection_thread, data_thread, storage_thread;
    pthread_create(&connection_thread, NULL, connection_manager_thread, &conn_args);
    pthread_create(&data_thread, NULL, data_manager_thread, &shared_data);
    pthread_create(&storage_thread, NULL, storage_manager_thread, &shared_data);

    pthread_join(connection_thread, NULL);
    pthread_join(data_thread, NULL);
    pthread_join(storage_thread, NULL);

    wait(NULL); // chờ log process

    return 0;
}