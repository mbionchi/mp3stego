#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "bits.h"

#include "mp3_frame.h"

static void free_frame(void *frame) {
/*  free(frame->main_data->gr[0].ch);
    free(frame->main_data->gr[1].ch);
    free(frame->main_data);*/
    free(((mp3_frame_t*)frame)->side_info->ch);
    free(((mp3_frame_t*)frame)->side_info);
    free(((mp3_frame_t*)frame)->header);
    free(frame);
}

int mp3_read_frames(FILE *fp, mp3_frame_t **frames) {
    node_t *first, *prev, *iter;
    mp3_frame_t *frame, *f_iter;
    int num_read = 0;
    first = malloc(sizeof(node_t));
    prev = NULL;
    iter = first;
    frame = mp3_get_frame(fp, NULL);
    while (frame) {
        num_read++;
        prev = append(&iter, frame);
        frame = mp3_get_frame(fp, (mp3_frame_t*)prev->val);
    }
    prev->next = NULL;
    free(iter);
    *frames = malloc(num_read*sizeof(mp3_frame_t));
    f_iter = *frames;
    iter = first;
    while (iter) {
        *f_iter = *(mp3_frame_t*)iter->val;
        f_iter++;
        iter = iter->next;
    }
    free_list(first, free_frame);
    return num_read;
}

mp3_frame_t *mp3_get_frame(FILE *fp, mp3_frame_t *prev) {
    mp3_frame_t *frame = malloc(sizeof(mp3_frame_t));
    if (!prev) {
        rewind(fp);
    } else {
        fseek(fp, prev->header->pos+4L, SEEK_SET);
    }
    frame->header = mp3_get_header(fp);
    if (frame->header) {
        frame->side_info = mp3_get_side_info(fp, frame->header);
        if (!frame->side_info) {
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
