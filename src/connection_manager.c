#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "connection_manager.h"
#include "log.h"

#define BUF_SIZE 256

void *connection_manager_thread(void *arg) {
    connection_manager_args_t *args = (connection_manager_args_t *)arg;
    shared_data_t *sd = args->shared_data;
    int port = args->port;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        pthread_exit(NULL);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        pthread_exit(NULL);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        pthread_exit(NULL);
    }
    printf("[Connection Manager] Listening on port %d...\n", port);
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        printf("[Connection Manager] New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        char logbuf[128];
        snprintf(logbuf, sizeof(logbuf), "Sensor node with %d has opened a new connection", client_addr.sin_port);
        write_log_event(logbuf);
        // Đọc 1 dòng dữ liệu sensor
        char buf[BUF_SIZE];
        ssize_t n = read(client_fd, buf, BUF_SIZE - 1);
        if (n > 0) {
            buf[n] = '\0';
            sensor_data_t data;
            if (sscanf(buf, "%d %d %d %f ", &data.timestamp,&data.room_id, &data.sensor_id, &data.temperature) == 4) {
                shared_data_add(sd, data);
                printf("[Connection Manager] Received: %d %d %d %.2f \n",data.timestamp,data.room_id, data.sensor_id, data.temperature);
            } else {
                printf("[Connection Manager] Invalid sensor data format: %s\n", buf);
            }
        }
        close(client_fd);
    }
    close(server_fd);
    return NULL;
}
