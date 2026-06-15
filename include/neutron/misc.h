/**
 * @file include/neutron/misc.h
 * @brief Misc utility structures
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_MISC_H
#define _NEUTRON_MISC_H

typedef struct _nt_rect {
    unsigned x;
    unsigned y;
    unsigned w;
    unsigned h;
} nt_rect_t;

#define NT_ITERATE_WINDOWS(win) for (nt_window_t *win = nt_window_list(); win; win = win->next)
#define NT_ITERATE_CHILDREN(wdgt) for (nt_widget_t *child = (wdgt)->children; child; child = child->next)
#define NT_ITERATE_TIMERS(timer) for (nt_timer_t *timer = nt_timer_get_list(); timer; timer = timer->next)
#define NT_RECT(_x,_y,_w,_h) ((nt_rect_t){.x = (_x), .y = (_y), .w = (_w), .h = (_h)})

#define NT_WIDGET_RECT(wdgt) NT_RECT(wdgt->layout_data.x, wdgt->layout_data.y, wdgt->layout_data.w, wdgt->layout_data.h)

#endif