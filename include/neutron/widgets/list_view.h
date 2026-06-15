/**
 * @file include/neutron/widgets/list_view.h
 * @brief Neutron list view
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_LIST_VIEW_H
#define _NEUTRON_WIDGETS_LIST_VIEW_H

struct _nt_widget;

typedef struct _nt_list_item {
    struct _nt_widget widget;
    struct _nt_widget *child;
} nt_list_item_t;

typedef struct _nt_list_view {
    struct _nt_widget widget;
} nt_list_view_t;

struct _nt_widget *nt_list_view_create();
void nt_list_view_append(struct _nt_widget *view, struct _nt_widget *to_append);

#endif
