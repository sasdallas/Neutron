/**
 * @file include/neutron/widgets/tab.h
 * @brief Single tab of content, serves as a sort of box
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_TAB_H
#define _NEUTRON_WIDGETS_TAB_H

#include <stdbool.h>

struct _nt_widget;

typedef enum {
    NT_TAB_STATE_DEFAULT,
    NT_TAB_STATE_HOVERED,
    NT_TAB_STATE_ACTIVE
} nt_tab_state_t;

typedef struct _nt_tab {
    struct _nt_widget widget;
    struct _nt_widget *header;
    struct _nt_widget *content;
    nt_tab_state_t state;
    bool closable;
} nt_tab_t;

struct _nt_widget *nt_tab_create(struct _nt_widget *header, struct _nt_widget *content);
struct _nt_widget *nt_tab_create_label(char *text, struct _nt_widget *content);

struct _nt_widget *nt_tab_get_header(struct _nt_widget *tab);
struct _nt_widget *nt_tab_get_content(struct _nt_widget *tab);
void nt_tab_set_content(struct _nt_widget *tab, struct _nt_widget *content);
void nt_tab_set_active(struct _nt_widget *tab, bool active);
bool nt_tab_is_active(struct _nt_widget *tab);

#endif
