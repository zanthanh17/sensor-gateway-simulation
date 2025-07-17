#ifndef SHARED_DATA_H
#define SHARED_DATA_H
#include <pthread.h>
#define MAX_SENSOR_DATA 128

typedef struct {



    int timestamp;
    int room_id; // 1: room 1, 2: room 2, 3: room 3
    int sensor_id;
    float temperature;
} sensor_data_t;

typedef struct {
    sensor_data_t data[MAX_SENSOR_DATA];
    int count;
    pthread_mutex_t mutex;
} shared_data_t;

void shared_data_init(shared_data_t *sd);
void shared_data_add(shared_data_t *sd, sensor_data_t d);
int shared_data_get(shared_data_t *sd, sensor_data_t *out);

#endif
