/**
 * @file neutron/platform/ethereal_key.c
 * @brief Ethereal key layout
 * 
 * 
 * @copyright
 * This file is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2025 Samuel Stuart
 */

#ifdef BUILDING_ETHEREAL

#include <neutron/neutron.h>
#include <ethereal/keyboard.h>
#include <kernel/fs/periphfs.h>
#include <stdio.h>

keyboard_t *kbd;

typedef struct {
    key_scancode_t sc;
    nt_key_t key;
} nt_platform_keymap_t;

#define NT_ETHEREAL_KEY(src, dst) { (src), (dst) }

static nt_platform_keymap_t nt_ethereal_keymap[] = {
    NT_ETHEREAL_KEY('1', NT_KEY_1),
    NT_ETHEREAL_KEY('2', NT_KEY_2),
    NT_ETHEREAL_KEY('3', NT_KEY_3),
    NT_ETHEREAL_KEY('4', NT_KEY_4),
    NT_ETHEREAL_KEY('5', NT_KEY_5),
    NT_ETHEREAL_KEY('6', NT_KEY_6),
    NT_ETHEREAL_KEY('7', NT_KEY_7),
    NT_ETHEREAL_KEY('8', NT_KEY_8),
    NT_ETHEREAL_KEY('9', NT_KEY_9),
    NT_ETHEREAL_KEY('0', NT_KEY_0),

    NT_ETHEREAL_KEY('-', NT_KEY_MINUS),
    NT_ETHEREAL_KEY('=', NT_KEY_EQUALS),
    NT_ETHEREAL_KEY('\b', NT_KEY_BACKSPACE),
    NT_ETHEREAL_KEY('\t', NT_KEY_TAB),

    NT_ETHEREAL_KEY('q', NT_KEY_Q),
    NT_ETHEREAL_KEY('w', NT_KEY_W),
    NT_ETHEREAL_KEY('e', NT_KEY_E),
    NT_ETHEREAL_KEY('r', NT_KEY_R),
    NT_ETHEREAL_KEY('t', NT_KEY_T),
    NT_ETHEREAL_KEY('y', NT_KEY_Y),
    NT_ETHEREAL_KEY('u', NT_KEY_U),
    NT_ETHEREAL_KEY('i', NT_KEY_I),
    NT_ETHEREAL_KEY('o', NT_KEY_O),
    NT_ETHEREAL_KEY('p', NT_KEY_P),

    NT_ETHEREAL_KEY('[', NT_KEY_LEFTBRACKET),
    NT_ETHEREAL_KEY(']', NT_KEY_RIGHTBRACKET),
    NT_ETHEREAL_KEY('\r', NT_KEY_ENTER),

    NT_ETHEREAL_KEY('a', NT_KEY_A),
    NT_ETHEREAL_KEY('s', NT_KEY_S),
    NT_ETHEREAL_KEY('d', NT_KEY_D),
    NT_ETHEREAL_KEY('f', NT_KEY_F),
    NT_ETHEREAL_KEY('g', NT_KEY_G),
    NT_ETHEREAL_KEY('h', NT_KEY_H),
    NT_ETHEREAL_KEY('j', NT_KEY_J),
    NT_ETHEREAL_KEY('k', NT_KEY_K),
    NT_ETHEREAL_KEY('l', NT_KEY_L),

    NT_ETHEREAL_KEY(';', NT_KEY_SEMICOLON),
    NT_ETHEREAL_KEY('\'', NT_KEY_APOSTROPHE),
    NT_ETHEREAL_KEY('`', NT_KEY_GRAVE),

    NT_ETHEREAL_KEY('\\', NT_KEY_BACKSLASH),

    NT_ETHEREAL_KEY('z', NT_KEY_Z),
    NT_ETHEREAL_KEY('x', NT_KEY_X),
    NT_ETHEREAL_KEY('c', NT_KEY_C),
    NT_ETHEREAL_KEY('v', NT_KEY_V),
    NT_ETHEREAL_KEY('b', NT_KEY_B),
    NT_ETHEREAL_KEY('n', NT_KEY_N),
    NT_ETHEREAL_KEY('m', NT_KEY_M),

    NT_ETHEREAL_KEY(',', NT_KEY_COMMA),
    NT_ETHEREAL_KEY('.', NT_KEY_PERIOD),
    NT_ETHEREAL_KEY('/', NT_KEY_SLASH),

    NT_ETHEREAL_KEY('*', NT_KEY_KEYPADMULTIPLY),
    NT_ETHEREAL_KEY(' ', NT_KEY_SPACE),

    NT_ETHEREAL_KEY(SCANCODE_ESCAPE, NT_KEY_ESCAPE),

    NT_ETHEREAL_KEY(SCANCODE_F1,  NT_KEY_F1),
    NT_ETHEREAL_KEY(SCANCODE_F2,  NT_KEY_F2),
    NT_ETHEREAL_KEY(SCANCODE_F3,  NT_KEY_F3),
    NT_ETHEREAL_KEY(SCANCODE_F4,  NT_KEY_F4),
    NT_ETHEREAL_KEY(SCANCODE_F5,  NT_KEY_F5),
    NT_ETHEREAL_KEY(SCANCODE_F6,  NT_KEY_F6),
    NT_ETHEREAL_KEY(SCANCODE_F7,  NT_KEY_F7),
    NT_ETHEREAL_KEY(SCANCODE_F8,  NT_KEY_F8),
    NT_ETHEREAL_KEY(SCANCODE_F9,  NT_KEY_F9),
    NT_ETHEREAL_KEY(SCANCODE_F10, NT_KEY_F10),
    NT_ETHEREAL_KEY(SCANCODE_F11, NT_KEY_F11),
    NT_ETHEREAL_KEY(SCANCODE_F12, NT_KEY_F12),

    NT_ETHEREAL_KEY(SCANCODE_DEL, NT_KEY_DELETE),
    
    NT_ETHEREAL_KEY(SCANCODE_RIGHT_ARROW, NT_KEY_RIGHT), 
    NT_ETHEREAL_KEY(SCANCODE_LEFT_ARROW, NT_KEY_LEFT),
    NT_ETHEREAL_KEY(SCANCODE_UP_ARROW, NT_KEY_UP),
    NT_ETHEREAL_KEY(SCANCODE_DOWN_ARROW, NT_KEY_DOWN),

    NT_ETHEREAL_KEY(SCANCODE_LEFT_SHIFT, NT_KEY_LEFTSHIFT), NT_ETHEREAL_KEY(SCANCODE_RIGHT_SHIFT, NT_KEY_RIGHTSHIFT),
    NT_ETHEREAL_KEY(SCANCODE_LEFT_ALT, NT_KEY_LEFTALT), NT_ETHEREAL_KEY(SCANCODE_RIGHT_ALT, NT_KEY_RIGHTALT),
    NT_ETHEREAL_KEY(SCANCODE_LEFT_SUPER, NT_KEY_LEFTSUPER), NT_ETHEREAL_KEY(SCANCODE_RIGHT_SUPER, NT_KEY_RIGHTSUPER),
    NT_ETHEREAL_KEY(SCANCODE_LEFT_CTRL, NT_KEY_LEFTCTRL), NT_ETHEREAL_KEY(SCANCODE_RIGHT_CTRL, NT_KEY_RIGHTCTRL)

    // todo the rest (pgup/pgdown/home/etc.)
};

void nt_platform_init_keyboard() {
    kbd = keyboard_create();
}

static unsigned int nt_platform_translate_modifiers(unsigned int mods) {
    uint32_t mods_out = 0;
    if (mods & (KEYBOARD_MOD_LEFT_SHIFT | KEYBOARD_MOD_RIGHT_SHIFT)) {
        mods_out |= NT_MOD_SHIFT;
    }

    if (mods & (KEYBOARD_MOD_LEFT_CTRL | KEYBOARD_MOD_RIGHT_CTRL)) {
        mods_out |= NT_MOD_CTRL;
    } 

    if (mods & (KEYBOARD_MOD_LEFT_ALT | KEYBOARD_MOD_RIGHT_ALT)) {
        mods_out |= NT_MOD_ALT;
    } 

    if (mods & (KEYBOARD_MOD_LEFT_SUPER | KEYBOARD_MOD_RIGHT_SUPER)) {
        mods_out |= NT_MOD_SUPER;
    } 

    return mods_out;
}

nt_key_t nt_platform_to_nt_key(key_scancode_t sc) {
    for (int i = 0; i < sizeof(nt_ethereal_keymap) / sizeof(nt_ethereal_keymap[0]); i++) {
        if (nt_ethereal_keymap[i].sc == sc) {
            return nt_ethereal_keymap[i].key;
        }
    }

    return NT_KEY_Q;
}

bool nt_platform_key_process(key_event_t *k, nt_event_t *event) {
    keyboard_event_t *e = keyboard_event(kbd, k);

    if (e) {
        nt_key_t ntkey = nt_platform_to_nt_key(e->scancode);
        uint32_t mods = nt_platform_translate_modifiers(e->mods);

        event->type = (e->type == KEYBOARD_EVENT_PRESS) ? NT_EVENT_KEY_DOWN : NT_EVENT_KEY_UP,
        event->key.key = ntkey;
        event->key.modifiers = mods;

        // translate utf
        event->key.utf[0] = e->ascii;
        event->key.utf[1] = 0;

        free(e);
    
        return true;
    }

    return false;
}

#endif
