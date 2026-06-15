/**
 * @file include/neutron/widgets/label.h
 * @brief Label widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_LABEL_H
#define _NEUTRON_WIDGETS_LABEL_H


typedef struct _nt_widget_label {
    struct _nt_widget widget;
    char *text;
    nt_font_handle_t *font;    
} nt_label_t;

nt_widget_t *nt_label_create(char *text);
void nt_label_set_text(nt_widget_t *w, char *text);
char *nt_label_get_text(nt_widget_t *w);

#endif