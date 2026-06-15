/**
 * @file neutron/widgets/column_view.c
 * @brief Neutron's overcomplicated column view widget
 * 
 * What have I created? I have sinned against God.
 * 
 * Tree structure that causes problems:
 *  NT_COLUMN_VIEW
 *      NT_COLUMN_VIEW_ROW
 *          NT_WIDGET
 *          NT_WIDGET
 *          NT_WIDGET 
 *      NT_COLUMN_VIEW_ROW
 *          ...
 *      NT_COLUMN_VIEW_ROW
 *          ...
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

#define NT_COLUMN_VIEW(w) ((nt_column_view_t*)w)
#define NT_COLUMN_VIEW_ROW(w) ((nt_column_view_row_t*)w)
#define TEXT_SPACING            4

static void nt_column_view_row_calc_size(nt_widget_t *w) {
    nt_column_view_t *parent = NT_COLUMN_VIEW(w->parent);
    int min_width = 0;
    int min_height = 0;
    int pref_width = 0;
    int pref_height = 0;

    int i = 0;
    NT_ITERATE_CHILDREN(w) {
        assert(i < parent->ncolumns);
        nt_column_t *col = parent->columns[i++];
        if (child->size_data.min_width > col->min_width) col->min_width = child->size_data.min_width;
        if (child->size_data.min_height > min_height) min_height = child->size_data.min_height;
        if (child->size_data.pref_height > pref_height) pref_height = child->size_data.pref_height;
        min_width += child->size_data.min_width;
        pref_width += child->size_data.pref_width;

        if (child->layout.expansion & NT_EXPAND_HORIZONTAL) {
            if (child->layout.halign == NT_ALIGN_STRETCH) {
                col->can_expand = true;
            }
        }
    }

    w->size_data.min_width = min_width;
    w->size_data.min_height = min_height;
    w->size_data.pref_width = pref_width;
    w->size_data.pref_height = pref_height;
}

static void nt_column_view_row_adjust_size(nt_widget_t *w) {
    // Similar to box, we should do two slow ass passes to find all expandable children widgets
    // and then allocate space for each.

    nt_column_view_t *parent = NT_COLUMN_VIEW(w->parent);
    int i = 0;
    int x = nt_widget_get_x_inner(w);
    int y = nt_widget_get_y_inner(w);
    int h = nt_widget_get_height_inner(w);

    NT_ITERATE_CHILDREN(w) {
        nt_column_t *col = parent->columns[i++];
        
        nt_rect_t r = {
            .x = x,
            .y = y,
            .w = col->allocated_width,
            .h = h
        };

        nt_widget_adjust_size(child, &r);
        x += r.w;
    }
}

static void nt_column_view_row_render(nt_widget_t *w, nt_render_surface_t *surf) {
    if (w->selected) {
        nt_render_rounded_rect_gradient(surf, &NT_RECT(0,0,nt_widget_get_width_inner(w), nt_widget_get_height_inner(w)-1), 0, NT_COLOR(0x5d,0xa3,0xec,255), NT_COLOR(0x39, 0x87, 0xe4, 255), false);
        nt_render_fill_rect(surf, &NT_RECT(0, nt_widget_get_height_inner(w)-1, nt_widget_get_width_inner(w), 1), NT_COLOR(0x46, 0x90, 0xD0, 255));
    }

    nt_column_view_t *view = NT_COLUMN_VIEW(w->parent);

    // !!! HACK
    if (view->draw_separator) {
        int wx = nt_widget_get_x_inner(w);
        NT_ITERATE_CHILDREN(w) {
            if (child != w->children) {
                int cx = child->layout_data.x - child->style.margin[LEFT];
                nt_render_fill_rect(surf, &NT_RECT(cx-wx, 0, 1, nt_widget_get_height_inner(w)), NT_COLOR(170,170,170,255));
            }
        }
    }
}

static bool nt_column_view_row_event(nt_widget_t *w, nt_event_t *ev) {
    if (ev->type == NT_EVENT_MOUSE_DOUBLE_CLICK) {
        nt_signal_emit_name(w, "double-clicked");
        return true;
    }

    if (ev->w != w) return true;
    if (ev->type == NT_EVENT_FOCUS_ENTER) {
        nt_widget_set_selected(w, true);
        nt_signal_emit_name(w, "selected");
        nt_widget_invalidate(w);
    } else if (ev->type == NT_EVENT_FOCUS_EXIT) {
        nt_widget_set_selected(w, false);
        nt_signal_emit_name(w, "deselected");
        nt_widget_invalidate(w);
    }

    return true;
}

nt_widget_vtable_t row_vtable = {
    .class_size = sizeof(nt_column_view_row_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_column_view_row_calc_size,
    .adjust_size = nt_column_view_row_adjust_size,
    .render = nt_column_view_row_render
};


static nt_widget_t *nt_column_view_row_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_COLUMN_VIEW_ROW, &row_vtable);
    nt_widget_set_selectable(w, true);
    nt_style_set_margin_all(&w->style, 0);
    nt_widget_set_expansion(w, NT_EXPAND_HORIZONTAL);
    nt_event_set_handler(w, NT_EVENT_FOCUS_ENTER, nt_column_view_row_event);
    nt_event_set_handler(w, NT_EVENT_FOCUS_EXIT, nt_column_view_row_event);
    nt_event_set_handler(w, NT_EVENT_MOUSE_DOUBLE_CLICK, nt_column_view_row_event);
    nt_widget_set_focusable(w, true);
    return w;
}

static void nt_column_view_calc_size(nt_widget_t *w) {
    nt_column_view_t *view = NT_COLUMN_VIEW(w);
    int min_width = 0;
    int min_height = 0;
    int pref_width = 0;
    int pref_height = 0;

    // TODO: slow as shit and TRASH move this to another area
    for (int i = 0; i < view->ncolumns; i++) {
        size_t width, height;
        nt_render_text_dimensions(w->style.font, view->columns[i]->column_name, &width, &height);
        min_width += width + 4;
        pref_width += width + 4;
        height += TEXT_SPACING;
        if (height > min_height) { min_height = height; pref_height = height; }
    }

    // Now actually add up each part of the children
    NT_ITERATE_CHILDREN(w) {
        if (child->size_data.min_width > min_width) {
            min_width = child->size_data.min_width;
        }

        if (child->size_data.pref_width > pref_width) {
            pref_width = child->size_data.pref_width;
        }

        min_height += child->size_data.min_height;
        pref_height += child->size_data.pref_height;
    }

    w->size_data.min_width = min_width;
    w->size_data.min_height = min_height;
    w->size_data.pref_width = pref_width;
    w->size_data.pref_height = pref_height;

}

static void nt_column_view_adjust_size(nt_widget_t *w) {
    nt_column_view_t *view = NT_COLUMN_VIEW(w);
    int y = 0;

    // TODO: USELESS RECALCULATION
    // TODO: Make some sort of column_view_header widget???
    for (int i = 0; i < view->ncolumns; i++) {
        size_t width, height;
        nt_render_text_dimensions(w->style.font, view->columns[i]->column_name, &width, &height);
        if (width+4 > view->columns[i]->min_width) view->columns[i]->min_width = width+4;
        height += TEXT_SPACING;
        if (height > y) { y = height; }
    }

    view->start_h = y;

    y += nt_widget_get_y_inner(w) + 2;
    int x = nt_widget_get_x_inner(w);
    int width = nt_widget_get_width_inner(w);

    // Some serious hacks happening here
    int expandable_columns = 0;
    int min_width_needed = 0;
    for (int i = 0; i < view->ncolumns; i++) {
        if (view->columns[i]->can_expand) expandable_columns += 1;
        min_width_needed += view->columns[i]->min_width;
    }

    assert(width >= min_width_needed && "Shrinking not implemented just yet");
    int expand_amnt = (expandable_columns) ? (width - min_width_needed) / expandable_columns : 0;
    
    // Now allocate width for each column, using even width
    for (int i = 0; i < view->ncolumns; i++) {
        nt_column_t *col = view->columns[i];
        col->allocated_width = col->min_width;
        if (col->can_expand) col->allocated_width += expand_amnt;
    }

    NT_ITERATE_CHILDREN(w) {
        nt_rect_t r = {
            .x = x,
            .y = y,
            .w = width,
            .h = child->size_data.pref_height
        };

        nt_widget_adjust_size(child, &r);

        y += r.h;
    }
}

static void nt_column_view_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_column_view_t *view = NT_COLUMN_VIEW(w);
    int y = w->style.padding[TOP] + (TEXT_SPACING/2);
    int x = w->style.padding[LEFT];
    nt_render_fill_rect_gradient(surf, &NT_RECT(x,w->style.padding[TOP],nt_widget_get_width_inner(w),view->start_h-w->style.padding[TOP]+3), 
    NT_COLOR(0xe6,0xe6,0xe6,0xff), NT_COLOR(0xd0,0xd0,0xd0,0xff), false);
    for (int i = 0; i < view->ncolumns; i++) {
        nt_column_t *col = view->columns[i];
        nt_render_draw_text(surf, w->style.font, x+2, y, col->column_name, NT_COLOR(0,0,0,255));

        x += col->allocated_width;

        // !!! extremely big hack
        if (view->draw_separator && i+1 != view->ncolumns) {
            nt_render_fill_rect(surf, &NT_RECT(x, 0, 1, view->start_h + y), NT_COLOR(170,170,170,255));
        }
    }

    nt_render_fill_rect(surf, &NT_RECT(w->style.padding[LEFT], w->style.padding[TOP] + view->start_h+1, nt_widget_get_width_inner(w), 1), NT_COLOR(170,170,170,255));
}

nt_widget_vtable_t column_view_vtable = {
    .class_size = sizeof(nt_column_view_t),
    .init = NULL,
    .free = NULL,
    .calc_size = nt_column_view_calc_size,
    .adjust_size = nt_column_view_adjust_size,
    .render = nt_column_view_render
};

nt_widget_t *nt_column_view_create() {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_COLUMN_VIEW, &column_view_vtable);
    nt_column_view_t *col = NT_COLUMN_VIEW(w);
    col->columns = NULL;
    col->ncolumns = 0;
    col->draw_separator = true;
    return w;
}

void nt_column_view_append_column(nt_widget_t *column_view, nt_column_t *column) {
    nt_column_view_t *view = NT_COLUMN_VIEW(column_view);

    view->columns = realloc(view->columns, sizeof(nt_column_t*) * ++view->ncolumns);
    view->columns[view->ncolumns-1] = column;

    nt_widget_invalidate(column_view);
}

nt_widget_t *nt_column_view_add_row(nt_widget_t *column_view, nt_row_data_t *row) {
    // This is very annoying and ugly to implement
    nt_widget_t *row_wdgt = nt_column_view_row_create();
    nt_column_view_t *view = NT_COLUMN_VIEW(column_view);

    // Create children
    nt_row_data_t *r = row;
    for (int i = 0; i < view->ncolumns; i++) {
        assert(r && "Too little row data for number of columns");
        nt_column_t *col = view->columns[i];
        nt_widget_t *child = col->factory(r->data, col->ctx);
        
        // !!! HACK to align with column headers and make transparent!
        nt_style_set_margin(&child->style, LEFT, 2);
        nt_style_set_bg_color(&child->style, NT_COLOR(0,0,0,0));

        assert(child);
        nt_widget_set_parent_append(child, row_wdgt);
        r = r->next;
    }

    nt_widget_set_parent_append(row_wdgt, column_view);
    return row_wdgt;
}

nt_column_t *nt_column_create(char *column_name, nt_column_factory_t factory, void *ctx) {
    nt_column_t *col = malloc(sizeof(nt_column_t));
    col->ctx = ctx;
    col->column_name = strdup(column_name);
    col->factory = factory;
    col->min_width = 0;
    col->can_expand = false;
    return col;
}

struct _nt_widget *__nt_column_factory_text(void *data, void *ctx) {
    nt_widget_t *lbl = nt_label_create((char*)data);
    return lbl;
}
