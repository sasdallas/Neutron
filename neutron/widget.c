/**
 * @file neutron/widget.c
 * @brief Neutron widget system
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
#include <neutron/widgets/scroll_container.h>
#include <string.h>

#define NT_SCROLLBAR_SIZE 10

nt_widget_t *nt_widget_create(nt_widget_type_t type, nt_widget_vtable_t *vtable) {
    assert(vtable->class_size >= sizeof(nt_widget_t));
    nt_widget_t *widget = malloc(vtable->class_size);

    memset(widget, 0, vtable->class_size);
    widget->vtbl = vtable;
    widget->type = type;

    nt_style_init(&widget->style);

    if (vtable->init) vtable->init(widget);

    // only invalidate once the widget has been set
    NT_DEBUG("Created widget %p of type %d\n", widget, type);
    return widget;
}

void nt_widget_set_parent(nt_widget_t *widget, nt_widget_t *parent) {
    // Deparent first
    if (widget->parent) {
        if (widget->prev) {
            widget->prev->next = widget->next;
        } else {
            widget->parent->children = widget->next;
        }

        if (widget->next) widget->next->prev = widget->prev;
    }

    nt_widget_mark_recalc(widget);

    widget->parent = parent;
    widget->next = parent->children;
    if (parent->children) parent->children->prev = widget;
    parent->children = widget;

    nt_widget_mark_recalc(widget);
}

// slow
void nt_widget_set_parent_append(nt_widget_t *widget, nt_widget_t *parent) {
    // Deparent first
    if (widget->parent) {
        if (widget->prev) {
            widget->prev->next = widget->next;
        } else {
            widget->parent->children = widget->next;
        }

        if (widget->next) widget->next->prev = widget->prev;
    }

    nt_widget_mark_recalc(widget);

    widget->parent = parent;

    if (parent->children) {
        nt_widget_t *to_add = parent->children;
        while (to_add->next) to_add = to_add->next;
        to_add->next = widget;
        widget->prev = to_add;
        widget->next = NULL;
    } else {
        parent->children = widget;
        widget->next = widget->prev = NULL;
    }

    nt_widget_mark_recalc(parent);

}

// should be called with the lowest level of widget
static bool nt_widget_dispatch_recur(nt_widget_t *widget, nt_event_t *event) {
    if (!widget) return true;

    if (widget->event_handlers[event->type]) {
        if (widget->event_handlers[event->type](widget, event) == false) {
            // Stop bubbling the event up
            return false;
        }
    }

    nt_widget_dispatch_recur(widget->parent, event);
    return true;
}

void nt_widget_dispatch(nt_widget_t *widget, nt_event_t *event) {
    event->w = widget;
    nt_widget_dispatch_recur(widget, event);
}

void nt_widget_set_context_menu(nt_widget_t *widget, nt_widget_t *menu) {
    widget->context_menu = menu;
}

// get the lowest widget at the coordinates
static nt_widget_t *nt_widget_at_recur(nt_widget_t *widget, int x, int y) {
    assert(widget);

    if (widget->type == NT_WIDGET_SCROLL_CONTAINER) {
        nt_scroll_container_t *sc = (nt_scroll_container_t *)widget;
        long viewport_w = widget->layout_data.w - (sc->v_bar ? NT_SCROLLBAR_SIZE : 0);
        long viewport_h = widget->layout_data.h - (sc->h_bar ? NT_SCROLLBAR_SIZE : 0);

        if (viewport_w < 0) viewport_w = 0;
        if (viewport_h < 0) viewport_h = 0;

        bool in_viewport =
            x >= widget->layout_data.x &&
            x < widget->layout_data.x + viewport_w &&
            y >= widget->layout_data.y &&
            y < widget->layout_data.y + viewport_h;

        if (!in_viewport) {
            return widget;
        }
    }

    NT_ITERATE_CHILDREN(widget) {
        if ((int)child->layout_data.x <= x && (int)child->layout_data.x + (int)child->layout_data.w > x && (int)child->layout_data.y <= y && (int)child->layout_data.y + (int)child->layout_data.h > y) {
            if (((child->flags & NT_WIDGET_INVIS) == 0)) return nt_widget_at_recur(child, x, y);
        }
    }

    return widget;
}  

// NOTE: Does not return invisible widgets
nt_widget_t *nt_widget_at(nt_window_t *window, int x, int y) {
    if (!window->root_frame) return NULL;
    
    // check popup
    if (window->popup) {
        nt_widget_t *popup = window->popup;
        if (popup->layout_data.x <= x && popup->layout_data.x + popup->layout_data.w > x && popup->layout_data.y <= y && popup->layout_data.y + popup->layout_data.h > y) {
            if (((popup->flags & NT_WIDGET_INVIS) == 0)) return nt_widget_at_recur(popup, x, y);
        }
    }

    return nt_widget_at_recur(window->root_frame, x, y);
}

void nt_widget_invalidate_all(nt_widget_t *widget) {
    if (!widget) return;
    widget->flags |= NT_WIDGET_DIRTY;
    NT_ITERATE_CHILDREN(widget) {
        nt_widget_invalidate_all(child);
    }
}

void nt_widget_invalidate(nt_widget_t *widget) {
    if (!widget->parent) return;
    nt_widget_invalidate_all(nt_widget_get_window(widget)->root_frame);
}

// note: recursive
// tgui design
void nt_widget_calculate_size(nt_widget_t *widget) {
    if ((widget->flags & NT_WIDGET_NEED_RECALC) == 0) return;

    // First calculate the sizes of all children
    if (widget->children) {
        nt_widget_t *child = widget->children;
        while (child) {
            nt_widget_calculate_size(child);
            child = child->next;
        }
    }

    if (widget->vtbl == NULL) {
        NT_ERROR("Holy fuck we are cooked. Widget->vtbl is NULL but widget isnt\n");
        NT_ERROR("Type is %d\n", widget->type);
        NT_ERROR("Parent is %p and type of parent is %d\n", widget->parent, widget->parent->type);
        for (;;);
        assert(0);
    }

    // First calculate the size of the widget using the class
    assert(widget->vtbl->calc_size && "TODO");
    widget->vtbl->calc_size(widget);

    // Now adjust the calculation to include the margin and padding
    int horiz_margin = widget->style.margin[LEFT] + widget->style.margin[RIGHT];
    int vert_margin = widget->style.margin[TOP] + widget->style.margin[BOTTOM];
    int horiz_padding = widget->style.padding[LEFT] + widget->style.padding[RIGHT];
    int vert_padding = widget->style.padding[TOP] + widget->style.padding[BOTTOM];

    widget->size_data.min_width += horiz_margin + horiz_padding;
    widget->size_data.pref_width += horiz_margin + horiz_padding;
    widget->size_data.min_height += vert_margin + vert_padding;
    widget->size_data.pref_height += vert_margin + vert_padding;

    // Use suggested stuff
    // !!! Usage of pref_width and pref_height is pretty useless at the moment since the widget framework doesn't really care whether its min/pref
    if (widget->size_data.min_width < widget->style.suggested_width) {
        widget->size_data.min_width = widget->style.suggested_width;
    }

    if (widget->size_data.min_height < widget->style.suggested_height) {
        widget->size_data.min_height = widget->style.suggested_height;
    }
    
    if (widget->size_data.pref_width < widget->style.suggested_width) {
        widget->size_data.pref_width = widget->style.suggested_width;
    }

    if (widget->size_data.pref_height < widget->style.suggested_height) {
        widget->size_data.pref_height = widget->style.suggested_height;
    }

    // !!! Usage of pref_width and pref_height is pretty useless at the moment since the widget framework doesn't really care whether its min/pref
    if (widget->size_data.min_width > widget->style.maximum_width) {
        widget->size_data.min_width = widget->style.maximum_width;
    }

    if (widget->size_data.min_height > widget->style.maximum_height) {
        widget->size_data.min_height = widget->style.maximum_height;
    }
    
    if (widget->size_data.pref_width > widget->style.maximum_width) {
        widget->size_data.pref_width = widget->style.maximum_width;
    }

    if (widget->size_data.pref_height > widget->style.maximum_height) {
        widget->size_data.pref_height = widget->style.maximum_height;
    }



    widget->flags &= ~(NT_WIDGET_NEED_RECALC);
}

// tgui design
void nt_widget_adjust_size(nt_widget_t *widget, nt_rect_t *region) {
    // TODO this method will adjust the widget's x and y depending on its alignment to fit into region
    
    long x = region->x;
    long y = region->y;
    long height = 0;
    long width = 0;

    // process horizontal
    long w_avail = (long)region->w - widget->size_data.pref_width;
    if (w_avail < 0) {
        width = region->w;
    } else {
        width = widget->size_data.pref_width;

        if (widget->layout.expansion & NT_EXPAND_HORIZONTAL) {
            // TODO support other NT_ALIGN
            
            if (widget->layout.halign == NT_ALIGN_STRETCH) {
                width = region->w;
            } else if (widget->layout.halign == NT_ALIGN_CENTER) {
                x += w_avail / 2;
            } else if (widget->layout.halign == NT_ALIGN_RIGHT) {
                x += w_avail;
            }
        }
    }

    // process vertical
    long h_avail = (long)region->h - widget->size_data.pref_height;
    if (h_avail < 0) {
        height = region->h;
    } else {
        height = widget->size_data.pref_height;

        if (widget->layout.expansion & NT_EXPAND_VERTICAL) {
            if (widget->layout.valign == NT_ALIGN_STRETCH) {
                height = region->h;
            } else if (widget->layout.valign == NT_ALIGN_CENTER) {
                y += h_avail / 2;
            } else if (widget->layout.valign == NT_ALIGN_TOP) {
                // do nothing
            } else if (widget->layout.valign == NT_ALIGN_BOT) {
                y += h_avail;
            }
        }
    }

    // Now do the thing if something changed
    if (widget->layout_data.x == x && widget->layout_data.y == y && widget->layout_data.w == width && widget->layout_data.h == height) {
        // TODO: This can break resizing in some cases where we dont redraw but it is also slow
        // return;
    }

    x += widget->style.margin[LEFT];
    y += widget->style.margin[TOP];
    width -= widget->style.margin[RIGHT] + widget->style.margin[LEFT];
    height -= widget->style.margin[TOP] + widget->style.margin[BOTTOM];

    widget->layout_data.x = x;
    widget->layout_data.y = y;
    widget->layout_data.w = width;
    widget->layout_data.h = height;

    if (!(widget->layout_data.w > 0 && widget->layout_data.h > 0)) {
        NT_ERROR("Widget has width=%d height=%d.\n", width, height);
        NT_ERROR("This widget is of type %d and is parented by a %d\n", widget->type, widget->parent->type);
        NT_ERROR("The parent has size %dx%d\n", widget->parent->layout_data.w, widget->parent->layout_data.h);
        assert(0);
    }

    nt_render_deinit_surface(&widget->surf);
    nt_render_init_surface(nt_widget_get_window(widget), &widget->surf, width, height);

    if (widget->vtbl->adjust_size) {
        widget->vtbl->adjust_size(widget);
    }
    
    nt_widget_invalidate(widget);
}

// this will mark every widget above it as NT_WIDGET_NEED_RECALC which will be processed by the window
// tgui design 
void nt_widget_mark_recalc(nt_widget_t *widget) {
    nt_widget_t *w = widget;
    while (w) {
        w->flags |= NT_WIDGET_NEED_RECALC;
        w = w->parent;
    }
}

void nt_widget_render(nt_widget_t *widget) {
    widget->flags &= ~(NT_WIDGET_DIRTY);

    // All children are invis if the widget is invisible
    // NOTE: Children retain their NT_WIDGET_DIRTY flag
    if (widget->flags & NT_WIDGET_INVIS) return;

    nt_style_draw_bg(widget, &widget->surf);
    nt_style_draw_border(widget, &widget->surf);

    if (widget->render_hook) {
        widget->render_hook(widget, &widget->surf);
    }

    if (widget->vtbl->render) {
        widget->vtbl->render(widget, &widget->surf);
    }

    nt_platform_blit_surface(nt_widget_get_window(widget), &widget->surf, widget->layout_data.x, widget->layout_data.y);

    if (widget->type == NT_WIDGET_SCROLL_CONTAINER) {
        nt_scroll_container_t *sc = (nt_scroll_container_t *)widget;
        long viewport_w = nt_widget_get_width_inner(widget) - (sc->v_bar ? NT_SCROLLBAR_SIZE : 0);
        long viewport_h = nt_widget_get_height_inner(widget) - (sc->h_bar ? NT_SCROLLBAR_SIZE : 0);

        if (viewport_w < 0) viewport_w = 0;
        if (viewport_h < 0) viewport_h = 0;

        nt_clip_t clip = {
            .x = nt_widget_get_x_inner(widget),
            .y = nt_widget_get_y_inner(widget),
            .w = viewport_w,
            .h = viewport_h,
        };

        if (clip.w > 0 && clip.h > 0) {
            nt_window_set_clip(nt_widget_get_window(widget), &clip);
            NT_ITERATE_CHILDREN(widget) { nt_widget_render(child); }
            nt_window_set_clip(nt_widget_get_window(widget), NULL);
        }

        return;
    }

    NT_ITERATE_CHILDREN(widget) { nt_widget_render(child); }
}

void nt_widget_free(nt_widget_t *widget) {
    // Iterate through children
    NT_ITERATE_CHILDREN(widget) {
        nt_widget_free(child);
    }

    if (widget->parent) {
        if (widget->parent->children == widget) widget->parent->children = widget->next;
        if (widget->next) widget->next->prev = widget->prev;
        if (widget->prev) widget->prev->next = widget->next;
    }

    if (widget->vtbl->free) {
        widget->vtbl->free(widget);
    }

    if (widget->surf.initialized) {
        nt_render_deinit_surface(&widget->surf);
    }
    
    nt_signal_free(widget);
    
    free(widget);


    // !!! THIS IS SUCH A HACK. NEED A PROPER SYSTEM TO HANDLE THIS!!! (refcounting?)
extern nt_widget_t *last_hovered_widget;
    if (last_hovered_widget == widget) {
        last_hovered_widget = NULL;
    }

    // !!! Less of a hack but still a hack!
    nt_window_t *win = nt_widget_get_window(widget);
    if (nt_window_get_focus(win) == widget) {
        nt_window_set_focus(win, NULL);
    }
}

void nt_widget_set_selected(nt_widget_t *widget, bool selected) {
    if ((widget->flags & NT_WIDGET_SELECTABLE) == 0) return;
    NT_DEBUG("nt_widget_set_selected type=%d\n", widget->type); 
    widget->selected = selected;

    // create event
    if (selected) {
        nt_event_t selected = { .type = NT_EVENT_SELECTED, .w = widget };
        nt_event_process(nt_widget_get_window(widget), &selected);
    } else {
        nt_event_t unselected = { .type = NT_EVENT_UNSELECTED, .w = widget };
        nt_event_process(nt_widget_get_window(widget), &unselected);
    }

    nt_widget_invalidate(widget);

    // !!! Holy
    NT_ITERATE_CHILDREN(widget) {
        nt_widget_set_selected(child, selected);
    }
}

void nt_widget_set_selectable(nt_widget_t *widget, bool selectable) {
    if (selectable) widget->flags |= NT_WIDGET_SELECTABLE;
    else widget->flags &= ~(NT_WIDGET_SELECTABLE);

    NT_ITERATE_CHILDREN(widget) {
        nt_widget_set_selectable(child, selectable);
    }
}

void nt_widget_measure(nt_widget_t *widget, nt_rect_t *rect) {
    if (widget->vtbl->measure) {
        widget->vtbl->measure(widget, rect);
    } else {
        rect->w = widget->size_data.pref_width;
        rect->h = widget->size_data.pref_height;
    }

    // Adjust for margins
    rect->w += widget->style.margin[LEFT] + widget->style.margin[RIGHT];
    rect->h += widget->style.margin[TOP] + widget->style.margin[BOTTOM];
}
