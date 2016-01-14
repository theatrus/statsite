#include <math.h>
#include "gauge.h"

/**
 * Initializes the gauge struct
 * @arg gauge The gauge struct to initialize
 * @return 0 on success.
 */
int init_gauge(gauge_t *gauge) {
    counter->count = 0;
    counter->sum = 0;
    return 0;
}

/**
 * Adds a new sample to the struct
 * @arg gauge The gauge to add to
 * @arg sample The new sample value
 * @return 0 on success.
 */
int gauge_add_sample(gauge_t *gauge, double sample) {
    if (gauge->count == 0) {
        gauge->sum = sample;
    }

    gauge->value = sample;
    gauge->count++;
    gauge->sum += sample;
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
