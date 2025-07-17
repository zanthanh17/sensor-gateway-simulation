#include "shared_data.h"
#include <string.h>

void shared_data_init(shared_data_t *sd) {
    sd->count = 0;
    pthread_mutex_init(&sd->mutex, NULL);
}

void shared_data_add(shared_data_t *sd, sensor_data_t d) {
    pthread_mutex_lock(&sd->mutex);
    if (sd->count < MAX_SENSOR_DATA) {
        sd->data[sd->count++] = d;
    }
    pthread_mutex_unlock(&sd->mutex);
}

int shared_data_get(shared_data_t *sd, sensor_data_t *out) {
    pthread_mutex_lock(&sd->mutex);
    if (sd->count > 0) {
        *out = sd->data[--sd->count];
        pthread_mutex_unlock(&sd->mutex);
        return 1;
    }
    pthread_mutex_unlock(&sd->mutex);
    return 0;
}
