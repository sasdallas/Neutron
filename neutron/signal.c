/**
 * @file neutron/signal.c
 * @brief Neutron signal system
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
#include <stdlib.h>


// TODO: Detailed signals, ordered signals, ...
void nt_signal_emit_data(struct _nt_widget *widget, nt_signal_t *signal, void *sig_data) {
    signal->signal_data = sig_data;

    nt_signal_handler_t *handler = signal->head;
    while (handler) {
        handler->handler(widget, signal, handler->data);
        handler = handler->next;
    }
}

void nt_signal_emit(nt_widget_t *widget, nt_signal_t *signal) {
    return nt_signal_emit_data(widget, signal, NULL);
}


void nt_signal_emit_name_data(struct _nt_widget *widget, char *signal, void *sig_data) {
    printf("nt_signal_emit_name %p %s\n", widget, signal);
    nt_signal_t *sig = widget->signal_head;
    while (sig) {
        // TODO: optimize
        if (!strcmp(sig->signal_name, signal)) {
            return nt_signal_emit_data(widget, sig, sig_data);
        }

        sig = sig->next;
    }

    // no signal handler available
}

void nt_signal_emit_name(struct _nt_widget *widget, char *signal) {
    return nt_signal_emit_name_data(widget, signal, NULL);
}

nt_signal_handler_t *nt_signal_connect_existing(struct _nt_widget *widget, nt_signal_t *signal, nt_signal_handler_cb_t handler, void *data) {
    nt_signal_handler_t *sighandler = malloc(sizeof(nt_signal_handler_t));
    sighandler->handler = handler;
    sighandler->data = data;
    sighandler->next = signal->head;
    signal->head = sighandler;
    return sighandler;
}

nt_signal_handler_t *nt_signal_connect(struct _nt_widget *widget, char *signal, nt_signal_handler_cb_t handler, void *data) {
    nt_signal_t *sig = widget->signal_head;
    while (sig) {
        if (!strcmp(sig->signal_name, signal)) {
            return nt_signal_connect_existing(widget, sig, handler, data);
        }
        sig = sig->next;
    }

    // Allocate a new signal
    nt_signal_t *new = malloc(sizeof(nt_signal_t));
    new->next = widget->signal_head;
    widget->signal_head = new;
    new->signal_name = strdup(signal);
    new->head = NULL;

    return nt_signal_connect_existing(widget, new, handler, data);
}

void nt_signal_disconnect(struct _nt_widget *widget, nt_signal_handler_t *handler) {
    NT_ERROR("nt_signal_disconnect is TODO\n");
    assert(0);
}

void nt_signal_free(struct _nt_widget *widget) {
    nt_signal_t *s = widget->signal_head;
    widget->signal_head = NULL;
    while (s) {
        nt_signal_t *nxt = s->next;
        if (s->signal_name) {
            free(s->signal_name);
        }

        free(s);
        s = nxt;
    }
}
