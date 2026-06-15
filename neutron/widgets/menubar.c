/**
 * @file neutron/widgets/menubar.c
 * @brief Menubar
 * 
 * Also contains the widget type NT_WIDGET_MENUBAR_ENTRY
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#include <neutron/neutron.h>

#define NT_MENUBAR(w) ((nt_menubar_t*)w)
#define NT_MENUBAR_ENTRY(w) ((nt_menubar_entry_t*)w)


// TODO: This is really hacky stuff
static void nt_menubar_entry_calc_size(nt_widget_t *w) {
    nt_menubar_entry_t *ent = NT_MENUBAR_ENTRY(w);
    w->size_data.min_width = ent->label->size_data.min_width;
    w->size_data.pref_width = ent->label->size_data.pref_width;
    w->size_data.min_height = ent->label->size_data.min_height;
    w->size_data.pref_height = ent->label->size_data.pref_height;
}

static void nt_menubar_entry_adjust_size(nt_widget_t *w) {
    nt_rect_t r = {
        .x = nt_widget_get_x_inner(w),
        .y = nt_widget_get_y_inner(w),
        .w = nt_widget_get_width_inner(w),
        .h = nt_widget_get_height_inner(w),
    };

    nt_menubar_entry_t *ent = NT_MENUBAR_ENTRY(w);
    nt_widget_adjust_size(ent->label, &r);

    // now adjust for the menu

    nt_rect_t mr = {
        .x = nt_widget_get_x_inner(w),
        .y = r.y + r.h,
        .w = ent->menu->size_data.pref_width,
        .h = ent->menu->size_data.pref_height,
    };

    nt_widget_adjust_size(ent->menu, &mr);
}

static void nt_menubar_entry_render(nt_widget_t *w, nt_render_surface_t *surf) {
    if (w->focused) {
        nt_render_clear(surf, NT_COLOR(0xbb,0xde,0xfb, 0xff));
    }
}

static bool nt_menubar_entry_event(nt_widget_t *w, nt_event_t *e) {
    nt_menubar_entry_t *ent = NT_MENUBAR_ENTRY(w);
    nt_window_t *win = nt_widget_get_window(w);

    if (w != e->w && e->w != ent->label) return false;

    // idk how this works it just does
    if (e->type == NT_EVENT_MOUSE_DOWN) {
        if (win->popup == ent->menu) {
            nt_window_set_focus(win, NULL);
            nt_menu_hide(ent->menu);
            nt_window_set_popup(win, NULL);
        } else {
            nt_menu_show(ent->menu);
        }

        return true;
    }

    if (e->type == NT_EVENT_FOCUS_ENTER) {
        // open menu
    } else if (e->type == NT_EVENT_FOCUS_EXIT) {
    } else if (e->type == NT_EVENT_MOUSE_ENTER) {
        // hack
        nt_menubar_t *mb = NT_MENUBAR(w->parent);
        if (mb->mouse_sel) {
            nt_window_set_focus(nt_widget_get_window(w), w);
            nt_menu_show(ent->menu);
        }
    }

    return true;
}


static void nt_menubar_calc_size(nt_widget_t *w) {
    w->size_data.min_height = 0;
    w->size_data.min_width = 0;
    w->size_data.pref_height = 0;
    w->size_data.pref_width = 0;
    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_height > w->size_data.min_height) w->size_data.min_height = child->size_data.min_height;
        if (child->size_data.pref_height > w->size_data.pref_height) w->size_data.pref_height = child->size_data.pref_height;
        w->size_data.min_width += child->size_data.min_width;
        w->size_data.pref_width += child->size_data.pref_width;
    }

    w->size_data.min_height += 1;
    w->size_data.pref_height += 1;
}

static void nt_menubar_adjust_size(nt_widget_t *w) {
    int cur_x = nt_widget_get_x_inner(w);
    int cur_y = nt_widget_get_y_inner(w);

    NT_ITERATE_CHILDREN(w) {
        nt_rect_t r = {
            .x = cur_x,
            .y = cur_y,
            .w = child->size_data.pref_width,
            .h = child->size_data.pref_height,
        };

        nt_widget_adjust_size(child, &r);
        cur_x += r.w + 3;
    }
}


static void nt_menubar_render(nt_widget_t *w, nt_render_surface_t *surf) {
    // The menubar is designed to visually blend into the top of the window
    nt_render_fill_rect(surf, &NT_RECT(0,nt_widget_get_height_inner(w)-1,nt_widget_get_width_inner(w),3), NT_COLOR(0xe8,0xe8,0xe8,255));
}

static bool nt_menubar_event(nt_widget_t *w, nt_event_t *e) {
    nt_menubar_t *mb = NT_MENUBAR(w);
    NT_DEBUG("nt_menubar_event %d %p\n", e->type, e->w);
    if (e->type == NT_EVENT_FOCUS_EXIT) {
        // Exited focus from the bar or a child
        mb->mouse_sel = false;
    } else if (e->type == NT_EVENT_FOCUS_ENTER) {
        // The menubar is not focusable, but its children are. This means that a valid child got focused
        mb->mouse_sel = true;
    }
}

nt_widget_vtable_t menubar_entry_vtable = {
    .class_size = sizeof(nt_menubar_entry_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_menubar_entry_calc_size,
    .adjust_size = nt_menubar_entry_adjust_size,
    .render = nt_menubar_entry_render,
};

nt_widget_vtable_t menubar_vtable = {
    .class_size = sizeof(nt_menubar_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_menubar_calc_size,
    .adjust_size = nt_menubar_adjust_size,
    .render = nt_menubar_render,
};

nt_widget_t *nt_menubar_entry_create(char *text, nt_widget_t *menu) {
    nt_widget_t *menubar_entry = nt_widget_create(NT_WIDGET_MENUBAR_ENTRY, &menubar_entry_vtable);
    menubar_entry->flags |= NT_WIDGET_MOUSE_INVAL;
    nt_widget_set_focusable(menubar_entry, true);
    nt_widget_set_expansion(menubar_entry, 0);
    nt_style_set_margin_all(&menubar_entry->style, 0);
    nt_style_set_bg_color(&menubar_entry->style, NT_COLOR(0,0,0,0));

    nt_menubar_entry_t *ent = NT_MENUBAR_ENTRY(menubar_entry);
    ent->selected = true;
    ent->menu = menu;

    nt_widget_set_parent(menu, menubar_entry);

    // add the label
    nt_widget_t *label = nt_label_create(text);
    nt_style_set_bg_color(&label->style, NT_COLOR(0,0,0,0));
    nt_style_set_font(&label->style, NT_SANS_10);

    // idk how this looks good but it does
    nt_style_set_margin(&label->style, TOP, 1);
    nt_style_set_margin(&label->style, BOTTOM, 1);
    nt_style_set_margin(&label->style, LEFT, 4);
    nt_style_set_margin(&label->style, RIGHT, 4);

    nt_widget_set_expansion(label, NT_EXPAND_HORIZONTAL);
    nt_widget_set_parent(label, menubar_entry);
    ent->label = label;


    nt_event_set_handler(menubar_entry, NT_EVENT_FOCUS_ENTER, nt_menubar_entry_event);
    nt_event_set_handler(menubar_entry, NT_EVENT_FOCUS_EXIT, nt_menubar_entry_event);
    nt_event_set_handler(menubar_entry, NT_EVENT_MOUSE_ENTER, nt_menubar_entry_event);
    nt_event_set_handler(menubar_entry, NT_EVENT_MOUSE_DOWN, nt_menubar_entry_event);

    return menubar_entry;
}

nt_widget_t *nt_menubar_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_MENUBAR, &menubar_vtable);
    
    nt_menubar_t *mb = NT_MENUBAR(w);
    mb->mouse_sel = false;

    nt_widget_set_focusable(w, false);
    nt_widget_set_vertical_alignment(w, NT_ALIGN_TOP);
    nt_widget_set_horizontal_alignment(w, NT_ALIGN_STRETCH);
    nt_widget_set_expansion(w, NT_EXPAND_HORIZONTAL);

    nt_style_set_margin_all(&w->style, 0);

    nt_event_set_handler(w, NT_EVENT_FOCUS_ENTER, nt_menubar_event);
    nt_event_set_handler(w, NT_EVENT_FOCUS_EXIT, nt_menubar_event);
    return w;
}

void nt_menubar_append(nt_widget_t *menubar, char *text, nt_widget_t *menu) {
    nt_widget_t *child = nt_menubar_entry_create(text, menu);
    nt_widget_set_parent_append(child, menubar);
}
