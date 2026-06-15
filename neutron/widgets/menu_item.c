/**
 * @file neutron/widgets/menu_item.c
 * @brief Menu item
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
#include <string.h>

#define NT_MENU_ITEM(w) ((nt_menu_item_t*)w)

static void nt_menu_item_calc_size(nt_widget_t *w) {
    nt_menu_item_t *item = NT_MENU_ITEM(w);

    if (item->type == NT_MENU_ITEM_DEFAULT || item->type == NT_MENU_ITEM_SUBMENU) {
        size_t text_width, text_height;
        nt_render_text_dimensions(w->style.font, item->text, &text_width, &text_height);

        w->size_data.min_width = text_width + 6 + 16;
        w->size_data.min_height = 18;
        w->size_data.pref_width = text_width + 6 + 16;
        w->size_data.pref_height = 18;
    } else if (item->type == NT_MENU_ITEM_SEPARATOR) {
        w->size_data.min_width = 10;
        w->size_data.min_height = 2;
        w->size_data.pref_width = 10;
        w->size_data.pref_height = 2;
    }
}

static void nt_menu_item_adjust_size(nt_widget_t *w) {
    nt_menu_item_t *item = NT_MENU_ITEM(w);
    if (item->type == NT_MENU_ITEM_SUBMENU) {
        assert(0 && "TODO");
    }
}

static void nt_menu_item_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_menu_item_t *item = NT_MENU_ITEM(w);
    
    if (item->type == NT_MENU_ITEM_DEFAULT) {
        if (w->hovered) {
            
            nt_render_fill_rect(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w),nt_widget_get_height_inner(w)), NT_COLOR(200,200,200,255));
        }
    
        nt_render_draw_text(surf, w->style.font, 6 + 16, 3, item->text, NT_COLOR(0,0,0,255));
        if (item->img) nt_render_draw_image(surf, item->img, 1, 1);  
    } else if (item->type == NT_MENU_ITEM_SEPARATOR) {
        nt_render_fill_rect(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w), 1), NT_COLOR(178,178,178,255));
        nt_render_fill_rect(surf, &NT_RECT(0,1,nt_widget_get_width_inner(w), 1), NT_COLOR(255,255,255,255));
    }
}

bool nt_menu_item_up(nt_widget_t *widget, nt_event_t *event) {
    if ((event->pointer.buttons & NT_MOUSE_BUTTON_LEFT) == 0) return true;

    nt_menu_item_t *item = NT_MENU_ITEM(widget);
    if (item->type == NT_MENU_ITEM_DEFAULT) {
        // First close and invalidate
        nt_widget_t *parent = widget->parent;
        while (parent && parent->type != NT_WIDGET_MENU) parent = parent->parent;
        if (parent) {
            nt_menu_hide(parent);
        }

        // because we catch the last mouse event and the invalidation framework is shit this is needed
        nt_widget_invalidate(widget);

        // Now emit the signal after the menu has closed
        // Note that if this is like an "About" button and it opens a dialog it leaves a half-baked menu unless the window is updated
        nt_signal_emit_name(widget, "pressed");
    }
    return true;
}

nt_widget_vtable_t menu_item_vtable = {
    .class_size = sizeof(nt_menu_item_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_menu_item_calc_size,
    .adjust_size = nt_menu_item_adjust_size,
    .render = nt_menu_item_render
};

static nt_menu_item_t *nt_menu_item_create(nt_menu_item_type_t type) {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_MENUITEM, &menu_item_vtable);
    w->flags |= NT_WIDGET_MOUSE_INVAL;
    nt_style_set_bg_color(&w->style, NT_COLOR(0,0,0,0));
    nt_style_set_margin(&w->style, TOP, 1);
    nt_style_set_margin(&w->style, BOTTOM, 1);
    nt_style_set_margin(&w->style, LEFT, 1);

    nt_style_set_margin(&w->style, RIGHT, 1);
    nt_widget_set_expansion(w, NT_EXPAND_HORIZONTAL);
    nt_widget_set_focusable(w, true);

    nt_menu_item_t *item = NT_MENU_ITEM(w);
    item->type = type;
    item->text = NULL;
    item->submenu = NULL;

    nt_event_set_handler(w, NT_EVENT_MOUSE_UP, nt_menu_item_up);
    
    return item;
}

nt_widget_t *nt_menu_item_create_button_icon(char *text, nt_image_t *icon) {
    nt_menu_item_t *item = nt_menu_item_create(NT_MENU_ITEM_DEFAULT);
    item->text = strdup(text);
    item->img = icon;
    return &item->widget;
}

nt_widget_t *nt_menu_item_create_button(char *text) {
    return nt_menu_item_create_button_icon(text, NULL);
}

nt_widget_t *nt_menu_item_create_separator() {
    nt_menu_item_t *item = nt_menu_item_create(NT_MENU_ITEM_SEPARATOR);
    return &item->widget;
}

nt_widget_t *nt_menu_item_create_submenu_icon(char *text, nt_widget_t *submenu, nt_image_t *icon) {
    nt_menu_item_t *item = nt_menu_item_create(NT_MENU_ITEM_SUBMENU);
    item->text = text;
    item->submenu = submenu;
    item->img = icon;
    nt_widget_set_parent(submenu, &item->widget);
    return &item->widget;
}

nt_widget_t *nt_menu_item_create_submenu(char *text, nt_widget_t *submenu) {
    return nt_menu_item_create_submenu_icon(text, submenu, NULL);
}
