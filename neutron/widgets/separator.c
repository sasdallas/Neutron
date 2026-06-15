/**
 * @file neutron/widgets/separator.c
 * @brief Separator
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

#define NT_SEPARATOR(w) ((nt_separator_t*)w)

void nt_separator_calc_size(nt_widget_t *w) {
    w->size_data.min_width = 1;
    w->size_data.pref_width = 1;
    w->size_data.min_height = 1;
    w->size_data.pref_height = 1;
}

void nt_separator_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_separator_t *sep = NT_SEPARATOR(w);
    nt_rect_t r = NT_RECT(w->style.padding[LEFT], w->style.padding[TOP], sep->is_vert ? 1 : nt_widget_get_width_inner(w), sep->is_vert ? nt_widget_get_height_inner(w) : 1);
    nt_render_fill_rect(surf, &r, w->style.fg);
}

nt_widget_vtable_t separator_vtable = {
    .class_size = sizeof(nt_separator_t),
    .calc_size = nt_separator_calc_size,
    .adjust_size = NULL,
    .init = NULL,
    .free = NULL,
    .render = nt_separator_render
};

nt_widget_t *nt_separator_create(int orientation) {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_SEPARATOR, &separator_vtable);
    nt_style_set_fg_color(&w->style, NT_COLOR(0xa2,0xa6,0xa2,0xff));
    nt_separator_t *sep = NT_SEPARATOR(w);
    sep->is_vert = (orientation == NT_SEPARATOR_VERTICAL);
    nt_widget_set_expansion(w, (sep->is_vert) ? NT_EXPAND_VERTICAL : NT_EXPAND_HORIZONTAL);
    return w;
}

