#include "mp3_frame.h"
#include "print_frame.h"

#define TAB "...."

void print_frame_info(mp3_frame_t *frame, int n) {
    int n_ch, i, j, k;
    n_ch = (frame->header->mode == 3)?1:2;
    fprintf(stdout, "FRAME[%d] AT POSITION %ld:\n", n, frame->header->pos);

    fprintf(stdout, "%sHEADER:\n", TAB);

    fprintf(stdout, "%s%sbitrate: %d\n", TAB, TAB, frame->header->bitrate);
    fprintf(stdout, "%s%ssampling_freq: %d\n", TAB, TAB, frame->header->sampling_freq);
    fprintf(stdout, "%s%spadding_bit: %d\n", TAB, TAB, frame->header->padding_bit);
    fprintf(stdout, "%s%sprivate_bit: %d\n", TAB, TAB, frame->header->private_bit);
    fprintf(stdout, "%s%smode: %d\n", TAB, TAB, frame->header->mode);
    fprintf(stdout, "%s%smode_ext: %d\n", TAB, TAB, frame->header->mode_ext);
    fprintf(stdout, "%s%scopyright: %d\n", TAB, TAB, frame->header->copyright);
    fprintf(stdout, "%s%soriginal: %d\n", TAB, TAB, frame->header->original);
    fprintf(stdout, "%s%semphasis: %d\n", TAB, TAB, frame->header->emphasis);

    fprintf(stdout, "%sSIDE INFO:\n", TAB);

    fprintf(stdout, "%s%smain_data_end: %d\n", TAB, TAB, frame->side_info->main_data_end);
    fprintf(stdout, "%s%sprivate_bits: %d\n", TAB, TAB, frame->side_info->private_bits);

    for (i=0; i<n_ch; i++) {
        fprintf(stdout, "%s%schannel[%d]:\n", TAB, TAB, i);
        for (j=0; j<4; j++) {
            fprintf(stdout, "%s%s%sscfsi[%d]: %d\n", TAB, TAB, TAB, j, frame->side_info->ch[i].scfsi[j]);
        }
        for (j=0; j<2; j++) {
            fprintf(stdout, "%s%s%sgranule[%d]:\n", TAB, TAB, TAB, j);
            fprintf(stdout, "%s%s%s%spart2_3_length: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].part2_3_length);
            fprintf(stdout, "%s%s%s%sbig_values: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].big_values);
            fprintf(stdout, "%s%s%s%sglobal_gain: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].global_gain);
            fprintf(stdout, "%s%s%s%sscalefac_compress: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].scalefac_compress);
            fprintf(stdout, "%s%s%s%sblocksplit_flag: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].blocksplit_flag);
            if (frame->side_info->ch[i].gr[j].blocksplit_flag) {
                fprintf(stdout, "%s%s%s%sblock_type: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].block_type);
                fprintf(stdout, "%s%s%s%sswitch_point: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].switch_point);
                for (k=0; k<2; k++) {
                    fprintf(stdout, "%s%s%s%stable_select[%d]: %d\n", TAB, TAB, TAB, TAB, k, frame->side_info->ch[i].gr[j].table_select[k]);
                }
                for (k=0; k<3; k++) {
                    fprintf(stdout, "%s%s%s%ssubblock_gain[%d]: %d\n", TAB, TAB, TAB, TAB, k, frame->side_info->ch[i].gr[j].subblock_gain[k]);
                }
            } else {
                for (k=0; k<3; k++) {
                    fprintf(stdout, "%s%s%s%stable_select[%d]: %d\n", TAB, TAB, TAB, TAB, k, frame->side_info->ch[i].gr[j].table_select[k]);
                }
                fprintf(stdout, "%s%s%s%sregion_address1: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].region_address1);
                fprintf(stdout, "%s%s%s%sregion_address2: %d\n", TAB, TAB, TAB, TAB, frame->side_info->ch[i].gr[j].region_address2);
            }
            fprintf(stdout, "%s%s%spreflag: %d\n", TAB, TAB, TAB, frame->side_info->ch[i].gr[j].preflag);
            fprintf(stdout, "%s%s%sscalefac_scale: %d\n", TAB, TAB, TAB, frame->side_info->ch[i].gr[j].scalefac_scale);
            fprintf(stdout, "%s%s%scount1table_select: %d\n", TAB, TAB, TAB, frame->side_info->ch[i].gr[j].count1table_select);
        }
    }
    fprintf(stdout, "%sMAIN DATA:\n", TAB);
    fprintf(stdout, "%s%sNUM_BYTES: %d\n", TAB, TAB, frame->main_data->length);
    fprintf(stdout, "%s%sBYTES:", TAB, TAB);
    for (i=0; i<frame->main_data->length; i++) {
        fprintf(stdout, " ");
        if (frame->main_data->arr[i]<0x10) {
            fprintf(stdout, "0");
        }
        fprintf(stdout, "%X", frame->main_data->arr[i]);
    }
    fprintf(stdout, "\n");
}
