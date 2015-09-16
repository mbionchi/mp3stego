/*
 * list.h:  structure definitions for a linked list node + function headers
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

#ifndef MP3STEGO_LIST_H
#define MP3STEGO_LIST_H

struct node {
    void *value;
    struct node *next;
};
typedef struct node node_t;

node_t *list_insert(node_t*, void*);
void list_free(node_t*,void (*)(void*));

#endif
