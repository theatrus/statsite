#include <math.h>
#include "gauge.h"

/**
 * Initializes the gauge struct
 * @arg gauge The gauge struct to initialize
 * @return 0 on success.
 */
int init_gauge(gauge_t *gauge) {
    gauge->count = 0;
    gauge->sum = 0;
    gauge->value = 0;
    return 0;
}

/**
 * Adds a new sample to the struct
 * @arg gauge The gauge to add to
 * @arg sample The new sample value
 * @arg delta   Is this a delta update?
 * @return 0 on success.
 */
int gauge_add_sample(gauge_t *gauge, double sample, bool delta) {
    if (delta) {
        gauge->value += sample;
    } else {
        gauge->value = sample;
    }
    gauge->sum += sample;
    gauge->count++;
    return 0;
}

/**
 * Returns the number of samples in the gauge
 * @arg gauge The gauge to query
 * @return The number of samples
 */
uint64_t gauge_count(gauge_t *gauge) {
    return gauge->count;
}

/**
 * Returns the mean gauge value
 * @arg gauge The gauge to query
 * @return The mean value of the gauge
 */
double gauge_mean(gauge_t *gauge) {
    return (gauge->count) ? gauge->sum / gauge->count : 0;
}

/**
 * Returns the sum of the gauge
 * @arg gauge The gauge to query
 * @return The sum of values
 */
double gauge_sum(gauge_t *gauge) {
    return gauge->sum;
}

/**
 * Returns the gauge value (for backwards compat)
 * @arg gauge  the gauge to query
 * @return The gauge value
 */
double gauge_value(gauge_t *gauge) {
    return gauge->value;
}
