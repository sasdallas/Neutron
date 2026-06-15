/**
 * @file include/neutron/icon.h
 * @brief Neutron icon framework
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_ICON_H
#define _NEUTRON_ICON_H

#include <stddef.h>

struct _nt_image;

typedef struct _nt_icon_cache_entry {
    struct _nt_icon_cache_entry *next;
    char *name;
    size_t size;
    struct _nt_image img;
} nt_icon_cache_entry_t;

int nt_icon_init();
struct _nt_image *nt_icon_get(char *icon_name, char **fallbacks, size_t size);

#endif
