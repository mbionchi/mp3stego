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
#include <stdint.h>

struct bitstringstream {
    uint8_t *bytes;
    unsigned long int pos;
    size_t size;
};
typedef struct bitstringstream bitstringstream_t;

bitstringstream_t *bss_init(uint8_t *, size_t, uint8_t);
int getbit(bitstringstream_t *);
int getnbits(bitstringstream_t *, size_t);
void putbit(uint8_t, bitstringstream_t*);
void putnbits(int, size_t, bitstringstream_t*);

#endif
