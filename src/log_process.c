#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "log.h"

int main()
{
    create_log_fifo(LOG_FIFO);

    int fd = open(LOG_FIFO, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open log FIFO");
        return EXIT_FAILURE;
    }

    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL)
    {
        perror("Failed to open log file");
        close(fd);
        return EXIT_FAILURE;
    }
    char buffer[MAX_LOG_LEN];
    int seq = 1;
    while (fgets(buffer, sizeof(buffer), fdopen(fd, "r")) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        if (strlen(buffer) == 0) continue;
        time_t now = time(NULL);
        char *time_stamp = ctime(&now);
        if (time_stamp) {
            time_stamp[strlen(time_stamp) - 1] = '\0';
        } else {
            time_stamp = "unknown-time";
        }
        fprintf(log_file, "%d %s %s\n", seq++, time_stamp, buffer);
        fflush(log_file);
    }

    fclose(log_file);
    close(fd);

    return 0;
}