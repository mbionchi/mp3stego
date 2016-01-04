#ifndef MP3STEGO_COMMON_H
#define MP3STEGO_COMMON_H

#include <stdio.h>

enum action {EMBED, EXTRACT, INFO};
typedef enum action action_t;

enum error_type {SUCCESS, NO_SPACE, NOT_MP3};
typedef enum error_type error_type_t;

struct options {
    FILE *stego_fp;
    FILE *data_fp;
    int verbose;
    action_t action;
};
typedef struct options options_t;

int parse_opts(int, char**, options_t*);

int get_mapping(size_t, int*, size_t, int*);

#endif
