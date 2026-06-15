/**
 * @file include/neutron/widgets/box.h
 * @brief Neutron box
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_BOX_H
#define _NEUTRON_WIDGETS_BOX_H

typedef struct _nt_box {
    struct _nt_widget widget;
    bool vertical;
    bool homogenous;
    int spacing;
} nt_box_t;

struct _nt_widget *nt_box_create_vertical();
struct _nt_widget *nt_box_create_horizontal();
void nt_box_append(struct _nt_widget *box, struct _nt_widget *child);
void nt_box_prepend(struct _nt_widget *box, struct _nt_widget *child);
void nt_box_set_spacing(struct _nt_widget *box, int spacing);
void nt_box_set_homogenous(struct _nt_widget *box, bool homogenous);

#endif