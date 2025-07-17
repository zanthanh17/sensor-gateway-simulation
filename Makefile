CC = gcc
CFLAGS = -Wall -pthread -Iinc

all: main log_process

main: src/main.c src/utils.c src/shared_data.c src/connection_manager.c src/data_manager.c src/storage_manager.c
	$(CC) $(CFLAGS) -o main src/main.c src/utils.c src/shared_data.c src/connection_manager.c src/data_manager.c src/storage_manager.c -lsqlite3

log_process: src/log_process.c src/utils.c
	$(CC) $(CFLAGS) -o log_process src/log_process.c src/utils.c

clean:
	rm -f main log_process logs/gateway.log logs/logFifo