/**
 * @file neutron/widgets/scroll_container.c
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

#include <neutron/neutron.h>

#define NT_SCROLLBAR_SIZE 10

#define NT_SCROLL_CONTAINER(w) ((nt_scroll_container_t*)w)

static void nt_scroll_container_translate_subtree(nt_widget_t *widget, int dx, int dy) {
    if (!widget) {
        return;
    }
    widget->layout_data.x += dx;
    widget->layout_data.y += dy;

    NT_ITERATE_CHILDREN(widget) {
        nt_scroll_container_translate_subtree(child, dx, dy);
    }
}

static int nt_scroll_container_viewport_width(nt_scroll_container_t *sc, nt_widget_t *widget) {
    int width = nt_widget_get_width_inner(widget) - (sc->v_bar ? NT_SCROLLBAR_SIZE : 0);
    return width > 0 ? width : 0;
}

static int nt_scroll_container_viewport_height(nt_scroll_container_t *sc, nt_widget_t *widget) {
    int height = nt_widget_get_height_inner(widget) - (sc->h_bar ? NT_SCROLLBAR_SIZE : 0);
    return height > 0 ? height : 0;
}

static void nt_scroll_container_clamp(nt_widget_t *widget) {
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(widget);

    int viewport_w = nt_scroll_container_viewport_width(sc, widget);
    int viewport_h = nt_scroll_container_viewport_height(sc, widget);

    int max_scroll_x = 0;
    int max_scroll_y = 0;

    if (sc->content_w > viewport_w) {
        max_scroll_x = sc->content_w - viewport_w;
    }
    if (sc->content_h > viewport_h) {
        max_scroll_y = sc->content_h - viewport_h;
    }

    if (sc->scroll_x < 0) sc->scroll_x = 0;
    if (sc->scroll_y < 0) sc->scroll_y = 0;
    if (sc->scroll_x > max_scroll_x) sc->scroll_x = max_scroll_x;
    if (sc->scroll_y > max_scroll_y) sc->scroll_y = max_scroll_y;
}

static bool nt_scroll_container_scroll(nt_widget_t *widget, nt_event_t *event) {
    if (!event || event->type != NT_EVENT_MOUSE_SCROLL) {
        return true;
    }

    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(widget);
    int old_scroll_x = sc->scroll_x;
    int old_scroll_y = sc->scroll_y;

    sc->scroll_x += event->scroll.delta_x * 20;
    sc->scroll_y -= event->scroll.delta_y * 20;
    nt_scroll_container_clamp(widget);

    int delta_x = sc->scroll_x - old_scroll_x;
    int delta_y = sc->scroll_y - old_scroll_y;

    if (delta_x || delta_y) {
        NT_ITERATE_CHILDREN(widget) {
            nt_scroll_container_translate_subtree(child, -delta_x, -delta_y);
        }

        nt_widget_invalidate(widget);
    }

    return false;
}

void nt_scroll_container_calc_size(nt_widget_t *widget) {
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(widget);

    if (!widget->children) {
        widget->size_data.min_width = 0;
        widget->size_data.min_height = 0;
        widget->size_data.pref_width = 0;
        widget->size_data.pref_height = 0;
        sc->content_w = 0;
        sc->content_h = 0;
        return;
    }

    widget->size_data = widget->children->size_data;
    if (sc->v_bar) {
        widget->size_data.min_width += NT_SCROLLBAR_SIZE;
        widget->size_data.pref_width += NT_SCROLLBAR_SIZE;
    }
    if (sc->h_bar) {
        widget->size_data.min_height += NT_SCROLLBAR_SIZE;
        widget->size_data.pref_height += NT_SCROLLBAR_SIZE;
    }

    sc->content_w = widget->children->layout_data.w;
    sc->content_h = widget->children->layout_data.h;
}

void nt_scroll_container_adjust_size(nt_widget_t *widget) {
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(widget);
    if (!widget->children) return;

    int viewport_w = nt_scroll_container_viewport_width(sc, widget);
    int viewport_h = nt_scroll_container_viewport_height(sc, widget);
    // int content_w = widget->children->size_data.pref_width;
    // int content_h = widget->children->size_data.pref_height;


    // sc->content_w = content_w;
    // sc->content_h = content_h;

    // nt_scroll_container_clamp(widget);
    // int layout_scroll_x = sc->scroll_x;
    // int layout_scroll_y = sc->scroll_y;

    int w = nt_widget_get_width_inner(widget);
    int h = nt_widget_get_height_inner(widget);

    if (sc->v_bar) w -= NT_SCROLLBAR_SIZE;
    if (sc->h_bar) h -= NT_SCROLLBAR_SIZE;

    nt_rect_t mrect = {
        .x = widget->layout_data.x,
        .y = widget->layout_data.y,
        .w = nt_widget_get_width_inner(widget) - NT_SCROLLBAR_SIZE,
        .h = nt_widget_get_height_inner(widget) - NT_SCROLLBAR_SIZE,
    };

    // Measure the widget
    nt_widget_measure(widget->children, &mrect);

    sc->content_w = mrect.w;
    sc->content_h = mrect.h;
    if (sc->content_w < viewport_w) sc->content_w = viewport_w;
    if (sc->content_h < viewport_h) sc->content_h = viewport_h;

    nt_scroll_container_clamp(widget);
    int layout_scroll_x = sc->scroll_x;
    int layout_scroll_y = sc->scroll_y;

    nt_rect_t rect = {
        .x = widget->layout_data.x - layout_scroll_x,
        .y = widget->layout_data.y - layout_scroll_y,
        .w = sc->content_w,
        .h = sc->content_h
    };

    nt_widget_adjust_size(widget->children, &rect);

    nt_scroll_container_clamp(widget);

    if (sc->scroll_x != layout_scroll_x || sc->scroll_y != layout_scroll_y) {
        nt_scroll_container_translate_subtree(
            widget->children,
            layout_scroll_x - sc->scroll_x,
            layout_scroll_y - sc->scroll_y
        );
    }
}

void nt_scroll_container_render(nt_widget_t *widget, nt_render_surface_t *surface) {
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(widget);
    int viewport_w = nt_scroll_container_viewport_width(sc, widget);
    int viewport_h = nt_scroll_container_viewport_height(sc, widget);

    if (sc->v_bar) {
        nt_render_fill_rect(surface, &NT_RECT(viewport_w, widget->style.padding[TOP], NT_SCROLLBAR_SIZE, viewport_h), NT_COLOR(230, 230, 230, 255));
        if (sc->content_h > viewport_h && sc->content_h > 0) {
            int thumb_h = (viewport_h * viewport_h) / sc->content_h;
            if (thumb_h < 12) thumb_h = 12;
            if (thumb_h > viewport_h) thumb_h = viewport_h;

            int thumb_y = 0;
            int travel = viewport_h - thumb_h;
            if (travel > 0 && sc->content_h > viewport_h) {
                thumb_y = (sc->scroll_y * travel) / (sc->content_h - viewport_h);
            }

            nt_render_fill_rect(surface, &NT_RECT(viewport_w + 2, thumb_y + 2, NT_SCROLLBAR_SIZE - 4, thumb_h - 4), NT_COLOR(140, 140, 140, 255));
        }
    }

    if (sc->h_bar) {
        nt_render_fill_rect(surface, &NT_RECT(0, viewport_h, viewport_w, NT_SCROLLBAR_SIZE), NT_COLOR(230, 230, 230, 255));
        if (sc->content_w > viewport_w && sc->content_w > 0) {
            int thumb_w = (viewport_w * viewport_w) / sc->content_w;
            if (thumb_w < 12) thumb_w = 12;
            if (thumb_w > viewport_w) thumb_w = viewport_w;

            int thumb_x = 0;
            int travel = viewport_w - thumb_w;
            if (travel > 0 && sc->content_w > viewport_w) {
                thumb_x = (sc->scroll_x * travel) / (sc->content_w - viewport_w);
            }

            nt_render_fill_rect(surface, &NT_RECT(thumb_x + 2, viewport_h + 2, thumb_w - 4, NT_SCROLLBAR_SIZE - 4), NT_COLOR(140, 140, 140, 255));
        }
    }
}

nt_widget_vtable_t scroll_container_vtable = {
    .class_size = sizeof(nt_scroll_container_t),
    .init = NULL,
    .free = NULL,
    .render = nt_scroll_container_render,
    .calc_size = nt_scroll_container_calc_size,
    .adjust_size = nt_scroll_container_adjust_size,
};

nt_widget_t *nt_scroll_container_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_SCROLL_CONTAINER, &scroll_container_vtable);
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(w);

    sc->v_bar = true;
    sc->h_bar = false;
    sc->scroll_x = 0;
    sc->scroll_y = 0;
    sc->content_w = 0;
    sc->content_h = 0;

    nt_event_set_handler(w, NT_EVENT_MOUSE_SCROLL, nt_scroll_container_scroll);

    return w;
}

void nt_scroll_container_set_child(nt_widget_t *container, nt_widget_t *child) {
    nt_widget_set_parent(child, container);
}

void nt_scroll_container_set_h_bar(nt_widget_t *container, bool h_bar) {
    nt_scroll_container_t *s = NT_SCROLL_CONTAINER(container);
    s->h_bar = h_bar;
}

void nt_scroll_container_set_v_bar(nt_widget_t *container, bool v_bar) {
    nt_scroll_container_t *s = NT_SCROLL_CONTAINER(container);
    s->v_bar = v_bar;
}

void nt_scroll_container_set_scroll(nt_widget_t *container, int scroll_x, int scroll_y) {
    nt_scroll_container_t *sc = NT_SCROLL_CONTAINER(container);
    int old_scroll_x = sc->scroll_x;
    int old_scroll_y = sc->scroll_y;

    sc->scroll_x = scroll_x;
    sc->scroll_y = scroll_y;
    nt_scroll_container_clamp(container);

    int dx = sc->scroll_x - old_scroll_x;
    int dy = sc->scroll_y - old_scroll_y;

    if (dx || dy) {
        NT_ITERATE_CHILDREN(container) {
            nt_scroll_container_translate_subtree(child, -dx, -dy);
        }

        nt_widget_invalidate(container);
    }
}
