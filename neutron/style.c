/**
 * @file neutron/style.c
 * @brief Neutron style manager
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
#include <string.h>

nt_style_t default_style = {
    .fg = NT_COLOR(0, 0, 0, 255),
    .bg = NT_COLOR(0xfa, 0xfa, 0xfa, 255),
    .font = NULL,
    .margin = { 8, 8, 8, 8 },
    .padding = { 0, 0, 0, 0 },
    .border = {
        .thickness = 0,
        .top = NT_COLOR(0,0,0,0),
        .bot = NT_COLOR(0,0,0,0),
        .gradient = NT_BORDER_GRADIENT_NONE,
        .rounded = 0,
    },
    .suggested_width = 0,
    .suggested_height = 0,
    .maximum_width = SIZE_MAX,
    .maximum_height = SIZE_MAX
};

void nt_style_init(nt_style_t *style) {
    if (default_style.font == NULL) {
        default_style.font = nt_render_load_font(NT_SANS_10);
    }
    
    memcpy(style, &default_style, sizeof(nt_style_t));
}

void nt_style_set_font(nt_style_t *style, int type) {
    style->font = nt_render_load_font(type);
}


void nt_style_draw_bg(nt_widget_t *widget, nt_render_surface_t *surf) {
    // the bg is very "special" (aka annoying)
    // if the border is rounded we dont want to interfere with it.
    // !!! yes these are some hacks
    if (NT_COLOR_A(widget->style.bg) == 0) {
        memset(surf->buffer, 0, surf->width * surf->height * 4);
        return;
    }

    if (widget->style.border.thickness && widget->style.border.rounded) {
        nt_render_clear(&widget->surf, NT_COLOR(0,0,0,0));
        nt_render_fill_rect(&widget->surf, &NT_RECT(widget->style.padding[LEFT], widget->style.padding[TOP], nt_widget_get_width_inner(widget), nt_widget_get_height_inner(widget)), widget->style.bg);
    } else {
        nt_render_clear(surf, widget->style.bg);
    }
}

void nt_style_draw_border(nt_widget_t *w, struct _nt_render_surface *surf) {
    nt_style_t *style = &w->style;
    if (style->border.thickness == 0) return;

    nt_rect_t border = {
        .x = 0,
        .y = 0,
        .w = w->layout_data.w,
        .h = w->layout_data.h,
    };

    if (style->border.gradient == NT_BORDER_GRADIENT_NONE) {
        if (style->border.rounded != 0) {
            nt_render_border_rounded_rect(surf, &border, style->border.thickness, style->border.rounded, style->border.top);
        } else {
            nt_render_border_rect(surf, &border, style->border.thickness, style->border.top);
        }
    } else {
        if (style->border.rounded != 0) {
            nt_render_border_rounded_rect_gradient(surf, &border, style->border.thickness, style->border.rounded, style->border.top, style->border.bot, (style->border.gradient == NT_BORDER_GRADIENT_HORIZ));
        } else {
            assert(0 && "Non-rounded gradient not supported");
        }
    }
}
