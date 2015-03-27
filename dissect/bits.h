/*
 * bits.h:  structure definitions for bitstreams + function headers
 * Copyright (C) 2014 Mike Bionchik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For contact info, see the AUTHORS file
 */

#ifndef MP3STEGO_BITS_H
#define MP3STEGO_BITS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* BITSTRINGS AND STREAMS */

struct bitstringstream {
    uint8_t *bytes;
    unsigned long int pos;
    size_t size;
};
typedef struct bitstringstream bitstringstream_t;

bitstringstream_t *bss_init(uint8_t *, size_t, uint8_t);
int getbit(bitstringstream_t *);
int getnbits(bitstringstream_t *, unsigned int);

/* FILE BIT STREAMS */

struct bitstream {
    FILE *fs;
    const char *mode;
    int8_t pos;
    int ch;
    void (*check_bs)(struct bitstream *bs);
};
typedef struct bitstream bitstream_t;

bitstream_t *bsopen(char *, char *);
int bsclose(bitstream_t *);
int bsgetb(bitstream_t *);
int bsputb(int, bitstream_t *);

/* this function writes size_t RIGHTMOST bits into a bitstream */
size_t bswrite(int, size_t, bitstream_t *);

#endif
