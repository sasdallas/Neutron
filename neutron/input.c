/**
 * @file neutron/input.c
 * @brief Generic neutron input handler
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#include <neutron/neutron.h>
#include <string.h>

nt_input_context_t *nt_input_context_create() {
    nt_input_context_t *resp = malloc(sizeof(nt_input_context_t));
    resp->buffer = malloc(32);
    resp->length = 0;
    resp->capacity = 32;
    resp->cursor = 0;
    resp->limit = -1;
    resp->buffer[0] = 0;
    return resp;
}

static int nt_input_context_process_special(nt_input_context_t *context, nt_key_t key) {
    if (key == NT_KEY_BACKSPACE) {
        if (context->cursor > 0) {
            nt_input_context_delete_range(context, context->cursor - 1, context->cursor);
        }
        return 1;
    }

    return 0;
}

void nt_input_context_process(nt_input_context_t *context, nt_event_t *event) {
    if (event->type != NT_EVENT_KEY_DOWN) return;

    NT_DEBUG("nt_input_context_process %s\n", event->key.utf);
    if (nt_input_context_process_special(context, event->key.key)) return;

    const char *utf = event->key.utf;
    if (!utf || utf[0] == '\0') return;

    for (const unsigned char *p = (const unsigned char *)utf; *p; ++p) {
        if (*p > 0x7F) {
            printf("unsupported\n");
            return;
        }
    }

    size_t n = strlen(utf);

    if (context->limit >= 0 && context->length + n > (size_t)context->limit) {
        n = (size_t)context->limit - context->length;
    }
    if (n == 0) return;

    nt_input_context_insert_text(context, utf, n);
}

void nt_input_context_destroy(nt_input_context_t *context) {
    free(context->buffer);
    free(context);
}

void nt_input_context_ensure_capacity(nt_input_context_t *context, size_t needed) {
    if (context->capacity >= needed) {
        return;
    }

    size_t new_capacity = context->capacity;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }

    context->buffer = realloc(context->buffer, new_capacity);
    context->capacity = new_capacity;
}

void nt_input_context_insert_text(nt_input_context_t *context, const char *text, size_t len) {
    if (!text || len == 0) {
        return;
    }

    if (context->limit >= 0 && context->length + len > (size_t)context->limit) {
        len = (size_t)context->limit - context->length;
    }
    if (len == 0) {
        return;
    }

    nt_input_context_ensure_capacity(context, context->length + len + 1);
    memmove(context->buffer + context->cursor + len, context->buffer + context->cursor, context->length - context->cursor + 1);
    memcpy(context->buffer + context->cursor, text, len);
    context->cursor += len;
    context->length += len;
}

void nt_input_context_set_text(nt_input_context_t *context, const char *text, size_t len) {
    context->cursor = len;
    context->length = len;
    
    free(context->buffer);
    context->capacity = len;
    context->buffer = malloc(len+1);
    memcpy(context->buffer, text, len);
    context->buffer[len] = 0;
}

void nt_input_context_delete_range(nt_input_context_t *context, size_t begin, size_t end) {
    if (begin >= end || end > context->length) {
        return;
    }

    memmove(context->buffer + begin, context->buffer + end, context->length - end + 1);
    context->length -= end - begin;

    if (context->cursor > end) {
        context->cursor -= end - begin;
    } else if (context->cursor > begin) {
        context->cursor = begin;
    }
}

void nt_input_context_set_cursor(nt_input_context_t *context, size_t pos) {
    if (pos > context->length) {
        pos = context->length;
    }
    context->cursor = pos;
}