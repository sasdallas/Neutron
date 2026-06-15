/**
 * @file include/neutron/widgets/checkbox.h
 * @brief Checkbox
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_CHECKBOX_H
#define _NEUTRON_WIDGETS_CHECKBOX_H

struct _nt_widget;

typedef struct _nt_checkbox {
    struct _nt_widget widget;
    struct _nt_widget *child;
    bool held;
    bool checked;
    bool disabled;
} nt_checkbox_t;

struct _nt_widget *nt_checkbox_create(struct _nt_widget *child, bool default_state);
struct _nt_widget *nt_checkbox_create_labelled(char *text, bool default_state);
void nt_checkbox_set_state(struct _nt_widget *widget, bool state);

#endif
