#include <neutron/neutron.h>
#include <stdio.h>
#include <stdlib.h>

nt_widget_t *status;
nt_widget_t *click_count_label;
int click_count;

static nt_widget_t *demo_label(char *text) {
    nt_widget_t *label = nt_label_create(text);
    nt_style_set_bg_color(&label->style, NT_COLOR(0, 0, 0, 0));
    return label;
}

static nt_widget_t *demo_section(void) {
    nt_widget_t *box = nt_box_create_vertical();
    nt_widget_set_expansion(box, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_style_set_margin_all(&box->style, 12);
    nt_box_set_spacing(box, 10);
    return box;
}

static nt_widget_t *demo_row(void) {
    nt_widget_t *box = nt_box_create_horizontal();
    nt_widget_set_expansion(box, NT_EXPAND_HORIZONTAL);
    nt_box_set_spacing(box, 8);
    return box;
}

static void set_status(char *message) {
    if (status) {
        nt_label_set_text(status, message);
    }
}

static void on_primary_pressed(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    (void)widget;
    (void)signal;
    (void)data;

    click_count++;

    char text[64];
    snprintf(text, sizeof(text), "Pressed %d time%s", click_count, click_count == 1 ? "" : "s");
    nt_label_set_text(click_count_label, text);
    set_status("Primary button pressed");
}

static void on_kill_pressed(nt_widget_t *w, nt_signal_t *s, void *data) {
    nt_window_close(nt_widget_get_window(w));
}

static void on_secondary_pressed(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    (void)widget;
    (void)signal;
    (void)data;

    set_status("Secondary action selected");


    nt_window_t *win = nt_window_create_child(nt_widget_get_window(widget), "Test", 250, 100);
    nt_widget_t *box = nt_box_create_vertical();

    nt_widget_t *lbl = nt_label_create("Dummy dialog :D");
    nt_widget_set_expansion(lbl, NT_EXPAND_VERTICAL | NT_EXPAND_HORIZONTAL);
    nt_box_append(box, lbl);
    nt_widget_set_expansion(box, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);

    nt_widget_t *button_box = nt_box_create_horizontal();
    nt_widget_set_expansion(button_box, NT_EXPAND_HORIZONTAL);
    nt_widget_t *btn1 = nt_button_create_label("Yes");
    nt_widget_t *btn2 = nt_button_create_label("No");
    
    nt_style_set_font(&lbl->style, NT_SANS_12);
    nt_widget_set_expansion(btn1, NT_EXPAND_HORIZONTAL);
    nt_widget_set_horizontal_alignment(btn1, NT_ALIGN_RIGHT);
    nt_box_append(button_box, btn1);
    nt_box_append(button_box, btn2);
    nt_box_append(box, button_box);

    nt_signal_connect(btn1, "pressed", on_kill_pressed, NULL);
    nt_window_set_root(win, box);
    nt_loop_until_exited(win);
}

static void on_menu_pressed(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    (void)widget;
    (void)signal;

    set_status((char *)data);
}

static void on_clear_textarea(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    (void)widget;
    (void)signal;

    nt_textarea_set_text((nt_widget_t *)data, "");
    set_status("Textarea cleared from context menu");
}

static void on_textarea_changed(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    (void)widget;
    (void)signal;
    (void)data;

    set_status("Textarea content changed");
}

static nt_widget_t *create_controls_tab(void) {
    nt_widget_t *content = demo_section();

    nt_widget_t *row = demo_row();
    nt_box_append(content, row);

    nt_widget_t *primary = nt_button_create_label("Primary action");
    nt_widget_t *secondary = nt_button_create_label("Secondary action");
    nt_signal_connect(primary, "pressed", on_primary_pressed, NULL);
    nt_signal_connect(secondary, "pressed", on_secondary_pressed, NULL);
    nt_box_append(row, primary);
    nt_box_append(row, secondary);

    nt_widget_t *cb = nt_checkbox_create_labelled("Test checkbox", true);
    nt_box_append(content, cb);

    click_count_label = demo_label("Pressed 0 times");
    nt_box_append(content, click_count_label);

    nt_widget_t *note = demo_label("I'm just a normal label");
    nt_box_append(content, note);

    return content;
}

static nt_widget_t *create_inputs_tab(void) {
    nt_widget_t *content = demo_section();

    nt_box_append(content, demo_label("Text entry"));

    nt_widget_t *name_row = demo_row();
    nt_box_append(name_row, demo_label("Name"));

    nt_widget_t *name_input = nt_input_create("Type a name");
    nt_widget_set_expansion(name_input, NT_EXPAND_HORIZONTAL);
    nt_input_set_max_chars(name_input, 48);
    nt_box_append(name_row, name_input);
    nt_box_append(content, name_row);

    nt_widget_t *readonly_row = demo_row();
    nt_box_append(readonly_row, demo_label("Read only"));

    nt_widget_t *readonly = nt_input_create("Focusable input disabled");
    nt_widget_set_expansion(readonly, NT_EXPAND_HORIZONTAL);
    nt_input_set_read_only(readonly, true);
    nt_box_append(readonly_row, readonly);
    nt_box_append(content, readonly_row);

    nt_widget_t *textarea = nt_textarea_create();
    nt_widget_set_expansion(textarea, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_textarea_set_editable(textarea, true);
    nt_textarea_set_text(textarea,
        "Neutron textarea demo\n\n"
        "Click here and type. Right click for a small context menu.");
    nt_signal_connect(textarea, "changed", on_textarea_changed, NULL);
    nt_box_append(content, textarea);

    nt_widget_t *context_menu = nt_menu_create();
    nt_widget_t *clear_item = nt_menu_item_create_button("Clear Text");
    nt_menu_add_item(context_menu, clear_item);
    nt_signal_connect(clear_item, "pressed", on_clear_textarea, textarea);
    nt_widget_set_context_menu(textarea, context_menu);

    return content;
}

static nt_widget_t *create_layout_tab(void) {
    nt_widget_t *content = demo_section();

    nt_box_append(content, demo_label("Grid layout"));

    nt_widget_t *grid = nt_grid_create(3, 3);
    nt_widget_set_expansion(grid, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_grid_set_column_spacing(grid, 8);
    nt_grid_set_row_spacing(grid, 8);
    nt_box_append(content, grid);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            char text[32];
            snprintf(text, sizeof(text), "Cell %d,%d", x + 1, y + 1);

            nt_widget_t *button = nt_button_create_label(text);
            nt_widget_set_expansion(button, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
            nt_grid_set(grid, x, y, button);
        }
    }

    return content;
}

static nt_widget_t *create_menus_tab(void) {
    nt_widget_t *content = demo_section();

    nt_box_append(content, demo_label("Menu widgets"));

    nt_widget_t *menubar = nt_menubar_create();
    nt_box_append(content, menubar);

    nt_widget_t *file_menu = nt_menu_create();
    nt_widget_t *new_item = nt_menu_item_create_button("New");
    nt_widget_t *open_item = nt_menu_item_create_button("Open");
    nt_widget_t *save_item = nt_menu_item_create_button("Save");
    nt_widget_t *separator = nt_menu_item_create_separator();
    nt_widget_t *quit_item = nt_menu_item_create_button("Quit");

    nt_menu_add_item(file_menu, new_item);
    nt_menu_add_item(file_menu, open_item);
    nt_menu_add_item(file_menu, save_item);
    nt_menu_add_item(file_menu, separator);
    nt_menu_add_item(file_menu, quit_item);
    nt_menubar_append(menubar, "File", file_menu);

    nt_signal_connect(new_item, "pressed", on_menu_pressed, "New selected");
    nt_signal_connect(open_item, "pressed", on_menu_pressed, "Open selected");
    nt_signal_connect(save_item, "pressed", on_menu_pressed, "Save selected");
    nt_signal_connect(quit_item, "pressed", on_menu_pressed, "Quit selected");

    nt_widget_t *edit_menu = nt_menu_create();
    nt_widget_t *cut_item = nt_menu_item_create_button("Cut");
    nt_widget_t *copy_item = nt_menu_item_create_button("Copy");
    nt_widget_t *paste_item = nt_menu_item_create_button("Paste");

    nt_menu_add_item(edit_menu, cut_item);
    nt_menu_add_item(edit_menu, copy_item);
    nt_menu_add_item(edit_menu, paste_item);
    nt_menubar_append(menubar, "Edit", edit_menu);

    nt_signal_connect(cut_item, "pressed", on_menu_pressed, "Cut selected");
    nt_signal_connect(copy_item, "pressed", on_menu_pressed, "Copy selected");
    nt_signal_connect(paste_item, "pressed", on_menu_pressed, "Paste selected");

    nt_widget_t *summary = demo_label("Use the menubar above, or right click this label for a context menu.");
    nt_box_append(content, summary);

    nt_widget_t *context_menu = nt_menu_create();
    nt_widget_t *inspect_item = nt_menu_item_create_button("Inspect");
    nt_widget_t *refresh_item = nt_menu_item_create_button("Refresh");
    nt_menu_add_item(context_menu, inspect_item);
    nt_menu_add_item(context_menu, refresh_item);
    nt_signal_connect(inspect_item, "pressed", on_menu_pressed, "Inspect selected");
    nt_signal_connect(refresh_item, "pressed", on_menu_pressed, "Refresh selected");
    nt_widget_set_context_menu(summary, context_menu);

    return content;
}


static nt_widget_t *create_list_view_tab() {
    nt_widget_t *content = demo_section();
    nt_box_append(content, demo_label("List view"));

    nt_widget_t *list_view = nt_list_view_create();
    nt_widget_set_expansion(list_view, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_list_view_append(list_view, demo_label("list view item 1"));
    nt_list_view_append(list_view, demo_label("list view item 2"));
    nt_list_view_append(list_view, demo_label("list view item 3"));
    nt_list_view_append(list_view, demo_label("list view item 4"));

    nt_box_append(content, list_view);

    return content;
}

void on_kill_user(nt_widget_t *widget, nt_signal_t *signal, void *data) {
    nt_window_t *win = nt_window_create_child(nt_widget_get_window(widget), "Test", 250, 100);
    nt_widget_t *box = nt_box_create_vertical();

    nt_widget_t *lbl = nt_label_create("Are you sure you want to do this????");
    nt_widget_set_expansion(lbl, NT_EXPAND_VERTICAL | NT_EXPAND_HORIZONTAL);
    nt_box_append(box, lbl);
    nt_widget_set_expansion(box, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);

    nt_widget_t *button_box = nt_box_create_horizontal();
    nt_widget_set_expansion(button_box, NT_EXPAND_HORIZONTAL);
    nt_widget_t *btn1 = nt_button_create_label("Yes");
    nt_widget_t *btn2 = nt_button_create_label("No");
    
    nt_style_set_font(&lbl->style, NT_SANS_12);
    nt_widget_set_expansion(btn1, NT_EXPAND_HORIZONTAL);
    nt_widget_set_horizontal_alignment(btn1, NT_ALIGN_RIGHT);
    nt_box_append(button_box, btn1);
    nt_box_append(button_box, btn2);
    nt_box_append(box, button_box);

    nt_signal_connect(btn1, "pressed", on_kill_pressed, NULL);
    nt_window_set_root(win, box);
    nt_loop_until_exited(win);
}

static nt_widget_t *create_column_view_tab() {
    nt_widget_t *content = demo_section();
    nt_box_append(content, demo_label("Column view"));

    nt_widget_t *column_view = nt_column_view_create();
    
    nt_style_set_padding_all(&column_view->style, 2);
    nt_style_set_border_thickness(&column_view->style, 1);
    nt_style_set_border_color(&column_view->style, NT_COLOR(170,170,170,255));
    nt_style_set_border_gradient(&column_view->style, NT_BORDER_GRADIENT_NONE);
    nt_style_set_border_rounded(&column_view->style, 2);
    
    nt_widget_set_expansion(column_view, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_column_view_append_column(column_view, nt_column_create("First Name", NT_DEFAULT_FACTORY_TEXT, NULL));
    nt_column_view_append_column(column_view, nt_column_create("Last Name", NT_DEFAULT_FACTORY_TEXT, NULL));
    nt_column_view_append_column(column_view, nt_column_create("Uses Ethereal", NT_DEFAULT_FACTORY_TEXT, NULL));

    nt_widget_t *context_menu = nt_menu_create();
    nt_widget_t *kill_user = nt_menu_item_create_button("Execute User");
    nt_menu_add_item(context_menu, kill_user);
    nt_signal_connect(kill_user, "pressed", on_kill_user, NULL);


    NT_CREATE_ROW_DATA(row1, 3);
        NT_ADD_ROW_DATA(row1, "shmuel");
        NT_ADD_ROW_DATA(row1, "start");
        NT_ADD_ROW_DATA(row1, "yes");
        nt_widget_t *row1_wdgt = nt_column_view_add_row(column_view, row1);
        nt_widget_set_context_menu(row1_wdgt, context_menu);
    NT_FINISH_ROW_DATA(row1)

    NT_CREATE_ROW_DATA(row2, 3);
        NT_ADD_ROW_DATA(row2, "bartholomew");
        NT_ADD_ROW_DATA(row2, "longassname the third");
        NT_ADD_ROW_DATA(row2, "no");
        nt_widget_t *row2_wdgt = nt_column_view_add_row(column_view, row2);
        nt_widget_set_context_menu(row2_wdgt, context_menu);
    NT_FINISH_ROW_DATA(row2)

    nt_box_append(content, column_view);
    return content;
}

nt_image_t *img;

static nt_widget_t *create_icon_view_tab() {
    nt_widget_t *content = demo_section();
    nt_box_append(content, demo_label("Icon view"));

    nt_widget_t *icon_view = nt_icon_view_create();

    nt_widget_t *scroll = nt_scroll_container_create();
    nt_widget_set_expansion(scroll, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_scroll_container_set_child(scroll, icon_view);
    nt_style_set_margin_all(&icon_view->style, 0);
    nt_widget_set_expansion(icon_view, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);

    img = nt_icon_get("ethereal", NULL, 32);

    nt_icon_store_t *store = nt_icon_store_create();
    nt_icon_store_append(store, "item 1", img, NULL);
    nt_icon_store_append(store, "item 2", img, NULL);
    nt_icon_store_append(store, "item 3", img, NULL);
    nt_icon_store_append(store, "item 4", img, NULL);
    nt_icon_store_append(store, "item 5", img, NULL);
    nt_icon_store_append(store, "item 6", img, NULL);

    nt_icon_view_set_store(icon_view, store);

    nt_box_append(content, scroll);
    return content;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (nt_init() < 0) {
        printf("nt_init failed\n");
        return 1;
    }

    nt_window_t *window = nt_window_create("Neutron Widget Demo", 820, 560);
    if (!window) {
        printf("Failed to create window\n");
        return 1;
    }

    nt_widget_t *root = nt_box_create_vertical();
    nt_widget_set_expansion(root, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_window_set_root(window, root);

    nt_widget_t *header = demo_label("Neutron Widget Framework Demo");
    nt_style_set_margin_all(&header->style, 8);
    nt_box_append(root, header);

    nt_widget_t *tabs = nt_tab_view_create();
    nt_widget_set_expansion(tabs, NT_EXPAND_HORIZONTAL | NT_EXPAND_VERTICAL);
    nt_box_append(root, tabs);

    nt_tab_view_append(tabs, nt_tab_create_label("Controls", create_controls_tab()));
    nt_tab_view_append(tabs, nt_tab_create_label("Inputs", create_inputs_tab()));
    nt_tab_view_append(tabs, nt_tab_create_label("Layout", create_layout_tab()));
    nt_tab_view_append(tabs, nt_tab_create_label("Menus", create_menus_tab()));;
    nt_tab_view_append(tabs, nt_tab_create_label("List", create_list_view_tab()));
    nt_tab_view_append(tabs, nt_tab_create_label("Columns", create_column_view_tab()));
    nt_tab_view_append(tabs, nt_tab_create_label("Icon", create_icon_view_tab()));

    status = demo_label("Ready");
    nt_style_set_margin_all(&status->style, 6);
    nt_box_append(root, status);

    nt_loop();
    return 0;
}
