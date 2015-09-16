/*
 * dissect: a dirty tool to display what's inside an mp3 file
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

#include "mp3_frame.h"

//TODO: get rid of this
#include "meow.h"
#include "print_frame.h"

#define TAB "...."

void print_frame_info(mp3_frame_t*,int);

int main(int argc, char **argv) {
    if (argc >= 2) {
        mp3_frame_t **frames;
        FILE *mp3_file = fopen(argv[1], "r");
        int n_read = mp3_read_frames(mp3_file, &frames);
        if (n_read > 0) {
            int i;
            printf("Read %d frames!\n", n_read);
            if (argc == 3) {
                i = atoi(argv[2]);
                if (i > n_read) {
                    fprintf(stderr, "No such frame!\n");
                } else {
                    print_frame_info(frames[i], i);
                    return 0;
                }
            } else {
                for (i=0; i<n_read; i++) {
                    print_frame_info(frames[i], i);
                }
                return 0;
            }
        } else {
            fprintf(stderr, "File %s doesn't seem to be an mp3 file!\n", argv[1]);
            return 1;
        }
    }
    return 1;
}
