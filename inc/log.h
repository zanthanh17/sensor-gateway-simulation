#ifndef LOG_H
#define LOG_H

#define LOG_FIFO "logs/logFifo"
#define LOG_FILE "logs/gateway.log"
#define MAX_LOG_LEN 256

void create_log_fifo(const char *fifo_path);
void write_log_event(const char *msg); // Ghi log vào FIFO, thread-safe

#endif
