#include "sink.h"

struct stream_sink {
    sink super;
};

sink* init_stream_sink(const sink_config_stream* sc, const statsite_config* config) {
    struct stream_sink* ss = calloc(1, sizeof(struct stream_sink));

    return (sink*)ss;
}
