#ifndef _SINK_H_
#define _SINK_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "metrics.h"

typedef struct sink {
    const statsite_config* global_config;
    const sink_config* sink_config;
    struct sink* next;
    int (*command)(struct sink*, metrics* m, void* data);
} sink;

extern int init_sinks(sink** sinks, statsite_config* config);
extern sink* init_stream_sink(const sink_config_stream*, const statsite_config*);

#endif
