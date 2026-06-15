/**
 * @file include/neutron/widgets/textarea.h
 * @brief Textarea widget
 *
 *
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 *
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_TEXTAREA_H
#define _NEUTRON_WIDGETS_TEXTAREA_H

#include <stdbool.h>
#include <stddef.h>

struct _nt_widget;
struct _nt_timer;
struct _nt_input_context;

typedef struct _nt_textarea {
	struct _nt_widget widget;
	struct _nt_input_context *input;
	size_t preferred_col;
	struct _nt_timer *timer;
	bool cursor_visible;
	bool editable;
} nt_textarea_t;

struct _nt_widget *nt_textarea_create();
void nt_textarea_set_text(struct _nt_widget *widget, const char *text);
const char *nt_textarea_get_text(struct _nt_widget *widget);
void nt_textarea_append_text(struct _nt_widget *widget, const char *text);
void nt_textarea_set_editable(struct _nt_widget *widget, bool editable);

#endif
