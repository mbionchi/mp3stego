#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "embed.h"
#include "extract.h"
#include "info.h"
#include "common.h"

void print_usage(FILE *fp) {
    fprintf(fp, "Usage: pbhide <embed|extract|info|help> <opts> <input_file> <output_file>\n");
}

void print_help(FILE *fp) {
    fprintf(fp, "pbhide:\n\nhelp page yet to be written\n");
}

int main(int argc, char **argv) {
    int rv = 0;
    options_t *opts;
    if (argc < 2) {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        print_usage(stderr);
        rv = 1;
    } else {
        action_t action;
        if (!strcmp(argv[1], "embed")) {
            action = EMBED;
        } else if (!strcmp(argv[1], "extract")) {
            action = EXTRACT;
        } else if (!strcmp(argv[1], "info")) {
            action = INFO;
        } else if (!strcmp(argv[1], "help")) {
            print_help(stdout);
            exit(0);
        } else {
            fprintf(stderr, "%s: invalid action: %s\n", argv[0], argv[1]);
            print_usage(stderr);
            rv = 1;
        }
        if (!rv) {
            opts = malloc(sizeof(options_t));
            opts->action = action;
            rv = parse_opts(argc-1, argv+1, opts);
            switch (action) {
                case EMBED:
                    rv = embed(opts);
                    break;
                case EXTRACT:
                    rv = extract(opts);
                    break;
                case INFO:
                    rv = info(opts);
                    break;
            }
            switch (rv) {
                case SUCCESS:
                    break;
                case NO_SPACE:
                    fprintf(stderr, "Not enough capacity in mp3 file for data.\n");
                    break;
                case NOT_MP3:
                    fprintf(stderr, "Not an mp3 file.\n");
                    break;
                default:
                    fprintf(stderr, "Unknown error.");
                    break;
            }
        }
        fclose(opts->stego_fp);
        fclose(opts->data_fp);
        free(opts);
    }
    return rv;
}
