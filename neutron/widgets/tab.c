/**
 * @file neutron/widgets/tab.c
 * @brief Tab header widget
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

#define NT_TAB(w) ((nt_tab_t*)(w))
#define NT_TAB_PAD_X 8
#define NT_TAB_PAD_TOP 3
#define NT_TAB_PAD_BOTTOM 4
#define NT_TAB_MIN_H 20

static void nt_tab_calc_size(nt_widget_t *widget) {
    nt_tab_t *tab = NT_TAB(widget);

    if (!tab->header) {
        widget->size_data.min_width = 18;
        widget->size_data.min_height = NT_TAB_MIN_H;
        widget->size_data.pref_width = 18;
        widget->size_data.pref_height = NT_TAB_MIN_H;
        return;
    }

    widget->size_data.min_width = tab->header->size_data.min_width + (NT_TAB_PAD_X * 2) + 1;
    widget->size_data.min_height = tab->header->size_data.min_height + NT_TAB_PAD_TOP + NT_TAB_PAD_BOTTOM;
    widget->size_data.pref_width = tab->header->size_data.pref_width + (NT_TAB_PAD_X * 2);
    widget->size_data.pref_height = tab->header->size_data.pref_height + NT_TAB_PAD_TOP + NT_TAB_PAD_BOTTOM;

    if (widget->size_data.min_height < NT_TAB_MIN_H) widget->size_data.min_height = NT_TAB_MIN_H;
    if (widget->size_data.pref_height < NT_TAB_MIN_H) widget->size_data.pref_height = NT_TAB_MIN_H;
}

static void nt_tab_adjust_size(nt_widget_t *widget) {
    nt_tab_t *tab = NT_TAB(widget);
    if (!tab->header) return;

    nt_rect_t rect = {
        .x = widget->layout_data.x + NT_TAB_PAD_X,
        .y = widget->layout_data.y + NT_TAB_PAD_TOP,
        .w = widget->layout_data.w > (NT_TAB_PAD_X * 2) ? widget->layout_data.w - (NT_TAB_PAD_X * 2) : widget->layout_data.w,
        .h = widget->layout_data.h > (NT_TAB_PAD_TOP + NT_TAB_PAD_BOTTOM)
            ? widget->layout_data.h - (NT_TAB_PAD_TOP + NT_TAB_PAD_BOTTOM)
            : widget->layout_data.h,
    };

    nt_widget_adjust_size(tab->header, &rect);
}

static void nt_tab_render(nt_widget_t *widget, nt_render_surface_t *surface) {
    nt_tab_t *tab = NT_TAB(widget);
    nt_rect_t r = { .x = 0, .y = 1, .w = widget->layout_data.w, .h = widget->layout_data.h };

    if (r.w == 0 || r.h == 0) return;

    nt_color_t border = NT_COLOR(0x9c, 0xa4, 0xad, 0xff);
    nt_color_t inner_top = NT_COLOR(0xf8, 0xf8, 0xf8, 0xff);
    nt_color_t inner_bottom = NT_COLOR(0xd8, 0xdc, 0xe2, 0xff);

    if (tab->state == NT_TAB_STATE_ACTIVE) {
        border = NT_COLOR(0xb9, 0xb9, 0xb9, 0xff);
        inner_top = tab->content->style.bg;
        inner_bottom = tab->content->style.bg;
    } else if (tab->state == NT_TAB_STATE_HOVERED) {
        // more windows 7-like
        // border = NT_COLOR(0x6f, 0xa6, 0xd0, 0xff);
        // inner_top = NT_COLOR(0xff, 0xff, 0xff, 0xff);
        // inner_bottom = NT_COLOR(0xd8, 0xec, 0xf8, 0xff);

        inner_top = NT_COLOR(0xf0,0xf0,0xf0,0xff);
        inner_bottom = NT_COLOR(0xd0,0xd0,0xd0,0xff);
    }

    // give it that file cabinet look
    if (tab->state != NT_TAB_STATE_ACTIVE && r.h > 1) {
        r.y += 1;
        r.h -= 1;
    }
    
    nt_render_border_rect(surface, &NT_RECT(r.x, r.y, r.w, r.h), 1, border);

    
    if (r.w > 2 && r.h > 2) {
        nt_render_rounded_rect_gradient(surface, &NT_RECT(r.x + 1, r.y + 1, r.w - 2, r.h - 1), 1, inner_top, inner_bottom, false);
    }

    if (tab->state == NT_TAB_STATE_ACTIVE && r.h > 0) {
        nt_render_fill_rect(surface, &NT_RECT(1, r.h - 1, r.w > 2 ? r.w - 2 : r.w, 1), NT_COLOR(0xff, 0xff, 0xff, 0xff));
    }
}

static bool nt_tab_enter(nt_widget_t *widget, nt_event_t *event) {
    (void)event;
    nt_tab_t *tab = NT_TAB(widget);
    if (tab->state == NT_TAB_STATE_DEFAULT) {
        tab->state = NT_TAB_STATE_HOVERED;
        nt_widget_invalidate(widget);
    }
    return true;
}

static bool nt_tab_leave(nt_widget_t *widget, nt_event_t *event) {
    (void)event;
    nt_tab_t *tab = NT_TAB(widget);
    if (tab->state == NT_TAB_STATE_HOVERED) {
        tab->state = NT_TAB_STATE_DEFAULT;
        nt_widget_invalidate(widget);
    }
    return true;
}

static bool nt_tab_down(nt_widget_t *widget, nt_event_t *event) {
    (void)event;
    nt_tab_t *tab = NT_TAB(widget);

    if (widget->parent && widget->parent->type == NT_WIDGET_TABVIEW) {
        nt_tab_view_select(widget->parent, widget);
        return false;
    }

    return true;
}

nt_widget_vtable_t tab_vtable = {
    .class_size = sizeof(nt_tab_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_tab_calc_size,
    .adjust_size = nt_tab_adjust_size,
    .render = nt_tab_render,
};

nt_widget_t *nt_tab_create(nt_widget_t *header, nt_widget_t *content) {
    nt_widget_t *widget = nt_widget_create(NT_WIDGET_TAB, &tab_vtable);
    nt_widget_set_focusable(widget, true);

    nt_tab_t *tab = NT_TAB(widget);
    tab->header = header;
    tab->content = content;
    tab->state = NT_TAB_STATE_DEFAULT;
    tab->closable = false;

    if (header) {
        nt_widget_set_parent(header, widget);
        nt_widget_set_horizontal_alignment(header, NT_ALIGN_CENTER);
        nt_widget_set_vertical_alignment(header, NT_ALIGN_BOT);
        nt_style_set_margin_all(&tab->widget.style,0);
        // nt_style_set_margin(&tab->widget.style, RIGHT, 1);
        nt_widget_set_expansion(header, NT_EXPAND_VERTICAL | NT_EXPAND_HORIZONTAL);
    }

    nt_event_set_handler(widget, NT_EVENT_MOUSE_ENTER, nt_tab_enter);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_LEAVE, nt_tab_leave);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_DOWN, nt_tab_down);

    return widget;
}

nt_widget_t *nt_tab_create_label(char *text, nt_widget_t *content) {
    nt_widget_t *label = nt_label_create(text);
    
    nt_style_set_margin_all(&label->style,0);
        nt_style_set_margin(&label->style, TOP, 2);
    nt_style_set_bg_color(&label->style, NT_COLOR(0, 0, 0, 0));
    return nt_tab_create(label, content);
}

nt_widget_t *nt_tab_get_header(nt_widget_t *tab) {
    return NT_TAB(tab)->header;
}

nt_widget_t *nt_tab_get_content(nt_widget_t *tab) {
    return NT_TAB(tab)->content;
}

void nt_tab_set_content(nt_widget_t *tab_widget, nt_widget_t *content) {
    nt_tab_t *tab = NT_TAB(tab_widget);
    tab->content = content;

    if (tab_widget->parent && tab_widget->parent->type == NT_WIDGET_TABVIEW && content) {
        nt_widget_set_parent_append(content, tab_widget->parent);
        nt_widget_set_visible(content, tab->state == NT_TAB_STATE_ACTIVE);
    }

    nt_widget_mark_recalc(tab_widget);
}

void nt_tab_set_active(nt_widget_t *tab_widget, bool active) {
    nt_tab_t *tab = NT_TAB(tab_widget);

    tab->state = active ? NT_TAB_STATE_ACTIVE : NT_TAB_STATE_DEFAULT;
    if (tab->content) nt_widget_set_visible(tab->content, active);
    nt_widget_invalidate(tab_widget);
}

bool nt_tab_is_active(nt_widget_t *tab) {
    return NT_TAB(tab)->state == NT_TAB_STATE_ACTIVE;
}

