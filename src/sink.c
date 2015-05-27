#include "sink.h"

int init_sinks(sink** sinks, statsite_config* config) {
    for(sink_config* sc = config->sink_configs; sc != NULL; sc = sc->next) {
        if (sc->type == SINK_TYPE_STREAM) {
            sink* actual_sink = init_stream_sink((sink_config_stream*)sc, config);
            if (*sinks) {
                actual_sink->next = *sinks;
            }
            *sinks = actual_sink;
        }
    }
    return 0;
}
