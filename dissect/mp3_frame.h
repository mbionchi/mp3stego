/*
 * mp3_frame.h:  structure definitions for mp3 file + function headers
 * Copyright (C) 2015 Mike Bionchik
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

#ifndef MP3STEGO_MP3_FRAME_H
#define MP3STEGO_MP3_FRAME_H

#include <stdio.h>
#include <stdint.h>

// TODO: fix this shit
#include "meow.h"

static const int SF[3] = { 44100, 48000, 32000 };
static const int BR[16] = { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1 };
static const int PB[4] = { 4, 4, 4, 6 };

struct mp3_header {
    long int pos;
/*  uint16_t syncword : 12;
    uint16_t id : 1;             these are implied by the signature
    uint16_t layer : 2;          0xFFFB, so no need to store/use them
    uint16_t protection_bit : 1; */
    uint16_t bitrate       : 4;
    uint16_t sampling_freq : 2;
    uint16_t padding_bit   : 1;
    uint16_t private_bit   : 1;
    uint16_t mode          : 2;
    uint16_t mode_ext      : 2;
    uint16_t copyright     : 1;
    uint16_t original      : 1;
    uint16_t emphasis      : 2;
};
typedef struct mp3_header mp3_header_t;

/* granule-specific side information */
struct si_gr {
    uint16_t part2_3_length     : 12;
    uint16_t big_values         : 9;
    uint16_t global_gain        : 8;
    uint16_t scalefac_compress  : 4;
    uint16_t blocksplit_flag    : 1;

    uint16_t block_type         : 2;
    uint16_t switch_point       : 1;

    uint16_t region_address1    : 4;
    uint16_t region_address2    : 3;

    uint16_t preflag            : 1;
    uint16_t scalefac_scale     : 1;
    uint16_t count1table_select : 1;

    uint16_t : 1;   //padding

    uint8_t table_select[3];
    uint8_t subblock_gain[3];
};
typedef struct si_gr si_gr_t;

/* channel-specific side information */
struct si_ch {
    _Bool scfsi[4];
    si_gr_t gr[2];
};
typedef struct si_ch si_ch_t;

/* and the side information itself */
struct mp3_side_info {
    uint16_t main_data_end;
    uint8_t private_bits;
    si_ch_t *ch;
};
typedef struct mp3_side_info mp3_side_info_t;

/* channel-specific main data stuff */
struct md_ch {
    uint8_t scalefac[21];
    uint8_t scalefac_w[12][3];
    size_t huffman_length;
    uint8_t *huffman;
};
typedef struct md_ch md_ch_t;

/* granule-specific main data stuff */
struct md_gr {
    md_ch_t *ch;
};
typedef struct md_gr md_gr_t;

/* main data per se */
struct mp3_main_data {
    md_gr_t gr[2];
};
typedef struct mp3_main_data mp3_main_data_t;

struct mp3_frame {
    mp3_header_t *header;
    mp3_side_info_t *side_info;
//    mp3_main_data_t *main_data;
    array_t *main_data;
};
typedef struct mp3_frame mp3_frame_t;

int mp3_read_frames(FILE*, mp3_frame_t***);
//mp3_frame_t *mp3_get_frame(FILE *, long int);
mp3_frame_t *mp3_get_frame(FILE*, mp3_frame_t*);
mp3_header_t *mp3_get_header(FILE*);
mp3_side_info_t *mp3_get_side_info(FILE*, mp3_header_t*);
//mp3_main_data_t *mp3_get_main_data(FILE*, mp3_header_t*, mp3_side_info_t*, mp3_header_t*, mp3_side_info_t*);
// TODO: change later
array_t *mp3_get_main_data(FILE*, mp3_header_t*, mp3_side_info_t*, mp3_header_t*, mp3_side_info_t*);

#endif
