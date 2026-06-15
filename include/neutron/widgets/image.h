/**
 * @file include/neutron/widgets/image.h
 * @brief Image widget
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_WIDGETS_IMAGE_H
#define _NEUTRON_WIDGETS_IMAGE_H

struct _nt_widget;
struct _nt_image;

typedef struct _nt_image_widget {
    struct _nt_widget widget;
    nt_image_t *img;
} nt_image_wdgt_t;

// will automatically use the backup if needed
struct _nt_widget *nt_image_create_from_file(char *filename);
struct _nt_widget *nt_image_create_from_image(nt_image_t *image);

#endif
