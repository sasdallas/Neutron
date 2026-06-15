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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>
#include <stdint.h>

static FT_Library __nt_library = NULL;
static FT_Face __nt_fonts[NT_NFONTS] = { NULL };

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

    return 0;
}

nt_font_handle_t *nt_render_load_font(nt_font_type_t type) {
    if (__nt_fonts[type] != NULL) {
        return (nt_font_handle_t*)__nt_fonts[type];
    }

    char *filepath;
    switch (type) {
        case NT_SANS_12:
        case NT_SANS_10:
            filepath = FONT_DIR "DejaVuSans.ttf";
            break;

        case NT_SANS_BOLD_10:
        case NT_SANS_BOLD_12:
            filepath = FONT_DIR "DejaVuSans-Bold.ttf";
            break;

        default:
            NT_ERROR("Unrecognized font type %d\n", type);
            return NULL;
    }

    FT_Error error = FT_New_Face(__nt_library, filepath, 0, &__nt_fonts[type]);
    if (error) {
        NT_ERROR("Error loading %s\n", filepath);
        return NULL;
    }

    switch (type) {
        case NT_SANS_10: case NT_SANS_BOLD_10: FT_Set_Pixel_Sizes(__nt_fonts[type], 10, 10); break;
        case NT_SANS_12: case NT_SANS_BOLD_12: FT_Set_Pixel_Sizes(__nt_fonts[type], 12, 12); break;
    }

    NT_DEBUG("Loaded font %d\n", type);
    return (nt_font_handle_t*)__nt_fonts[type];
}

static nt_color_t nt_render_alpha_blend_glyph(nt_color_t bottom, nt_color_t top, unsigned char mask) {
    float a = mask / 255.0f;  // Correct scaling 0-1

    uint8_t a_new = NT_COLOR_A(bottom) * (1.0f - a) + NT_COLOR_A(top) * a;
    uint8_t r     = NT_COLOR_R(bottom) * (1.0f - a) + NT_COLOR_R(top) * a;
    uint8_t g     = NT_COLOR_G(bottom) * (1.0f - a) + NT_COLOR_G(top) * a;
    uint8_t b     = NT_COLOR_B(bottom) * (1.0f - a) + NT_COLOR_B(top) * a;

    return NT_COLOR(r, g, b, a_new);
}

void nt_render_draw_text(nt_render_surface_t *surface, nt_font_handle_t *font, unsigned x, unsigned y, const char *text, nt_color_t color) {
    char *str = (char*)text;

    int cur_x = x;
    int cur_y = y + (((FT_Face)font)->size->metrics.ascender >> 6);


    // !!! Hack
    FT_UInt first_idx = FT_Get_Char_Index((FT_Face)font, text[0]);
    if (first_idx) {
        if (!FT_Load_Glyph((FT_Face)font, first_idx, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT)) {
            if (!FT_Render_Glyph(((FT_Face)font)->glyph, FT_RENDER_MODE_NORMAL)) {
                FT_GlyphSlot first_slot = ((FT_Face)font)->glyph;

                if (first_slot->bitmap_left < 0) {
                    cur_x -= first_slot->bitmap_left;
                }
            }
        }
    }

    while (*str) {
        if  (*str == '\n') {
            cur_x = x;
            cur_y += (((FT_Face)font)->size->metrics.height >> 6);
            goto _nextchar;
        }

        FT_UInt idx = FT_Get_Char_Index((FT_Face)font, *str);

        if (FT_Load_Glyph((FT_Face)font, idx, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT)) goto _nextchar;
        if (FT_Render_Glyph(((FT_Face)font)->glyph, FT_RENDER_MODE_NORMAL)) goto _nextchar;

        FT_GlyphSlot slot = ((FT_Face)font)->glyph;

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
// void nt_render_draw_text(nt_render_surface_t *surface, nt_font_handle_t *font, unsigned x, unsigned y, const char *text, nt_color_t color) {
//     char *str = (char*)text;

//     int cur_x = x;
//     int cur_y = y + (((FT_Face)font)->size->metrics.ascender >> 6);

//     FT_UInt first_idx = FT_Get_Char_Index((FT_Face)font, text[0]);
//     if (first_idx) {
//         if (!FT_Load_Glyph((FT_Face)font, first_idx, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT)) {
//             if (!FT_Render_Glyph(((FT_Face)font)->glyph, FT_RENDER_MODE_NORMAL)) {
//                 FT_GlyphSlot first_slot = ((FT_Face)font)->glyph;

//                 if (first_slot->bitmap_left < 0) {
//                     cur_x -= first_slot->bitmap_left;
//                 }
//             }
//         }
//     }

//     FT_UInt prev_idx = 0;

//     while (*str) {
//         if (*str == '\n') {
//             cur_x = x;
//             cur_y += (((FT_Face)font)->size->metrics.height >> 6);
//             prev_idx = 0;
//             goto _nextchar;
//         }

//         FT_UInt idx = FT_Get_Char_Index((FT_Face)font, *str);

//         // ---- FIX 1: kerning ----
//         if (prev_idx && idx) {
//             FT_Vector delta;
//             FT_Get_Kerning((FT_Face)font, prev_idx, idx, FT_KERNING_DEFAULT, &delta);
//             cur_x += delta.x >> 6;
//         }

//         if (FT_Load_Glyph((FT_Face)font, idx, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT)) goto _nextchar;
//         if (FT_Render_Glyph(((FT_Face)font)->glyph, FT_RENDER_MODE_NORMAL)) goto _nextchar;

//         FT_GlyphSlot slot = ((FT_Face)font)->glyph;

//         int render_x = cur_x + slot->bitmap_left;
//         int render_y = cur_y - slot->bitmap_top;

//         for (int _y = render_y; _y < render_y + slot->bitmap.rows; _y++) {
//             if (_y < 0 || _y >= surface->height) continue;

//             for (int _x = render_x; _x < render_x + slot->bitmap.width; _x++) {
//                 if (_x < 0 || _x >= surface->width) continue;

//                 unsigned int *buffer = &NT_PIXEL(surface, _x, _y);
//                 *buffer = nt_render_alpha_blend_glyph(
//                     *buffer,
//                     color,
//                     slot->bitmap.buffer[((_y - render_y) * slot->bitmap.width + (_x - render_x))]
//                 );
//             }
//         }

//         // ---- FIX 2: prevent cumulative truncation drift ----
//         cur_x += (int)(slot->advance.x / 64.0f);
//         cur_y += (int)(slot->advance.y / 64.0f); // harmless but left unchanged structurally

//         prev_idx = idx;

//     _nextchar:
//         str++;
//     }
// }

void nt_render_text_dimensions(nt_font_handle_t *font, const char *text, size_t *w, size_t *h) {
    FT_Face face = (FT_Face)font;

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

        if (!FT_Load_Glyph(face, glyph, FT_LOAD_NO_BITMAP | FT_LOAD_FORCE_AUTOHINT)) {
            current_width += face->glyph->advance.x >> 6;
        }

        str++;
    }

    if (current_width > max_width) max_width = current_width;
    if (w) *w = max_width;

    // who knows why this works, apparently underscores need this
    if (h) *h = (face->size->metrics.height >> 6) * lines + 1;
}


void nt_render_free_font(nt_font_handle_t *font) {
    // NO-OP
}
