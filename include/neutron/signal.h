/**
 * @file include/neutron/signal.h
 * @brief Neutron signal component
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_SIGNAL_H
#define _NEUTRON_SIGNAL_H

struct _nt_widget;
struct _nt_signal;

typedef void (*nt_signal_handler_cb_t)(struct _nt_widget *, struct _nt_signal*, void *);

typedef struct _nt_signal_handler {
    struct _nt_signal_handler *next;
    nt_signal_handler_cb_t handler;
    void *data;
} nt_signal_handler_t;

typedef struct _nt_signal {
    struct _nt_signal *next;
    char *signal_name;
    nt_signal_handler_t *head;
    void *signal_data;  // internal data passed by nt_signal_emit_data/nt_signal_emit_name_data
                        // !!! signal_data is hacky and not MT safe (not like anything else in this codebase is but)
} nt_signal_t;

#define NT_SIGNAL_NAME(s) (s)->signal_name
#define NT_SIGNAL_DATA(s) (s)->signal_data

void nt_signal_emit_data(struct _nt_widget *widget, nt_signal_t *signal, void *sig_data);
void nt_signal_emit(struct _nt_widget *widget, nt_signal_t *signal);
void nt_signal_emit_name_data(struct _nt_widget *widget, char *signal, void *sig_data);
void nt_signal_emit_name(struct _nt_widget *widget, char *signal);
nt_signal_handler_t *nt_signal_connect(struct _nt_widget *widget, char *signal, nt_signal_handler_cb_t handler, void *data);
nt_signal_handler_t *nt_signal_connect_existing(struct _nt_widget *widget, nt_signal_t *signal, nt_signal_handler_cb_t handler, void *data);
void nt_signal_disconnect(struct _nt_widget *widget, nt_signal_handler_t *handler);
void nt_signal_free(struct _nt_widget *widget);

#endif