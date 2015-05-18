#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "pcg_basic.h"

/* A small set of random words from /usr/share/dict and some bird names. */
static const char* METRIC_NAMES[] = {
    "abiotic",
    "agone",
    "badly",
    "badminton",
    "blarney",
    "delist",
    "echinal",
    "elderbush",
    "farenheit",
    "flashing",
    "galactopyra",
    "gieway",
    "parados",
    "aedicule",
    "chloromethane",
    "convex",
    "tussive",
    "wainrope",
    "heartbeat",
    "hydrosulphurous",
    "interceptive",
    "blooding",
    "unarduous",
    "tricuspidate",
    "uncivilize",
    "dishonestly",
    "diecious",
    "submissive",
    "guaran",
    "illuviating",
    "penguin",
    "grouse",
    "grebe",
    "flamingo",
    "heron",
    "stork",
    "meeesauce",
    "divers",
    "cassowary",
    "turkey",
    "partridge",
    "quail",
    "cormorant",
    "vulture",
    "goshawk",
    "rail",
    "crane",
    "stilt",
    "avocet",
    "plover",
    "snipe",
    "dove",
    "pigeon",
    "auk",
    "puffin",
    "skua",
    "parrot",
    "macaw",
    "cuckoo",
    "owl",
    "roller",
    "kingfisher",
    "hornbill",
    "woodpecker",
    "flycatcher",
    "jay",
    "robin"
};

static const int METRIC_NAMES_LEN = sizeof(METRIC_NAMES) / sizeof(METRIC_NAMES[0]);

/*
 * Build a metric name consisting of random words up to depth max_depth.
 * The metric name is written into "into", and will be null terminated.
 * The length sans the null terminator (strlen) will be returned.
 */
static int generate_metric(char* into, pcg32_random_t* r, int max_depth) {
    int length = 0;
    *into = '\0';
    if (max_depth <= 0)
        return length;
    
    int desired_words = pcg32_boundedrand_r(r, max_depth);
    desired_words++; /* Min 1, max max_depth */

    for (int i = 0; i < desired_words; i++) {
        int word = pcg32_boundedrand_r(r, METRIC_NAMES_LEN);
        int len = strlen(METRIC_NAMES[word]);
        memcpy(into, METRIC_NAMES[word], len);
        into += len;
        *into = '.';
        into += 1;
        length += len + 1;
    }
    into -= 1;
    *into = '\0'; // Clear the last dot
    return length - 1;
}

int main(int argc, char** argv) {
    int sock = 0;
    char send_data[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    pcg32_random_t rand;

    /* Seed the RNG with a known baseline for repeatable tests */
    pcg32_srandom_r(&rand, 100, 0);
    
    
    if (argc < 3) {
        fprintf(stderr, "Usage: %s address port\n", argv[0]);
        exit(1);
    }
    
    host = gethostbyname(argv[1]);
    int port = atoi(argv[2]);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(struct sockaddr)) == -1) {
        perror("Connect");
        exit(1);
    }

    while(1) {
            send(sock,send_data,strlen(send_data), 0);
    }
    return 0;
}
