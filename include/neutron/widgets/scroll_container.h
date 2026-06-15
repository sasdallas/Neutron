/**
 * @file include/neutron/widgets/scroll_container.h
 * @brief Scroll container 
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_SCROLL_CONTAINER_H
#define _NEUTRON_WIDGETS_SCROLL_CONTAINER_H

#include <stdbool.h>

struct _nt_widget;
typedef struct _nt_scroll_container {
    struct _nt_widget widget;
    int scroll_x;
    int scroll_y;
    int content_w;
    int content_h;
    bool h_bar, v_bar;
} nt_scroll_container_t;

struct _nt_widget *nt_scroll_container_create();
void nt_scroll_container_set_child(struct _nt_widget *container, struct _nt_widget *child);
void nt_scroll_container_set_h_bar(struct _nt_widget *container, bool h_bar);
void nt_scroll_container_set_v_bar(struct _nt_widget *container, bool v_bar); 
void nt_scroll_container_set_scroll(struct _nt_widget *container, int scroll_x, int scroll_y);

#endif
