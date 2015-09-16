/*
 * mp3_frame.c: definitions for functions that deal with mp3 frames
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

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "bits.h"
#include "meow.h"

#include "mp3_frame.h"

/* debug shit */
static int current_frame = 0;

/* not so sure about this one either */
#include "print_frame.h"

static void free_frame(void *frame) {
//    free(frame->main_data->gr[0].ch); // ...
//    free(frame->main_data->gr[1].ch); //will be used in future
//    free(frame->main_data);           // ...
    free(((mp3_frame_t*)frame)->side_info->ch);
    free(((mp3_frame_t*)frame)->side_info);
    free(((mp3_frame_t*)frame)->header);
    free(frame);
}

/*
 * mp3_read_frames(FILE*,mp3_frame_t**):  reads all frames from a given file,
 *   allocating new memory for the array of frames;
 *
 * return value:  number of frames read from the file;
 */
int mp3_read_frames(FILE *fp, mp3_frame_t ***frames) {
    node_t *first, *last, *iter;
    int i, num_read = 0;
    first = malloc(sizeof(node_t));
    first->value = mp3_get_frame(fp, NULL);
    last = first;
    while (last->value) {
        num_read++;
        /*last = list_insert(last,
                mp3_get_frame(fp, ((mp3_frame_t*)last->value)->header->pos+4L));
         */
        last = list_insert(last, mp3_get_frame(fp, (mp3_frame_t*)last->value));
    }
    *frames = malloc((num_read-1)*sizeof(mp3_frame_t*));
    iter = first;
    i = 0;
    while (iter != last) {
        (*frames)[i] = iter->value;
        i++;
        iter = iter->next;
    }
    list_free(first, &free_frame);
    return i;
}

//mp3_frame_t *mp3_get_frame(FILE *fp, long int at) {
mp3_frame_t *mp3_get_frame(FILE *fp, mp3_frame_t *prev) {
    mp3_frame_t *frame = malloc(sizeof(mp3_frame_t));
    //fseek(fp, at, SEEK_SET);
    if (!prev) {
        rewind(fp);
    } else {
        fseek(fp, prev->header->pos+4L, SEEK_SET);
    }
    frame->header = mp3_get_header(fp);
    if (frame->header) {
        frame->side_info = mp3_get_side_info(fp, frame->header);
        if (frame->side_info) {
            frame->main_data = mp3_get_main_data(fp, prev?prev->header:NULL, prev?prev->side_info:NULL, frame->header, frame->side_info);
            if (!frame->main_data) {
                fprintf(stderr, "EAT ME");
                print_head(frame);
                fclose(stdout);
                free_frame(frame);
                frame = NULL;
            } else {
                print_frame_info(frame, current_frame);
                current_frame++;
            }
        } else {
            free_frame(frame);
            frame = NULL;
        }
    } else {
        free_frame(frame);
        frame = NULL;
    }
    return frame;
}

mp3_header_t *mp3_get_header(FILE *fp) {
    mp3_header_t *head = malloc(sizeof(mp3_header_t));
    uint8_t *tmpbuf = malloc(2*sizeof(uint8_t));
    bitstringstream_t *bss = bss_init(tmpbuf, 2, 0);
    int n_read;
    _Bool found_eof = 1;
    int ch = fgetc(fp);
    while (ch != EOF) {
        if (ch == 0xFF) {
            ch = fgetc(fp);
            if (ch == 0xFB) {
                head->pos = ftell(fp) - 2;
                n_read = fread(tmpbuf, 1, 2, fp);
                if (n_read == 2) {
                    head->bitrate = getnbits(bss, 4);
                    head->sampling_freq = getnbits(bss, 2);
                    head->padding_bit = getbit(bss);
                    head->private_bit = getbit(bss);
                    head->mode = getnbits(bss, 2);
                    head->mode_ext = getnbits(bss, 2);
                    head->copyright = getbit(bss);
                    head->original = getbit(bss);
                    head->emphasis = getnbits(bss, 2);
                    found_eof = 0;
                    break;
                } else
                    break;
            } else
                ch = fgetc(fp);
        } else
            ch = fgetc(fp);
    }
    free(bss);
    free(tmpbuf);
    if (found_eof) {
        free(head);
        head = NULL;
    } else if (head->sampling_freq >= 3 || head->bitrate >= 15) {
        free(head);
        /* good heavens this is crap */
        return mp3_get_header(fp);
    }
    return head;
}

mp3_side_info_t *mp3_get_side_info(FILE *fp, mp3_header_t *head) {
    mp3_side_info_t *si = malloc(sizeof(mp3_side_info_t));
    uint8_t *side_info_bytes;
    bitstringstream_t *bss;
    size_t n_read;
    unsigned int n_ch, i, j, k;
    fseek(fp, head->pos + (long)4, SEEK_SET);
    if (head->mode == 3) {
        n_ch = 1;
        side_info_bytes = malloc(17*sizeof(uint8_t));
        n_read = fread(side_info_bytes, 1, 17, fp);
        if (n_read != 17) {
            return NULL;
        }
    } else {
        n_ch = 2;
        side_info_bytes = malloc(32*sizeof(uint8_t));
        n_read = fread(side_info_bytes, 1, 32, fp);
        if (n_read != 32) {
            return NULL;
        }
    }
    bss = bss_init(side_info_bytes, n_read, 0);
    si->ch = malloc(n_ch*sizeof(si_ch_t));
    si->main_data_end = getnbits(bss, 9);
    si->private_bits = n_ch==1 ? getnbits(bss, 5) : getnbits(bss, 3);
    for (i=0; i<n_ch; i++) {
        for (j=0; j<4; j++) {
            si->ch[i].scfsi[j] = getbit(bss);
        }
    }
    for (j=0; j<2; j++) {
        for (i=0; i<n_ch; i++) {
            si->ch[i].gr[j].part2_3_length = getnbits(bss, 12);
            si->ch[i].gr[j].big_values = getnbits(bss, 9);
            si->ch[i].gr[j].global_gain = getnbits(bss, 8);
            si->ch[i].gr[j].scalefac_compress = getnbits(bss, 4);
            si->ch[i].gr[j].blocksplit_flag = getbit(bss);
            if (si->ch[i].gr[j].blocksplit_flag) {
                si->ch[i].gr[j].block_type = getnbits(bss, 2);
                si->ch[i].gr[j].switch_point = getbit(bss);
                /* iterating on regions */
                for (k=0; k<2; k++) {
                    si->ch[i].gr[j].table_select[k] = getnbits(bss, 5);
                }
                /* iterating on windows */
                for (k=0; k<3; k++) {
                    si->ch[i].gr[j].subblock_gain[k] = getnbits(bss, 3);
                }
            } else {
                /* iterating on regions */
                for (k=0; k<3; k++) {
                    si->ch[i].gr[j].table_select[k] = getnbits(bss, 5);
                }
                si->ch[i].gr[j].region_address1 = getnbits(bss, 4);
                si->ch[i].gr[j].region_address2 = getnbits(bss, 3);
            }
            si->ch[i].gr[j].preflag = getbit(bss);
            si->ch[i].gr[j].scalefac_scale = getbit(bss);
            si->ch[i].gr[j].count1table_select = getbit(bss);
        }
    }
    free(bss);
    free(side_info_bytes);
    return si;
}

//mp3_main_data_t *mp3_get_main_data(FILE *fp, mp3_header_t *head_prev, mp3_side_info_t *si_prev, mp3_header_t *head_this, mp3_side_info_t *si_this, mp3_header_t *head_next) {
// TODO: fix this to return main_data_t, not the byte array thingy
// TODO: that is, decode the byte array we get and produce main_data
array_t *mp3_get_main_data(FILE *fp, mp3_header_t *head_prev, mp3_side_info_t *si_prev, mp3_header_t *head_this, mp3_side_info_t *si_this) {

#ifdef MAINDATA

    mp3_main_data_t *main_data = malloc(sizeof(mp3_main_data_t));
    bitstringstream_t *bss;
    int n_ch = head_this->mode==3?1:2;
    main_data->gr[0].ch = malloc(n_ch*sizeof(md_ch_t));
    main_data->gr[1].ch = malloc(n_ch*sizeof(md_ch_t));
    int slen1, slen2, switch_point_s, switch_point_l, i, j, k, l;
    _Bool short_blockp;
    size_t n_read;
    uint8_t *md_buffer;
    long start_at, end_at;
    mp3_header_t *head_next;
    if (!head_prev) {
        start_at = head_this->pos+4+(n_ch==1?17:32);
    } else {
        if (si_prev->main_data_end==0) {
            start_at = head_this->pos+4+(n_ch==1?17:32);
        } else {
            start_at = head_this->pos-si_prev->main_data_end+1;
        }
    }
    fseek(fp, head_this->pos+4L, SEEK_SET);
    head_next = mp3_get_header(fp);
    if (head_next) {
        end_at = head_next->pos-si_this->main_data_end;
    } else {
        int ch;
        fseek(fp, start_at, SEEK_SET);
        ch = fgetc(fp);
        while (ch!=EOF) {
            ch = fgetc(fp);
        }
        end_at = ftell(fp);
    }
    fseek(fp, start_at, SEEK_SET);
    md_buffer = malloc(end_at-start_at);
    if (!md_buffer) {
        perror("\nTROUBLE");
        fprintf(stderr, "TROUBLE: end_at - start_at == %ld\n", end_at-start_at);
        return NULL;
    }
    n_read = fread(md_buffer, 1, end_at-start_at, fp);
    array_t *meow = malloc(sizeof(array_t));
    meow->arr = md_buffer;
    meow->length = n_read;

    /*
     * stuff below is just a template -- it doesn't read anything atm
     */

    /* iterating on granules */
    for (i=0; i<2; i++) {
        /* iterating on channels */
        for (j=0; j<n_ch; j++) {
            short_blockp = si_this->ch[i].gr[j].blocksplit_flag && si_this->ch[i].gr[j].block_type == 2;
            switch (si_this->ch[j].gr[i].scalefac_compress) {
                case 0: slen1=0; slen2=0; break;
                case 1: slen1=0; slen2=1; break;
                case 2: slen1=0; slen2=2; break;
                case 3: slen1=0; slen2=3; break;
                case 4: slen1=3; slen2=0; break;
                case 5: slen1=1; slen2=1; break;
                case 6: slen1=1; slen2=2; break;
                case 7: slen1=1; slen2=3; break;
                case 8: slen1=2; slen2=1; break;
                case 9: slen1=2; slen2=2; break;
                case 10: slen1=2; slen2=3; break;
                case 11: slen1=3; slen2=1; break;
                case 12: slen1=3; slen2=2; break;
                case 13: slen1=3; slen2=3; break;
                case 14: slen1=4; slen2=2; break;
                case 15: slen1=4; slen2=3; break;
            }
            switch (si_this->ch[i].gr[j].switch_point) {
                case 0: switch_point_l=0; switch_point_s=0; break;
                case 1: switch_point_l=8; switch_point_s=3; break;
            }
            if (short_blockp) {
                for (k=0; k<switch_point_l; k++) {
//                    if (si->ch[i].scfsi[k]
                }
                for (k=switch_point_s; k<12; k++) {
                    /* iterate on windows */
                    for (l=0; l<3; l++) {
//                    if (si->ch[i].scfsi[k]
                    }
                }
            } else {
                for (k=0; k<21; k++) {
//                    if (si->ch[i].scfsi[k]==0 || gr==0) {
//                    }
                }
            }
        }
    }
    return meow;

#else
    array_t *meow = malloc(sizeof(array_t));
    meow->length=0;
    return meow;
#endif
}

/* taken from MPGAUDIO.pdf:
for (gr=0; gr<2; gr++) {
    for (ch=0; ch<n_ch; ch++) {
        if (blocksplit_flag[gr] == 1 && block_type[gr] == 2) {
            for (cb=0; cb<switch_point_l[gr]; cb++) {
                if (scfsi[cb]==0) || (gr==0) {
                    scalefac[cb][gr];
                }
            }
            for (cb=switch_point_s[gr]; cb<cblimit_short; cb++) {
                for (window=0; window<3; window++) {
                    if (scfsi[cb]==0) || (gr==0) {
                        scalefac[cb][window][gr];
                    }
                }
            }
        } else {
            for (cb=0; cb<cblimit; cb++) {
                if (scfsi[cb]==0) || (gr==0) {
                    scalefac[cb][gr];
                }
            }
        }
        Huffmancodebits;
        while (position != main_data_end) {
            ancillary_bit;
        }
    }
}
*/
