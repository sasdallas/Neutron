/**
 * @file neutron/widgets/textarea.c
 * @brief Text area widget
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

#define NT_TEXTAREA(w) ((nt_textarea_t *)(w))
#define NT_TEXTAREA_MARGIN_X 6
#define NT_TEXTAREA_MARGIN_Y 5

static size_t nt_textarea_line_start(const nt_textarea_t *textarea, size_t index) {
	const nt_input_context_t *ctx = textarea->input;
	if (index > ctx->length) {
		index = ctx->length;
	}

	while (index > 0 && ctx->buffer[index - 1] != '\n') {
		index--;
	}
	return index;
}

static size_t nt_textarea_line_end(const nt_textarea_t *textarea, size_t index) {
	const nt_input_context_t *ctx = textarea->input;
	if (index > ctx->length) {
		index = ctx->length;
	}

	while (index < ctx->length && ctx->buffer[index] != '\n') {
		index++;
	}
	return index;
}

static size_t nt_textarea_column_of_cursor(const nt_textarea_t *textarea) {
	size_t start = nt_textarea_line_start(textarea, textarea->input->cursor);
	return textarea->input->cursor - start;
}

static void nt_textarea_move_vertical(nt_textarea_t *textarea, int dir) {
	nt_input_context_t *ctx = textarea->input;
	size_t cur_start = nt_textarea_line_start(textarea, ctx->cursor);
	size_t cur_end = nt_textarea_line_end(textarea, ctx->cursor);

	if (dir < 0) {
		if (cur_start == 0) {
			nt_input_context_set_cursor(ctx, 0);
			return;
		}

		size_t prev_end = cur_start - 1;
		size_t prev_start = nt_textarea_line_start(textarea, prev_end);
		size_t prev_len = prev_end - prev_start;
		size_t col = textarea->preferred_col;
		if (col > prev_len) col = prev_len;
		nt_input_context_set_cursor(ctx, prev_start + col);
		return;
	}

	if (cur_end >= ctx->length) {
		nt_input_context_set_cursor(ctx, ctx->length);
		return;
	}

	size_t next_start = cur_end + 1;
	size_t next_end = nt_textarea_line_end(textarea, next_start);
	size_t next_len = next_end - next_start;
	size_t col = textarea->preferred_col;
	if (col > next_len) col = next_len;
	nt_input_context_set_cursor(ctx, next_start + col);
}

static bool nt_textarea_cursor_timer(nt_timer_t *timer, void *data) {
	nt_textarea_t *textarea = NT_TEXTAREA(data);
	textarea->cursor_visible = !textarea->cursor_visible;
	nt_widget_invalidate(&textarea->widget);
	return true;
}

static void nt_textarea_reset_blink(nt_textarea_t *textarea) {
	textarea->cursor_visible = true;
	if (textarea->timer) {
		nt_timer_reset(textarea->timer);
	}
}

void nt_textarea_calc_size(nt_widget_t *widget) {
	widget->size_data.min_width = 80;
	widget->size_data.min_height = 50;
	widget->size_data.pref_width = 80;
	widget->size_data.pref_height = 50;
}

void nt_textarea_adjust_size(nt_widget_t *widget) {
	(void)widget;
}

void nt_textarea_render(nt_widget_t *widget, nt_render_surface_t *surface) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	const nt_input_context_t *ctx = textarea->input;

	int width = nt_widget_get_width_inner(widget);
	int height = nt_widget_get_height_inner(widget);

	nt_rect_t outer = NT_RECT(0, 0, width, height);
	nt_render_rounded_rect_gradient(surface, &outer, 4, NT_COLOR(0xe2, 0xe3, 0xea, 0xff), NT_COLOR(0xdb, 0xdf, 0xe6, 0xff), false);
	nt_render_rounded_rect(surface, &NT_RECT(1, 1, width - 2, height - 2), 4, NT_COLOR(255, 255, 255, 255));
	nt_render_fill_rect(surface, &NT_RECT(2, 0, width - 4, 1), NT_COLOR(0xab, 0xad, 0xa3, 0xff));

	size_t line_h = 0;
	size_t ignored = 0;
	nt_render_text_dimensions(widget->style.font, "Mg", &ignored, &line_h);
	if (line_h == 0) line_h = 12;

	int draw_x = NT_TEXTAREA_MARGIN_X;
	int draw_y = NT_TEXTAREA_MARGIN_Y;

	size_t line_start = 0;
	while (line_start <= ctx->length) {
		size_t line_end = line_start;
		while (line_end < ctx->length && ctx->buffer[line_end] != '\n') {
			line_end++;
		}

		size_t line_len = line_end - line_start;
		char line[1024];
		const char *to_draw = "";
		if (line_len > 0) {
			size_t n = (line_len >= sizeof(line)) ? sizeof(line) - 1 : line_len;
			memcpy(line, ctx->buffer + line_start, n);
			line[n] = '\0';
			to_draw = line;
		}

		if (draw_y + (int)line_h > height - NT_TEXTAREA_MARGIN_Y) {
			break;
		}

		nt_render_draw_text(surface, widget->style.font, draw_x, draw_y, to_draw, NT_COLOR(0, 0, 0, 255));

		if (widget->focused && textarea->cursor_visible && ctx->cursor >= line_start && ctx->cursor <= line_end) {
			size_t cursor_cols = ctx->cursor - line_start;
			char prefix[1024];
			size_t prefix_n = (cursor_cols >= sizeof(prefix)) ? sizeof(prefix) - 1 : cursor_cols;
			memcpy(prefix, ctx->buffer + line_start, prefix_n);
			prefix[prefix_n] = '\0';

			size_t cursor_w = 0;
			size_t cursor_h = 0;
			nt_render_text_dimensions(widget->style.font, prefix, &cursor_w, &cursor_h);
			int cursor_x = draw_x + (int)cursor_w;
			nt_render_fill_rect(surface, &NT_RECT(cursor_x, draw_y, 1, (int)line_h), NT_COLOR(0, 0, 0, 255));
		}

		draw_y += (int)line_h;

		if (line_end >= ctx->length) {
			break;
		}

		line_start = line_end + 1;
	}
}

static bool nt_textarea_focus(nt_widget_t *widget, nt_event_t *event) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	if (event->type == NT_EVENT_FOCUS_EXIT) {
		nt_timer_set_enabled(textarea->timer, false);
	} else {
		nt_timer_set_enabled(textarea->timer, true);
		textarea->cursor_visible = true;
	}

	nt_widget_invalidate(widget);
	return true;
}

static size_t nt_textarea_position_from_click(nt_textarea_t *textarea, int click_x, int click_y) {
	const nt_input_context_t *ctx = textarea->input;

	click_x = nt_event_to_rel_x(&textarea->widget, click_x);
	click_y = nt_event_to_rel_y(&textarea->widget, click_y);

	int width = nt_widget_get_width_inner(&textarea->widget);
	int height = nt_widget_get_height_inner(&textarea->widget);

	size_t line_h = 0;
	size_t ignored = 0;
	nt_render_text_dimensions(textarea->widget.style.font, "Mg", &ignored, &line_h);
	if (line_h == 0) line_h = 12;

	int draw_x = NT_TEXTAREA_MARGIN_X;
	int draw_y = NT_TEXTAREA_MARGIN_Y;

	// Clamp click coordinates to content area
	if (click_x < draw_x) click_x = draw_x;
	if (click_x > width - NT_TEXTAREA_MARGIN_X) click_x = width - NT_TEXTAREA_MARGIN_X;

	size_t line_start = 0;
	while (line_start <= ctx->length) {
		size_t line_end = line_start;
		while (line_end < ctx->length && ctx->buffer[line_end] != '\n') {
			line_end++;
		}

		int line_bottom = draw_y + (int)line_h;
		if (click_y < line_bottom && click_y >= draw_y) {
			// Click is on this line, find the column
			size_t line_len = line_end - line_start;
			for (size_t col = 0; col <= line_len; col++) {
				char prefix[1024];
				size_t prefix_n = (col >= sizeof(prefix)) ? sizeof(prefix) - 1 : col;
				if (prefix_n > 0) {
					memcpy(prefix, ctx->buffer + line_start, prefix_n);
				}
				prefix[prefix_n] = '\0';

				size_t text_w = 0;
				size_t text_h = 0;
				nt_render_text_dimensions(textarea->widget.style.font, prefix, &text_w, &text_h);
				int col_x = draw_x + (int)text_w;

				// Check if click is before this column
				if (click_x <= col_x) {
					return line_start + col;
				}
			}
			// Click is past end of line
			return line_end;
		}

		draw_y = line_bottom;

		if (line_end >= ctx->length) {
			break;
		}

		line_start = line_end + 1;
	}

	// Click is below all text, return end of buffer
	return ctx->length;
}

static bool nt_textarea_mouse_down(nt_widget_t *widget, nt_event_t *event) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	if (event->pointer.buttons & NT_MOUSE_BUTTON_LEFT) {
		size_t new_pos = nt_textarea_position_from_click(textarea, event->pointer.x, event->pointer.y);
		nt_input_context_set_cursor(textarea->input, new_pos);
		textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
		nt_textarea_reset_blink(textarea);
		nt_widget_invalidate(widget);
	}

	return true;
}

static bool nt_textarea_key_down(nt_widget_t *widget, nt_event_t *event) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	nt_input_context_t *ctx = textarea->input;
	bool changed = false;
	bool moved = false;

	switch (event->key.key) {
		case NT_KEY_LEFT:
			if (ctx->cursor > 0) {
				nt_input_context_set_cursor(ctx, ctx->cursor - 1);
				moved = true;
			}
			textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
			break;

		case NT_KEY_RIGHT:
			if (ctx->cursor < ctx->length) {
				nt_input_context_set_cursor(ctx, ctx->cursor + 1);
				moved = true;
			}
			textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
			break;

		case NT_KEY_UP:
			textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
			nt_textarea_move_vertical(textarea, -1);
			moved = true;
			break;

		case NT_KEY_DOWN:
			textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
			nt_textarea_move_vertical(textarea, 1);
			moved = true;
			break;

		case NT_KEY_HOME:
			nt_input_context_set_cursor(ctx, nt_textarea_line_start(textarea, ctx->cursor));
			textarea->preferred_col = 0;
			moved = true;
			break;

		case NT_KEY_END:
			nt_input_context_set_cursor(ctx, nt_textarea_line_end(textarea, ctx->cursor));
			textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
			moved = true;
			break;

		case NT_KEY_BACKSPACE:
			if (textarea->editable && ctx->cursor > 0) {
				nt_input_context_process(ctx, event);
				changed = true;
			}
			break;

		case NT_KEY_DELETE:
			if (textarea->editable && ctx->cursor < ctx->length) {
				nt_input_context_delete_range(ctx, ctx->cursor, ctx->cursor + 1);
				changed = true;
			}
			break;

		case NT_KEY_ENTER:
		case NT_KEY_KEYPADENTER:
			if (textarea->editable) {
				nt_input_context_insert_text(ctx, "\n", 1);
				changed = true;
			}
			break;

		case NT_KEY_TAB:
			if (textarea->editable) {
				nt_input_context_insert_text(ctx, "\t", 1);
				changed = true;
			}
			break;

		default: {
			if (textarea->editable && event->key.utf[0]) {
				size_t utf_len = strlen(event->key.utf);
				if (utf_len > 0) {
					nt_input_context_insert_text(ctx, event->key.utf, utf_len);
					changed = true;
				}
			}
			break;
		}
	}

	if (changed || moved) {
		textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
		nt_textarea_reset_blink(textarea);
		nt_widget_invalidate(widget);
	}

	return true;
}

static bool nt_textarea_mouse_enter(nt_widget_t *widget, nt_event_t *event) {
    nt_platform_set_cursor(nt_widget_get_window(widget), NT_CURSOR_TEXT);
    return true;
}

static bool nt_textarea_mouse_leave(nt_widget_t *widget, nt_event_t *event) {
    nt_platform_set_cursor(nt_widget_get_window(widget), NT_CURSOR_ARROW);
    
    return true;
}

static nt_widget_vtable_t textarea_vtable = {
	.class_size = sizeof(nt_textarea_t),
	.init = NULL,
	.free = NULL,
	.render = nt_textarea_render,
	.calc_size = nt_textarea_calc_size,
	.adjust_size = nt_textarea_adjust_size,
};

nt_widget_t *nt_textarea_create() {
	nt_widget_t *widget = nt_widget_create(NT_WIDGET_TEXTAREA, &textarea_vtable);
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	nt_widget_set_focusable(widget, true);

	textarea->input = nt_input_context_create();
	textarea->preferred_col = 0;
	textarea->editable = true;
	textarea->cursor_visible = true;
	textarea->timer = nt_timer_create(600, nt_textarea_cursor_timer, textarea, false);

	nt_event_set_handler(widget, NT_EVENT_KEY_DOWN, nt_textarea_key_down);
	nt_event_set_handler(widget, NT_EVENT_MOUSE_DOWN, nt_textarea_mouse_down);
	nt_event_set_handler(widget, NT_EVENT_FOCUS_ENTER, nt_textarea_focus);
	nt_event_set_handler(widget, NT_EVENT_FOCUS_EXIT, nt_textarea_focus);

	nt_event_set_handler(widget, NT_EVENT_MOUSE_ENTER, nt_textarea_mouse_enter);
	nt_event_set_handler(widget, NT_EVENT_MOUSE_LEAVE, nt_textarea_mouse_leave);
	return widget;
}

void nt_textarea_set_text(nt_widget_t *widget, const char *text) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	if (!text) text = "";

	size_t len = strlen(text);
	nt_input_context_ensure_capacity(textarea->input, len + 1);
	memcpy(textarea->input->buffer, text, len + 1);
	textarea->input->length = len;
	nt_input_context_set_cursor(textarea->input, 0);

	textarea->preferred_col = nt_textarea_column_of_cursor(textarea);
	nt_widget_invalidate(widget);
}

const char *nt_textarea_get_text(nt_widget_t *widget) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	return textarea->input->buffer;
}

void nt_textarea_append_text(nt_widget_t *widget, const char *text) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	if (!text || !text[0]) {
		return;
	}

	size_t old_cursor = textarea->input->cursor;
	nt_input_context_set_cursor(textarea->input, textarea->input->length);
	nt_input_context_insert_text(textarea->input, text, strlen(text));
	nt_input_context_set_cursor(textarea->input, old_cursor);
	nt_widget_invalidate(widget);
}

void nt_textarea_set_editable(nt_widget_t *widget, bool editable) {
	nt_textarea_t *textarea = NT_TEXTAREA(widget);
	textarea->editable = editable;
}