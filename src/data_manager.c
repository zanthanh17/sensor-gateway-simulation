#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "data_manager.h"
#include "log.h"

#define MAX_SENSOR_ID 1000
#define AVG_WINDOW 5
#define HOT_THRESHOLD 28.0
#define COLD_THRESHOLD 18.0

typedef enum { NORMAL, TOO_HOT, TOO_COLD } sensor_state_t;
typedef struct {
    float temps[AVG_WINDOW];
    int idx;
    int count;
    sensor_state_t last_state;
} running_avg_t;

void *data_manager_thread(void *arg) {
    shared_data_t *sd = (shared_data_t *)arg;
    running_avg_t avg[MAX_SENSOR_ID];
    memset(avg, 0, sizeof(avg));
    for (int i = 0; i < MAX_SENSOR_ID; ++i) avg[i].last_state = NORMAL;
    while (1) {
        sensor_data_t data;
        if (shared_data_get(sd, &data)) {
            int id = data.sensor_id;
            if (id < 0 || id >= MAX_SENSOR_ID) {
                printf("[Data Manager] Received sensor data with invalid sensor node ID %d\n", id);
                char logbuf[128];
                snprintf(logbuf, sizeof(logbuf), "Received sensor data with invalid sensor node ID %d", id);
                write_log_event(logbuf);
                continue;
            }
            running_avg_t *a = &avg[id];
            a->temps[a->idx] = data.temperature;
            a->idx = (a->idx + 1) % AVG_WINDOW;
            if (a->count < AVG_WINDOW) a->count++;
            // Tính trung bình
            float sum = 0;
            for (int i = 0; i < a->count; ++i) sum += a->temps[i];
            float avg_temp = sum / a->count;
            sensor_state_t new_state = NORMAL;
            if (avg_temp > HOT_THRESHOLD) new_state = TOO_HOT;
            else if (avg_temp < COLD_THRESHOLD) new_state = TOO_COLD;
            if (new_state != a->last_state) {
                if (new_state == TOO_HOT) {
                    printf("[Data Manager] Sensor %d reports it's too hot (running avg temperature = %.2f)\n", id, avg_temp);
                    char logbuf[128];
                    snprintf(logbuf, sizeof(logbuf), "Sensor node with %d reports it's too hot (running avg temperature = %.2f)", id, avg_temp);
                    write_log_event(logbuf);
                }
                else if (new_state == TOO_COLD) {
                    printf("[Data Manager] Sensor %d reports it's too cold (running avg temperature = %.2f)\n", id, avg_temp);
                    char logbuf[128];
                    snprintf(logbuf, sizeof(logbuf), "Sensor node with %d reports it's too cold (running avg temperature = %.2f)", id, avg_temp);
                    write_log_event(logbuf);
                }
                else
                    printf("[Data Manager] Sensor %d is back to normal (running avg temperature = %.2f)\n", id, avg_temp);
                a->last_state = new_state;
            }
        } else {
            usleep(100000); // 100ms nếu không có dữ liệu
        }
    }
    return NULL;
}
