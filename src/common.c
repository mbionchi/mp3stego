#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int parse_opts(int argc, char **argv, options_t *opts) {
    int opt, rv=0;
    opterr = 0;
    opts->verbose = 0;

    while ((opt = getopt(argc,argv,"v")) != -1) {
        switch (opt) {
            case 'v':
                opts->verbose = 1;
                printf("got -v\n");
                break;
            case '?':
                rv = 1;
                break;
            default:
                rv = 1;
                break;
        }
    }
    if (optind < argc) {
        opts->stego_fp = fopen(argv[optind], opts->action==EMBED?"r+":"r");
        optind++;
    }
    if (optind < argc) {
        opts->data_fp = fopen(argv[optind], opts->action==EMBED?"r":"w");
    }
    return rv;
}

int get_mapping(size_t first_s, int* first, size_t map_s, int* map) {
    int i = 0;
    for (i = 0; i < map_s; i++) {
        map[i] = -1;
    }

    for (i = 0; i < first_s; i++) {
        int r = rand() % first_s;
        while (map[r] != -1) {
            r = rand() % first_s;
        }
        map[r] = i;
    }

    return composite_s;
} 
