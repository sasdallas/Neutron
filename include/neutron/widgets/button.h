/**
 * @file include/neutron/widgets/button.h
 * @brief Button
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_BUTTON_H
#define _NEUTRON_WIDGETS_BUTTON_H

struct _nt_widget;

typedef enum {
    NT_BTN_STATE_DISABLED,
    NT_BTN_STATE_DEFAULT,
    NT_BTN_STATE_HOVERED,
    NT_BTN_STATE_HELD,
} nt_button_state_t;

typedef struct _nt_button {
    struct _nt_widget widget;
    struct _nt_widget *inner;
    nt_button_state_t state;
} nt_button_t;

struct _nt_widget *nt_button_create(struct _nt_widget *inner);
struct _nt_widget *nt_button_create_label(char *text);

#endif