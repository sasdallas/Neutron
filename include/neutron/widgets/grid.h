/**
 * @file include/neutron/widgets/grid.h
 * @brief Grid layout system
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_GRID_H
#define _NEUTRON_WIDGETS_GRID_H

#include <stddef.h>

typedef struct _nt_grid {
    struct _nt_widget widget;
    struct _nt_widget **cells;
    size_t grid_w;
    size_t grid_h;

    int col_spacing;
    int row_spacing;
} nt_grid_t;

struct _nt_widget *nt_grid_create(size_t grid_w, size_t grid_h);
void nt_grid_set(struct _nt_widget *grid, int x, int y, struct _nt_widget *widget);

void nt_grid_set_column_spacing(struct _nt_widget *grid, int spacing);
void nt_grid_set_row_spacing(struct _nt_widget *grid, int spacing);

#endif