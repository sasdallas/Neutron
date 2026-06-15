/**
 * @file include/neutron/style.h
 * @brief Neutron style
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_STYLE_H
#define _NEUTRON_STYLE_H

#include <stddef.h>
#include <stdbool.h>

typedef unsigned int nt_color_t;

#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3
#define NDIR 4

#define NT_BORDER_GRADIENT_NONE 0
#define NT_BORDER_GRADIENT_HORIZ 1
#define NT_BORDER_GRADIENT_VERT 2

struct _nt_render_surface;
struct _nt_font_handle;
struct _nt_widget;
enum nt_font_type;

typedef struct _nt_style {
    nt_color_t fg;
    nt_color_t bg;

    struct _nt_font_handle *font;
    size_t margin[NDIR];
    size_t padding[NDIR];

    struct {
        unsigned char rounded;
        unsigned char gradient; // 0 = no gradient, 1 = vert gradient, 2 = horiz gradient
        nt_color_t top;
        nt_color_t bot;
        int thickness; // 0 = no border
    } border;

    size_t suggested_width; // if pref_width is less than this it will be set to this
    size_t suggested_height; // if pref_height is less than this it will be set to this
    size_t maximum_width; // if pref_width is greater than this it will be set to this
    size_t maximum_height; // if pref_height is greater than this it will be set to this
} nt_style_t;

#define NT_COLOR(r, g, b, a) ((nt_color_t)(((a) << 24)) | ((r) << 16) | ((g) << 8) | ((b)))

#define NT_COLOR_A(color) (((color) & 0xFF000000) / 0x1000000)
#define NT_COLOR_R(color) (((color) & 0x00FF0000) / 0x10000)
#define NT_COLOR_G(color) (((color) & 0x0000FF00) / 0x100)
#define NT_COLOR_B(color) (((color) & 0x000000FF))

#define NT_COLOR_INVERT_IF_SELECTED(w, c) (((w)->selected ? (~((uint32_t)(c) & 0xFFFFFF) | NT_COLOR_A(c)) : (c)))

void nt_style_init(nt_style_t *style);
void nt_style_set_font(nt_style_t *style, int type);
void nt_style_draw_bg(struct _nt_widget *widget, struct _nt_render_surface *surf);
static inline void nt_style_set_fg_color(nt_style_t *style, nt_color_t color) { style->fg = color; }
static inline void nt_style_set_bg_color(nt_style_t *style, nt_color_t color) { style->bg = color; }
static inline void nt_style_set_margin(nt_style_t *style, int dir, size_t pad) { style->margin[dir] = pad; }
static inline void nt_style_set_margin_all(nt_style_t *style, size_t pad) { style->margin[0] = pad; style->margin[1] = pad; style->margin[2] = pad; style->margin[3] = pad; }
static inline void nt_style_set_padding(nt_style_t *style, int dir, size_t pad) { style->padding[dir] = pad; }
static inline void nt_style_set_padding_all(nt_style_t *style, size_t pad) { style->padding[0] = pad; style->padding[1] = pad; style->padding[2] = pad; style->padding[3] = pad; }
static inline void nt_style_set_suggested_width(nt_style_t *style, size_t sug_width) { style->suggested_width = sug_width; }
static inline void nt_style_set_suggested_height(nt_style_t *style, size_t sug_height) { style->suggested_height = sug_height; }
static inline void nt_style_set_maximum_width(nt_style_t *style, size_t max_width) { style->maximum_width = max_width; }
static inline void nt_style_set_maximum_height(nt_style_t *style, size_t max_height) { style->maximum_height = max_height; }


// set to 0 to disable border
static inline void nt_style_set_border_thickness(nt_style_t *style, int thickness) {
    style->border.thickness = thickness;
}

static inline void nt_style_set_border_rounded(nt_style_t *style, unsigned char radius) {
    style->border.rounded = radius;
}

static inline void nt_style_set_border_gradient(nt_style_t *style, int gradient) {
    style->border.gradient = gradient;
}

static inline void nt_style_set_border_gradient_colors(nt_style_t *style, nt_color_t start, nt_color_t finish) {
    style->border.top = start;
    style->border.bot = finish;    
}

static inline void nt_style_set_border_color(nt_style_t *style, nt_color_t color) {
    style->border.top = color;
}

void nt_style_draw_border(struct _nt_widget *widget, struct _nt_render_surface *surf);

#endif
