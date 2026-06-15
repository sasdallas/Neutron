/**
 * @file include/neutron/widgets/menu_item.h
 * @brief Menu item widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_MENU_ITEM_H
#define _NEUTRON_WIDGETS_MENU_ITEM_H

struct _nt_image;
struct _nt_widget;
typedef enum {
    NT_MENU_ITEM_DEFAULT,
    NT_MENU_ITEM_SUBMENU,
    NT_MENU_ITEM_SEPARATOR,
} nt_menu_item_type_t;

typedef struct _nt_menu_item {
    struct _nt_widget widget;
    
    nt_menu_item_type_t type;
    char *text;
    struct _nt_widget *submenu;
    struct _nt_image *img;
} nt_menu_item_t;

struct _nt_widget *nt_menu_item_create_button(char *text);
struct _nt_widget *nt_menu_item_create_separator();
struct _nt_widget *nt_menu_item_create_submenu(char *text, struct _nt_widget *submenu);
struct _nt_widget *nt_menu_item_create_button_icon(char *text, struct _nt_image *icon);
struct _nt_widget *nt_menu_item_create_submenu_icon(char *text, struct _nt_widget *submenu, struct _nt_image *icon);

#define nt_menu_item_create_button_icon_cb(lbl, icon, cb, data) ({ struct _nt_widget *wdgt = nt_menu_item_create_button_icon((lbl), (icon)); if (wdgt) { nt_signal_connect(wdgt, "pressed", (cb), (data)); }; wdgt; })
#define nt_menu_item_create_button_cb(lbl, cb, data)  nt_menu_item_create_button_icon_cb(lbl, NULL, cb, data)


#endif
