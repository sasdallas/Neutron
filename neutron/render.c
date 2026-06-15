/**
 * @file neutron/render.c
 * @brief Neutron renderer
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
#include <math.h>

int nt_render_init() {
    extern int nt_render_font_init();
    return nt_render_font_init();
}

void nt_render_init_surface(nt_window_t *window, nt_render_surface_t *surface, size_t width, size_t height) {
    surface->width = width;
    surface->height = height;
    surface->buffer = malloc(width * height * 4);
    surface->initialized = true;
    nt_platform_init_surface(window, surface, width, height);
}

void nt_render_deinit_surface(nt_render_surface_t *surface) {
    if (!surface->initialized) return;

    // platform frees buffer    
    nt_platform_deinit_surface(surface);
    surface->initialized = false;
}

void nt_render_clear(nt_render_surface_t *surface, nt_color_t color) {
    unsigned int *buffer = (unsigned int*)surface->buffer;
    for (int i = 0; i < surface->height; i++) {
        for (int x = 0; x < surface->width; x++) {
            buffer[x] = color;
        }

        buffer += surface->width;
    }
}

void nt_render_fill_rect(nt_render_surface_t *surface, nt_rect_t *rect, nt_color_t color) {
    if (rect->x >= surface->width || rect->y >= surface->height) return;
    if (rect->x + rect->w > surface->width) rect->w = surface->width - rect->x;
    if (rect->y + rect->h > surface->height) rect->h = surface->height - rect->y;

    unsigned int *buffer_base = &NT_PIXEL(surface, rect->x, rect->y);

    for (int y = 0; y < rect->h; y++) {
        for (int x = 0; x < rect->w; x++) {
            buffer_base[x] = color;
        }

        buffer_base += surface->width;
    }
}

void nt_render_border_rect(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, nt_color_t color) {
    if (!surface || !rect) return;
    if (thickness <= 0) return;

    int32_t x = rect->x;
    int32_t y = rect->y;
    int32_t w = rect->w;
    int32_t h = rect->h;

    if (w <= 0 || h <= 0) return;

    int32_t sw = (int32_t)surface->width;
    int32_t sh = (int32_t)surface->height;

    int32_t x0 = x < 0 ? 0 : x;
    int32_t y0 = y < 0 ? 0 : y;
    int32_t x1 = (x + w) > sw ? sw : (x + w);
    int32_t y1 = (y + h) > sh ? sh : (y + h);

    if (x0 >= x1 || y0 >= y1) return;

    for (int32_t py = y0; py < y1; py++) {
        int32_t ly = py - y;
        bool border_y = (ly < thickness) || (ly >= (h - thickness));

        for (int32_t px = x0; px < x1; px++) {
            int32_t lx = px - x;
            if (border_y || lx < thickness || lx >= (w - thickness)) {
                NT_PIXEL(surface, px, py) = color;
            }
        }
    }
}

nt_color_t nt_alpha_blend(nt_color_t top, nt_color_t bottom) {
    // TODO: Rewrite without stolen code...
    // SOURCE: ToaruOS (klange)
	if (NT_COLOR_A(bottom) == 0) return top;
	if (NT_COLOR_A(top) == 255) return top;
	if (NT_COLOR_A(top) == 0) return bottom;

	uint8_t a = NT_COLOR_A(top);
	uint16_t t = 0xFF ^ a;
	uint8_t d_r = NT_COLOR_R(top) + (((uint32_t)(NT_COLOR_R(bottom) * t + 0x80) * 0x101) >> 16UL);
	uint8_t d_g = NT_COLOR_G(top) + (((uint32_t)(NT_COLOR_G(bottom) * t + 0x80) * 0x101) >> 16UL);
	uint8_t d_b = NT_COLOR_B(top) + (((uint32_t)(NT_COLOR_B(bottom) * t + 0x80) * 0x101) >> 16UL);
	uint8_t d_a = NT_COLOR_A(top) + (((uint32_t)(NT_COLOR_A(bottom) * t + 0x80) * 0x101) >> 16UL);
	return NT_COLOR(d_r, d_g, d_b, d_a);
}


static void nt_render_rounded_rect_pattern(nt_render_surface_t *surface, nt_rect_t *rect, int radius, nt_color_t (*pattern)(int, int, uint8_t, void*), void* data) {
    if (!surface || !rect || !pattern) return;

    int32_t x = rect->x;
    int32_t y = rect->y;
    int32_t width = rect->w;
    int32_t height = rect->h;

    if (width <= 0 || height <= 0) return;
    if (surface->width <= 0 || surface->height <= 0) return;

    if (radius < 0) radius = 0;
    int32_t max_radius = (width < height ? width : height) / 2;
    if (radius > max_radius) radius = max_radius;

    int32_t x0 = x < 0 ? 0 : x;
    int32_t y0 = y < 0 ? 0 : y;
    int32_t x1 = x + width > surface->width ? surface->width : x + width;
    int32_t y1 = y + height > surface->height ? surface->height : y + height;

    if (x0 >= x1 || y0 >= y1) return;

    float r = (float)radius;
    float inner = r - 1.0f;

    for (int32_t py = y0; py < y1; py++) {
        for (int32_t px = x0; px < x1; px++) {
            int32_t lx = px - x;
            int32_t ly = py - y;
            uint8_t a = 255;

            if (radius > 0) {
                bool corner = false;
                float dx = 0.0f;
                float dy = 0.0f;

                if (lx < radius && ly < radius) {
                    corner = true;
                    dx = (float)(radius - 1 - lx);
                    dy = (float)(radius - 1 - ly);
                } else if (lx >= width - radius && ly < radius) {
                    corner = true;
                    dx = (float)(lx - (width - radius));
                    dy = (float)(radius - 1 - ly);
                } else if (lx < radius && ly >= height - radius) {
                    corner = true;
                    dx = (float)(radius - 1 - lx);
                    dy = (float)(ly - (height - radius));
                } else if (lx >= width - radius && ly >= height - radius) {
                    corner = true;
                    dx = (float)(lx - (width - radius));
                    dy = (float)(ly - (height - radius));
                }

                if (corner) {
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist >= r) continue;

                    if (dist > inner) {
                        float alpha = 1.0f - (dist - inner);
                        if (alpha < 0.0f) alpha = 0.0f;
                        if (alpha > 1.0f) alpha = 1.0f;
                        a = (uint8_t)(alpha * 255.0f);
                    }
                }
            }

            nt_color_t c = pattern(lx, ly, a, data);
            NT_PIXEL(surface, px, py) = nt_alpha_blend(c, NT_PIXEL(surface, px, py));
        }
    }
}

static void nt_render_rect_pattern(nt_render_surface_t *surface, nt_rect_t *rect, nt_color_t (*pattern)(int, int, uint8_t, void*), void* data) {
    if (!surface || !rect || !pattern) return;

    int32_t x = rect->x;
    int32_t y = rect->y;
    int32_t width = rect->w;
    int32_t height = rect->h;

    if (width <= 0 || height <= 0) return;
    if (surface->width <= 0 || surface->height <= 0) return;


    int32_t x0 = x < 0 ? 0 : x;
    int32_t y0 = y < 0 ? 0 : y;
    int32_t x1 = x + width > surface->width ? surface->width : x + width;
    int32_t y1 = y + height > surface->height ? surface->height : y + height;

    if (x0 >= x1 || y0 >= y1) return;

    for (int32_t py = y0; py < y1; py++) {
        for (int32_t px = x0; px < x1; px++) {
            int32_t lx = px - x;
            int32_t ly = py - y;
            uint8_t a = 255;

            nt_color_t c = pattern(lx, ly, a, data);
            NT_PIXEL(surface, px, py) = nt_alpha_blend(c, NT_PIXEL(surface, px, py));
        }
    }
}

void nt_render_rounded_rect(nt_render_surface_t *surface, nt_rect_t *rect, int radius, nt_color_t color) {
    int32_t x = rect->x;
    int32_t y = rect->y;
    int32_t width  = rect->w;
    int32_t height = rect->h;

    int32_t max_w = surface->width;
    int32_t max_h = surface->height;

    // Clip to the screen bounds
    int32_t x0 = x < 0 ? 0 : x;
    int32_t y0 = y < 0 ? 0 : y;
    int32_t x1 = x + width  > max_w ? max_w : x + width;
    int32_t y1 = y + height > max_h ? max_h : y + height;

    if (x0 >= x1 || y0 >= y1) return;

	// Center rectangle
    for (int32_t py = y0; py < y1; py++) {
        for (int32_t px = x0; px < x1; px++) {

            int32_t lx = px - x;
            int32_t ly = py - y;

            if ((lx < radius || lx >= width - radius) &&
                (ly < radius || ly >= height - radius)) {
                continue;
            }

            NT_PIXEL(surface, px, py) = color;
        }
    }

	// Corners
    float nr = (float)radius + 1.0f;

    for (int dy = 0; dy <= nr; dy++) {
        for (int dx = 0; dx <= nr; dx++) {

            if (nr * nr < dx * dx + dy * dy) continue;

            float dist = sqrtf(dx * dx + dy * dy);
            float alpha = 1.0f;

            if (dist > (float)(radius - 1)) {
                alpha = 1.0f - (dist - (float)(radius - 1));
                if (alpha < 0.0f) alpha = 0.0f;
            }

            uint8_t a = (uint8_t)(alpha * 255.0f);

            nt_color_t c = NT_COLOR(
                NT_COLOR_R(color) * a / 255,
                NT_COLOR_G(color) * a / 255,
                NT_COLOR_B(color) * a / 255,
                a
            );

            int16_t px[4] = {
                x + radius - dx - 1,
                x + width  - radius + dx,
                x + radius - dx - 1,
                x + width  - radius + dx
            };

            int16_t py[4] = {
                y + radius - dy - 1,
                y + radius - dy - 1,
                y + height - radius + dy,
                y + height - radius + dy
            };

            for (int i = 0; i < 4; i++) {
                if (px[i] < 0 || px[i] >= max_w ||
                    py[i] < 0 || py[i] >= max_h)
                    continue;

                NT_PIXEL(surface, px[i], py[i]) = nt_alpha_blend(c, NT_PIXEL(surface, px[i], py[i]));
            }
        }
    }
}


struct nt_grad_data {
    bool is_horizontal;
    size_t size;
    nt_color_t start;
    nt_color_t end;
};

nt_color_t nt_pattern_grad(int x, int y, uint8_t alpha, void *data) {
    struct nt_grad_data *grad = (struct nt_grad_data*)data;
    if (!grad || grad->size == 0) return 0;

    int32_t use = (grad->is_horizontal) ? x : y;
    float denom = (grad->size > 1) ? (float)(grad->size - 1) : 1.0f;
    float point = (float)use / denom;
    if (point < 0.0f) point = 0.0f;
    if (point > 1.0f) point = 1.0f;

    uint8_t a = (uint8_t)((NT_COLOR_A(grad->start) * (1.0f - point)) + (NT_COLOR_A(grad->end) * point));
    uint8_t r = (uint8_t)((NT_COLOR_R(grad->start) * (1.0f - point)) + (NT_COLOR_R(grad->end) * point));
    uint8_t g = (uint8_t)((NT_COLOR_G(grad->start) * (1.0f - point)) + (NT_COLOR_G(grad->end) * point));
    uint8_t b = (uint8_t)((NT_COLOR_B(grad->start) * (1.0f - point)) + (NT_COLOR_B(grad->end) * point));
    
    nt_color_t c =  0 |
            ((a * alpha / 255) & 0xFF) << 24 |
            ((r * alpha / 255) & 0xFF) << 16 |
            ((g * alpha / 255) & 0xFF) << 8 |
            ((b * alpha / 255) & 0xFF) << 0;

    return c;
}

void nt_render_rounded_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, int radius, nt_color_t start, nt_color_t end, bool horiz) {
    struct nt_grad_data data = {
        .is_horizontal = horiz,
        .size = (horiz) ? rect->w : rect->h,
        .start = start,
        .end = end,
    };

    nt_render_rounded_rect_pattern(surface, rect, radius, nt_pattern_grad, &data);
}

void nt_render_fill_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, nt_color_t start, nt_color_t end, bool horiz) {
    struct nt_grad_data data = {
        .is_horizontal = horiz,
        .size = (horiz) ? rect->w : rect->h,
        .start = start,
        .end = end
    };

    nt_render_rect_pattern(surface, rect, nt_pattern_grad, &data);
}

static nt_color_t nt_pattern_border_solid(int x, int y, uint8_t alpha, void *data) {
    struct {
        int thickness;
        int width;
        int height;
        nt_color_t color;
    } *d = data;

    if (!d) return 0;

    int w = d->width;
    int h = d->height;
    int t = d->thickness;

    if (t <= 0) return 0;
    if (x < 0 || y < 0 || x >= w || y >= h) return 0;

    bool is_border = (x < t) || (x >= (w - t)) || (y < t) || (y >= (h - t));
    if (!is_border) return 0;

    uint8_t a = (NT_COLOR_A(d->color) * alpha) / 255;
    uint8_t r = (NT_COLOR_R(d->color) * alpha) / 255;
    uint8_t g = (NT_COLOR_G(d->color) * alpha) / 255;
    uint8_t b = (NT_COLOR_B(d->color) * alpha) / 255;

    nt_color_t c =  0 |
            ((a) & 0xFF) << 24 |
            ((r) & 0xFF) << 16 |
            ((g) & 0xFF) << 8 |
            ((b) & 0xFF) << 0;

    return c;
}

static nt_color_t nt_pattern_border_grad(int x, int y, uint8_t alpha, void *data) {
    struct {
        struct nt_grad_data grad;
        int thickness;
        int width;
        int height;
    } *d = data;

    if (!d) return 0;

    int w = d->width;
    int h = d->height;
    int t = d->thickness;

    if (t <= 0) return 0;
    if (x < 0 || y < 0 || x >= w || y >= h) return 0;

    bool is_border = (x < t) || (x >= (w - t)) || (y < t) || (y >= (h - t));
    if (!is_border) return 0;

    return nt_pattern_grad(x, y, alpha, &d->grad);
}


void nt_render_border_rounded_rect(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, int radius, nt_color_t color) {
    if (!surface || !rect) return;
    if (thickness <= 0) return;

    struct {
        int thickness;
        int width;
        int height;
        nt_color_t color;
    } data = {
        .thickness = thickness,
        .width = rect->w,
        .height = rect->h,
        .color = color
    };

    nt_render_rounded_rect_pattern(surface, rect, radius, nt_pattern_border_solid, &data);
}

void nt_render_border_rounded_rect_gradient(nt_render_surface_t *surface, nt_rect_t *rect, int thickness, int radius, nt_color_t start, nt_color_t end, bool horiz) {
    if (!surface || !rect) return;
    if (thickness <= 0) return;

    struct {
        struct nt_grad_data grad;
        int thickness;
        int width;
        int height;
    } data;

    data.grad.is_horizontal = horiz;
    data.grad.size = (horiz) ? rect->w : rect->h;
    data.grad.start = start;
    data.grad.end = end;
    data.thickness = thickness;
    data.width = rect->w;
    data.height = rect->h;

    nt_render_rounded_rect_pattern(surface, rect, radius, nt_pattern_border_grad, &data);
}

