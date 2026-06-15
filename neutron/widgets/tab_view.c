/**
 * @file neutron/widgets/tab_view.c
 * @brief Tab view widget
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

#define NT_TAB_VIEW(w) ((nt_tab_view_t*)(w))
#define NT_TAB(w) ((nt_tab_t*)(w))

static void nt_tab_view_calc_size(nt_widget_t *widget) {
    nt_tab_view_t *view = NT_TAB_VIEW(widget);
    int tabs_w = 0;
    int tabs_h = 0;
    int content_w = 0;
    int content_h = 0;
    size_t tab_count = 0;

    NT_ITERATE_CHILDREN(widget) {
        if (child->type == NT_WIDGET_TAB) {
            tabs_w += child->size_data.pref_width;
            if (child->size_data.pref_height > tabs_h) tabs_h = child->size_data.pref_height;
            tab_count++;
        } else {
            if (child->size_data.min_width > content_w) content_w = child->size_data.min_width;
            if (child->size_data.min_height > content_h) content_h = child->size_data.min_height;
        }
    }

    if (tabs_h < 20 && tab_count) tabs_h = 20;

    view->tab_count = tab_count;
    view->header_height = tabs_h;

    widget->size_data.min_width = content_w > tabs_w ? content_w : tabs_w;
    widget->size_data.min_height = content_h + tabs_h;
    widget->size_data.pref_width = widget->size_data.min_width;
    widget->size_data.pref_height = widget->size_data.min_height;
}

static void nt_tab_view_adjust_size(nt_widget_t *widget) {
    nt_tab_view_t *view = NT_TAB_VIEW(widget);
    int x = widget->layout_data.x;
    int y = widget->layout_data.y;
    int w = widget->layout_data.w;
    int h = widget->layout_data.h;
    int header_h = (int)view->header_height;
    int cur_x = x;

    NT_ITERATE_CHILDREN(widget) {
        if (child->type != NT_WIDGET_TAB) continue;

        int tab_w = child->size_data.pref_width;
        if (tab_w < child->size_data.min_width) tab_w = child->size_data.min_width;

        nt_widget_adjust_size(child, &NT_RECT(cur_x, y, tab_w, child->size_data.pref_height));
        cur_x += tab_w - 1;
    }

    int content_y = y + header_h - 1;
    int content_h = h > header_h ? h - header_h + 1 : 0;

    NT_ITERATE_CHILDREN(widget) {
        if (child->type == NT_WIDGET_TAB) continue;
        nt_widget_adjust_size(child, &NT_RECT(x + 1, content_y + 1, w > 2 ? w - 2 : w, content_h > 2 ? content_h - 2 : content_h));
    }
}

static void nt_tab_view_render(nt_widget_t *widget, nt_render_surface_t *surface) {
    nt_tab_view_t *view = NT_TAB_VIEW(widget);
    int header_h = (int)view->header_height;

    if (widget->layout_data.w == 0 || widget->layout_data.h == 0) return;

    if (header_h < widget->layout_data.h) {
        nt_rect_t panel = {
            .x = 0,
            .y = header_h - 1,
            .w = widget->layout_data.w,
            .h = widget->layout_data.h - header_h + 1,
        };

        nt_render_rounded_rect_gradient(surface, &panel, 2, NT_COLOR(0xff, 0xff, 0xff, 0xff), NT_COLOR(0xf1, 0xf4, 0xf8, 0xff), false);
        nt_render_border_rect(surface, &panel, 1, NT_COLOR(0xb9, 0xb9, 0xb9, 0xff));
    }
}

nt_widget_vtable_t tab_view_vtable = {
    .class_size = sizeof(nt_tab_view_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_tab_view_calc_size,
    .adjust_size = nt_tab_view_adjust_size,
    .render = nt_tab_view_render,
};

nt_widget_t *nt_tab_view_create() {
    nt_widget_t *widget = nt_widget_create(NT_WIDGET_TABVIEW, &tab_view_vtable);
    nt_tab_view_t *view = NT_TAB_VIEW(widget);
    view->selected = NULL;
    view->tab_count = 0;
    view->header_height = 0;

    nt_widget_set_expansion(widget, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_style_set_bg_color(&widget->style, NT_COLOR(0, 0, 0, 0));

    return widget;
}

void nt_tab_view_append(nt_widget_t *tab_view, nt_widget_t *tab_widget) {
    if (!tab_view || !tab_widget || tab_view->type != NT_WIDGET_TABVIEW || tab_widget->type != NT_WIDGET_TAB) return;

    nt_tab_view_t *view = NT_TAB_VIEW(tab_view);
    nt_tab_t *tab = NT_TAB(tab_widget);

    nt_widget_set_parent_append(tab_widget, tab_view);

    if (tab->content) {
        nt_widget_set_parent_append(tab->content, tab_view);
        nt_widget_set_expansion(tab->content, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
        nt_style_set_margin_all(&tab->content->style, 0);
        nt_widget_set_visible(tab->content, false);
    }

    view->tab_count++;

    if (!view->selected) {
        nt_tab_view_select(tab_view, tab_widget);
    }

    nt_widget_mark_recalc(tab_view);
}

void nt_tab_view_select(nt_widget_t *tab_view, nt_widget_t *tab_widget) {
    if (!tab_view || !tab_widget || tab_view->type != NT_WIDGET_TABVIEW || tab_widget->type != NT_WIDGET_TAB) return;
    if (tab_widget->parent != tab_view) return;

    nt_tab_view_t *view = NT_TAB_VIEW(tab_view);

    if (view->selected == tab_widget) return;

    if (view->selected) {
        nt_tab_set_active(view->selected, false);
    }

    view->selected = tab_widget;
    nt_tab_set_active(tab_widget, true);
    nt_signal_emit_name(tab_view, "selection-changed");
    nt_widget_invalidate(tab_view);
}

void nt_tab_view_select_index(nt_widget_t *tab_view, size_t index) {
    nt_widget_t *tab = nt_tab_view_get_tab(tab_view, index);
    if (tab) nt_tab_view_select(tab_view, tab);
}

nt_widget_t *nt_tab_view_get_selected(nt_widget_t *tab_view) {
    if (!tab_view || tab_view->type != NT_WIDGET_TABVIEW) return NULL;
    return NT_TAB_VIEW(tab_view)->selected;
}

nt_widget_t *nt_tab_view_get_tab(nt_widget_t *tab_view, size_t index) {
    if (!tab_view || tab_view->type != NT_WIDGET_TABVIEW) return NULL;

    size_t cur = 0;
    NT_ITERATE_CHILDREN(tab_view) {
        if (child->type != NT_WIDGET_TAB) continue;
        if (cur == index) return child;
        cur++;
    }

    return NULL;
}

size_t nt_tab_view_get_tab_count(nt_widget_t *tab_view) {
    if (!tab_view || tab_view->type != NT_WIDGET_TABVIEW) return 0;
    return NT_TAB_VIEW(tab_view)->tab_count;
}
