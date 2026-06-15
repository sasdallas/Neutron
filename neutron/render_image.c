/**
 * @file neutron/render_image.c
 * @brief Neutron image renderer
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

#define STB_IMAGE_IMPLEMENTATION
#include <neutron/external/stb_image.h>

#include <errno.h>


int nt_render_create_image(nt_image_t *image, char *filename) {
    stbi_uc *img_buf = stbi_load(filename, &image->width, &image->height, &image->nchannels, 0);
    if (!img_buf) {
        NT_WARN("Failed to load image \"%s\"\n", filename);
    }

    image->bitmap = (unsigned int*)img_buf;
    return 0;
}

int nt_render_free_image(nt_image_t *image) {
    stbi_image_free(image->bitmap);
    return 0;
}

nt_color_t blend(nt_color_t top, nt_color_t bottom) {
    uint32_t r_top = NT_COLOR_R(top), g_top = NT_COLOR_G(top), b_top = NT_COLOR_B(top), a_top = NT_COLOR_A(top);
    uint32_t r_bot = NT_COLOR_R(bottom), g_bot = NT_COLOR_G(bottom), b_bot = NT_COLOR_B(bottom), a_bot = NT_COLOR_A(bottom);

    // If top is fully opaque, skip blending
    if (a_top == 255) return top;
    // If top is fully transparent, keep bottom
    if (a_top == 0) return bottom;

    // Calculate output alpha
    uint32_t a_out = a_top + ((a_bot * (255 - a_top)) / 255);
    
    if (a_out == 0) return NT_COLOR(0, 0, 0, 0);

    // Standard alpha blending
    uint32_t r_out = (r_top * a_top + r_bot * a_bot * (255 - a_top) / 255) / a_out;
    uint32_t g_out = (g_top * a_top + g_bot * a_bot * (255 - a_top) / 255) / a_out;
    uint32_t b_out = (b_top * a_top + b_bot * a_bot * (255 - a_top) / 255) / a_out;

    // pre-multiply
    return NT_COLOR(r_out, g_out, b_out, a_out);
}

void nt_render_draw_image(nt_render_surface_t *surf, nt_image_t *img, unsigned x, unsigned y) {
    uint8_t *dst = (uint8_t *)surf->buffer;
    uint8_t *src = (uint8_t *)img->bitmap;
    int pitch = surf->width * 4;

    int copy_w = (img->width < surf->width) ? img->width : surf->width;
    int copy_h = (img->height < surf->height) ? img->height : surf->height;

    for (int _y = 0; _y < copy_h; ++_y) {
        uint8_t *drow = dst + ((y + _y) * pitch);
        uint8_t *srow = src + (_y * img->width * img->nchannels);

        for (int _x = 0; _x < copy_w; ++_x) {
            uint8_t r = 0, g = 0, b = 0, a = 255;

            switch (img->nchannels) {
                case 4:
                    b = srow[_x * 4 + 0];
                    g = srow[_x * 4 + 1];
                    r = srow[_x * 4 + 2];
                    a = srow[_x * 4 + 3];
                    break;
                case 3:
                    b = srow[_x * 3 + 0];
                    g = srow[_x * 3 + 1];
                    r = srow[_x * 3 + 2];
                    break;
                case 2:
                    r = g = b = srow[_x * 2 + 0];
                    a = srow[_x * 2 + 1];
                    break;
                
                case 1:
                    r = g = b = srow[_x];
                    break;
                
                default:
                    break;
            }

            r = (r * a) / 255;
            g = (g * a) / 255;
            b = (b * a) / 255;

            nt_color_t *dst = (nt_color_t*)&drow[(x+_x) * 4];
            uint32_t src_pixel = NT_COLOR(b,g,r,a);
            *dst = blend(src_pixel,  *dst);
        }
    }
}
