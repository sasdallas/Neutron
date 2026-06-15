/**
 * @file neutron/widgets/icon_view.c
 * @brief Icon view
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

#define NT_ICON_VIEW(w) ((nt_icon_view_t*)w)

static void nt_icon_view_calc_size(nt_widget_t *w) {
    // Give it the biggest width of one entry, the combined height of every entry
    int min_width = 0;
    int min_height = 0;
    int pref_width = 0;
    int pref_height = 0;
    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > min_width) min_width = child->size_data.min_width;
        if (child->size_data.pref_width > pref_width) pref_width = child->size_data.pref_width;
        
        min_height += child->size_data.min_height;
        pref_height += child->size_data.pref_height;
    }

    w->size_data.min_width = min_width;
    w->size_data.min_height = min_height;
    w->size_data.pref_width = pref_width;
    w->size_data.pref_height = pref_height;
}

static void nt_icon_view_adjust_size(nt_widget_t *w) {
    nt_icon_view_t *view = NT_ICON_VIEW(w);
    int width = nt_widget_get_width_inner(w);
    int height = nt_widget_get_height_inner(w);
    int x = nt_widget_get_x_inner(w);
    int y = nt_widget_get_y_inner(w);

    // Determine the number of columns we can place
    int min_width = 0;
    int min_height = 0;
    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > min_width) min_width = child->size_data.min_width;
        if (child->size_data.min_height > min_height) min_height = child->size_data.min_height;
    }

    int children_amnt = view->store->nentries;

    if (min_width == 0) {
        // avoid division by zero
        return;
    }

    int grid_w = width / min_width;

    int gx = 0;
    int gy = 0;
    NT_ITERATE_CHILDREN(w) {
        nt_rect_t r = {
            .x = x + (gx * min_width),
            .y = y + (gy * min_height),
            .w = min_width,
            .h = min_height,
        };

        nt_widget_adjust_size(child, &r);

        gx++;
        if (gx >= grid_w) {
            gx = 0;
            gy++;
        }
    }
}

static void nt_icon_view_measure(nt_widget_t *w, nt_rect_t *r) {
    nt_icon_view_t *view = NT_ICON_VIEW(w);
    int width = r->w;
    int height = r->h;

    // Determine the number of columns we can place
    int min_width = 0;
    int min_height = 0;
    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > min_width) min_width = child->size_data.min_width;
        if (child->size_data.min_height > min_height) min_height = child->size_data.min_height;
    }

    if (min_width == 0) {
        r->w = 1;
        r->h = 1;
        return;
    }

    int children_amnt = view->store->nentries;
    int grid_w = width / min_width;
    int grid_h = ((children_amnt + grid_w - 1) / grid_w);

    r->w = grid_w * min_width;
    r->h = grid_h * min_height;
}

static void nt_icon_store_free(nt_icon_store_t*);

static void nt_icon_view_free(nt_widget_t *w) {
    nt_icon_view_t *icon_view = NT_ICON_VIEW(w);
    nt_icon_store_free(icon_view->store);
}

nt_widget_vtable_t icon_view_vtable = {
    .class_size = sizeof(nt_icon_view_t),
    .calc_size = nt_icon_view_calc_size,
    .adjust_size = nt_icon_view_adjust_size,
    .init = NULL,
    .free = nt_icon_view_free,
    .render = NULL,
    .measure = nt_icon_view_measure,
};

// !!! HACKY NEED TO REDO SELECTION SYSTEM TO HAPPEN IN GENERIC EVENT LAYER
bool nt_icon_view_box_event(nt_widget_t *widget, nt_event_t *event) {
    // if (!event->w || event->w->type != NT_WIDGET_BOX) return true;
    if (widget->type != NT_WIDGET_BOX) return true; // pass up in chain
    if (event->type == NT_EVENT_FOCUS_ENTER) {
        nt_widget_set_selected(event->w, true);
        nt_signal_emit_name_data(event->w->parent, "icon-selected", (void*)event->w->priv);
    } else if (event->type == NT_EVENT_FOCUS_EXIT) {
        nt_widget_set_selected(event->w, false);
        nt_signal_emit_name_data(event->w->parent, "icon-deselected", (void*)event->w->priv);
    } else if (event->type == NT_EVENT_SELECTED) {
        NT_ITERATE_CHILDREN(event->w) {
            nt_widget_set_selected(child, true);
        }
    } else if (event->type == NT_EVENT_UNSELECTED) {
        NT_ITERATE_CHILDREN(event->w) {
            nt_widget_set_selected(child, false);
        }
    } else if (event->type == NT_EVENT_MOUSE_DOUBLE_CLICK) {
        // if children are double-clicked we want to capture that event!
        nt_signal_emit_name_data(widget->parent, "icon-double-clicked", (void*)widget->priv);
    }

    return false;
}

nt_widget_t *nt_icon_view_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_ICON_VIEW, &icon_view_vtable);
    nt_icon_view_t *view = NT_ICON_VIEW(w);
    view->store = NULL;
    return w;
}

void nt_icon_view_set_store(nt_widget_t *icon_view, nt_icon_store_t *store) {
    nt_icon_view_t *icon = NT_ICON_VIEW(icon_view);
    
    if (icon->store) {
        nt_icon_store_free(icon->store);
    }

    icon->store = store;

    nt_icon_entry_t *ent = icon->store->head;
    while (ent) {
        nt_widget_t *sub = nt_box_create_vertical();
        nt_widget_set_expansion(sub, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
        nt_widget_set_selectable(sub, true);
        nt_widget_set_focusable(sub, true);

        // !!! Hack
        sub->priv = (void*)ent;
        
        // !!! Major hacks need to fixup selection
        nt_event_set_handler(sub, NT_EVENT_FOCUS_ENTER, nt_icon_view_box_event);
        nt_event_set_handler(sub, NT_EVENT_FOCUS_EXIT, nt_icon_view_box_event);
        nt_event_set_handler(sub, NT_EVENT_SELECTED, nt_icon_view_box_event);
        nt_event_set_handler(sub, NT_EVENT_UNSELECTED, nt_icon_view_box_event);
        nt_event_set_handler(sub, NT_EVENT_MOUSE_DOUBLE_CLICK, nt_icon_view_box_event);

        nt_widget_t *img = nt_image_create_from_image(ent->icon);
        nt_style_set_margin_all(&img->style, 4);
        nt_style_set_bg_color(&img->style, NT_COLOR(0,0,0,0));
        nt_widget_set_expansion(img, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
        nt_widget_set_horizontal_alignment(img, NT_ALIGN_CENTER);

        nt_widget_t *lbl = nt_label_create(ent->name);
        nt_style_set_margin_all(&lbl->style, 4);
        nt_style_set_font(&lbl->style, NT_SANS_12);
        nt_style_set_bg_color(&lbl->style, NT_COLOR(0,0,0,0));
        nt_widget_set_expansion(lbl, NT_EXPAND_HORIZONTAL);
        nt_widget_set_horizontal_alignment(lbl, NT_ALIGN_CENTER);

        nt_box_prepend(sub, lbl);
        nt_box_prepend(sub, img);

        nt_widget_set_parent_append(sub, icon_view);

        ent = ent->next;
    }
}

nt_icon_store_t *nt_icon_store_create() {
    nt_icon_store_t *store = malloc(sizeof(nt_icon_store_t));
    store->head = NULL;
    store->tail = NULL;
    store->nentries = 0;
    return store;
}

void nt_icon_store_append(nt_icon_store_t *store, char *name, nt_image_t *icon, void *priv) {
    // Append on tail
    nt_icon_entry_t *ent = malloc(sizeof(nt_icon_entry_t));
    ent->name = name;
    ent->icon = icon;
    ent->priv = priv;
    ent->next = NULL;

    if (store->tail == NULL) {
        store->head = ent;
        store->tail = ent;
    } else {
        store->tail->next = ent;
        store->tail = ent;
    }

    store->nentries++;
}


static void nt_icon_store_free(nt_icon_store_t *store) {
    nt_icon_entry_t *head = store->head;

    while (head) {
        nt_icon_entry_t *nxt = head->next;
        free(head);
        head = nxt;
    }

    free(store);
} 