/**
 * @file include/neutron/neutron.h
 * @brief Neutron UI framework
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_H
#define _NEUTRON_H

#include <neutron/widget.h>
#include <neutron/style.h>
#include <neutron/platform.h>
#include <neutron/window.h>
#include <neutron/misc.h>
#include <neutron/render.h>
#include <neutron/event.h>
#include <neutron/signal.h>
#include <neutron/timer.h>
#include <neutron/input.h>
#include <neutron/keycodes.h>
#include <neutron/dialog.h>
#include <neutron/icon.h>
#include <neutron/widgets/label.h>
#include <neutron/widgets/box.h>
#include <neutron/widgets/grid.h>
#include <neutron/widgets/button.h>
#include <neutron/widgets/scroll_container.h>
#include <neutron/widgets/input.h>
#include <neutron/widgets/textarea.h>
#include <neutron/widgets/menubar.h>
#include <neutron/widgets/menu.h>
#include <neutron/widgets/menu_item.h>
#include <neutron/widgets/input.h>
#include <neutron/widgets/tab_view.h>
#include <neutron/widgets/tab.h>
#include <neutron/widgets/image.h>
#include <neutron/widgets/list_view.h>
#include <neutron/widgets/column_view.h>
#include <neutron/widgets/checkbox.h>
#include <neutron/widgets/icon_view.h>
#include <neutron/widgets/separator.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NT_INFO(fmt, ...) ({ fprintf(stderr, "neutron: %s:%03d: \033[34minfo:\033[0m " fmt, __FILE__, __LINE__, ## __VA_ARGS__); })
#define NT_WARN(fmt, ...) ({ fprintf(stderr, "neutron: %s:%03d: \033[1;33mwarning:\033[0m " fmt, __FILE__, __LINE__, ## __VA_ARGS__); })
#define NT_ERROR(fmt, ...) ({ fprintf(stderr, "neutron: %s:%03d: \033[1;31merror:\033[0m " fmt, __FILE__, __LINE__, ## __VA_ARGS__); })
#define NT_DEBUG(fmt, ...) ({ fprintf(stderr, "neutron: %s:%03d: \033[1;37mdebug:\033[0m " fmt, __FILE__, __LINE__, ## __VA_ARGS__); })

int nt_init();
void nt_loop();
void nt_loop_until_exited(nt_window_t *win);

#endif