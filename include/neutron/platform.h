/**
 * @file include/neutron/platform.h
 * @brief Platform-specific functions for Neutron
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_PLATFORM_H
#define _NEUTRON_PLATFORM_H

#include <stddef.h>
#include <stdbool.h>

typedef struct _nt_clip {
    unsigned x;
    unsigned y;
    unsigned w;
    unsigned h;
} nt_clip_t;

typedef enum {
    NT_CURSOR_ARROW,
    NT_CURSOR_HAND,
    NT_CURSOR_TEXT,
    NT_CURSOR_RESIZE_H,
    NT_CURSOR_RESIZE_V,
} nt_platform_cursor_t;

int nt_platform_init();

struct _nt_window;
struct _nt_render_surface;

/* window functions */
int nt_platform_create_window(struct _nt_window *window);
int nt_platform_create_child_window(struct _nt_window *window);
int nt_platform_create_window_undecorated(struct _nt_window *window);
void nt_platform_get_window_pos(struct _nt_window *window, int *x, int *y);
void nt_platform_set_window_pos(struct _nt_window *window, int x, int y);
int nt_platform_destroy_window(struct _nt_window *window);
void nt_platform_check_events(struct _nt_window *window);
void nt_platform_resize_window(struct _nt_window *window, int w, int h);
void nt_platform_set_window_visible(struct _nt_window *window, bool visible);

/* misc. */
void nt_platform_set_cursor(struct _nt_window *window, nt_platform_cursor_t cur);

/* rendering functions */
void nt_platform_set_clip(struct _nt_window *window, nt_clip_t *clip);
void nt_platform_flip(struct _nt_window *window);
void nt_platform_init_surface(struct _nt_window *window, struct _nt_render_surface *surface, size_t width, size_t height);
void nt_platform_deinit_surface(struct _nt_render_surface *surface);
void nt_platform_blit_surface(struct _nt_window *window, struct _nt_render_surface *surface, int x, int y);

#endif