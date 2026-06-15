/**
 * @file neutron/platform/ethereal.c
 * @brief Ethereal platform API
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifdef BUILDING_ETHEREAL

#include <ethereal/celestial.h>
#include <neutron/neutron.h>

extern void nt_platform_init_keyboard();
extern bool nt_platform_key_process(key_event_t *k, nt_event_t *event);

int nt_platform_init() {
    nt_platform_init_keyboard();
    return 0;
}

void nt_platform_event_handler(window_t *win, uint32_t event_type, void *event);

int nt_platform_create_window(nt_window_t *window) {
    wid_t wid = celestial_createWindow(0, window->width, window->height);
    if (wid < 0) return 1;
    window_t *win = celestial_getWindow(wid);
    if (!win) return 1;
    celestial_setTitle(win, window->title);

    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_MOTION, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_BUTTON_DOWN, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_BUTTON_UP, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_ENTER, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_EXIT, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_KEY_EVENT, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_RESIZE, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_SCROLL, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_WINDOW_CLOSE, nt_platform_event_handler);

    win->d = window;
    window->platform = win;
    return 0;
}

int nt_platform_create_window_undecorated(nt_window_t *window) {
    wid_t wid = celestial_createWindowUndecorated(0, window->width, window->height);
    if (wid < 0) return 1;
    window_t *win = celestial_getWindow(wid);
    if (!win) return 1;
    celestial_setTitle(win, window->title);

    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_MOTION, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_BUTTON_DOWN, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_BUTTON_UP, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_ENTER, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_EXIT, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_KEY_EVENT, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_RESIZE, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_MOUSE_SCROLL, nt_platform_event_handler);
    celestial_setHandler(win, CELESTIAL_EVENT_WINDOW_CLOSE, nt_platform_event_handler);

    win->d = window;
    window->platform = win;
    return 0;
}


int nt_platform_create_child_window(nt_window_t *window) {
    return nt_platform_create_window(window);
}

void nt_platform_get_window_pos(nt_window_t *window, int *x, int *y) {
    *x = ((window_t*)window->platform)->x;
    *y = ((window_t*)window->platform)->y;
}

void nt_platform_set_window_pos(nt_window_t *window, int x, int y) {
    celestial_setWindowPosition((window_t*)window->platform, x, y);
}

int nt_platform_destroy_window(nt_window_t *window) {
    celestial_closeWindow((window_t*)window->platform);
    return 0;
}

void nt_platform_check_events(nt_window_t *window) {
    celestial_poll((window_t*)window->platform);   
}

void nt_platform_resize_window(nt_window_t *window, int w, int h) {
    celestial_resizeWindow((window_t*)window->platform, w, h);
}

void nt_platform_set_window_visible(nt_window_t *window, bool visible) {
    celestial_setWindowVisible((window_t*)window->platform, visible);
}

int cur_mappings[] = {
    [NT_CURSOR_ARROW] = CELESTIAL_MOUSE_DEFAULT,
    [NT_CURSOR_HAND] = CELESTIAL_MOUSE_DEFAULT,
    [NT_CURSOR_TEXT] = CELESTIAL_MOUSE_TEXT,
    [NT_CURSOR_RESIZE_H] = CELESTIAL_MOUSE_DEFAULT,
    [NT_CURSOR_RESIZE_V] = CELESTIAL_MOUSE_DEFAULT,
};

/* misc. */
void nt_platform_set_cursor(nt_window_t *window, nt_platform_cursor_t cur) {
    celestial_setMouseCursor(cur_mappings[cur]);
}

/* rendering functions */

void nt_platform_set_clip(nt_window_t *window, nt_clip_t *clip) {
    // 
}

void nt_platform_flip(nt_window_t *window) {
    gfx_context_t *ctx = celestial_getGraphicsContext((window_t*)window->platform);
    gfx_render(ctx);
    celestial_flip((window_t*)window->platform);
}

void nt_platform_init_surface(nt_window_t *window, struct _nt_render_surface *surface, size_t width, size_t height) {

}

void nt_platform_deinit_surface(struct _nt_render_surface *surface) {
    free(surface->buffer);
}

void nt_platform_blit_surface(nt_window_t *window, struct _nt_render_surface *surface, int x, int y) {
    gfx_context_t *ctx = celestial_getGraphicsContext((window_t*)window->platform);
    sprite_t sp = {
        .width = surface->width,
        .height = surface->height,
        .alpha = SPRITE_ALPHA_BLEND,
        .bitmap = (uint32_t*)surface->buffer
    };


    if (window->clip == NULL) {
        gfx_renderSprite(ctx, &sp, x, y);
    } else {
        // !!! we love hacks
        gfx_rect_t r = { .x = 0, .y = 0, .width = sp.width, .height = sp.height };
        if (window->clip->x >= x) {
            r.x = window->clip->x - x;
        }
        if (window->clip->y >= y) {
            r.y = window->clip->y - y;
        }

        if (window->clip->x + window->clip->w < x + sp.width) {
            r.width = window->clip->w - r.x;
        }

        if (window->clip->y + window->clip->h < y + sp.height) {
            r.height = window->clip->h - r.y;
        }

        if (r.x > sp.width || r.y > sp.height) return;
        gfx_renderSpriteRegion(ctx, &sp, &r, x, y);
    }
}

static unsigned char nt_platform_to_buttons(uint32_t buttons) {
    unsigned char ret = 0;
    if (buttons & CELESTIAL_MOUSE_BUTTON_LEFT) ret |= NT_MOUSE_BUTTON_LEFT;
    if (buttons & CELESTIAL_MOUSE_BUTTON_RIGHT) ret |= NT_MOUSE_BUTTON_RIGHT;
    if (buttons & CELESTIAL_MOUSE_BUTTON_MIDDLE) ret |= NT_MOUSE_BUTTON_MIDDLE;
    return ret;
}

void nt_platform_event_handler(window_t *win, uint32_t event_type, void *event) {
    if (event_type == CELESTIAL_EVENT_MOUSE_MOTION) {
        celestial_event_mouse_motion_t *motion = (celestial_event_mouse_motion_t *)event;
        nt_event_t motion_event = {
            .type = NT_EVENT_MOUSE_MOTION,
            .pointer = {
                .x = motion->x,
                .y = motion->y,
                .buttons = nt_platform_to_buttons(motion->buttons)
            }
        };

        nt_event_process((nt_window_t*)win->d, &motion_event);
    } else if (event_type == CELESTIAL_EVENT_MOUSE_BUTTON_DOWN) {
        celestial_event_mouse_button_down_t *button = (celestial_event_mouse_button_down_t*)event;
        nt_event_t button_event = {
            .type = NT_EVENT_MOUSE_DOWN,
            .pointer = {
                .x = button->x,
                .y = button->y,
                .buttons = nt_platform_to_buttons(button->held)
            }
        };

        nt_event_process((nt_window_t*)win->d, &button_event);
    } else if (event_type == CELESTIAL_EVENT_MOUSE_BUTTON_UP) {
        celestial_event_mouse_button_up_t *button = (celestial_event_mouse_button_up_t*)event;
        nt_event_t button_event = {
            .type = NT_EVENT_MOUSE_UP,
            .pointer = {
                .x = button->x,
                .y = button->y,
                .buttons = nt_platform_to_buttons(button->released)
            }
        };

        nt_event_process((nt_window_t*)win->d, &button_event);
    } else if (event_type == CELESTIAL_EVENT_MOUSE_SCROLL) {
        celestial_event_mouse_scroll_t *scroll = (celestial_event_mouse_scroll_t*)event;
        nt_event_t scroll_event = {
            .type = NT_EVENT_MOUSE_SCROLL,
            .scroll = {
                .x = scroll->x,
                .y = scroll->y,
                .delta_x = 0,
                .delta_y = (scroll->direction == CELESTIAL_MOUSE_SCROLL_DOWN) ? -1 : 1
            }
        };

        nt_event_process((nt_window_t*)win->d, &scroll_event);
    } else if (event_type == CELESTIAL_EVENT_KEY_EVENT) {
        nt_event_t e;
        celestial_event_key_t *key = (celestial_event_key_t *)event;
        if (nt_platform_key_process(&key->ev, &e)) {
            nt_event_process((nt_window_t*)win->d, &e);
        }
    } else if (event_type == CELESTIAL_EVENT_WINDOW_CLOSE) {
        nt_window_closed((nt_window_t*)win->d);
    } 
}

#endif
