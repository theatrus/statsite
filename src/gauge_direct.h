#ifndef GAUGE_DIRECT_H
#define GAUGE_DIRECT_H
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    double value;       // redundant if count == 1, keeping it to reduce footprint of changes
} gauge_direct_t;


/**
 * Initializes the gauge_direct struct
 * @arg gauge The gauge struct to initialize
 * @return 0 on success.
 */
int init_gauge_direct(gauge_direct_t *gauge);

/**
 * Adds a new sample to the struct
 * @arg gauge The gauge to add to
 * @arg sample The new sample value
 * @return 0 on success.
 */
int gauge_direct_add_sample(gauge_direct_t *gauge, double sample);

/**
 * Returns the gauge value
 * @arg gauge  the gauge to query
 * @return The gauge value
 */
double gauge_direct_value(gauge_direct_t *gauge);

#endif
