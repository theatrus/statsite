#ifndef _SINK_H_
#define _SINK_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

typedef struct sink {
    statsite_config* global_config;
    sink_config* sink_config;
    struct sink* next;
} sink;

extern sink* init_stream_sink(const sink_config_stream*, const statsite_config*);

#endif
