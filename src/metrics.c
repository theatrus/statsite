#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "metrics.h"
#include "set.h"

static int counter_delete_cb(void *data, const char *key, void *value);
static int timer_delete_cb(void *data, const char *key, void *value);
static int set_delete_cb(void *data, const char *key, void *value);
static int gauge_delete_cb(void *data, const char *key, void *value);
static int gauge_direct_delete_cb(void* data, const char* key, void* value);
static int iter_cb(void *data, const char *key, void *value);

struct cb_info {
    metric_type type;
    void *data;
    metric_callback cb;
};

/**
 * Initializes the metrics struct.
 * @arg eps The maximum error for the quantiles
 * @arg quantiles A sorted array of double quantile values, must be on (0, 1)
 * @arg num_quants The number of entries in the quantiles array
 * @arg histograms A radix tree with histogram settings. This is not owned
 * by the metrics object. It is assumed to exist for the life of the metrics.
 * @arg set_precision The precision to use for sets
 * @return 0 on success.
 */
int init_metrics(double timer_eps, double *quantiles, uint32_t num_quants, radix_tree *histograms, unsigned char set_precision, metrics *m) {
    // Copy the inputs
    m->timer_eps = timer_eps;
    m->num_quants = num_quants;
    m->quantiles = malloc(num_quants * sizeof(double));
    memcpy(m->quantiles, quantiles, num_quants * sizeof(double));
    m->histograms = histograms;
    m->set_precision = set_precision;

    // Allocate the hashmaps
    int res = hashmap_init(0, &m->counters);
    if (res) return res;
    res = hashmap_init(0, &m->timers);
    if (res) return res;
    res = hashmap_init(0, &m->sets);
    if (res) return res;
    res = hashmap_init(0, &m->gauges);
    if (res) return res;
    res = hashmap_init(0, &m->gauges_direct);
    if (res) return res;

    return 0;
}

/**
 * Initializes the metrics struct, with preset configurations.
 * This defaults to a timer epsilon of 0.01 (1% error), and quantiles at
 * 0.5, 0.95, and 0.99. Histograms are disabed, and the set
 * precision is 12 (2% variance).
 * @return 0 on success.
 */
int init_metrics_defaults(metrics *m) {
    double quants[] = {0.5, 0.95, 0.99};
    return init_metrics(0.01, (double*)&quants, 3, NULL, 12, m);
}

/**
 * Destroys the metrics
 * @return 0 on success.
 */
int destroy_metrics(metrics *m) {
    // Clear the copied quantiles array
    free(m->quantiles);

    // Nuke the counters
    hashmap_iter(m->counters, counter_delete_cb, NULL);
    hashmap_destroy(m->counters);

    // Nuke the timers
    hashmap_iter(m->timers, timer_delete_cb, NULL);
    hashmap_destroy(m->timers);

    // Nuke the timers
    hashmap_iter(m->sets, set_delete_cb, NULL);
    hashmap_destroy(m->sets);

    // Nuke the gauges
    hashmap_iter(m->gauges, gauge_delete_cb, NULL);
    hashmap_destroy(m->gauges);

    // Nuke the gauge_directs
    hashmap_iter(m->gauges_direct, gauge_direct_delete_cb, NULL);
    hashmap_destroy(m->gauges_direct);


    return 0;
}

/**
 * Increments the counter with the given name
 * by a value.
 * @arg name The name of the counter
 * @arg val The value to add
 * @arg sample_rate the sample rate of val
 * @return 0 on success
 */
static int metrics_increment_counter(metrics *m, char *name, double val, double sample_rate) {
    counter *c;
    int res = hashmap_get(m->counters, name, (void**)&c);

    // New counter
    if (res == -1) {
        c = malloc(sizeof(counter));
        init_counter(c);
        hashmap_put(m->counters, name, c);
    }

    // Add the sample value
    return counter_add_sample(c, val, sample_rate);
}

/**
 * Adds a new timer sample for the timer with a
 * given name.
 * @arg name The name of the timer
 * @arg val The sample to add
 * @return 0 on success.
 */
static int metrics_add_timer_sample(metrics *m, char *name, double val, double sample_rate) {
    if (isnan(val) || isinf(val)) {
        syslog(LOG_ERR, "Invalid timer sample value supplied, name=%s", name);
        return -1;
    }

    timer_hist *t;
    histogram_config *conf;
    int res = hashmap_get(m->timers, name, (void**)&t);

    // New timer
    if (res == -1) {
        t = malloc(sizeof(timer_hist));
        init_timer(m->timer_eps, m->quantiles, m->num_quants, &t->tm);
        hashmap_put(m->timers, name, t);

        // Check if we have any histograms configured
        if (m->histograms && !radix_longest_prefix(m->histograms, name, (void**)&conf)) {
            t->conf = conf;
            t->counts = calloc(conf->num_bins, sizeof(unsigned int));
        } else {
            t->conf = NULL;
            t->counts = NULL;
        }
    }

    // Add the histogram value
    if (t->conf) {
        conf = t->conf;
        if (val < conf->min_val)
            t->counts[0]++;
        else if (val >= conf->max_val)
            t->counts[conf->num_bins - 1]++;
        else {
            int idx = ((val - conf->min_val) / conf->bin_width) + 1;
            t->counts[idx]++;
        }
    }

    // Add the sample value
    return timer_add_sample(&t->tm, val, sample_rate);
}

/**
 * Sets a gauge value
 * @arg name The name of the gauge
 * @arg val The value to set
 * @arg delta Is this a delta update
 * @return 0 on success
 */
static int metrics_set_gauge(metrics *m, char *name, double val, bool delta) {
    gauge_t *g;
    int res = hashmap_get(m->gauges, name, (void**)&g);

    // New gauge
    if (res == -1) {
        g = malloc(sizeof(gauge_t));
        init_gauge(g);
        hashmap_put(m->gauges, name, g);
    }

    return gauge_add_sample(g, val, delta);
}

/**
 * Sets a direct gauge value
 * @arg name The name of the gauge
 * @arg val The value to set
 * @return 0 on success
 */
static int metrics_set_gauge_direct(metrics *m, char *name, double val) {
    gauge_direct_t *g;
    int res = hashmap_get(m->gauges_direct, name, (void**)&g);

    // New gauge
    if (res == -1) {
        g = malloc(sizeof(gauge_direct_t));
        init_gauge_direct(g);
        hashmap_put(m->gauges_direct, name, g);
    }

    return gauge_direct_add_sample(g, val);
}

/**
 * Adds a new sampled value
 * arg type The type of the metrics
 * @arg name The name of the metric
 * @arg val The sample to add
 * @arg sample_rate The sample rate of val
 * @return 0 on success.
 */
int metrics_add_sample(metrics *m, metric_type type, char *name, double val, double sample_rate) {
    switch (type) {
        case GAUGE_DIRECT:
            return metrics_set_gauge_direct(m, name, val);

        case GAUGE:
            return metrics_set_gauge(m, name, val, false);

        case GAUGE_DELTA:
            return metrics_set_gauge(m, name, val, true);

        case COUNTER:
            return metrics_increment_counter(m, name, val, sample_rate);

        case TIMER:
            return metrics_add_timer_sample(m, name, val, sample_rate);

        default:
            return -1;
    }
}

/**
 * Adds a value to a named set.
 * @arg name The name of the set
 * @arg value The value to add
 * @return 0 on success
 */
int metrics_set_update(metrics *m, char *name, char *value) {
    set_t *s;
    int res = hashmap_get(m->sets, name, (void**)&s);

    // New set
    if (res == -1) {
        s = malloc(sizeof(set_t));
        set_init(m->set_precision, s);
        hashmap_put(m->sets, name, s);
    }

    // Add the sample value
    set_add(s, value);
    return 0;
}

/**
 * Iterates through all the metrics
 * @arg m The metrics to iterate through
 * @arg data Opaque handle passed to the callback
 * @arg cb A callback function to invoke. Called with a type, name
 * and value. If the type is KEY_VAL, it is a pointer to a double,
 * for a counter, it is a pointer to a counter, and for a timer it is
 * a pointer to a timer. Return non-zero to stop iteration.
 * @return 0 on success, or the return of the callback
 */
int metrics_iter(metrics *m, void *data, metric_callback cb) {

    // Store our data in a small struct
    struct cb_info info = {COUNTER, data, cb};

    // Send the counters
    bool should_break = hashmap_iter(m->counters, iter_cb, &info);
    if (should_break) return should_break;

    // Send the timers
    info.type = TIMER;
    should_break = hashmap_iter(m->timers, iter_cb, &info);
    if (should_break) return should_break;

    // Send the gauges
    info.type = GAUGE;
    should_break = hashmap_iter(m->gauges, iter_cb, &info);
    if (should_break) return should_break;

    // Send the direct gauges
    info.type = GAUGE_DIRECT;
    should_break = hashmap_iter(m->gauges_direct, iter_cb, &info);
    if (should_break) return should_break;

    // Send the sets
    info.type = SET;
    should_break = hashmap_iter(m->sets, iter_cb, &info);

    return should_break;
}

// Counter map cleanup
static int counter_delete_cb(void *data, const char *key, void *value) {
    free(value);
    return 0;
}

// Timer map cleanup
static int timer_delete_cb(void *data, const char *key, void *value) {
    timer_hist *t = value;
    destroy_timer(&t->tm);
    if (t->counts) free(t->counts);
    free(t);
    return 0;
}

// Set map cleanup
static int set_delete_cb(void *data, const char *key, void *value) {
    set_t *s = value;
    set_destroy(s);
    free(s);
    return 0;
}

// Gauge map cleanup
static int gauge_delete_cb(void *data, const char *key, void *value) {
    gauge_t *g = value;
    free(g);
    return 0;
}

// Gauge map cleanup
static int gauge_direct_delete_cb(void *data, const char *key, void *value) {
    gauge_direct_t *g = value;
    free(g);
    return 0;
}

// Callback to invoke the user code
static int iter_cb(void *data, const char *key, void *value) {
    struct cb_info *info = data;
    return info->cb(info->data, info->type, (char*)key, value);
}

