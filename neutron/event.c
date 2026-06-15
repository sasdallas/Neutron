/**
 * @file neutron/event.c
 * @brief Neutron event system
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
#include <time.h>
#include <sys/time.h>

nt_widget_t *last_hovered_widget = NULL;

static bool nt_event_point_in_widget(nt_widget_t *widget, unsigned int x, unsigned int y) {
    return widget
        && widget->layout_data.x <= x
        && widget->layout_data.x + widget->layout_data.w > x
        && widget->layout_data.y <= y
        && widget->layout_data.y + widget->layout_data.h > y;
}

static nt_widget_t *nt_event_context_menu_for(nt_widget_t *widget) {
    while (widget) {
        if (widget->context_menu) return widget->context_menu;
        widget = widget->parent;
    }

    return NULL;
}

static void nt_event_hide_popup(nt_window_t *window) {
    if (!window || !window->popup) return;

    nt_widget_t *popup = window->popup;
    
    
    if (popup->type == NT_WIDGET_MENU) {
        nt_menu_hide(popup);
    }

    nt_window_set_popup(window, NULL);
}

static void nt_dispatch_all(nt_widget_t *w, nt_event_t *e) {
    if (!w) return;
    NT_ITERATE_CHILDREN(w) {
        nt_dispatch_all(child, e);
    }

    if (w->event_handlers[e->type]) {
        w->event_handlers[e->type](w, e);
    }
}

static uint64_t nt_event_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void nt_event_process(nt_window_t *window, nt_event_t *event) {
    if (!window || !event) return;
    
    if (nt_event_is_pointer(event)) {
        if (event->type == NT_EVENT_MOUSE_DOWN && window->popup && !nt_event_point_in_widget(window->popup, event->pointer.x, event->pointer.y)) {
            nt_event_hide_popup(window);
        }

        // Use X and Y to determine the widget
        nt_widget_t *target = nt_widget_at(window, event->pointer.x, event->pointer.y);

        // Synthetically send exit/enter events
        // !!!: If nt_widget_free was done on last_hovered_widget the hack in widget.c takes care of it.
        // !!!: Should probably add some refcounting or something..
        if (target != last_hovered_widget) {
            // Send a mouse leave event
            if (last_hovered_widget) {
                nt_event_t mouse_leave = {
                    .type = NT_EVENT_MOUSE_LEAVE,
                    .pointer = event->pointer
                };

                last_hovered_widget->hovered = false;
                nt_widget_dispatch(last_hovered_widget, &mouse_leave);

                if (last_hovered_widget->flags & NT_WIDGET_MOUSE_INVAL) nt_widget_invalidate(last_hovered_widget);
            }

            if (target) {
                nt_event_t mouse_enter = {
                    .type = NT_EVENT_MOUSE_ENTER,
                    .pointer = event->pointer
                };

                target->hovered = true;
                nt_widget_dispatch(target, &mouse_enter);

                if (target->flags & NT_WIDGET_MOUSE_INVAL) nt_widget_invalidate(target); 
            }

            last_hovered_widget = target;
        }

        if (!target) return;
        if (event->type == NT_EVENT_MOUSE_DOWN) {
            // TODO This pattern is weird but it lets us do some UI hacks easier
            nt_window_set_focus(window, target);

            if (event->pointer.buttons & NT_MOUSE_BUTTON_RIGHT) {
                nt_widget_t *menu = nt_event_context_menu_for(target);
                if (menu) {
                    nt_menu_show_at(menu, window, event->pointer.x, event->pointer.y);
                    return;
                }
            }

            if (event->pointer.buttons & NT_MOUSE_BUTTON_LEFT) {
                // get the time of the event
                uint64_t cur_time = nt_event_time();
                if (cur_time-target->last_click_time <= 400) {
                    nt_event_t double_click = {
                        .type = NT_EVENT_MOUSE_DOUBLE_CLICK,
                        .pointer = event->pointer,
                    };

                    nt_widget_dispatch(target, &double_click);
                }

                target->last_click_time = cur_time;
            }
        }

        nt_widget_dispatch(target, event);
    } else if (nt_event_is_keyboard(event)) {
        nt_widget_t *target = nt_window_get_focus(window);
        if (!target) return;

        nt_widget_dispatch(target, event);
    } else if (nt_event_is_selected(event)) {
        // Synthetic, widget already selected
        nt_widget_dispatch(event->w, event);
    } else if (nt_event_is_window(event)) {
        // SPECIAL: Dispatch to every widget
        nt_dispatch_all(window->root_frame, event);
    } else {
        NT_ERROR("Unimplemented event type %d\n", event->type);
    }
}

void nt_event_set_handler(nt_widget_t *widget, nt_event_type_t type, nt_event_handler_t event_handler) {
    if (type < 0 || type >= NT_NEVENTS) return;
    widget->event_handlers[type] = event_handler;
}
