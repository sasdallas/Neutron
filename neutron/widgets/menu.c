/**
 * @file neutron/widgets/menu.c
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

#include <neutron/neutron.h>

static void nt_menu_calc_size(nt_widget_t *w) {
    
    w->size_data.min_width = 0;
    w->size_data.min_height = 0;
    w->size_data.pref_width = 0;
    w->size_data.pref_height = 0;

    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > w->size_data.min_width) {
            w->size_data.min_width = child->size_data.min_width;
        }
        if (child->size_data.pref_width > w->size_data.pref_width) {
            w->size_data.pref_width = child->size_data.pref_width;
        }

        w->size_data.min_height += child->size_data.min_height;
        w->size_data.pref_height += child->size_data.pref_height;
    }
    
    w->size_data.min_width += 24;
    w->size_data.min_height += 8;
    w->size_data.pref_width += 24;
    w->size_data.pref_height += 8;
}

static void nt_menu_adjust_size(nt_widget_t *w) {
    int x = nt_widget_get_x_inner(w);
    int cur_y = nt_widget_get_y_inner(w) + 2;
    int width = nt_widget_get_width_inner(w);

    NT_ITERATE_CHILDREN(w) {
        nt_rect_t r = {
            .x = x,
            .y = cur_y,
            .w = width - 4,
            .h = child->size_data.pref_height,
        };

        nt_widget_adjust_size(child, &r);
        cur_y += r.h;
    }
}

static void nt_menu_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_render_fill_rect(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w)-4, nt_widget_get_height_inner(w)-4), NT_COLOR(0xf0,0xf0,0xf0,255));
    nt_render_border_rect(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w)-4, nt_widget_get_height_inner(w)-4), 1, NT_COLOR(0xa8,0xa8,0xa8,255));
    
    int inner_w = nt_widget_get_width_inner(w);
    int inner_h = nt_widget_get_height_inner(w);

    // TODO: seems to produce an odd visual bug
    for (int i = 0; i < 4; ++i) {
        uint8_t alpha = (uint8_t)(16 * (4 - i));

        nt_render_fill_rect(
            surf,
            &NT_RECT(inner_w - 4 + i, 5, 1, inner_h),
            NT_COLOR(0, 0, 0, alpha)
        );

        // bottom side
        nt_render_fill_rect(
            surf,
            &NT_RECT(5, inner_h - 4 + i, inner_w, 1),
            NT_COLOR(0, 0, 0, alpha)
        );
    }
}

nt_widget_vtable_t menu_vtable = {
    .class_size = sizeof(nt_menu_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_menu_calc_size,
    .adjust_size = nt_menu_adjust_size,
    .render = nt_menu_render
};

nt_widget_t *nt_menu_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_MENU, &menu_vtable);
    nt_style_set_margin_all(&w->style, 0);
    nt_style_set_bg_color(&w->style, NT_COLOR(0,0,0,0));
    nt_widget_set_visible(w, false);
    
    return w;
}


void nt_menu_add_item(nt_widget_t *menu, nt_widget_t *item) {
    nt_widget_set_parent_append(item, menu);
}

void nt_menu_show(nt_widget_t *w) {
    nt_widget_set_visible(w, true);
    nt_window_set_popup(nt_widget_get_window(w), w);
}

void nt_menu_show_at(nt_widget_t *w, nt_window_t *window, int x, int y) {
    if (!w || !window) return;
    NT_DEBUG("nt_menu_show_at\n");

    w->window = window;
    nt_widget_calculate_size(w);

    long menu_w = w->size_data.pref_width;
    long menu_h = w->size_data.pref_height;

    if (x + menu_w > (long)window->width) x = (int)((long)window->width - menu_w);
    if (y + menu_h > (long)window->height) y = (int)((long)window->height - menu_h);
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    nt_rect_t rect = {
        .x = (unsigned)x,
        .y = (unsigned)y,
        .w = (unsigned)menu_w,
        .h = (unsigned)menu_h,
    };

    nt_widget_adjust_size(w, &rect);
    nt_widget_set_visible(w, true);
    nt_window_set_popup(window, w);
}

void nt_menu_hide(nt_widget_t *w) {
    nt_widget_set_visible(w, false);
}
