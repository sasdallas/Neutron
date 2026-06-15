/**
 * @file include/neutron/timer.h
 * @brief Neutron timer component
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_TIMER_H
#define _NEUTRON_TIMER_H

struct _nt_timer;

// If returning false, the timer will be destroyed.
typedef bool (*nt_timer_callback_t)(struct _nt_timer *timer, void *data);

typedef struct _nt_timer {
    struct _nt_timer *next;
    bool enabled;
    long ms;
    unsigned long long next_period;
    nt_timer_callback_t callback;
    void *data;
} nt_timer_t;

#define NT_TIMER_ENABLED true
#define NT_TIMER_DISABLED false

nt_timer_t *nt_timer_create(long ms, nt_timer_callback_t callback, void *data, bool enabled);
void nt_timer_delete(nt_timer_t *timer);
void nt_timer_reset(nt_timer_t *timer);
void nt_timer_update();
nt_timer_t *nt_timer_get_list();

static inline void nt_timer_set_enabled(nt_timer_t *timer, bool enabled) {
    timer->enabled = enabled;
    if (enabled) nt_timer_reset(timer);
}

#endif
