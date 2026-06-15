/**
 * @file neutron/window.c
 * @brief Window component
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

nt_window_t *nt_window_head = NULL;


nt_window_t *nt_window_create(char *title, size_t width, size_t height) {
    nt_window_t *win = malloc(sizeof(nt_window_t));
    win->title = title;
    win->width = width;
    win->height = height;
    win->parent = NULL;
    win->focused = NULL;
    win->root_frame = NULL;
    win->exited = false;
    win->popup = NULL;
    win->clip = NULL;

    int status = nt_platform_create_window(win);
    if (status < 0) {
        NT_ERROR("nt_platform_create_window failed with error code %d\n", status);
        return NULL;
    }

    win->next = nt_window_head;
    nt_window_head = win;

    return win;
}

nt_window_t *nt_window_create_undecorated(size_t width, size_t height) {
    nt_window_t *win = malloc(sizeof(nt_window_t));
    win->width = width;
    win->height = height;
    win->parent = NULL;
    win->focused = NULL;
    win->root_frame = NULL;
    win->exited = false;
    win->popup = NULL;
    win->clip = NULL;

    int status = nt_platform_create_window_undecorated(win);
    if (status < 0) {
        NT_ERROR("nt_platform_create_window_undecorated failed with error code %d\n", status);
        return NULL;
    }

    win->next = nt_window_head;
    nt_window_head = win;

    return win;
}

nt_window_t *nt_window_create_child(nt_window_t *parent, char *title, size_t width, size_t height) {
    nt_window_t *win = malloc(sizeof(nt_window_t));
    win->title = title;
    win->width = width;
    win->height = height;
    win->parent = parent;
    win->focused = NULL;
    win->root_frame = NULL;
    win->popup = NULL;
    win->clip = NULL;
    win->exited = false;

    int status = nt_platform_create_child_window(win);
    if (status < 0) {
        NT_ERROR("nt_platform_create_window failed with error code %d\n", status);
        return NULL;
    }

    win->next = nt_window_head;
    nt_window_head = win;

    return win;
}

void nt_window_set_pos(nt_window_t *win, int x, int y) {
    nt_platform_set_window_pos(win, x, y);    
}

nt_window_t *nt_window_list() {
    return nt_window_head;
}

void nt_window_update(nt_window_t *window) {
    if (!window->root_frame) return;
    if ((window->root_frame->flags & NT_WIDGET_NEED_RECALC)) {
        nt_widget_calculate_size(window->root_frame);
        nt_widget_adjust_size(window->root_frame, &NT_RECT(0, 0, window->width, window->height));
    }

    if ((window->root_frame->flags & NT_WIDGET_DIRTY)) {
        nt_widget_render(window->root_frame);
        if (window->popup) nt_widget_render(window->popup); // !!!: double draw
        nt_platform_flip(window);
    }
}

void nt_window_set_root(nt_window_t *window, nt_widget_t *root) {
    window->root_frame = root;
    root->window = window;
    nt_style_set_margin_all(&root->style, 0);
}

void nt_window_set_focus(nt_window_t *win, nt_widget_t *widget) {
    if (win->focused == widget) return;

    if (!widget) {
        if (win->focused) {
            win->focused->focused = false;
            nt_event_t focus = {
                .type = NT_EVENT_FOCUS_EXIT
            };

            nt_widget_dispatch(win->focused, &focus);
            nt_widget_invalidate(win->focused);
            win->focused = NULL;
        }
        return;
    }

    // Else there actually is a widget
    if ((widget->flags & NT_WIDGET_FOCUS) == 0) {
        // Bubble up to parent
        return nt_window_set_focus(win, widget->parent);
    }
    
    if (win->focused) {
        win->focused->focused = false;
        nt_event_t focus = {
            .type = NT_EVENT_FOCUS_EXIT
        };

        nt_widget_dispatch(win->focused, &focus);
        nt_widget_invalidate(win->focused);
    }

    win->focused = widget;
    widget->focused = true;
    
    nt_event_t focus = {
        .type = NT_EVENT_FOCUS_ENTER
    };
    nt_widget_dispatch(widget, &focus);

    nt_widget_invalidate(widget);
}

void nt_window_set_popup(nt_window_t *win, nt_widget_t *widget) {\
    win->popup = widget;
    nt_widget_invalidate(win->root_frame);
}

void nt_window_closed(nt_window_t *win) {
    NT_DEBUG("nt_window_closed %016llX\n", win);
    nt_event_t closed = {
        .type = NT_EVENT_WINDOW_CLOSE,
        .window = { .window = win }
    };

    nt_event_process(win, &closed);

    // remove
    if (win == nt_window_head) {
        nt_window_head = win->next;
    } else {
        NT_ITERATE_WINDOWS(l) {
            if (l->next == win) {
                l->next = win->next;
                break;
            }
        }
    }

    // !!! dont free the window structure as if its main window I dont give a shit and if its child window nt_wait_child_window will do it
    // !!! this only works if parent waited
    NT_DEBUG("Window %p has closed\n", win);
    nt_widget_free(win->root_frame);
    win->exited = true;
}

void nt_window_close(nt_window_t *win) {
    nt_platform_destroy_window(win);
}

void nt_window_set_visible(nt_window_t *win, bool visible) {
    nt_platform_set_window_visible(win, visible);
}

void nt_window_set_clip(nt_window_t *win, nt_clip_t *clip) {
    win->clip = clip;
    nt_platform_set_clip(win, clip);
}
