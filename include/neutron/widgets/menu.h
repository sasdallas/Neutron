/**
 * @file include/neutron/widgets/menu.h
 * @brief Menu widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_MENU_H
#define _NEUTRON_WIDGETS_MENU_H

struct _nt_widget;
struct _nt_window;
typedef struct _nt_menu {
    struct _nt_widget widget;
} nt_menu_t;

#define NT_MENU(w) ((nt_menu_t*)w)

struct _nt_widget *nt_menu_create();
void nt_menu_add_item(struct _nt_widget *menu, struct _nt_widget *item);

void nt_menu_show(struct _nt_widget *menu);
void nt_menu_show_at(struct _nt_widget *menu, struct _nt_window *window, int x, int y);
void nt_menu_hide(struct _nt_widget *menu);

#endif
