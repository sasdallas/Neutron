/**
 * @file include/neutron/widgets/separator.h
 * @brief Separator widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_SEPARATOR_H
#define _NEUTRON_WIDGETS_SEPARATOR_H

#include <stdbool.h>
struct _nt_widget;

#define NT_SEPARATOR_HORIZONTAL 0
#define NT_SEPARATOR_VERTICAL   1

typedef struct _nt_separator {
    struct _nt_widget widget;
    bool is_vert;
} nt_separator_t;

struct _nt_widget *nt_separator_create(int orientation);

#endif
