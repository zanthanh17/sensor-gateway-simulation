#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <stdlib.h>
#include "storage_manager.h"
#include "log.h"

#define DB_PATH "db/sensordata.db"
#define MAX_RETRY 3

void *storage_manager_thread(void *arg) {
    shared_data_t *sd = (shared_data_t *)arg;
    sqlite3 *db = NULL;
    int retry = 0;
    int rc;
    while (retry < MAX_RETRY) {
        rc = sqlite3_open(DB_PATH, &db);
        if (rc == SQLITE_OK) break;
        fprintf(stderr, "[Storage Manager] Unable to connect to SQL server (try %d): %s\n", retry+1, sqlite3_errmsg(db));
        sqlite3_close(db);
        retry++;
        sleep(1);
    }
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[Storage Manager] Could not connect to SQL server after %d attempts. Exiting.\n", MAX_RETRY);
        write_log_event("Unable to connect to SQL server.");
        exit(1);
    }
    printf("[Storage Manager] Connection to SQL server established.\n");
    write_log_event("Connection to SQL server established.");
    // Tạo bảng nếu chưa có
    const char *create_table = "CREATE TABLE IF NOT EXISTS sensor_data (timestamp INT, room_id INT, sensor_id INT, temperature REAL);";
    rc = sqlite3_exec(db, create_table, 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[Storage Manager] Failed to create table: %s\n", sqlite3_errmsg(db));
        write_log_event("Failed to create table sensor_data.");
        sqlite3_close(db);
        exit(1);
    }
    printf("[Storage Manager] Table sensor_data ready.\n");
    write_log_event("New table sensor_data created.");
    // Vòng lặp lấy dữ liệu và ghi vào DB
    while (1) {
        sensor_data_t data;
        if (shared_data_get(sd, &data)) {
            char sql[256];
            snprintf(sql, sizeof(sql),
                "INSERT INTO sensor_data (timestamp, room_id, sensor_id, temperature) VALUES (%d, %d, %d, %.2f);",
                data.timestamp, data.room_id, data.sensor_id, data.temperature);
            rc = sqlite3_exec(db, sql, 0, 0, 0);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "[Storage Manager] Failed to insert data: %s\n", sqlite3_errmsg(db));
            } else {
                printf("[Storage Manager] Inserted: %d %d %d %.2f\n", data.timestamp, data.room_id, data.sensor_id, data.temperature);
            }
        } else {
            usleep(100000); // 100ms nếu không có dữ liệu
        }
    }
    sqlite3_close(db);
    return NULL;
}
