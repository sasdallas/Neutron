/**
 * @file neutron/widgets/box.c
 * @brief Neutron box widget
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

#define NT_BOX(w) ((nt_box_t*)w)

void nt_box_calc_size(nt_widget_t *widget) {
    nt_box_t *box = NT_BOX(widget);

    int min_width = 0;
    int min_height = 0;
    int pref_width = 0;
    int pref_height = 0;
    int children = 0;
    
    NT_ITERATE_CHILDREN(widget) {
        if (box->vertical) {
            if (child->size_data.min_width > min_width) min_width = child->size_data.min_width;
            if (child->size_data.pref_width > pref_width) pref_width = child->size_data.pref_width;

            min_height = min_height + child->size_data.min_height;
            pref_height = pref_height + child->size_data.pref_height;
        } else {
            if (child->size_data.min_height > min_height) min_height = child->size_data.min_height;
            if (child->size_data.pref_height > pref_height) pref_height = child->size_data.pref_height;

            min_width = min_width + child->size_data.min_width;
            pref_width = pref_width + child->size_data.pref_width;
        }

        children++;
    }

    if (children && box->spacing) {
        if (box->vertical) {
            min_height += box->spacing * (children-1);
            pref_height += box->spacing * (children-1);
        } else {
            min_width += box->spacing * (children-1);
            pref_width += box->spacing * (children-1);
        }
    }

    widget->size_data.min_width = min_width;
    widget->size_data.min_height = min_height;
    widget->size_data.pref_width = pref_width;
    widget->size_data.pref_height = pref_height;
}

void nt_box_adjust_size(nt_widget_t *widget) {
    nt_box_t *box = NT_BOX(widget);
    int cur_x = nt_widget_get_x_inner(widget);
    int cur_y = nt_widget_get_y_inner(widget);
    int w = nt_widget_get_width_inner(widget);
    int h = nt_widget_get_height_inner(widget);


    int children = 0;
    int children_expandable = 0;
    if (box->homogenous) {
        // Count the children to evenly distribute w
        NT_ITERATE_CHILDREN(widget) { children++; }
    }

    if (box->homogenous && children == 0) {
        return;
    }

    // This is shit code, but distribute excess width and height to expandable children.
    long expand_amnt = 0;

    if (!box->homogenous) {
        int excess_w = nt_widget_get_excess_width(widget);
        int excess_h = nt_widget_get_excess_height(widget);
        if ((box->vertical && excess_h) || (!box->vertical && excess_w)) {
            int expandable_children = 0;
            NT_ITERATE_CHILDREN(widget) {
                if (box->vertical) {
                    if (child->layout.expansion & NT_EXPAND_VERTICAL) {
                        expandable_children++;
                    }
                } else {
                    if (child->layout.expansion & NT_EXPAND_HORIZONTAL) {
                        expandable_children++;
                    }
                }
            }

            if (expandable_children > 0) {
                expand_amnt = (box->vertical) ? excess_h / expandable_children : excess_w / expandable_children;
            } else {
                expand_amnt = 0;
            }
        }
    }

    NT_ITERATE_CHILDREN(widget) {
        if (box->vertical) {
            nt_rect_t rect = {
                .x = cur_x,
                .y = cur_y,
                .w = w,
                .h = (box->homogenous) ? (h / children) : child->size_data.pref_height 
            };

            if (child->layout.expansion & NT_EXPAND_VERTICAL) {
                rect.h += expand_amnt;
            }

            // NT_DEBUG("nt box adjusting size to %dx%dx%dx%d\n", rect.x, rect.y, rect.w, rect.h);
            nt_widget_adjust_size(child, &rect);

            cur_y += rect.h + box->spacing;
        } else {
            nt_rect_t rect = {
                .x = cur_x,
                .y = cur_y,
                .w = (box->homogenous) ? (w / children) : child->size_data.pref_width,
                .h = h,
            };

            if (child->layout.expansion & NT_EXPAND_HORIZONTAL) rect.w += expand_amnt;

            nt_widget_adjust_size(child, &rect);

            cur_x += rect.w + box->spacing;
        }
    }
} 

static void nt_box_render(nt_widget_t *w, nt_render_surface_t *surf) {
    if (w->selected) {
        nt_render_rounded_rect_gradient(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w), nt_widget_get_height_inner(w)-1), 4, NT_COLOR(0x5d,0xa3,0xec,255), NT_COLOR(0x39, 0x87, 0xe4, 255), false);
        nt_render_border_rounded_rect(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w),nt_widget_get_height_inner(w)-1), 1, 4, NT_COLOR(0x46,0x90,0xd0,255)); 
    }
}

nt_widget_vtable_t box_vtable = {
    .class_size = sizeof(nt_box_t),
    .init = NULL,
    .free = NULL,
    .render = nt_box_render,
    .calc_size = nt_box_calc_size,
    .adjust_size = nt_box_adjust_size
};


nt_widget_t *nt_box_create(bool vertical) {
    nt_widget_t *box_widget = nt_widget_create(NT_WIDGET_BOX, &box_vtable);
    nt_box_t *box = NT_BOX(box_widget);
    box->vertical = vertical;
    box->spacing = 0;
    box->homogenous = false;
    return box_widget;
}

nt_widget_t *nt_box_create_vertical() {
    return nt_box_create(true);
}

nt_widget_t *nt_box_create_horizontal() {
    return nt_box_create(false);
}

void nt_box_append(nt_widget_t *box, nt_widget_t *child) {
    // Manually undo set_parent
    nt_widget_set_parent(child, box);
    if (!child->next) return;
    box->children = child->next;
    box->children->prev = NULL;

    nt_widget_t *to_insert = box->children;
    while (to_insert->next) to_insert = to_insert->next;
    to_insert->next = child;
    child->prev = to_insert;
    child->next = NULL;
}

void nt_box_prepend(nt_widget_t *box, nt_widget_t *child) {
    nt_widget_set_parent(child, box);
}

void nt_box_set_spacing(nt_widget_t *box, int spacing) {
    nt_box_t *b = NT_BOX(box);
    b->spacing = spacing;
}

void nt_box_set_homogenous(nt_widget_t *box, bool homogenous) {
    nt_box_t *b = NT_BOX(box);
    b->homogenous = homogenous;
}