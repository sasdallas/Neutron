/**
 * @file neutron/widgets/image.c
 * @brief Neutron image widget
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

#define NT_IMAGE(w) ((nt_image_wdgt_t*)(w))

void nt_image_calc_size(nt_widget_t *w) {
    nt_image_wdgt_t *img = NT_IMAGE(w);
    if (img->img) {
        w->size_data.pref_width = w->size_data.min_width = img->img->width;
        w->size_data.pref_height = w->size_data.min_height = img->img->height;
    }
}

void nt_image_render(nt_widget_t *w, nt_render_surface_t *surf) {
    nt_image_wdgt_t *img = NT_IMAGE(w);
    if (img->img) {
        nt_render_draw_image(surf, img->img, w->style.padding[LEFT], w->style.padding[TOP]);
    }
}

nt_widget_vtable_t image_vtable = {
    .class_size = sizeof(nt_image_wdgt_t),
    .calc_size = nt_image_calc_size,
    .adjust_size = NULL,
    .init = NULL,
    .free = NULL,
    .render = nt_image_render
};

nt_widget_t *nt_image_create_from_image(nt_image_t *image) {
    nt_widget_t *w = nt_widget_create(NT_WIDGET_IMAGE, &image_vtable);
    nt_image_wdgt_t *img_wdgt = NT_IMAGE(w);
    img_wdgt->img = image;
    return w;
}

struct _nt_widget *nt_image_create_from_file(char *filename) {
    nt_image_t *img = malloc(sizeof(nt_image_t));
    int ret = nt_render_create_image(img, filename);
    if (ret != 0) {
        NT_WARN("nt_image_create_from_file() failed\n");
        // TODO: free
        return NULL;
    }

    return nt_image_create_from_image(img);
}
