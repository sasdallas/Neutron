/**
 * @file neutron/widgets/checkbox.c
 * @brief Neutron checkbox
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

#define NT_CHECKBOX(w) ((nt_checkbox_t*)w)

#define CHECK_SPACING_H     4
#define CHECK_SPACING_V     2

nt_image_t *check_image;
static bool check_image_loaded = false;

#define CHECK_WIDTH         12
#define CHECK_HEIGHT        12

static void nt_checkbox_calc_size(nt_widget_t *w) {
    nt_checkbox_t *checkbox = NT_CHECKBOX(w);
    
    int width = CHECK_WIDTH + CHECK_SPACING_H;
    int height = CHECK_HEIGHT + CHECK_SPACING_V;
    if (checkbox->child != NULL) {
        width += checkbox->child->size_data.pref_width;
        height+= checkbox->child->size_data.pref_height;
    }

    w->size_data.min_width = w->size_data.pref_width = width;
    w->size_data.min_height = w->size_data.pref_height = height;
}

static void nt_checkbox_adjust_size(nt_widget_t *w) {
    nt_checkbox_t *checkbox = NT_CHECKBOX(w);
    if (checkbox->child == NULL) return;

    nt_rect_t adj = {
        .x = nt_widget_get_x_inner(w) + CHECK_WIDTH + CHECK_SPACING_H,
        .y = nt_widget_get_y_inner(w) ,
        .w = nt_widget_get_width_inner(w) - (CHECK_WIDTH + CHECK_SPACING_H),
        .h = nt_widget_get_height_inner(w)
    };

    nt_widget_adjust_size(checkbox->child, &adj);
}

static void nt_checkbox_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_checkbox_t *checkbox = NT_CHECKBOX(w);

    if (checkbox->disabled) {
        assert(0 && "TODO");
    } else {
        if (checkbox->checked) {
            nt_render_rounded_rect(surf, &NT_RECT(0, 0, CHECK_WIDTH, CHECK_HEIGHT), 4, NT_COLOR(0x46,0x90,0xd0,255));
            
            if (checkbox->held) {
                nt_render_rounded_rect_gradient(surf, &NT_RECT(1, 1, CHECK_WIDTH - 2, CHECK_HEIGHT - 2), 4, NT_COLOR(0x3d,0x83,0xcc,255), NT_COLOR(0x19, 0x67, 0xc4, 255), false);
            } else {
                nt_render_rounded_rect_gradient(surf, &NT_RECT(1, 1, CHECK_WIDTH - 2, CHECK_HEIGHT - 2), 4, NT_COLOR(0x5d,0xa3,0xec,255), NT_COLOR(0x39, 0x87, 0xe4, 255), false);
            }

            nt_render_draw_image(surf, check_image, 1,1);
        } else {
            nt_render_rounded_rect(surf, &NT_RECT(0, 0, CHECK_WIDTH, CHECK_HEIGHT), 4, NT_COLOR(170,170,170,255));
            if (checkbox->held) {
                nt_render_rounded_rect(surf, &NT_RECT(1, 1, CHECK_WIDTH - 2, CHECK_HEIGHT - 2), 4, NT_COLOR(150,150,150,255));
            } else {
                nt_render_rounded_rect(surf, &NT_RECT(1, 1, CHECK_WIDTH - 2, CHECK_HEIGHT - 2), 4, NT_COLOR(255,255,255,255));
            }
        }
    }
}

static bool nt_checkbox_event(nt_widget_t *w, nt_event_t *ev) {
    nt_checkbox_t *cb = NT_CHECKBOX(w);
    
    if (cb->disabled) {
        return true;
    }

    if (ev->pointer.y - nt_widget_get_y_inner(w) > CHECK_HEIGHT + 1) {
        return true;
    }

    if (ev->type == NT_EVENT_MOUSE_DOWN) {
        cb->held = true;
        nt_widget_invalidate(w);
    } else if (ev->type == NT_EVENT_MOUSE_UP) {
        cb->held = false;
        cb->checked = !cb->checked;
        nt_signal_emit_name(w, "checkbox-toggled");
        nt_widget_invalidate(w);
    } else if (ev->type == NT_EVENT_MOUSE_LEAVE) {
        if (cb->held) {
            cb->held = false;
            nt_widget_invalidate(w);
        }
    }
}

nt_widget_vtable_t checkbox_vtable = {
    .class_size = sizeof(nt_checkbox_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_checkbox_calc_size,
    .adjust_size = nt_checkbox_adjust_size,
    .render = nt_checkbox_render
};


nt_widget_t *nt_checkbox_create(nt_widget_t *child, bool default_state) {
    if (!check_image_loaded) {
        check_image = nt_icon_get("checkmark", NULL, 10);
        check_image_loaded = true;
    }

    nt_widget_t *w = nt_widget_create(NT_WIDGET_CHECKBOX, &checkbox_vtable);
    nt_checkbox_t *cb = NT_CHECKBOX(w);
    cb->child = child;
    cb->checked = default_state;
    cb->disabled = false;
    cb->held = false;

    nt_event_set_handler(w, NT_EVENT_MOUSE_DOWN, nt_checkbox_event);
    nt_event_set_handler(w, NT_EVENT_MOUSE_UP, nt_checkbox_event);
    nt_event_set_handler(w, NT_EVENT_MOUSE_LEAVE, nt_checkbox_event);

    if (child) {
        nt_widget_set_parent(child, w);
    }

    return w;
}

nt_widget_t *nt_checkbox_create_labelled(char *text, bool default_state) {
    nt_widget_t *lbl = nt_label_create(text);
    nt_style_set_margin_all(&lbl->style, 0);
    return nt_checkbox_create(lbl, default_state);
}

void nt_checkbox_set_state(nt_widget_t *widget, bool state) {
    nt_checkbox_t *cb = NT_CHECKBOX(widget);
    if (cb->checked != state) {
        cb->checked = state;
        nt_widget_invalidate(widget);
    }
}
