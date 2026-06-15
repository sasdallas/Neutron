/**
 * @file include/neutron/input.h
 * @brief Handles text management systems, such as input buffers and whatnot.
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_INPUT_H
#define _NEUTRON_INPUT_H

#include <stddef.h>

struct _nt_event;

typedef struct _nt_input_context {
    char *buffer;
    size_t length;
    size_t capacity;
    size_t cursor;
    int limit;
} nt_input_context_t;

nt_input_context_t *nt_input_context_create();
void nt_input_context_process(nt_input_context_t *context, struct _nt_event *event);
void nt_input_context_destroy(nt_input_context_t *context);

void nt_input_context_set_text(nt_input_context_t *context, const char *text, size_t len);
void nt_input_context_insert_text(nt_input_context_t *context, const char *text, size_t len);
void nt_input_context_delete_range(nt_input_context_t *context, size_t begin, size_t end);
void nt_input_context_set_cursor(nt_input_context_t *context, size_t pos);
void nt_input_context_ensure_capacity(nt_input_context_t *context, size_t needed);

static inline size_t nt_input_context_get_length(nt_input_context_t *context) {
    return context->length;
}

static inline void nt_input_context_set_limit(nt_input_context_t *context, int limit) {
    context->limit = limit;
}

#endif
