/**
 * @file neutron/widgets/button.c
 * @brief Neutron button
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

#define NT_BUTTON(w) ((nt_button_t*)(w))

void nt_button_calc_size(nt_widget_t *widget) {
    nt_button_t *btn = NT_BUTTON(widget);

    widget->size_data.min_width = btn->inner->size_data.min_width;
    widget->size_data.min_height = btn->inner->size_data.min_height;
    widget->size_data.pref_width = btn->inner->size_data.min_width;
    widget->size_data.pref_height = btn->inner->size_data.pref_height;
}

void nt_button_adjust_size(nt_widget_t *widget) {
    nt_button_t *btn = NT_BUTTON(widget);
    
    nt_widget_adjust_size(btn->inner, &NT_WIDGET_RECT(widget));
}

void nt_button_render(nt_widget_t *widget, nt_render_surface_t *surface) {
    nt_button_t *btn = NT_BUTTON(widget);

    nt_rect_t r = { .x = 0, .y = 0, .w = widget->layout_data.w, .h = widget->layout_data.h };
    // TODO: nt_render_rounded_rect_gradient_border or something
    if (btn->state == NT_BTN_STATE_DEFAULT) {
        nt_render_rounded_rect_gradient(surface, &r, 4, NT_COLOR(0xab,0xab,0xab,0xff), NT_COLOR(0xb0,0xb0,0xb0,0xff), false);
        nt_render_rounded_rect(surface, &NT_RECT(r.x+1, r.y + 1, r.w - 2, r.h - 2), 3, NT_COLOR(0xfc,0xfc,0xfc,0xff));
        nt_render_rounded_rect_gradient(surface, &NT_RECT(r.x+2,r.y+2, r.w-4,r.h-3), 2, NT_COLOR(0xf6,0xf6,0xf6,0xff), NT_COLOR(0xd2,0xd2,0xd2,0xff), false);
    } else if (btn->state == NT_BTN_STATE_HOVERED) {
        nt_render_rounded_rect_gradient(surface, &r, 4, NT_COLOR(0xab,0xab,0xab,0xff), NT_COLOR(0xb0,0xb0,0xb0,0xff), false);
        nt_render_rounded_rect(surface, &NT_RECT(r.x+1, r.y + 1, r.w - 2, r.h - 2), 3, NT_COLOR(0xfc,0xfc,0xfc,0xff));
        nt_render_rounded_rect_gradient(surface, &NT_RECT(r.x+2,r.y+2, r.w-4,r.h-3), 2, NT_COLOR(0xf0,0xf0,0xf0,0xff), NT_COLOR(0xe5,0xe5,0xe5,0xff), false);
    } else if (btn->state == NT_BTN_STATE_HELD) {
        nt_render_rounded_rect_gradient(surface, &r, 4, NT_COLOR(0xab,0xab,0xab,0xff), NT_COLOR(0xb0,0xb0,0xb0,0xff), false);
        nt_render_rounded_rect(surface, &NT_RECT(r.x+1, r.y + 1, r.w - 2, r.h - 2), 3, NT_COLOR(0xb4,0xb4,0xb4,0xff));
        nt_render_rounded_rect_gradient(surface, &NT_RECT(r.x+2,r.y+2, r.w-4,r.h-3), 2, NT_COLOR(0xd2,0xd2,0xd2,0xff), NT_COLOR(0xc2,0xc2,0xc2,0xff), false);
    } else {
        NT_WARN("TODO: Render button disabled\n");
    }

}

nt_widget_vtable_t button_vtable = {
    .class_size = sizeof(nt_button_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_button_calc_size,
    .adjust_size = nt_button_adjust_size,
    .render = nt_button_render,
};

bool nt_button_enter(nt_widget_t *widget, nt_event_t *event) {
    nt_button_t *btn = NT_BUTTON(widget);

    if (btn->state != NT_BTN_STATE_DISABLED) {
        btn->state = NT_BTN_STATE_HOVERED;
        nt_widget_invalidate(widget);
    }

    return true;
} 

bool nt_button_leave(nt_widget_t *widget, nt_event_t *event) {
    nt_button_t *btn = NT_BUTTON(widget);

    if (btn->state != NT_BTN_STATE_DISABLED) {
        btn->state = NT_BTN_STATE_DEFAULT;
        nt_widget_invalidate(widget);
    }

    return true;
}

bool nt_button_down(nt_widget_t *widget, nt_event_t *event) {
    nt_button_t *btn = NT_BUTTON(widget);
    if (btn->state != NT_BTN_STATE_DISABLED) {
        btn->state = NT_BTN_STATE_HELD;
        nt_widget_invalidate(widget);
    }

    return true;
}

bool nt_button_up(nt_widget_t *widget, nt_event_t *event) {
    nt_button_t *btn = NT_BUTTON(widget);
    if (btn->state == NT_BTN_STATE_DISABLED) return true;
    btn->state = NT_BTN_STATE_HOVERED;
    nt_widget_invalidate(widget);
    nt_signal_emit_name(widget, "pressed");
    return true;
}


nt_widget_t *nt_button_create(nt_widget_t *inner) {
    nt_widget_t *widget = nt_widget_create(NT_WIDGET_BUTTON, &button_vtable);
    nt_widget_set_focusable(widget, true);

    nt_button_t *btn = NT_BUTTON(widget);
    btn->inner = inner;
    btn->state = NT_BTN_STATE_DEFAULT;
    nt_widget_set_parent(inner, widget);

    // Modify some subparameters
    nt_widget_set_expansion(inner, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_widget_set_horizontal_alignment(inner, NT_ALIGN_CENTER);
    nt_widget_set_vertical_alignment(inner, NT_ALIGN_CENTER);

    nt_event_set_handler(widget, NT_EVENT_MOUSE_ENTER, nt_button_enter);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_LEAVE, nt_button_leave);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_DOWN, nt_button_down);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_UP, nt_button_up);

    return widget;
}

nt_widget_t *nt_button_create_label(char *text) {
    nt_widget_t *lbl = nt_label_create(text);
    nt_style_set_bg_color(&lbl->style, NT_COLOR(0,0,0,0));
    return nt_button_create(lbl);
}
