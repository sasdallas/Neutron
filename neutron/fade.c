/**
 * @file neutron/fade.c
 * @brief Fade manager
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

#define FADE_INTERVAL 5

bool nt_fade_callback(nt_timer_t *t, nt_fade_t *f) {
    uint64_t now = nt_timer_get_ms();
    uint64_t dt = now - f->last_update;
    if (dt < 3) return true;

    f->last_update = now;

    float step = (float)dt / (float)f->duration;
    if (f->fade_in) {
        f->time += step;
        if (f->time >= 1.0f) {
            f->time = 1.0f;
            nt_timer_set_enabled(f->timer, false);
        }
    } else {
        f->time -= step;
        if (f->time <= 0.0f) {
            f->time = 0.0f;
            nt_timer_set_enabled(f->timer, false);
        }
    }
    
    f->fade_callback(f);
    return true;
}

nt_fade_t *nt_fade_create(unsigned int duration, void (*fade_callback)(struct _nt_fade*), void *priv) {
    nt_fade_t *f = malloc(sizeof(nt_fade_t));
    memset(f, 0, sizeof(nt_fade_t));
    f->fade_in = true;
    f->priv = priv;
    f->fade_callback = fade_callback;
    f->duration = duration;
    f->timer = nt_timer_create(FADE_INTERVAL, (nt_timer_callback_t)nt_fade_callback, f, false);
    return f;
}

void nt_fade_set(nt_fade_t *fade, bool fade_in) {
    fade->fade_in = fade_in;
}

void nt_fade_start(nt_fade_t *fade) {
    fade->last_update = nt_timer_get_ms(); 
    nt_timer_set_enabled(fade->timer, true);
}

void nt_fade_stop(nt_fade_t *fade) {
    nt_timer_set_enabled(fade->timer, false);
}

void nt_fade_color_callback(nt_fade_t *fade) {
#define LERP(a,b,c) (float)(a) + ((float)(b) - (float)(a)) * (float)(c)

    nt_fade_color_t *color = (nt_fade_color_t*)fade;
    uint8_t a = LERP(NT_COLOR_A(color->a), NT_COLOR_A(color->b), fade->time);
    uint8_t r = LERP(NT_COLOR_R(color->a), NT_COLOR_R(color->b), fade->time);
    uint8_t g = LERP(NT_COLOR_G(color->a), NT_COLOR_G(color->b), fade->time);
    uint8_t b = LERP(NT_COLOR_B(color->a), NT_COLOR_B(color->b), fade->time);

    nt_color_t new = NT_COLOR(r,g,b,a);
    nt_style_set_bg_color(&color->w->style, new);
    nt_widget_invalidate(color->w);
}

nt_fade_t *nt_fade_create_color(struct _nt_widget *w, unsigned int duration, nt_color_t a, nt_color_t b) {
    nt_fade_color_t *c = malloc(sizeof(nt_fade_color_t));

    c->a = a;
    c->b = b;
    c->w = w;

    nt_fade_t *f = &c->f;
    memset(f, 0, sizeof(nt_fade_t));
    f->fade_in = true;
    f->duration = duration;
    f->timer = nt_timer_create(FADE_INTERVAL, (nt_timer_callback_t)nt_fade_callback, f, false);
    f->fade_callback = nt_fade_color_callback;

    return &c->f;
}
