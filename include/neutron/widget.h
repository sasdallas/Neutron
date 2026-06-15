/**
 * @file include/neutron/widget.h
 * @brief Neutron widget framework
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGET_H
#define _NEUTRON_WIDGET_H

#include <neutron/style.h>
#include <neutron/misc.h>
#include <neutron/render.h>
#include <neutron/event.h>
#include <neutron/signal.h>
#include <stddef.h>

typedef enum _nt_widget_type {
    NT_WIDGET_BOX,
    NT_WIDGET_GRID,
    NT_WIDGET_LABEL,
    NT_WIDGET_BUTTON,
    NT_WIDGET_CANVAS,
    NT_WIDGET_SCROLL_CONTAINER,
    NT_WIDGET_INPUT,
    NT_WIDGET_TEXTAREA,
    NT_WIDGET_MENUBAR,
    NT_WIDGET_MENUBAR_ENTRY,
    NT_WIDGET_MENU,
    NT_WIDGET_MENUITEM,
    NT_WIDGET_TABVIEW,
    NT_WIDGET_TAB,
    NT_WIDGET_IMAGE,
    NT_WIDGET_LIST_VIEW,
    NT_WIDGET_LIST_ITEM,
    NT_WIDGET_COLUMN_VIEW,
    NT_WIDGET_COLUMN_VIEW_ROW,
    NT_WIDGET_CHECKBOX,
    NT_WIDGET_ICON_VIEW,
    NT_WIDGET_ICON_ENTRY,
    NT_WIDGET_SEPARATOR,
    NT_WIDGET_CUSTOM,           // special custom widget, can be used outside of internal
} nt_widget_type_t;

typedef enum _nt_alignment {
    NT_ALIGN_STRETCH,
    NT_ALIGN_LEFT,
    NT_ALIGN_RIGHT,
    NT_ALIGN_CENTER,
    NT_ALIGN_TOP,
    NT_ALIGN_BOT
} nt_alignment_t;

/* Expansion flags */
#define NT_NO_EXPAND                0x0
#define NT_EXPAND_HORIZONTAL        0x1
#define NT_EXPAND_VERTICAL          0x2

/* widget flags */
#define NT_WIDGET_NEED_RECALC   0x1     // The widget needs to recalculate its size
#define NT_WIDGET_DIRTY         0x2     // Dirty, needs render
#define NT_WIDGET_FOCUS         0x4     // This widget can be focused
#define NT_WIDGET_MOUSE_INVAL   0x8     // Invalidate the widget on mouse enter/leave/click/release
#define NT_WIDGET_INVIS         0x10    // Invisible widget
#define NT_WIDGET_SELECTABLE    0x20    // This widget is selectable

struct _nt_widget;
struct _nt_render_surface;

typedef struct _nt_widget_vtable {
    size_t class_size;
    void (*init)(struct _nt_widget*);
    void (*free)(struct _nt_widget*);
    void (*render)(struct _nt_widget*, struct _nt_render_surface*);
    void (*calc_size)(struct _nt_widget*);
    void (*adjust_size)(struct _nt_widget*);
    
    // measure is a method used to compute what a widget's dimensions would look like inside of a given rectangle
    // its similar to gtk_widget_get_preferred_width_for_height and vice versa, but less versatile and less good
    // if measure is not specified it will try to use the widget calculated size
    // results returned in the same rectangle!
    void (*measure)(struct _nt_widget *, struct _nt_rect *);
} nt_widget_vtable_t;

typedef struct _nt_widget_layout_data {
    unsigned char expansion;
    nt_alignment_t halign;
    nt_alignment_t valign;
} nt_widget_layout_data_t;

typedef struct _nt_widget {
    struct _nt_widget *prev;
    struct _nt_widget *next;

    struct _nt_widget *parent;
    struct _nt_widget *children;

    struct _nt_window *window;

    nt_widget_vtable_t *vtbl;
    
    nt_widget_type_t type;
    nt_style_t style;
    nt_widget_layout_data_t layout;
    nt_render_surface_t surf;
    unsigned char flags;

    bool hovered;
    bool focused;
    bool selected;

    nt_signal_t *signal_head;
    nt_event_handler_t event_handlers[NT_NEVENTS];
    struct _nt_widget *context_menu;

    // !!! WE love hacking in double click
    uint64_t last_click_time;   // Last click time in millis

    // Sizing data for the widget
    // Heavily inspired by TGUI
    struct {
        long min_width;
        long min_height;
        long pref_width;
        long pref_height;
    } size_data;

    struct {
        int x;
        int y;
        int w;
        int h;
    } layout_data;

    // !!! bad design
    void (*render_hook)(struct _nt_widget*, struct _nt_render_surface*);
    
    void *priv; // private data, should be used sparingly
} nt_widget_t;

static inline struct _nt_window *nt_widget_get_window(nt_widget_t *widget) {
    while (widget) {
        if (widget->window) {
            return widget->window;
        }
        widget = widget->parent;
    }
    return NULL;
}

nt_widget_t *nt_widget_create(nt_widget_type_t type, nt_widget_vtable_t *vtable);
void nt_widget_free(nt_widget_t *widget);
void nt_widget_set_parent(nt_widget_t *widget, nt_widget_t *parent);
void nt_widget_invalidate(nt_widget_t *widget);
void nt_widget_mark_recalc(nt_widget_t *widget);
void nt_widget_calculate_size(nt_widget_t *widget);
void nt_widget_adjust_size(nt_widget_t *widget, struct _nt_rect *region);
void nt_widget_render(nt_widget_t *widget);
void nt_widget_dispatch(nt_widget_t *widget, nt_event_t *event);
nt_widget_t *nt_widget_at(struct _nt_window *window, int x, int y);
void nt_widget_set_parent_append(nt_widget_t *widget, nt_widget_t *parent);
void nt_widget_set_context_menu(nt_widget_t *widget, nt_widget_t *menu);
void nt_widget_set_selected(nt_widget_t *widget, bool selected);
void nt_widget_set_selectable(nt_widget_t *widget, bool selectable);
void nt_widget_measure(nt_widget_t *widget, struct _nt_rect *rect);

// set expansion mask
static inline void nt_widget_set_expansion(nt_widget_t *widget, unsigned char expansion) {
    widget->layout.expansion = expansion;
}

// set horizontal alignment
static inline void nt_widget_set_horizontal_alignment(nt_widget_t *widget, nt_alignment_t align) {
    widget->layout.halign = align;
}

// set vertical alignment
static inline void nt_widget_set_vertical_alignment(nt_widget_t *widget, nt_alignment_t align) {
    widget->layout.valign = align;
}

// return the X of the widget with margin
static inline int nt_widget_get_x_inner(nt_widget_t *widget) {
    return widget->layout_data.x + widget->style.padding[LEFT];
}

// return the Y of the widget with margin
static inline int nt_widget_get_y_inner(nt_widget_t *widget) {
    return widget->layout_data.y + widget->style.padding[TOP];
}

// returns the width of the widget minus margin
static inline int nt_widget_get_width_inner(nt_widget_t *widget) {
    return widget->layout_data.w - widget->style.padding[RIGHT] - widget->style.padding[LEFT];
}

// returns the height of the widget minus margin
static inline int nt_widget_get_height_inner(nt_widget_t *widget) {
    return widget->layout_data.h - widget->style.padding[BOTTOM] - widget->style.padding[TOP];
}

// set render hook
// render hook is used when you want to add some extra pizazz that the style system cant give you
static inline void nt_widget_set_render_hook(nt_widget_t *w, void (*render_hook)(struct _nt_widget*,struct _nt_render_surface*)) {
    w->render_hook = render_hook;
}

// set focusable
static inline void nt_widget_set_focusable(nt_widget_t *widget, bool focusable) {
    if (focusable) widget->flags |= NT_WIDGET_FOCUS;
    else widget->flags &= ~(NT_WIDGET_FOCUS);
}

// set visible
static inline void nt_widget_set_visible(nt_widget_t *widget, bool visible) {
    if (visible) widget->flags &= ~(NT_WIDGET_INVIS);
    else widget->flags |= NT_WIDGET_INVIS;
    nt_widget_invalidate(widget);
}

// TODO Possibly account for padding
// abs x/y -> rel x/y, used for events
static inline unsigned int nt_event_to_rel_x(struct _nt_widget *self, unsigned int x) { return x - (nt_widget_get_x_inner(self)); }
static inline unsigned int nt_event_to_rel_y(struct _nt_widget *self, unsigned int y) { return y - (nt_widget_get_y_inner(self)); }

// Get excess width/height by taking the allocated w/h and subtracting the minimum w/h
static inline int nt_widget_get_excess_width(nt_widget_t *widget) {
    return nt_widget_get_width_inner(widget) - (widget->size_data.min_width - widget->style.margin[LEFT] - widget->style.margin[RIGHT]);
}

static inline int nt_widget_get_excess_height(nt_widget_t *widget) {
    return nt_widget_get_height_inner(widget) - (widget->size_data.min_height - widget->style.margin[TOP] - widget->style.margin[BOTTOM]);
}

#endif
