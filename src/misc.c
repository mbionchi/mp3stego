#include "misc.h"

#include <stdio.h>

/* TODO */
void print_usage(FILE *fp, char *name) {
    fprintf(fp, "%s usage:\n\n %s (embed|extract|info|help)", name, name);
}

/* TODO */
void print_help(FILE *fp, char *name) {
    fprintf(fp, "%s help:\n\n WRITEME\n", name);
}
