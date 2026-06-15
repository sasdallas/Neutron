/**
 * @file include/neutron/widgets/menubar.h
 * @brief Menu bar
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NT_WIDGETS_MENUBAR_H
#define _NT_WIDGETS_MENUBAR_H

struct _nt_widget;

typedef struct _nt_menubar_entry {
    struct _nt_widget widget;
    struct _nt_widget *menu;
    struct _nt_widget *label;
    bool selected;
} nt_menubar_entry_t;

typedef struct _nt_menubar {
    struct _nt_widget widget;
    bool mouse_sel; // hack
} nt_menubar_t;

struct _nt_widget *nt_menubar_create();
void nt_menubar_append(struct _nt_widget *menubar, char *text, struct _nt_widget *menu);

#endif
