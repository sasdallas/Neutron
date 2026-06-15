/**
 * @file include/neutron/render.h
 * @brief Neutron renderer
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_RENDER_H
#define _NEUTRON_RENDER_H

#include <stddef.h>
#include <neutron/style.h>
#include <stdbool.h>

struct _nt_window;

typedef struct _nt_render_surface {
    bool initialized;
    size_t width;
    size_t height;
    char *buffer;
    void *platform;
} nt_render_surface_t;

typedef enum nt_font_type {
    NT_SANS_10,
    NT_SANS_12,
    NT_SANS_BOLD_10,
    NT_SANS_BOLD_12,
    NT_NFONTS
} nt_font_type_t;

typedef void nt_font_handle_t;

typedef struct _nt_image {
    unsigned int *bitmap;
    int width;
    int height;
    int nchannels;
} nt_image_t;

#define NT_PIXEL(surf, x, y) (*(unsigned int*)(((char*)(surf)->buffer) + ((y) * ((surf)->width * 4)) + ((x)*4)))

int nt_render_init();

void nt_render_init_surface(struct _nt_window *window, nt_render_surface_t *surface, size_t width, size_t height);
void nt_render_deinit_surface(nt_render_surface_t *surface);

void nt_render_clear(nt_render_surface_t *surface, nt_color_t color);

nt_color_t nt_alpha_blend(nt_color_t top, nt_color_t bottom);

/* rectangle */
void nt_render_fill_rect(nt_render_surface_t *surface, nt_rect_t *rect, nt_color_t color);
void nt_render_fill_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, nt_color_t start, nt_color_t end, bool horiz);
void nt_render_border_rect(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, nt_color_t color);
void nt_render_border_rounded_rect(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, int radius, nt_color_t color);
void nt_render_border_rounded_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, int radius, nt_color_t start, nt_color_t end, bool horiz);
void nt_render_rounded_rect(nt_render_surface_t *surface, nt_rect_t *rect, int radius, nt_color_t color);
void nt_render_rounded_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, int radius, nt_color_t start, nt_color_t end, bool horiz);

/* font */
nt_font_handle_t *nt_render_load_font(nt_font_type_t type);
void nt_render_draw_text(nt_render_surface_t *surface, nt_font_handle_t *font, unsigned x, unsigned y, const char *text, nt_color_t color);
void nt_render_text_dimensions(nt_font_handle_t *font, const char *text, size_t *w, size_t *h);
void nt_render_free_font(nt_font_handle_t *font);

/* image */
int nt_render_create_image(nt_image_t *image, char *filename);
int nt_render_free_image(nt_image_t *image);
void nt_render_draw_image(nt_render_surface_t *surface, nt_image_t *image, unsigned x, unsigned y);

#endif