/**
 * @file neutron/timer.c
 * @brief Neutron timer implementation
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
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

nt_timer_t *timer_head = NULL;

unsigned long long nt_timer_get_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long long)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
}

nt_timer_t *nt_timer_create(long ms, nt_timer_callback_t callback, void *data, bool enabled) {
    nt_timer_t *timer = malloc(sizeof(nt_timer_t));
    timer->ms = ms;
    timer->callback = callback;
    timer->data = data;
    timer->enabled = enabled;
    if (enabled) timer->next_period = nt_timer_get_ms() + ms;
    else timer->next_period = 0;
    timer->next = timer_head;
    timer_head = timer;
    return timer;
}

void nt_timer_delete(nt_timer_t *timer) {
    if (timer == timer_head) {
        timer_head = timer->next;
        return;
    }

    NT_ITERATE_TIMERS(n) {
        if (n->next == timer) {
            n->next = timer->next;
            break;
        }
    }

    free(timer);
}

void nt_timer_reset(nt_timer_t *timer) {
    timer->next_period = nt_timer_get_ms() + timer->ms;
}

void nt_timer_update() {
    unsigned long long now = nt_timer_get_ms();
    nt_timer_t *timer = timer_head;
    while (timer) {
        if (timer->enabled && now >= timer->next_period) {
            
            bool res = timer->callback(timer, timer->data);
            if (res) {
                // arm timer
                timer->next_period = nt_timer_get_ms() + timer->ms;
            } else {
                NT_INFO("Dropping timer %p\n", timer);
                nt_timer_t *next = timer->next;
                nt_timer_delete(timer);
                timer = next;
                continue;
            }
        }

        timer = timer->next;
    }
}

nt_timer_t *nt_timer_get_list() { return timer_head; }