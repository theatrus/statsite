#ifndef GAUGE_H
#define GAUGE_H
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t count;     // Count of items
    double sum;         // Sum of the values
    double value;       // redundant if count == 1, keeping it to reduce footprint of changes
    double min;         // min of all of the gauge samples recieved
    double max;         // max of all of the gauge samples received
} gauge_t;


/**
 * Initializes the gauge struct
 * @arg gauge The gauge struct to initialize
 * @return 0 on success.
 */
int init_gauge(gauge_t *gauge);

/**
 * Adds a new sample to the struct
 * @arg gauge The gauge to add to
 * @arg sample The new sample value
 * @arg delta   Is this a delta update?
 * @return 0 on success.
 */
int gauge_add_sample(gauge_t *gauge, double sample, bool delta);

/**
 * Returns the number of samples in the gauge
 * @arg gauge The gauge to query
 * @return The number of samples
 */
uint64_t gauge_count(gauge_t *gauge);

/**
 * Returns the mean gauge value
 * @arg gauge The gauge to query
 * @return The mean value
 */
double gauge_mean(gauge_t *gauge);

/**
 * Returns the sum of the gauge
 * @arg gauge The gauge to query
 * @return The sum of values
 */
double gauge_sum(gauge_t *gauge);

/**
 * Returns the min of the gauge
 * @arg gauge The gauge to query
 * @return The min of values
 */
double gauge_min(gauge_t *gauge);

/**
 * Returns the max of the gauge
 * @arg gauge The gauge to query
 * @return The max of values
 */
double gauge_max(gauge_t *gauge);

/**
 * Returns the gauge value (for backwards compat)
 * @arg gauge  the gauge to query
 * @return The gauge value
 */
double gauge_value(gauge_t *gauge);

#endif
