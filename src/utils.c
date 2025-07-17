#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "log.h"

void create_log_fifo(const char *fifo_path)
{
    if (access(fifo_path, F_OK) == -1)
    {
        if (mkfifo(fifo_path, 0666) == -1)
        {
            perror("mkfifo");
        }
    }
}

void write_log_event(const char *msg)
{
    static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&log_mutex);
    int fd = open(LOG_FIFO, O_WRONLY | O_NONBLOCK);
    if (fd != -1) {
        write(fd, msg, strlen(msg));
        write(fd, "\n", 1);
        close(fd);
    }
    pthread_mutex_unlock(&log_mutex);
}