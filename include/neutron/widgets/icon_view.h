/**
 * @file include/neutron/widgets/icon_view.h
 * @brief Icon view
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_ICON_VIEW_H
#define _NEUTRON_WIDGETS_ICON_VIEW_H

struct _nt_widget;
struct _nt_image;

// this should really be something equivalent to GtkListStore
typedef struct _nt_icon_entry {
    struct _nt_icon_entry *next;
    char *name;
    struct _nt_image *icon;
    void *priv;
} nt_icon_entry_t;

typedef struct _nt_icon_store {
    int nentries;
    nt_icon_entry_t *head;
    nt_icon_entry_t *tail;
} nt_icon_store_t;

typedef struct _nt_icon_view {
    struct _nt_widget widget;
    nt_icon_store_t *store;
} nt_icon_view_t;

struct _nt_widget *nt_icon_view_create();
void nt_icon_view_set_store(struct _nt_widget *icon_view, nt_icon_store_t *store);

nt_icon_store_t *nt_icon_store_create();
void nt_icon_store_append(nt_icon_store_t *store, char *name, struct _nt_image *icon, void *priv);

#endif
