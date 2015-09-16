/*
 * list.c:  a linked list implementation, can be used with any data type
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

#include <stdlib.h>

#include "list.h"

node_t *list_insert(node_t *at, void *val) {
    node_t *node = malloc(sizeof(node_t));
    node->value = val;
    node->next = NULL;
    at->next = node;
    return node;
}

void list_free(node_t *begin, void (*free_item)(void *ptr)) {
    node_t *tmp;
    while (begin != NULL) {
        tmp = begin;
        begin = begin->next;
        (*free_item)(tmp->value);
        free(tmp);
    }
}
