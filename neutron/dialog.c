/**
 * @file neutron/dialog.c
 * @brief Neutron dialog framework
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

nt_image_t *temp_icon = NULL;

int nt_dialog_init() {
    // Load icons
    temp_icon = nt_icon_get("missing", NULL, 16);
    return (temp_icon == NULL);
}

nt_dialog_t *nt_dialog_create(nt_window_t *parent, char *dlg_title, size_t width, size_t height) {
    nt_dialog_t *dlg = malloc(sizeof(nt_dialog_t));
    dlg->win = nt_window_create_child(parent, dlg_title, width, height);
    dlg->btn_clicked = NT_DIALOG_BTN_NONE;
    
    // setup initial dialog structure
    // Dialog structure:
    // NT_BOX (vert)
    //      NT_BOX (vert)
    //      NT_SEPARATOR (horiz)
    //      NT_BOX (horiz)

    nt_widget_t *box = nt_box_create_vertical();
    nt_widget_set_expansion(box, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);

    dlg->content_box = nt_box_create_vertical();
    nt_widget_set_expansion(dlg->content_box, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_box_append(box, dlg->content_box);

    nt_widget_t *sep = nt_separator_create(NT_SEPARATOR_HORIZONTAL);
    nt_style_set_margin_all(&sep->style, 0);
    nt_box_append(box, sep);


    dlg->btn_box = nt_box_create_horizontal();
    nt_style_set_margin_all(&dlg->btn_box->style, 2);
    nt_widget_set_expansion(dlg->btn_box, NT_EXPAND_HORIZONTAL);
    nt_box_append(box, dlg->btn_box);

    nt_window_set_root(dlg->win, box);

    return dlg;
}

void nt_dialog_pressed_signal(nt_widget_t *w, nt_signal_t *sig, void *data) {
    nt_dialog_t *dlg = (nt_dialog_t*)data;
    dlg->btn_clicked = (int)(uintptr_t)w->priv; // !!! ugh, to be fixed...
    nt_window_close(nt_widget_get_window(w));
}

nt_widget_t *nt_dialog_add_button(nt_dialog_t *dlg, char *btn_text, int btn_id) {
    nt_widget_t *btn = nt_button_create_label(btn_text);
    if (!dlg->btn_box->children) {
        nt_widget_set_expansion(btn, NT_EXPAND_HORIZONTAL);
        nt_widget_set_horizontal_alignment(btn, NT_ALIGN_RIGHT);
    }
    
    nt_box_append(dlg->btn_box, btn);
    btn->priv = (void*)(uintptr_t)btn_id; // !!! ugh, to be fixed..
    nt_signal_connect(btn, "pressed", nt_dialog_pressed_signal, dlg);
    return btn;
}

nt_dialog_t *nt_dialog_create_message(struct _nt_window *parent, char *dlg_title, char *dlg_subtext, nt_msg_dialog_type_t type, nt_msg_dialog_btns_t buttons) {
    // todo type properly
    nt_dialog_t *dlg = nt_dialog_create(parent, dlg_title, 250, 120);
    
    switch (buttons) {
        case NT_DIALOG_MSG_BTNS_YES_NO:
            nt_dialog_add_button(dlg, "Yes", NT_DIALOG_BTN_YES);
            nt_dialog_add_button(dlg, "No", NT_DIALOG_BTN_NO);
            break;

        case NT_DIALOG_MSG_BTNS_OK:
            nt_dialog_add_button(dlg, "OK", NT_DIALOG_BTN_OK);
            break;

        case NT_DIALOG_MSG_BTNS_CANCEL:
            nt_dialog_add_button(dlg, "Cancel", NT_DIALOG_BTN_CANCEL);
            break;

        case NT_DIALOG_MSG_BTNS_OK_CANCEL:
            nt_dialog_add_button(dlg, "OK", NT_DIALOG_BTN_OK);
            nt_dialog_add_button(dlg, "Cancel", NT_DIALOG_BTN_CANCEL);
            break;

        case NT_DIALOG_MSG_BTNS_CLOSE:
            nt_dialog_add_button(dlg, "Close", NT_DIALOG_BTN_CLOSE);
            break;

        case NT_DIALOG_MSG_BTNS_NONE:
        default:
            break;
    }

    nt_widget_t *lbl = nt_label_create(dlg_subtext);
    nt_widget_set_expansion(lbl,  NT_EXPAND_VERTICAL);
    nt_box_append(dlg->content_box, lbl);

    return dlg;
}

unsigned char nt_dialog_open(nt_dialog_t *dlg) {
    nt_loop_until_exited(dlg->win);
    unsigned char id = dlg->btn_clicked;
    free(dlg); // all widgets already freed
    return id;
}

