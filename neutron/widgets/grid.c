/**
 * @file neutron/widgets/grid.c
 * @brief Grid widget
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

#define NT_GRID(w) ((nt_grid_t*)w)
#define NT_GRID_AT(g, x, y) ((g)->cells[((y) * (g)->grid_w) + (x)])

void nt_grid_free(nt_widget_t *w) { NT_WARN("TODO: nt_grid_free\n"); }

void nt_grid_calc_size(nt_widget_t *w) {
    nt_grid_t *g = NT_GRID(w);

    // We should calculate the size of each column first
    long col_min_w = 0;
    long col_pref_w = 0;
    long row_min_h = 0;
    long row_pref_h = 0;

    if (g->grid_w == 0 || g->grid_h == 0) {
        return;
    }

    long col_min[g->grid_w];
    long col_pref[g->grid_w];
    long row_min[g->grid_h];
    long row_pref[g->grid_h];
    memset(col_min, 0, sizeof(col_min));
    memset(col_pref, 0, sizeof(col_pref));
    memset(row_min, 0, sizeof(row_min));
    memset(row_pref, 0, sizeof(row_pref));

    bool has_children = false;
    for (size_t y = 0; y < g->grid_h; ++y) {
        for (size_t x = 0; x < g->grid_w; ++x) {
            nt_widget_t *child = NT_GRID_AT(g, x, y);
            if (!child) {
                continue;
            }

            has_children = true;


            NT_DEBUG("nt_grid_calc_size %p min_width=%d min_height=%d pref_width=%d pref_height = %d\n", child,
                    child->size_data.min_width, child->size_data.min_height, child->size_data.pref_width, child->size_data.pref_height);

            if (child->size_data.min_width > col_min[x]) {
                col_min[x] = child->size_data.min_width;
            }
            if (child->size_data.pref_width > col_pref[x]) {
                col_pref[x] = child->size_data.pref_width;
            }
            if (child->size_data.min_height > row_min[y]) {
                row_min[y] = child->size_data.min_height;
            }
            if (child->size_data.pref_height > row_pref[y]) {
                row_pref[y] = child->size_data.pref_height;
            }
        }
    }

    if (!has_children) return;

    for (size_t x = 0; x < g->grid_w; ++x) {
        col_min_w += col_min[x];
        col_pref_w += col_pref[x];
    }

    for (size_t y = 0; y < g->grid_h; ++y) {
        row_min_h += row_min[y];
        row_pref_h += row_pref[y];
    }

    int row_spacing = (g->grid_h > 0) ? g->row_spacing * (g->grid_h - 1) : 0;
    int col_spacing = (g->grid_w > 0) ? g->col_spacing * (g->grid_w - 1) : 0;
    row_min_h += row_spacing;
    row_pref_h += row_spacing;
    col_min_w += col_spacing;
    col_pref_w += col_spacing;

    w->size_data.min_width = col_min_w;
    w->size_data.pref_width = col_pref_w;
    w->size_data.min_height = row_min_h;
    w->size_data.pref_height = row_pref_h;
}

void nt_grid_adjust_size(nt_widget_t *w) {
    nt_grid_t *g = NT_GRID(w);

    if (g->grid_w == 0 || g->grid_h == 0) {
        return;
    }

    long col_min[g->grid_w];
    long col_pref[g->grid_w];
    long row_min[g->grid_h];
    long row_pref[g->grid_h];
    bool col_expand[g->grid_w];
    bool row_expand[g->grid_h];
    memset(col_min, 0, sizeof(col_min));
    memset(col_pref, 0, sizeof(col_pref));
    memset(row_min, 0, sizeof(row_min));
    memset(row_pref, 0, sizeof(row_pref));
    memset(col_expand, 0, sizeof(col_expand));
    memset(row_expand, 0, sizeof(row_expand));


    for (size_t y = 0; y < g->grid_h; ++y) {
        for (size_t x = 0; x < g->grid_w; ++x) {
            nt_widget_t *child = NT_GRID_AT(g, x, y);
            if (!child) {
                continue;
            }

            if (child->size_data.min_width > col_min[x]) {
                col_min[x] = child->size_data.min_width;
            }
            if (child->size_data.pref_width > col_pref[x]) {
                col_pref[x] = child->size_data.pref_width;
            }
            if (child->size_data.min_height > row_min[y]) {
                row_min[y] = child->size_data.min_height;
            }
            if (child->size_data.pref_height > row_pref[y]) {
                row_pref[y] = child->size_data.pref_height;
            }

            if (child->layout.expansion & NT_EXPAND_HORIZONTAL) {
                col_expand[x] = true;
            }
            if (child->layout.expansion & NT_EXPAND_VERTICAL) {
                row_expand[y] = true;
            }
        }
    }

    long col_width[g->grid_w];
    long row_height[g->grid_h];
    for (size_t x = 0; x < g->grid_w; ++x) col_width[x] = col_min[x];
    for (size_t y = 0; y < g->grid_h; ++y) row_height[y] = row_min[y];

    long content_min_w = 0;
    long content_min_h = 0;
    for (size_t x = 0; x < g->grid_w; ++x) content_min_w += col_width[x];
    for (size_t y = 0; y < g->grid_h; ++y) content_min_h += row_height[y];
    content_min_w += (g->grid_w > 0) ? g->col_spacing * (g->grid_w - 1) : 0;
    content_min_h += (g->grid_h > 0) ? g->row_spacing * (g->grid_h - 1) : 0;

    int extra_w = nt_widget_get_excess_width(w);
    int extra_h = nt_widget_get_excess_height(w);

    // Check expandable rows and columns
    int expandable_cols = 0;
    int expandable_rows = 0;
    for (size_t x = 0; x < g->grid_w; ++x) {
        if (col_expand[x]) expandable_cols++;
    }
    for (size_t y = 0; y < g->grid_h; ++y) {
        if (row_expand[y]) expandable_rows++;
    }

    // Adjust widths
    if (extra_w > 0 && expandable_cols > 0) {
        long each = extra_w / expandable_cols;
        long remainder = extra_w % expandable_cols;

        for (size_t x = 0; x < g->grid_w; ++x) {
            if (!col_expand[x]) continue;
            col_width[x] += each;
            if (remainder > 0) {
                col_width[x] += 1;
                remainder--;
            }
        }
    }

    // Adjust heights
    if (extra_h > 0 && expandable_rows > 0) {
        long each = extra_h / expandable_rows;
        long remainder = extra_h % expandable_rows;

        for (size_t y = 0; y < g->grid_h; ++y) {
            if (!row_expand[y]) continue;
            row_height[y] += each;
            if (remainder > 0) {
                row_height[y] += 1;
                remainder--;
            }
        }
    }

    int cur_y = w->layout_data.y;
    for (int y = 0; y < g->grid_h; y++) {
        int cur_x = w->layout_data.x;
        for (int x = 0; x < g->grid_w; x++) {
            nt_widget_t *child = NT_GRID_AT(g, x, y);
            if (child) {
                nt_rect_t region = NT_RECT(
                    cur_x,
                    cur_y,
                    col_width[x],
                    row_height[y]
                );

                nt_widget_adjust_size(child, &region);
            }

            cur_x += col_width[x];
            if (x + 1 < g->grid_w) {
                cur_x += g->col_spacing;
            }
        }

        cur_y += row_height[y];
        if (y + 1 < g->grid_h) {
            cur_y += g->row_spacing;
        }
    }   
}

void nt_grid_render(nt_widget_t *w, nt_render_surface_t *surface) {
}

nt_widget_vtable_t nt_grid_vtable = {
    .class_size = sizeof(nt_grid_t),
    .init = NULL,
    .free = nt_grid_free,
    .render = nt_grid_render,
    .calc_size = nt_grid_calc_size,
    .adjust_size = nt_grid_adjust_size,
};

nt_widget_t *nt_grid_create(size_t grid_w, size_t grid_h) {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_GRID, &nt_grid_vtable);
    nt_grid_t *g = NT_GRID(w);
    g->grid_w = grid_w;
    g->grid_h = grid_h;
    g->cells = malloc(sizeof(nt_widget_t*) * grid_w * grid_h);
    g->col_spacing = 0;
    g->row_spacing = 0;
    memset(g->cells, 0, sizeof(nt_widget_t*) * grid_w * grid_h);
    return w;
}

void nt_grid_set(nt_widget_t *w, int x, int y, nt_widget_t *widget) {
    NT_DEBUG("nt_grid_set %p %d %d\n", widget, x, y);
    nt_grid_t *g = NT_GRID(w);
    g->cells[y * g->grid_w + x] = widget;
    nt_widget_set_parent(widget, w);
}

inline void nt_grid_set_column_spacing(struct _nt_widget *grid, int spacing) {
    nt_grid_t *g = NT_GRID(grid);
    g->col_spacing = spacing;
}

inline void nt_grid_set_row_spacing(struct _nt_widget *grid, int spacing) {
    nt_grid_t *g = NT_GRID(grid);
    g->row_spacing = spacing;
}