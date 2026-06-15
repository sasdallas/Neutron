/**
 * @file neutron/platform/linux.c
 * @brief Linux platform for Neutron
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#if defined(BUILDING_LINUX)

#include <neutron/neutron.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

bool nt_platform_xkeyevent_to_event(XKeyEvent *xkey, bool pressed, nt_event_t *event);

typedef struct {
    Window window;
    GC gc;
    Pixmap pixmap;
    Visual *visual;
    Picture picture;
} x11_window_t;

Display *display;
Visual *visual;
Colormap colormap;
XVisualInfo visual_info;
int screen;

/* X atoms */
Atom wmDelete;

int nt_platform_init() {
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    visual = DefaultVisual(display, screen);
    colormap = DefaultColormap(display, screen);

    wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    return 0;
}

int nt_platform_create_window(nt_window_t *window) {
    x11_window_t *xwin = malloc(sizeof(x11_window_t));

    if (!XMatchVisualInfo(display, screen, 24, TrueColor, &visual_info)) {
        NT_WARN("XMatchVisualInfo failed, using defaults\n");

        visual_info.visual = DefaultVisual(display, screen);
        visual_info.depth = DefaultDepth(display, screen);
    }

    xwin->visual = visual_info.visual;

    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), visual_info.visual, AllocNone);
    attr.event_mask = StructureNotifyMask;
    Window w = XCreateWindow(display, DefaultRootWindow(display), 0, 0, window->width, window->height, 0, visual_info.depth, 0, visual_info.visual, CWBackPixel | CWColormap | CWEventMask, &attr);
    XSelectInput(display, w, ExposureMask | StructureNotifyMask | ButtonMotionMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask );
    XMapWindow(display, w);

    XSetWMProtocols(display, w, &wmDelete, 1);
    XStoreName(display, w, window->title);

    xwin->window = w;
    XGCValues gcvalues;
    xwin->gc = XCreateGC(display, xwin->window, 0, &gcvalues);
    xwin->pixmap = XCreatePixmap(display, w, window->width, window->height, visual_info.depth);
    XRenderPictFormat *fmt_win = XRenderFindVisualFormat(display, xwin->visual);
    xwin->picture = XRenderCreatePicture(display, xwin->pixmap, fmt_win, 0, NULL);

    window->platform = xwin;

extern void nt_platform_x11_input_init(Window window);
    nt_platform_x11_input_init(w);
    return 0;
}


static void nt_platform_remove_decorations(x11_window_t *xwin) {
    Atom hints_atom = XInternAtom(display, "_MOTIF_WM_HINTS", True);
    if (hints_atom == None) return;

typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
} MotifWmHints;

    MotifWmHints hints;
    hints.flags |= (1 << 1); // remove decorations
    hints.decorations = 0;

    XChangeProperty(display, xwin->window, hints_atom, hints_atom, 32, PropModeReplace, (unsigned char *)&hints, 5);
}

int nt_platform_create_window_undecorated(struct _nt_window *window) {
    x11_window_t *xwin = malloc(sizeof(x11_window_t));

    if (!XMatchVisualInfo(display, screen, 24, TrueColor, &visual_info)) {
        NT_WARN("XMatchVisualInfo failed, using defaults\n");

        visual_info.visual = DefaultVisual(display, screen);
        visual_info.depth = DefaultDepth(display, screen);
    }

    xwin->visual = visual_info.visual;

    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), visual_info.visual, AllocNone);
    attr.event_mask = StructureNotifyMask;
    Window w = XCreateWindow(display, DefaultRootWindow(display), 0, 0, window->width, window->height, 0, visual_info.depth, 0, visual_info.visual, CWBackPixel | CWColormap | CWEventMask, &attr);
    
    xwin->window = w;
    nt_platform_remove_decorations(xwin);

    XSelectInput(display, w, ExposureMask | StructureNotifyMask | ButtonMotionMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask );
    XMapWindow(display, w);

    XSetWMProtocols(display, w, &wmDelete, 1);

    XGCValues gcvalues;
    xwin->gc = XCreateGC(display, xwin->window, 0, &gcvalues);
    xwin->pixmap = XCreatePixmap(display, w, window->width, window->height, visual_info.depth);
    XRenderPictFormat *fmt_win = XRenderFindVisualFormat(display, xwin->visual);
    xwin->picture = XRenderCreatePicture(display, xwin->pixmap, fmt_win, 0, NULL);

    window->platform = xwin;

extern void nt_platform_x11_input_init(Window window);
    nt_platform_x11_input_init(w);
    return 0;
}

int nt_platform_create_child_window(nt_window_t *window) {
    x11_window_t *xwinparent = window->parent->platform;

    x11_window_t *xwin = malloc(sizeof(x11_window_t));
    if (!xwin) return -1;

    memset(xwin, 0, sizeof(x11_window_t));

    xwin->visual = xwinparent->visual;

    XSetWindowAttributes attr = {0};
    attr.background_pixel = BlackPixel(display, DefaultScreen(display));
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), visual_info.visual, AllocNone);
    attr.event_mask = ExposureMask | StructureNotifyMask | ButtonMotionMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask;

    Window w = XCreateWindow(display, DefaultRootWindow(display), 0, 00, window->width, window->height, 0, visual_info.depth, InputOutput, visual_info.visual, CWBackPixel | CWColormap | CWEventMask, &attr);

    XSetTransientForHint(display, w, xwinparent->window);
    XSetWMProtocols(display, w, &wmDelete, 1);
    XStoreName(display, w, window->title);
    XMapRaised(display, w);

    xwin->window = w;
    XGCValues gcvalues = {0};
    xwin->gc = XCreateGC(display, w, 0, &gcvalues);
    xwin->pixmap = XCreatePixmap(display, w, window->width, window->height, visual_info.depth);
    XRenderPictFormat *fmt = XRenderFindVisualFormat(display, xwin->visual);
    xwin->picture = XRenderCreatePicture(display, xwin->pixmap, fmt, 0, NULL);

    window->platform = xwin;

    return 0;
}

void nt_platform_get_window_pos(struct _nt_window *window, int *x, int *y) {
    x11_window_t *xwin = window->platform;
    Window child;
    XTranslateCoordinates(display, DefaultRootWindow(display), xwin->window, 0, 0, x, y, &child);
    *x = -(*x);
    *y = -(*y);
}

void nt_platform_set_window_pos(struct _nt_window *window, int x, int y) {
    NT_DEBUG("nt_platform_set_window_pos %d %d\n", x, y);
    x11_window_t *xwin = window->platform;
    XMoveWindow(display, xwin->window, x, y);
}

void nt_platform_resize_window(struct _nt_window *window, int w, int h) {
    x11_window_t *xwin = window->platform;
    XResizeWindow(display, xwin->window, w, h);
    XFlush(display);
}

nt_window_t *nt_platform_get_window(Window window) {
    NT_ITERATE_WINDOWS(win) {
        x11_window_t *xwin = win->platform;
        if (xwin->window == window) {
            return win;
        }
    }

    return NULL;
}

static unsigned char nt_platform_convert_buttons(int xbuttons) {
    switch (xbuttons) {
        case Button1: return NT_MOUSE_BUTTON_LEFT;
        case Button2: return NT_MOUSE_BUTTON_MIDDLE;
        case Button3: return NT_MOUSE_BUTTON_RIGHT;
        default: return 0;
    }
}

void nt_platform_set_clip(struct _nt_window *window, nt_clip_t *clip) {
    x11_window_t *xwin = window->platform;

    if (!clip) {
        XRenderPictureAttributes attrs = {
            .clip_mask = None,
        };

        XRenderChangePicture(display, xwin->picture, CPClipMask, &attrs);
        return;
    }

    XRectangle rect = {
        .x = (short)clip->x,
        .y = (short)clip->y,
        .width = (unsigned short)clip->w,
        .height = (unsigned short)clip->h,
    };

    XRenderSetPictureClipRectangles(display, xwin->picture, 0, 0, &rect, 1);
}


void nt_platform_set_cursor(struct _nt_window *window, nt_platform_cursor_t cur) {
    x11_window_t *xwin = window->platform;

    Cursor cursor;

    switch (cur) {
        case NT_CURSOR_HAND:
            cursor = XCreateFontCursor(display, XC_hand2);
            break;
        case NT_CURSOR_TEXT:
            cursor = XCreateFontCursor(display, XC_xterm);
            break;
        case NT_CURSOR_RESIZE_H:
            cursor = XCreateFontCursor(display, XC_sb_h_double_arrow);
            break;
        case NT_CURSOR_RESIZE_V:
            cursor = XCreateFontCursor(display, XC_sb_v_double_arrow);
            break;
        case NT_CURSOR_ARROW:
        default:
            cursor = XCreateFontCursor(display, XC_arrow);
            break;
    }

    XDefineCursor(display, xwin->window, cursor);
    XFreeCursor(display, cursor);
}

XMotionEvent last_motion = { 0 };
bool have_motion = false;

void nt_platform_check_events(struct _nt_window *window) {
    while (XPending(display)) {
        XEvent ev;
        XNextEvent(display, &ev);


        if (ev.type == Expose) {
            nt_window_t *win = nt_platform_get_window(ev.xexpose.window);
            if (win) {
                nt_platform_flip(win);
            }
        } else if (ev.type == KeyPress || ev.type == KeyRelease) {
            nt_window_t *win = nt_platform_get_window(ev.xkey.window);
            if (win) {
                nt_event_t key_event;
                if (nt_platform_xkeyevent_to_event(&ev.xkey, ev.type == KeyPress, &key_event)) {
                    nt_event_process(win, &key_event);
                }
            }
        } else if (ev.type == ButtonPress && (ev.xbutton.button == Button4 || ev.xbutton.button == Button5)) {
            nt_event_t scroll_event = {
                .type = NT_EVENT_MOUSE_SCROLL,
                .scroll = {
                    .x = ev.xbutton.x,
                    .y = ev.xbutton.y,
                    .delta_x = 0,
                    .delta_y = (ev.xbutton.button == Button4) ? 1 : -1,
                }
            };

            nt_event_process(nt_platform_get_window(ev.xbutton.window), &scroll_event);
        } else if (ev.type == ButtonPress) {
            nt_event_t button_press = {
                .type = NT_EVENT_MOUSE_DOWN,
                .pointer = {
                    .x = ev.xbutton.x,
                    .y = ev.xbutton.y,
                    .buttons = nt_platform_convert_buttons(ev.xbutton.button),
                }
            };

            nt_event_process(nt_platform_get_window(ev.xbutton.window), &button_press);
        } else if (ev.type == ButtonRelease && (ev.xbutton.button == Button4 || ev.xbutton.button == Button5)) {
            continue;
        } else if (ev.type == ButtonRelease) {
            nt_event_t button_release = {
                .type = NT_EVENT_MOUSE_UP,
                .pointer = {
                    .x = ev.xbutton.x,
                    .y = ev.xbutton.y,
                    .buttons = nt_platform_convert_buttons(ev.xbutton.button),
                }
            };

            nt_event_process(nt_platform_get_window(ev.xbutton.window), &button_release);
        } else if (ev.type == MotionNotify) {
            // motion events can get slow after stacking up a bit so do this shit
            have_motion = true;
            last_motion = ev.xmotion;
        } else if (ev.type == ConfigureNotify) {
            XConfigureEvent xce = ev.xconfigure;
            nt_window_t *win = nt_platform_get_window(xce.window);
            
            if (win && (xce.width != win->width || xce.height != win->height)) {
                win->width = xce.width;
                win->height = xce.height;

                x11_window_t *xwin = win->platform;

                XRenderFreePicture(display, xwin->picture);
                XFreePixmap(display, xwin->pixmap);
                
                xwin->pixmap = XCreatePixmap(display, xwin->window, win->width, win->height, visual_info.depth);
                
                XRenderPictFormat *fmt_win = XRenderFindVisualFormat(display, xwin->visual);
                xwin->picture = XRenderCreatePicture(display, xwin->pixmap, fmt_win, 0, NULL);

                nt_widget_mark_recalc(win->root_frame);

                if (win->root_frame) {
                    nt_window_update(win);
                }
            }
        } else if (ev.type == ClientMessage) {
            if (ev.xclient.data.l[0] == wmDelete) {
                nt_window_closed(nt_platform_get_window(ev.xclient.window));
                XDestroyWindow(display, ev.xclient.window);
            }
        }
    }

    if (have_motion) {
        nt_event_t motion_event = {
            .type = NT_EVENT_MOUSE_MOTION,
            .pointer = {
                .x = last_motion.x,
                .y = last_motion.y,
                .buttons = 0,
            }
        };

        nt_event_process(nt_platform_get_window(last_motion.window), &motion_event);
        have_motion = false;
    }
}

int nt_platform_destroy_window(nt_window_t *w) {
    x11_window_t *xwin = w->platform;
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = ClientMessage;
    ev.xclient.window = xwin->window;
    ev.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = wmDelete;
    ev.xclient.data.l[1] = CurrentTime;

    // Send the message to the target window
    XSendEvent(display, xwin->window, False, NoEventMask, &ev);
    XFlush(display);
    return 0;
}

void nt_platform_set_window_visible(nt_window_t *window, bool visible) {
    x11_window_t *xwin = window->platform;
    if (visible) {
        XMapWindow(display, xwin->window);
    } else {
        XUnmapWindow(display, xwin->window);
    }

    XFlush(display);
}

typedef struct x11_surf {
    Picture pic;
    Pixmap pixmap;
    XImage *image;
    GC gc;
} x11_surf_t;

void nt_platform_init_surface(struct _nt_window *window, struct _nt_render_surface *surface, size_t width, size_t height) {
    x11_window_t *xwin = window->platform;
    x11_surf_t *xsurf = malloc(sizeof(x11_surf_t));

    xsurf->pixmap = XCreatePixmap(display, xwin->window, width, height, 32);
    xsurf->image = XCreateImage(display, DefaultVisual(display, screen), 32, ZPixmap, 0, (char*)surface->buffer, surface->width, surface->height, 32, surface->width * 4);
    xsurf->gc = XCreateGC(display, xsurf->pixmap, 0, NULL);
    XRenderPictFormat *fmt = XRenderFindStandardFormat(display, PictStandardARGB32);
    xsurf->pic = XRenderCreatePicture(display, xsurf->pixmap, fmt, 0, NULL);

    surface->platform = (void*)xsurf;
}


void nt_platform_deinit_surface(struct _nt_render_surface *surface) {
    x11_surf_t *xsurf = surface->platform;
    XRenderFreePicture(display, xsurf->pic);
    XDestroyImage(xsurf->image);
    XFreeGC(display, xsurf->gc);
    XFreePixmap(display, xsurf->pixmap);
    free(xsurf);
}

void nt_platform_blit_surface(struct _nt_window *window, struct _nt_render_surface *surface, int x, int y) {
    x11_window_t *xwin = window->platform;
    x11_surf_t *xsurf = surface->platform;

    XPutImage(display, xsurf->pixmap, xsurf->gc, xsurf->image, 0, 0, 0, 0, surface->width, surface->height);

    XRenderComposite(display, PictOpOver, xsurf->pic, None, xwin->picture,
        0, 0,
        0, 0,
        x, y,
        surface->width, surface->height);
}


void nt_platform_flip(struct _nt_window *window) {
    x11_window_t *xwin = window->platform;
    XCopyArea(display, xwin->pixmap, xwin->window, xwin->gc, 0, 0, window->width, window->height, 0, 0);
    XFlush(display);
}

#endif
