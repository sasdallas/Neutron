/**
 * @file include/neutron/widgets/tab_view.h
 * @brief Tab view widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_TAB_VIEW_H
#define _NEUTRON_WIDGETS_TAB_VIEW_H

#include <stdbool.h>
#include <stddef.h>

struct _nt_widget;

typedef struct _nt_tab_view {
    struct _nt_widget widget;
    struct _nt_widget *selected;
    size_t tab_count;
    size_t header_height;
} nt_tab_view_t;

struct _nt_widget *nt_tab_view_create();
void nt_tab_view_append(struct _nt_widget *tab_view, struct _nt_widget *tab);
void nt_tab_view_select(struct _nt_widget *tab_view, struct _nt_widget *tab);
void nt_tab_view_select_index(struct _nt_widget *tab_view, size_t index);
struct _nt_widget *nt_tab_view_get_selected(struct _nt_widget *tab_view);
struct _nt_widget *nt_tab_view_get_tab(struct _nt_widget *tab_view, size_t index);
size_t nt_tab_view_get_tab_count(struct _nt_widget *tab_view);


#endif
