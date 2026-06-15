/**
 * @file neutron/widgets/list_view.c
 * @brief List view
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

#define NT_LIST_VIEW(w) ((nt_list_view_t*)w)
#define NT_LIST_ITEM(w) ((nt_list_item_t*)w)

static void nt_list_view_calc_size(nt_widget_t *w) {
    // similar to vertical box
    int min_width = 0;
    int min_height = 0;
    int pref_width = 0;
    int pref_height = 0;

    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > min_width) min_width = child->size_data.min_width;
        if (child->size_data.pref_width > pref_width) pref_width = child->size_data.pref_width;

        min_height = min_height + child->size_data.min_height;
        pref_height = pref_height + child->size_data.pref_height;
    }

    w->size_data.min_width = min_width;
    w->size_data.min_height = min_height;
    w->size_data.pref_width = pref_width;
    w->size_data.pref_height = pref_height;
}

static void nt_list_view_adjust_size(nt_widget_t *widget) {
    int cur_x = nt_widget_get_x_inner(widget);
    int cur_y = nt_widget_get_y_inner(widget);
    int w = nt_widget_get_width_inner(widget);
    int h = nt_widget_get_height_inner(widget);

    // lists are not homogenous
    NT_ITERATE_CHILDREN(widget) {
        nt_rect_t rect = {
            .x = cur_x,
            .y = cur_y,
            .w = w,
            .h = child->size_data.pref_height 
        };


        nt_widget_adjust_size(child, &rect);

        cur_y += rect.h;
    }
}

static void nt_list_item_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_list_item_t *item = NT_LIST_ITEM(w);
    
    if (item->child->selected) {
        // Draw a box
        nt_render_rounded_rect_gradient(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w), nt_widget_get_height_inner(w)-1), 0, NT_COLOR(0x5d,0xa3,0xec,255), NT_COLOR(0x39, 0x87, 0xe4, 255), false);
        nt_render_fill_rect(surf, &NT_RECT(0, nt_widget_get_height_inner(w)-1, nt_widget_get_width_inner(w), 1), NT_COLOR(0x46, 0x90, 0xD0, 255));
    }
}

static bool nt_list_item_event(nt_widget_t *w, nt_event_t *ev) {
    nt_list_item_t *item = NT_LIST_ITEM(w);
    if (ev->type == NT_EVENT_FOCUS_ENTER) {
        nt_widget_set_selected(item->child, true);
        nt_signal_emit_name(item->child, "selected");
        nt_widget_invalidate(w);
    } else if (ev->type == NT_EVENT_FOCUS_EXIT) {
        nt_widget_set_selected(item->child, false);
        nt_signal_emit_name(item->child, "deselected");
        nt_widget_invalidate(w);
    }

    return true;
}

static void nt_list_item_calc_size(nt_widget_t *w) {
    nt_list_item_t *item = NT_LIST_ITEM(w);

    w->size_data.min_width = item->child->size_data.min_width;
    w->size_data.min_height = item->child->size_data.min_height;
    w->size_data.pref_width = item->child->size_data.pref_width;
    w->size_data.pref_height = item->child->size_data.pref_height;
} 

static void nt_list_item_adjust_size(nt_widget_t *w) {
    nt_list_item_t *item = NT_LIST_ITEM(w);
    
    nt_rect_t rect = {
        .x = nt_widget_get_x_inner(w),
        .y = nt_widget_get_y_inner(w),
        .w = nt_widget_get_width_inner(w),
        .h = nt_widget_get_height_inner(w)
    };

    nt_widget_adjust_size(item->child, &rect);
}

nt_widget_vtable_t list_item_vtable = {
    .class_size = sizeof(nt_list_item_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_list_item_calc_size,
    .adjust_size = nt_list_item_adjust_size,
    .render = nt_list_item_render 
};

nt_widget_vtable_t list_view_vtable = {
    .class_size = sizeof(nt_list_view_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_list_view_calc_size,
    .adjust_size = nt_list_view_adjust_size,
    .render = NULL,
};


nt_widget_t *nt_list_view_create() {
    nt_widget_t *list_view = nt_widget_create(NT_WIDGET_LIST_VIEW, &list_view_vtable);
    nt_style_set_bg_color(&list_view->style, NT_COLOR(255,255,255,255));
    nt_style_set_padding_all(&list_view->style, 1);
    nt_style_set_border_thickness(&list_view->style, 1);
    nt_style_set_border_color(&list_view->style, NT_COLOR(170,170,170,255));
    nt_style_set_border_gradient(&list_view->style, NT_BORDER_GRADIENT_NONE);
    nt_style_set_border_rounded(&list_view->style, 2);
    return list_view;
}

void nt_list_view_append(struct _nt_widget *view, struct _nt_widget *to_append) {
    nt_widget_t *container = nt_widget_create(NT_WIDGET_LIST_ITEM, &list_item_vtable);
    nt_widget_set_selectable(container, true);

    nt_list_item_t *item = NT_LIST_ITEM(container);
    item->child = to_append;
    nt_widget_set_selectable(to_append, true);
    
    nt_style_set_bg_color(&container->style, NT_COLOR(255,255,255,255));
    nt_event_set_handler(container, NT_EVENT_FOCUS_ENTER, nt_list_item_event);
    nt_event_set_handler(container, NT_EVENT_FOCUS_EXIT, nt_list_item_event);
    nt_widget_set_focusable(container, true);
    nt_style_set_margin_all(&container->style, 0);
    nt_style_set_margin_all(&to_append->style, 1);

    nt_widget_set_expansion(container, NT_EXPAND_HORIZONTAL);
    nt_widget_set_parent(to_append, container);
    nt_widget_set_parent_append(container, view);

    // !!!: hack!!
    if (view->children == NULL) {
        nt_widget_set_selected(to_append, true);
    }
}
