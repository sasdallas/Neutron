/**
 * @file neutron/main.c
 * @brief Neutron main
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

int nt_init() {
    int r = nt_platform_init();
    if (r < 0) return r;

    r = nt_render_init();
    if (r < 0) return r;

    r = nt_icon_init();
    if (r < 0) return r;

    r = nt_dialog_init();
    if (r < 0) return r;

    return 0;
}

#define HAS_UPDATE(win) (win->root_frame && win->root_frame->flags & (NT_WIDGET_NEED_RECALC | NT_WIDGET_DIRTY))

void nt_loop() {
    while (1) {
        nt_timer_update();

        int expiration = nt_timer_next_expiration();
        if (expiration > 0) {
            expiration = expiration + 50;
            if (expiration < 0) expiration = 0;
        }

        NT_ITERATE_WINDOWS(win) {
            nt_platform_check_events(win);
            while (HAS_UPDATE(win)) nt_window_update(win);
        }

        nt_platform_wait_events(expiration);
        if (!nt_window_list()) break;
    }
}

void nt_loop_until_exited(nt_window_t *win) {
    while (win->exited == false) {
        // To avoid wasting CPU time this is necessary
        nt_timer_update();
        while (HAS_UPDATE(win)) {
            nt_window_update(win);
        }

        int expiration = nt_timer_next_expiration();
        nt_platform_wait_events(expiration);
        nt_platform_check_events(win);
    }

    free(win);
}
