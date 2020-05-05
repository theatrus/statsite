#include <math.h>
#include "gauge_direct.h"


int init_gauge_direct(gauge_direct_t *gauge) {
    gauge->value = 0;
    return 0;
}

int gauge_direct_add_sample(gauge_direct_t *gauge, double sample) {
    gauge->value = sample;
    return 0;
}

double gauge_direct_value(gauge_direct_t *gauge) {
    return gauge->value;
}
