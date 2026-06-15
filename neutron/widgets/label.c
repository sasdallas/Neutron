/**
 * @file neutron/widgets/label.c
 * @brief Label widget
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

static void nt_label_init(nt_widget_t *widget) { }
static void nt_label_free(nt_widget_t *widget) {
    nt_label_t *label = (nt_label_t*)widget;
    free(label->text);
}

static void nt_label_calc_size(nt_widget_t *widget) {
    nt_label_t *label = (nt_label_t*)widget;

    size_t w;
    size_t h;
    nt_render_text_dimensions(widget->style.font, label->text, &w, &h);

    widget->size_data.pref_width = w;
    widget->size_data.pref_height = h;
    widget->size_data.min_width = w;
    widget->size_data.min_height = h;
}

static void nt_label_render(nt_widget_t *widget, nt_render_surface_t *surface) {
    nt_label_t *label = (nt_label_t*)widget;

    nt_render_draw_text(surface, widget->style.font,
        widget->style.padding[LEFT],
        widget->style.padding[TOP],
        label->text, NT_COLOR_INVERT_IF_SELECTED(widget, widget->style.fg));
}

nt_widget_vtable_t label_vtable = {
    .class_size = sizeof(nt_label_t),
    .init = nt_label_init,
    .free = nt_label_free,
    .calc_size = nt_label_calc_size,
    .adjust_size = NULL,
    .render = nt_label_render,
};

nt_widget_t *nt_label_create(char *text) {
    nt_widget_t *wdgt = nt_widget_create(NT_WIDGET_LABEL, &label_vtable);
    nt_widget_set_focusable(wdgt, false);
    nt_widget_set_selectable(wdgt, true);
    assert(wdgt);
    
    nt_label_t *lbl = (nt_label_t*)wdgt;
    lbl->text = strdup(text);

    return wdgt;
}

void nt_label_set_text(nt_widget_t *w, char *text) {
    nt_label_t *lbl = (nt_label_t*)w;
    char *old = lbl->text;
    lbl->text = strdup(text);
    free(old);
    nt_widget_mark_recalc(w);
}

char *nt_label_get_text(nt_widget_t *w) {
    nt_label_t *lbl = (nt_label_t*)w;
    return lbl->text;
}
