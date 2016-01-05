#include "bits.h"

#include <stdint.h>
#include <stdlib.h>

bitstringstream_t *bss_init(uint8_t *bytes, size_t nmemb, uint8_t offset) {
    bitstringstream_t *bss = malloc(sizeof(bitstringstream_t));
    bss->bytes = bytes;
    bss->pos = offset;
    bss->size = nmemb*8;
    return bss;
}

int getbit(bitstringstream_t *bss) {
    if (bss->pos<bss->size*8) {
        bss->pos++;
        return (bss->bytes[(bss->pos-1)/8] & (1 << (7-(bss->pos-1)%8))) >> (7-(bss->pos-1)%8);
    } else {
        return EOF;
    }
}

int getnbits(bitstringstream_t *bss, size_t nb) {
    size_t i;
    int tmp, ret=0;
    for (i=0; i<nb; i++) {
        tmp = getbit(bss);
        if (tmp!=EOF)
            ret |= tmp << (nb-i-1);
        else
            return EOF;
    }
    return ret;
}

void putbit(uint8_t bit, bitstringstream_t *bss) {
    if (bss->pos<bss->size*8) {
        bss->bytes[(bss->pos)/8] &= ~(1 << (7-bss->pos%8));
        bss->bytes[(bss->pos)/8] |= (bit & 1) << (7-bss->pos%8);
        bss->pos++;
    }
}

void putnbits(int bits, size_t nb, bitstringstream_t *bss) {
    int i;
    for (i=0; i<nb; i++) {
        putbit(bits&1, bss);
        bits>>1;
    }
}
