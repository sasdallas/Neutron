/**
 * @file include/neutron/widgets/input.h
 * @brief Neutron input widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_INPUT_H
#define _NEUTRON_WIDGETS_INPUT_H

struct _nt_widget;
struct _nt_input_context;
struct _nt_timer;

typedef struct _nt_input {
    struct _nt_widget widget;
    struct _nt_input_context *input;
    char *placeholder;
    struct _nt_timer *timer;
    bool cursor;
} nt_input_t;

struct _nt_widget *nt_input_create(char *placeholder);
void nt_input_set_text(struct _nt_widget *widget, char *text);
char *nt_input_get_text(struct _nt_widget *widget);
void nt_input_set_max_chars(struct _nt_widget *widget, int max);
void nt_input_set_read_only(struct _nt_widget *widget, bool read_only);

#endif
