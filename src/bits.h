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
