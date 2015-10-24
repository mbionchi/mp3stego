#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int rv = 0;
    if (argc < 2) {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        print_usage(stderr);
        rv = 1;
    } else if (!strcmp(argv[1], "embed")) {
        rv = embed_main(argc-1, argv+1);
    } else if (!strcmp(argv[1], "extract")) {
        rv = extract_main(argc-1, argv+1);
    } else if (!strcmp(argv[1], "info")) {
        rv = info_main(argc-1, argv+1);
    } else if (!strcmp(argv[1], "help")) {
        print_help(stdout);
    } else {
        fprintf(stderr, "%s: invalid action: %s\n", argv[0], argv[1]);
        print_usage(stderr);
        rv = 1;
    }
    return rv;
}
