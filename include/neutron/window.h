/**
 * @file include/neutron/window.h
 * @brief Window components
 * 
 * Neutron supports sub-windows and creating main windows
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WINDOW_H
#define _NEUTRON_WINDOW_H

#include <stddef.h>

struct _nt_clip;

typedef struct _nt_window {
    struct _nt_window *parent;
    struct _nt_window *next;

    char *title;
    size_t width;
    size_t height;

    struct _nt_clip *clip;
    nt_widget_t *root_frame;
    nt_widget_t *focused;
    nt_widget_t *popup; // the popup is always drawn after all widgets have drawn


    bool exited;
    void *platform;
} nt_window_t;

nt_window_t *nt_window_create(char *title, size_t width, size_t height);
nt_window_t *nt_window_create_undecorated(size_t width, size_t height);
nt_window_t *nt_window_create_child(nt_window_t *parent, char *title, size_t width, size_t height);
void nt_window_set_pos(nt_window_t *win, int x, int y);
void nt_window_set_root(nt_window_t *win, nt_widget_t *widget);
nt_window_t *nt_window_list();
void nt_window_update(nt_window_t *win);
void nt_window_closed(nt_window_t *win);
void nt_window_set_visible(nt_window_t *win, bool visible);
void nt_window_set_clip(nt_window_t *win, nt_clip_t *clip);

void nt_window_close(nt_window_t *win);
void nt_window_set_focus(nt_window_t *win, nt_widget_t *widget);
void nt_window_set_popup(nt_window_t *win, nt_widget_t *widget);

static inline nt_widget_t *nt_window_get_focus(nt_window_t *win) {
    return win->focused;
}

#endif