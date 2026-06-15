/**
 * @file include/neutron/keycodes.h
 * @brief List of neutron keycodes
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifndef _NEUTRON_KEYCODES_H
#define _NEUTRON_KEYCODES_H

/* modifier bitmasks */
#define NT_MOD_SHIFT        0x1
#define NT_MOD_RSHIFT       0x2
#define NT_MOD_SUPER        0x4
#define NT_MOD_CTRL         0x8
#define NT_MOD_RCTRL        0x10
#define NT_MOD_ALT          0x20
#define NT_MOD_RALT         0x40
#define NT_MOD_CAPS_LOCK    0x80
#define NT_MOD_NUM_LOCK     0x100

typedef enum {
    NT_KEY_UNKNOWN = 0,

    /* Letters */
    NT_KEY_A, NT_KEY_B, NT_KEY_C, NT_KEY_D, NT_KEY_E, NT_KEY_F, NT_KEY_G, NT_KEY_H, NT_KEY_I, NT_KEY_J, NT_KEY_K, NT_KEY_L, NT_KEY_M,
    NT_KEY_N, NT_KEY_O, NT_KEY_P, NT_KEY_Q, NT_KEY_R, NT_KEY_S, NT_KEY_T, NT_KEY_U, NT_KEY_V, NT_KEY_W, NT_KEY_X, NT_KEY_Y, NT_KEY_Z,

    /* Numbers */
    NT_KEY_0, NT_KEY_1, NT_KEY_2, NT_KEY_3, NT_KEY_4,
    NT_KEY_5, NT_KEY_6, NT_KEY_7, NT_KEY_8, NT_KEY_9,

    /* Punctuation */
    NT_KEY_SPACE,
    NT_KEY_ENTER,
    NT_KEY_ESCAPE,
    NT_KEY_TAB,
    NT_KEY_BACKSPACE,
    NT_KEY_MINUS,
    NT_KEY_EQUALS,
    NT_KEY_LEFTBRACKET,
    NT_KEY_RIGHTBRACKET,
    NT_KEY_BACKSLASH,
    NT_KEY_SEMICOLON,
    NT_KEY_APOSTROPHE,
    NT_KEY_GRAVE,
    NT_KEY_COMMA,
    NT_KEY_PERIOD,
    NT_KEY_SLASH,

    /* Navigation */
    NT_KEY_INSERT,
    NT_KEY_DELETE,
    NT_KEY_HOME,
    NT_KEY_END,
    NT_KEY_PAGEUP,
    NT_KEY_PAGEDOWN,
    NT_KEY_LEFT,
    NT_KEY_RIGHT,
    NT_KEY_UP,
    NT_KEY_DOWN,

    /* Lock keys */
    NT_KEY_CAPSLOCK,
    NT_KEY_SCROLLLOCK,
    NT_KEY_NUMLOCK,
    NT_KEY_PRINTSCREEN,
    NT_KEY_PAUSE,

    /* Function keys */
    NT_KEY_F1,
    NT_KEY_F2,
    NT_KEY_F3,
    NT_KEY_F4,
    NT_KEY_F5,
    NT_KEY_F6,
    NT_KEY_F7,
    NT_KEY_F8,
    NT_KEY_F9,
    NT_KEY_F10,
    NT_KEY_F11,
    NT_KEY_F12,
    NT_KEY_F13,
    NT_KEY_F14,
    NT_KEY_F15,
    NT_KEY_F16,
    NT_KEY_F17,
    NT_KEY_F18,
    NT_KEY_F19,
    NT_KEY_F20,
    NT_KEY_F21,
    NT_KEY_F22,
    NT_KEY_F23,
    NT_KEY_F24,

    /* Modifiers */
    NT_KEY_LEFTSHIFT,
    NT_KEY_RIGHTSHIFT,
    NT_KEY_LEFTCTRL,
    NT_KEY_RIGHTCTRL,
    NT_KEY_LEFTALT,
    NT_KEY_RIGHTALT,
    NT_KEY_LEFTSUPER,
    NT_KEY_RIGHTSUPER,
    NT_KEY_MENU,

    /* Keypad */
    NT_KEY_KEYPAD0,
    NT_KEY_KEYPAD1,
    NT_KEY_KEYPAD2,
    NT_KEY_KEYPAD3,
    NT_KEY_KEYPAD4,
    NT_KEY_KEYPAD5,
    NT_KEY_KEYPAD6,
    NT_KEY_KEYPAD7,
    NT_KEY_KEYPAD8,
    NT_KEY_KEYPAD9,
    NT_KEY_KEYPADDECIMAL,
    NT_KEY_KEYPADDIVIDE,
    NT_KEY_KEYPADMULTIPLY,
    NT_KEY_KEYPADSUBTRACT,
    NT_KEY_KEYPADADD,
    NT_KEY_KEYPADENTER,
    NT_KEY_KEYPADEQUALS
} nt_key_t;

#endif
