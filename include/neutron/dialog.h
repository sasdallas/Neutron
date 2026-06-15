/**
 * @file include/neutron/dialog.h
 * @brief Neutron dialog system
 * 
 * Built-in set of Neutron dialogs
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_DIALOG_H
#define _NEUTRON_DIALOG_H

#include <stddef.h>

struct _nt_window;
struct _nt_widget;

typedef enum {
    NT_DIALOG_MSG_TYPE_INFO,
    NT_DIALOG_MSG_TYPE_WARNING,
    NT_DIALOG_MSG_TYPE_ERROR,
    NT_DIALOG_MSG_TYPE_QUESTION,
    NT_DIALOG_MSG_TYPE_OTHER
} nt_msg_dialog_type_t;

// default button ids
#define NT_DIALOG_BTN_NONE      0
#define NT_DIALOG_BTN_OK        1
#define NT_DIALOG_BTN_YES       2
#define NT_DIALOG_BTN_NO        3
#define NT_DIALOG_BTN_CANCEL    4
#define NT_DIALOG_BTN_CLOSE     5

typedef enum {
    NT_DIALOG_MSG_BTNS_NONE,
    NT_DIALOG_MSG_BTNS_YES_NO,
    NT_DIALOG_MSG_BTNS_OK,
    NT_DIALOG_MSG_BTNS_CANCEL,
    NT_DIALOG_MSG_BTNS_OK_CANCEL,
    NT_DIALOG_MSG_BTNS_CLOSE, 
} nt_msg_dialog_btns_t;

typedef struct _nt_dialog {
    struct _nt_window *win;
    struct _nt_widget *content_box; // vertical box
    struct _nt_widget *btn_box; // horizontal box
    unsigned char btn_clicked;
} nt_dialog_t;

int nt_dialog_init();
nt_dialog_t *nt_dialog_create(struct _nt_window *parent, char *dlg_title, size_t width, size_t height);
nt_dialog_t *nt_dialog_create_message(struct _nt_window *parent, char *dlg_title, char *dlg_subtext, nt_msg_dialog_type_t type, nt_msg_dialog_btns_t buttons);
nt_widget_t *nt_dialog_add_button(nt_dialog_t *dlg, char *btn_text, int btn_id);

// blocks until dialog is closed
unsigned char nt_dialog_open(nt_dialog_t *dlg);

#endif
