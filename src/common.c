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
