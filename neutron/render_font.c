/**
 * @file neutron/render_font.c
 * @brief FreeType font renderer
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
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

static FT_Library __nt_library = NULL;
static nt_font_handle_t __nt_fonts[NT_NFONTS] = { NULL };
static FT_Stroker stroker;

#ifdef __ETHEREAL__
#define FONT_DIR "/usr/share/fonts/"
#else
#define FONT_DIR "fonts/"
#endif

int nt_render_font_init() {
    if (FT_Init_FreeType(&__nt_library)) {
        NT_ERROR("FreeType init failed\n");
        return 1;
    }

    if (FT_Stroker_New(__nt_library, &stroker)) {
        NT_ERROR("FT_Stroker_New failed\n");
        return 1;
    }

    return 0;
}

nt_font_handle_t *nt_render_load_font(nt_font_type_t type) {
    if (__nt_fonts[type].face != NULL) {
        return &__nt_fonts[type];
    }

    char *filepath;
    switch (type) {
        case NT_SANS_16:
        case NT_SANS_12:
        case NT_SANS_10:
            filepath = FONT_DIR "DejaVuSans.ttf";
            break;

        case NT_SANS_BOLD_10:
        case NT_SANS_BOLD_12:
        case NT_SANS_BOLD_16:
            filepath = FONT_DIR "DejaVuSans-Bold.ttf";
            break;

        default:
            NT_ERROR("Unrecognized font type %d\n", type);
            return NULL;
    }


    __nt_fonts[type].type = type;
    FT_Error error = FT_New_Face(__nt_library, filepath, 0, (FT_Face*)&__nt_fonts[type].face);
    if (error) {
        NT_ERROR("Error loading %s\n", filepath);
        return NULL;
    }

    switch (type) {
        case NT_SANS_10:
        case NT_SANS_BOLD_10:
            FT_Set_Pixel_Sizes(__nt_fonts[type].face, 10, 10);
            break;

        case NT_SANS_12:
        case NT_SANS_BOLD_12:
            FT_Set_Pixel_Sizes(__nt_fonts[type].face, 12, 12);
            break;

        case NT_SANS_16:
        case NT_SANS_BOLD_16:
            FT_Set_Pixel_Sizes(__nt_fonts[type].face, 16, 16);
            break;
    }

    NT_DEBUG("Loaded font %d\n", type);
    return &__nt_fonts[type];
}

static nt_color_t nt_render_alpha_blend_glyph(nt_color_t bottom, nt_color_t top, unsigned char mask) {
    float a = mask / 255.0f;  // Correct scaling 0-1

    uint8_t a_new = NT_COLOR_A(bottom) * (1.0f - a) + NT_COLOR_A(top) * a;
    uint8_t r     = NT_COLOR_R(bottom) * (1.0f - a) + NT_COLOR_R(top) * a;
    uint8_t g     = NT_COLOR_G(bottom) * (1.0f - a) + NT_COLOR_G(top) * a;
    uint8_t b     = NT_COLOR_B(bottom) * (1.0f - a) + NT_COLOR_B(top) * a;

    return NT_COLOR(r, g, b, a_new);
}

static FT_BitmapGlyph nt_render_get_outline_bitmap(FT_Face face, FT_UInt glyph_idx, int load_flags, int radius_pixels) {
    if (FT_Load_Glyph(face, glyph_idx, load_flags | FT_LOAD_NO_BITMAP)) {
        return NULL;
    }

    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) {
        return NULL;
    }

    FT_Stroker_Set(stroker, radius_pixels * 64,  FT_STROKER_LINEJOIN_ROUND, FT_STROKER_LINECAP_ROUND, 0);

    if (FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1)) {
        FT_Done_Glyph(glyph);
        return NULL;
    }

    if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 1)) {
        FT_Done_Glyph(glyph);
        return NULL;
    }

    return (FT_BitmapGlyph)glyph;
}

static uint8_t *nt_render_create_blur_mask(const uint8_t *src, int src_w, int src_h, int *out_w, int *out_h, int radius) {
    int pad = radius;
    int dst_w = src_w + (pad * 2);
    int dst_h = src_h + (pad * 2);
    *out_w = dst_w;
    *out_h = dst_h;

    uint8_t *dst = (uint8_t *)malloc(dst_w * dst_h);
    if (!dst) return NULL;
    memset(dst, 0, dst_w * dst_h);

    for (int y = 0; y < src_h; y++) {
        for (int x = 0; x < src_w; x++) {
            dst[(y + pad) * dst_w + (x + pad)] = src[y * src_w + x];
        }
    }

    uint8_t *tmp = (uint8_t *)malloc(dst_w * dst_h);
    if (!tmp) { free(dst); return NULL; }

    for (int y = 0; y < dst_h; y++) {
        for (int x = 0; x < dst_w; x++) {
            int sum = 0, count = 0;
            for (int k = -radius; k <= radius; k++) {
                int nx = x + k;
                if (nx >= 0 && nx < dst_w) {
                    sum += dst[y * dst_w + nx];
                    count++;
                }
            }
            tmp[y * dst_w + x] = sum / count;
        }
    }
    
    for (int x = 0; x < dst_w; x++) {
        for (int y = 0; y < dst_h; y++) {
            int sum = 0, count = 0;
            for (int k = -radius; k <= radius; k++) {
                int ny = y + k;
                if (ny >= 0 && ny < dst_h) {
                    sum += tmp[ny * dst_w + x];
                    count++;
                }
            }
            dst[y * dst_w + x] = sum / count;
        }
    }

    free(tmp);
    return dst;
}

void nt_render_draw_text_stroke(nt_render_surface_t *surface, nt_font_handle_t *fonth, 
                                unsigned x, unsigned y, const char *text,
                                nt_color_t text_color, nt_color_t glow_color, int glow_radius) {
    char *str = (char*)text;
    FT_Face font = fonth->face;
    int cur_x = x + glow_radius;
    int cur_y = y + (font->size->metrics.ascender >> 6);

    int load_flags = FT_LOAD_DEFAULT;
    if (fonth->type == NT_SANS_BOLD_10 || fonth->type == NT_SANS_BOLD_12 || fonth->type == NT_SANS_BOLD_16) {
        load_flags |= FT_LOAD_TARGET_LIGHT;
    } else {
        load_flags |= FT_LOAD_FORCE_AUTOHINT;
    }

    FT_UInt first_idx = FT_Get_Char_Index(font, text[0]);
    if (first_idx) {
        if (!FT_Load_Glyph(font, first_idx, load_flags)) {
            if (font->glyph->bitmap_left < 0) {
                cur_x -= font->glyph->bitmap_left;
            }
        }
    }

    while (*str) {
        if (*str == '\n') {
            cur_x = x + glow_radius;
            cur_y += (font->size->metrics.height >> 6);
            goto _nextchar;
        }

        FT_UInt idx = FT_Get_Char_Index(font, *str);

        if (FT_Load_Glyph(font, idx, load_flags)) goto _nextchar;
        if (FT_Render_Glyph(font->glyph, FT_RENDER_MODE_NORMAL)) goto _nextchar;

        FT_GlyphSlot slot = font->glyph;

        if (glow_radius > 0 && slot->bitmap.width > 0 && slot->bitmap.rows > 0) {
            int glow_w, glow_h;
            uint8_t *glow_mask = nt_render_create_blur_mask(slot->bitmap.buffer, 
                                                            slot->bitmap.width, slot->bitmap.rows, 
                                                            &glow_w, &glow_h, glow_radius);
            if (glow_mask) {
                int glow_render_x = cur_x + slot->bitmap_left - glow_radius;
                int glow_render_y = cur_y - slot->bitmap_top - glow_radius;

                for (int _y = glow_render_y; _y < glow_render_y + glow_h; _y++) {
                    if (_y < 0 || _y >= surface->height) continue;
                    for (int _x = glow_render_x; _x < glow_render_x + glow_w; _x++) {
                        if (_x < 0 || _x >= surface->width) continue;

                        unsigned int *buffer = &NT_PIXEL(surface, _x, _y);
                        uint8_t mask = glow_mask[(_y - glow_render_y) * glow_w + (_x - glow_render_x)];
                        
                        int intensified_mask = mask * 2; 
                        if (intensified_mask > 255) intensified_mask = 255;

                        if (mask > 0) {
                            *buffer = nt_render_alpha_blend_glyph(*buffer, glow_color, intensified_mask);
                        }
                    }
                }
                free(glow_mask);
            }
        }

        int render_x = cur_x + slot->bitmap_left;
        int render_y = cur_y - slot->bitmap_top;

        for (int _y = render_y; _y < render_y + slot->bitmap.rows; _y++) {
            if (_y < 0 || _y >= surface->height) continue;
            for (int _x = render_x; _x < render_x + slot->bitmap.width; _x++) {
                if (_x < 0 || _x >= surface->width) continue;

                unsigned int *buffer = &NT_PIXEL(surface, _x, _y);
                uint8_t mask = slot->bitmap.buffer[((_y - render_y) * slot->bitmap.width + (_x - render_x))];
                if (mask > 0) {
                    *buffer = nt_render_alpha_blend_glyph(*buffer, text_color, mask);
                }
            }
        }

        cur_x += slot->advance.x >> 6;
        cur_y += slot->advance.y >> 6;

    _nextchar:
        str++;
    }
}

void nt_render_draw_text(nt_render_surface_t *surface, nt_font_handle_t *fonth, unsigned x, unsigned y, const char *text, nt_color_t color) {
    char *str = (char*)text;

    FT_Face font = fonth->face;

    int cur_x = x;
    int cur_y = y + ((font)->size->metrics.ascender >> 6);

    int load_flags = FT_LOAD_DEFAULT;
    if (fonth->type == NT_SANS_BOLD_10 || fonth->type == NT_SANS_BOLD_12 || fonth->type == NT_SANS_BOLD_16) {
        load_flags |= FT_LOAD_TARGET_LIGHT;
    } else {
        load_flags |= FT_LOAD_FORCE_AUTOHINT;
    }


    // !!! Hack
    FT_UInt first_idx = FT_Get_Char_Index(font, text[0]);
    if (first_idx) {
        if (!FT_Load_Glyph(font, first_idx, load_flags)) {
            if (!FT_Render_Glyph((font)->glyph, FT_RENDER_MODE_NORMAL)) {
                FT_GlyphSlot first_slot = (font)->glyph;

                if (first_slot->bitmap_left < 0) {
                    cur_x -= first_slot->bitmap_left;
                }
            }
        }
    }

    while (*str) {
        if  (*str == '\n') {
            cur_x = x;
            cur_y += ((font)->size->metrics.height >> 6);
            goto _nextchar;
        }

        FT_UInt idx = FT_Get_Char_Index(font, *str);

        if (FT_Load_Glyph(font, idx, load_flags)) goto _nextchar;
        if (FT_Render_Glyph((font)->glyph, FT_RENDER_MODE_NORMAL)) goto _nextchar;

        FT_GlyphSlot slot = (font)->glyph;

        int render_x = cur_x + slot->bitmap_left;
        int render_y = cur_y - slot->bitmap_top;


        for (int _y = render_y; _y < render_y + slot->bitmap.rows; _y++) {
            if (_y < 0 || _y >= surface->height) continue;

            for (int _x = render_x; _x < render_x + slot->bitmap.width; _x++) {
                if (_x < 0 || _x >= surface->width) continue;

                unsigned int *buffer = &NT_PIXEL(surface, _x, _y);
                *buffer = nt_render_alpha_blend_glyph(*buffer, color, slot->bitmap.buffer[((_y - render_y) * slot->bitmap.width + (_x - render_x))]);
            }
        }

        cur_x += slot->advance.x >> 6;
        cur_y += slot->advance.y >> 6;

    _nextchar:
        str++;
    }
}

static void nt_render_text_dimensions_inner(nt_font_handle_t *font, const char *text, size_t *w, size_t *h, bool glow, int gradius) {
    FT_Face face = (FT_Face)font->face;

    int load_flags = FT_LOAD_NO_BITMAP;
    if (font->type == NT_SANS_BOLD_10 || font->type == NT_SANS_BOLD_12 || font->type == NT_SANS_BOLD_16) {
        load_flags |= FT_LOAD_TARGET_LIGHT;
    } else {
        load_flags |= FT_LOAD_FORCE_AUTOHINT;
    }


    size_t current_width = 0;
    size_t max_width = 0;
    size_t lines = 1;

    const char *str = text;
    while (*str) {
        if (*str == '\n') {
            if (current_width > max_width) max_width = current_width;
            current_width = 0;
            lines++;
            str++;
            continue;
        }

        FT_UInt glyph = FT_Get_Char_Index(face, (unsigned char)*str);

        if (!FT_Load_Glyph(face, glyph, load_flags)) {
            current_width += face->glyph->advance.x >> 6;
        }

        str++;
    }

    if (current_width > max_width) max_width = current_width;
    
    if (glow) {
        if (w) *w = max_width + (gradius * 2);
        if (h) *h = ((face->size->metrics.height >> 6) * lines + 1) + (gradius * 2);
    } else {
        if (w) *w = max_width;

        // who knows why this works, apparently underscores need this
        if (h) *h = (face->size->metrics.height >> 6) * lines + 1;
    }
}


void nt_render_text_dimensions_glow(nt_font_handle_t *font, const char *text, size_t *w, size_t *h, int radius) {
    return nt_render_text_dimensions_inner(font,text,w,h,true,radius);
}

void nt_render_text_dimensions(nt_font_handle_t *font, const char *text, size_t *w, size_t *h) {
    return nt_render_text_dimensions_inner(font,text,w,h,false,0);
}


void nt_render_free_font(nt_font_handle_t *font) {
    // NO-OP
}
