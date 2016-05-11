#include <math.h>
#include "gauge.h"


int init_gauge(gauge_t *gauge) {
    gauge->count = 0;
    gauge->sum = 0;
    gauge->value = 0;
    gauge->min = 0;
    gauge->max = 0;
    return 0;
}

int gauge_add_sample(gauge_t *gauge, double sample, bool delta) {
    if (gauge->count == 0) {
        gauge->min = gauge->max = sample;
    }

    if (delta) {
        gauge->value += sample;
    } else {
        gauge->value = sample;
    }

    if (gauge->min > sample)
        gauge->min = sample;
    else if (gauge->max < sample)
        gauge->max = sample;

    gauge->sum += sample;
    gauge->count++;
    return 0;
}

uint64_t gauge_count(gauge_t *gauge) {
    return gauge->count;
}

double gauge_mean(gauge_t *gauge) {
    return (gauge->count) ? gauge->sum / gauge->count : 0;
}

double gauge_sum(gauge_t *gauge) {
    return gauge->sum;
}

double gauge_value(gauge_t *gauge) {
    return gauge->value;
}

double gauge_min(gauge_t *gauge) {
    return gauge->min;
}

double gauge_max(gauge_t *gauge) {
    return gauge->max;
}