/**
 * @file include/neutron/fade.h
 * @brief Fade manager
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2026 Samuel Stuart
 */

#ifndef _NEUTRON_FADE_H
#define _NEUTRON_FADE_H

#include <stddef.h>
#include <stdint.h>
#include <neutron/style.h>

struct _nt_timer;
struct _nt_widget;

typedef struct _nt_fade {
    unsigned int duration;
    bool fade_in;           // Fade in or out
    uint64_t last_update;
    float time;
    struct _nt_timer *timer;
    void (*fade_callback)(struct _nt_fade*);
    void *priv;
} nt_fade_t;

typedef struct _nt_fade_color {
    nt_fade_t f;
    nt_color_t a;
    nt_color_t b;
    struct _nt_widget *w;
} nt_fade_color_t;

nt_fade_t *nt_fade_create(unsigned int duration, void (*fade_callback)(struct _nt_fade*), void *priv);
void nt_fade_set(nt_fade_t *fade, bool fade_in);
void nt_fade_start(nt_fade_t *fade);
void nt_fade_stop(nt_fade_t *fade);

nt_fade_t *nt_fade_create_color(struct _nt_widget *w, unsigned int duration, nt_color_t a, nt_color_t b);

#endif
