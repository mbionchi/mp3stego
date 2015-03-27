/*
 * bits.c:  definitions for functions that operate on bitstreams
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

#include "bits.h"

#include <stdint.h>
#include <string.h>

bitstringstream_t *bss_init(uint8_t *bytes, size_t nmemb, uint8_t offset)
{
    bitstringstream_t *bss = malloc(sizeof(bitstringstream_t));
    bss->bytes = bytes;
    bss->pos = offset;
    bss->size = nmemb*8;
    return bss;
}

int getbit(bitstringstream_t *bss)
{
    int ret;
    if ( bss->pos >= bss->size )
        ret = EOF;
    else {
        ret = ((bss->bytes[bss->pos/8] & (1 << (7-bss->pos%8))) >> (7-bss->pos%8));
        ++bss->pos;
    }
    return ret;
}

int getnbits(bitstringstream_t *bss, unsigned int nb)
{
    unsigned int i, ret = 0;
    int tmp;
    for ( i = 0; i < nb; ++i ) {
        tmp = getbit(bss);
        if ( tmp != EOF )
            ret |= tmp << (nb-i-1);
        else
            return EOF;
    }
    return ret;
}

static void check_bs_r(bitstream_t *bs)
{
    if ( bs->pos < 0 ) {
        bs->ch = fgetc(bs->fs);
        bs->pos += 8;
    } else if ( bs->pos > 7 ) {
        fseek(bs->fs, -2, SEEK_CUR);
        bs->ch = fgetc(bs->fs);
        bs->pos -= 8;
    }
}

static void check_bs_w(bitstream_t *bs)
{
    if ( bs->pos < 0 ) {
        fputc(bs->ch, bs->fs);
        bs->ch = 0;
        bs->pos += 8;
    }
}

static void init_bs(bitstream_t *bs)
{
    if ( strcmp(bs->mode, "r") == 0 ) {
        bs->check_bs = &check_bs_r;
        bs->ch = fgetc(bs->fs);
    } else {
        bs->check_bs = &check_bs_w;
        bs->ch = 0;
    }
    bs->pos = 7;
}

bitstream_t *bsopen(char *path, char *mode)
{
    bitstream_t *bs = malloc(sizeof(bitstream_t));
    bs->fs = fopen(path, mode);
    if ( bs->fs != NULL && ( strcmp(mode, "r") == 0 || strcmp(mode, "w") == 0 ) ) {
        bs->mode = mode;
        init_bs(bs);
        return bs;
    } else {
        free(bs);
        return NULL;
    }
}

int bsclose(bitstream_t *bs)
{
    int ret;
    ret = fclose(bs->fs);
    free(bs);
    return ret;
}

int bsgetb(bitstream_t *bs)
{
    int ret;
    if ( bs->ch != EOF ) {
        ret = (bs->ch & (1 << bs->pos)) >> bs->pos;
        --bs->pos;
        bs->check_bs(bs);
        return ret;
    } else
        return EOF;
}

int bsputb(int b, bitstream_t *bs)
{
    b &= 1;
    bs->ch ^= (-b ^ bs->ch) & (1 << bs->pos);
    --bs->pos;
    bs->check_bs(bs);
    return b;
}

size_t bswrite(int bits, size_t nb, bitstream_t *bs)
{
    size_t n_written = 0;
    if ( nb <= 32 ) {
        --nb;
        while ( nb > 0 ) {
            bsputb((bits & (1 << nb)) >> nb, bs);
            --nb;
            ++n_written;
        }
    }
    return n_written;
}
