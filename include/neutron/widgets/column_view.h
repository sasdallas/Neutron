/**
 * @file include/neutron/widgets/column_view.h
 * @brief Neutron column view
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_COLUMN_VIEW_H
#define _NEUTRON_WIDGETS_COLUMN_VIEW_H

#include <stdbool.h>

struct _nt_widget;

typedef struct _nt_widget *(*nt_column_factory_t)(void *data, void *ctx);

// public structure (important to note!)
typedef struct _nt_column {
    char *column_name;
    void *ctx;
    nt_column_factory_t factory;

    // hacky shit happening here
    int min_width;
    bool can_expand;
    int allocated_width; // allocated width of the column
} nt_column_t;

// public structure (important to note!)
// nt_row_data_t is a linked list of pointers to objects, each of which will be passed to column factories to build widgets
// If the number of objects is known in advance it can be declared statically using NT_CREATE_ROW_DATA()
// This system is overcomplicated as fuck but so is GTK so I don't care
typedef struct _nt_row_data {
    struct _nt_row_data *next;
    void *data;
} nt_row_data_t;

// private structure
typedef struct _nt_column_view {
    struct _nt_widget widget;
    nt_column_t **columns;
    int ncolumns;
    bool draw_separator;
    int start_h;
} nt_column_view_t;

// all entries are stored as child widgets ordered by columns
// private structure
typedef struct _nt_column_view_row {
    struct _nt_widget widget;
} nt_column_view_row_t;

struct _nt_widget *nt_column_view_create();
void nt_column_view_append_column(struct _nt_widget *column_view, nt_column_t *column);
struct _nt_widget* nt_column_view_add_row(struct _nt_widget *column_view, nt_row_data_t *row);
nt_column_t *nt_column_create(char *column_name, nt_column_factory_t factory, void *ctx);

/* row data manipulation macros */
#define NT_CREATE_ROW_DATA(rname, nobjects) nt_row_data_t rname[nobjects];\
                                            int __##rname##_nt_row_index = 0; // this is stupid but looks good
#define NT_FINISH_ROW_DATA(rname)           

#define NT_ADD_ROW_DATA(rname, obj) {\
                                        rname[__##rname##_nt_row_index].next = ((__##rname##_nt_row_index+1) >= sizeof(rname) / sizeof(rname[0])) ? NULL : &rname[__##rname##_nt_row_index+1];\
                                        rname[__##rname##_nt_row_index++].data = (void*)(uintptr_t)obj;\
                                    }

/* default factories */
struct _nt_widget *__nt_column_factory_text(void *data, void *ctx);
struct _nt_widget *__nt_column_factory_number(void *data, void *ctx);
#define NT_DEFAULT_FACTORY_TEXT __nt_column_factory_text
#define NT_DEFAULT_FACTORY_NUMBER __nt_column_factory_number

#endif
