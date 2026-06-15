/**
 * @file neutron/widgets/input.c
 * @brief Neutron input widget
 * 
 * Basic input widget
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

#define NT_INPUT(w) ((nt_input_t*)w)


void nt_input_calc_size(nt_widget_t *w) {
    nt_input_t *input = NT_INPUT(w);
    
    w->size_data.min_width = 15;
    w->size_data.min_height = 20;
    w->size_data.pref_width = 15;
    w->size_data.pref_height = 20;
}

void nt_input_adjust_size(nt_widget_t *w) {
    nt_input_t *input = NT_INPUT(w);
    (void)input;
}

void nt_input_render(nt_widget_t *w, nt_render_surface_t *surface) {
    nt_input_t *input = NT_INPUT(w);
    (void)input;

    nt_rect_t r = NT_RECT(0,0,nt_widget_get_width_inner(w), nt_widget_get_height_inner(w));
    
    if (w->flags & NT_WIDGET_FOCUS) {
        nt_render_rounded_rect_gradient(surface, &r, 4, NT_COLOR(0xe2,0xe3,0xea,0xff), NT_COLOR(0xdb,0xdf,0xe6,0xff), false);
        nt_render_rounded_rect(surface, &NT_RECT(r.x+1, r.y+1, r.w-2, r.h-2), 4, NT_COLOR(255,255,255,255));
    } else {
        nt_render_rounded_rect_gradient(surface, &r, 4, NT_COLOR(0xd2,0xd3,0xda,0xff), NT_COLOR(0xcb,0xcf,0xd6,0xff), false);
        nt_render_rounded_rect(surface, &NT_RECT(r.x+1, r.y+1, r.w-2, r.h-2), 4, NT_COLOR(0xF0,0xF0,0xF0,255));
    }

    nt_render_fill_rect(surface, &NT_RECT(r.x+2,r.y,r.w-4,1), NT_COLOR(0xab, 0xad, 0xa3,0xff));    

    size_t width, height;

    char *text = input->input->buffer;
    if (nt_input_context_get_length(input->input) == 0) {
        text = input->placeholder;
    }

    if (text) {
        nt_render_text_dimensions(w->style.font, input->input->buffer, &width, &height);
        nt_render_draw_text(surface, w->style.font, 4, (nt_widget_get_height_inner(w)-height)/2, text, (text == input->placeholder) ? NT_COLOR(0x82,0x82,0x82,0xff) : NT_COLOR(0,0,0,255));
    }

    if (w->focused && input->cursor) {
        if (width < nt_widget_get_width_inner(w)) {
            nt_render_fill_rect(surface, &NT_RECT(width+4, r.y+4, 1, r.h - 8), NT_COLOR(0,0,0,255));
        }
    }
}

nt_widget_vtable_t input_vtable = {
    .class_size = sizeof(nt_input_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_input_calc_size,
    .adjust_size = nt_input_adjust_size,
    .render = nt_input_render,
};

bool nt_input_key_down(nt_widget_t *widget, nt_event_t *event) {
    nt_input_t *input = NT_INPUT(widget);
    
    if (event->key.key == NT_KEY_ENTER || event->key.key == NT_KEY_KEYPADENTER) {
        nt_signal_emit_name(widget, "enter-pressed");
        nt_window_set_focus(nt_widget_get_window(widget), NULL);
        return true;
    }

    nt_input_context_process(input->input, event);
    nt_timer_reset(input->timer);
    nt_widget_invalidate(widget);
    return true;
}

static bool nt_cursor_flip(nt_timer_t *timer, void *data) {
    nt_input_t *input = NT_INPUT(data);
    input->cursor = !input->cursor;
    nt_widget_invalidate(&input->widget);
    return true;
}

bool nt_input_focus(nt_widget_t *widget, nt_event_t *event) {
    nt_input_t *input = NT_INPUT(widget);
    if (event->type == NT_EVENT_FOCUS_EXIT) {
        nt_timer_set_enabled(input->timer, false);
    } else {
        nt_timer_set_enabled(input->timer, true);
    }

    nt_widget_invalidate(widget);
    return true;
}


static bool nt_input_mouse_enter(nt_widget_t *widget, nt_event_t *event) {
    nt_platform_set_cursor(nt_widget_get_window(widget), NT_CURSOR_TEXT);
    return true;
}

static bool nt_input_mouse_leave(nt_widget_t *widget, nt_event_t *event) {
    nt_platform_set_cursor(nt_widget_get_window(widget), NT_CURSOR_ARROW);
    return true;
}

nt_widget_t *nt_input_create(char *placeholder) {
    nt_widget_t *widget = nt_widget_create(NT_WIDGET_INPUT, &input_vtable);
    
    nt_input_t *input = NT_INPUT(widget);

    nt_widget_set_focusable(widget, true);
    input->input = nt_input_context_create();
    if (placeholder) input->placeholder = strdup(placeholder);
    else input->placeholder = NULL;
    input->timer = nt_timer_create(600, nt_cursor_flip, (void*)input, false);
    input->cursor = true;

    nt_event_set_handler(widget, NT_EVENT_KEY_DOWN, nt_input_key_down);
    nt_event_set_handler(widget, NT_EVENT_FOCUS_ENTER, nt_input_focus);
    nt_event_set_handler(widget, NT_EVENT_FOCUS_EXIT, nt_input_focus);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_ENTER, nt_input_mouse_enter);
    nt_event_set_handler(widget, NT_EVENT_MOUSE_LEAVE, nt_input_mouse_leave);

    return widget;
}


void nt_input_set_text(struct _nt_widget *widget, char *text) {
    nt_input_t *input = NT_INPUT(widget);
    nt_input_context_set_text(input->input, text, strlen(text));
}

char *nt_input_get_text(struct _nt_widget *widget) {
    nt_input_t *input = NT_INPUT(widget);
    return input->input->buffer; // ??? unsafe?
}

void nt_input_set_max_chars(struct _nt_widget *widget, int max) {
    nt_input_t *input = NT_INPUT(widget);
    nt_input_context_set_limit(input->input, max);
}

void nt_input_set_read_only(struct _nt_widget *widget, bool read_only) {
    nt_window_t *wnd = nt_widget_get_window(widget);

    if (wnd) {
        if (nt_window_get_focus(wnd) == widget) {
            nt_window_set_focus(wnd, NULL);
        }
    }


    nt_widget_set_focusable(widget, read_only == false);
}