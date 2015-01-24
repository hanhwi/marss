/*
 * QEMU SDL display driver
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "qemu-common.h"
#include "x_keymap.h"

static const uint8_t x_keycode_to_pc_keycode[115] = {
   0xc7,      /*  97  Home   */
   0xc8,      /*  98  Up     */
   0xc9,      /*  99  PgUp   */
   0xcb,      /* 100  Left   */
   0x4c,        /* 101  KP-5   */
   0xcd,      /* 102  Right  */
   0xcf,      /* 103  End    */
   0xd0,      /* 104  Down   */
   0xd1,      /* 105  PgDn   */
   0xd2,      /* 106  Ins    */
   0xd3,      /* 107  Del    */
   0x9c,      /* 108  Enter  */
   0x9d,      /* 109  Ctrl-R */
   0x0,       /* 110  Pause  */
   0xb7,      /* 111  Print  */
   0xb5,      /* 112  Divide */
   0xb8,      /* 113  Alt-R  */
   0xc6,      /* 114  Break  */
   0x0,         /* 115 */
   0x0,         /* 116 */
   0x0,         /* 117 */
   0x0,         /* 118 */
   0x0,         /* 119 */
   0x0,         /* 120 */
   0x0,         /* 121 */
   0x0,         /* 122 */
   0x0,         /* 123 */
   0x0,         /* 124 */
   0x0,         /* 125 */
   0x0,         /* 126 */
   0x0,         /* 127 */
   0x0,         /* 128 */
   0x79,         /* 129 Henkan */
   0x0,         /* 130 */
   0x7b,         /* 131 Muhenkan */
   0x0,         /* 132 */
   0x7d,         /* 133 Yen */
   0x0,         /* 134 */
   0x0,         /* 135 */
   0x47,         /* 136 KP_7 */
   0x48,         /* 137 KP_8 */
   0x49,         /* 138 KP_9 */
   0x4b,         /* 139 KP_4 */
   0x4c,         /* 140 KP_5 */
   0x4d,         /* 141 KP_6 */
   0x4f,         /* 142 KP_1 */
   0x50,         /* 143 KP_2 */
   0x51,         /* 144 KP_3 */
   0x52,         /* 145 KP_0 */
   0x53,         /* 146 KP_. */
   0x47,         /* 147 KP_HOME */
   0x48,         /* 148 KP_UP */
   0x49,         /* 149 KP_PgUp */
   0x4b,         /* 150 KP_Left */
   0x4c,         /* 151 KP_ */
   0x4d,         /* 152 KP_Right */
   0x4f,         /* 153 KP_End */
   0x50,         /* 154 KP_Down */
   0x51,         /* 155 KP_PgDn */
   0x52,         /* 156 KP_Ins */
   0x53,         /* 157 KP_Del */
};

/* This table is generated based off the xfree86 -> scancode mapping above
 * and the keycode mappings in /usr/share/X11/xkb/keycodes/evdev
 * and  /usr/share/X11/xkb/keycodes/xfree86
 */

static const uint8_t evdev_keycode_to_pc_keycode[61] = {
    0,         /*  97 EVDEV - RO   ("Internet" Keyboards) */
    0,         /*  98 EVDEV - KATA (Katakana) */
    0,         /*  99 EVDEV - HIRA (Hiragana) */
    0x79,      /* 100 EVDEV - HENK (Henkan) */
    0x70,      /* 101 EVDEV - HKTG (Hiragana/Katakana toggle) */
    0x7b,      /* 102 EVDEV - MUHE (Muhenkan) */
    0,         /* 103 EVDEV - JPCM (KPJPComma) */
    0x9c,      /* 104 KPEN */
    0x9d,      /* 105 RCTL */
    0xb5,      /* 106 KPDV */
    0xb7,      /* 107 PRSC */
    0xb8,      /* 108 RALT */
    0,         /* 109 EVDEV - LNFD ("Internet" Keyboards) */
    0xc7,      /* 110 HOME */
    0xc8,      /* 111 UP */
    0xc9,      /* 112 PGUP */
    0xcb,      /* 113 LEFT */
    0xcd,      /* 114 RGHT */
    0xcf,      /* 115 END */
    0xd0,      /* 116 DOWN */
    0xd1,      /* 117 PGDN */
    0xd2,      /* 118 INS */
    0xd3,      /* 119 DELE */
    0,         /* 120 EVDEV - I120 ("Internet" Keyboards) */
    0,         /* 121 EVDEV - MUTE */
    0,         /* 122 EVDEV - VOL- */
    0,         /* 123 EVDEV - VOL+ */
    0,         /* 124 EVDEV - POWR */
    0,         /* 125 EVDEV - KPEQ */
    0,         /* 126 EVDEV - I126 ("Internet" Keyboards) */
    0,         /* 127 EVDEV - PAUS */
    0,         /* 128 EVDEV - ???? */
    0,         /* 129 EVDEV - I129 ("Internet" Keyboards) */
    0xf1,      /* 130 EVDEV - HNGL (Korean Hangul Latin toggle) */
    0xf2,      /* 131 EVDEV - HJCV (Korean Hangul Hanja toggle) */
    0x7d,      /* 132 AE13 (Yen)*/
    0xdb,      /* 133 EVDEV - LWIN */
    0xdc,      /* 134 EVDEV - RWIN */
    0xdd,      /* 135 EVDEV - MENU */
    0,         /* 136 EVDEV - STOP */
    0,         /* 137 EVDEV - AGAI */
    0,         /* 138 EVDEV - PROP */
    0,         /* 139 EVDEV - UNDO */
    0,         /* 140 EVDEV - FRNT */
    0,         /* 141 EVDEV - COPY */
    0,         /* 142 EVDEV - OPEN */
    0,         /* 143 EVDEV - PAST */
    0,         /* 144 EVDEV - FIND */
    0,         /* 145 EVDEV - CUT  */
    0,         /* 146 EVDEV - HELP */
    0,         /* 147 EVDEV - I147 */
    0,         /* 148 EVDEV - I148 */
    0,         /* 149 EVDEV - I149 */
    0,         /* 150 EVDEV - I150 */
    0,         /* 151 EVDEV - I151 */
    0,         /* 152 EVDEV - I152 */
    0,         /* 153 EVDEV - I153 */
    0,         /* 154 EVDEV - I154 */
    0,         /* 155 EVDEV - I156 */
    0,         /* 156 EVDEV - I157 */
    0,         /* 157 EVDEV - I158 */
};

/* This table is borrwed from vncdisplaykeymap_xorgxquartz2rfb.c in gtk-vnc project */
static const uint8_t xquartz_keycode_to_pc_keycode[] = {
  [0x8] = 0x1e,        /* 8 => 30 via 30 (KEY_A) */
  [0x9] = 0x1f,        /* 9 => 31 via 31 (KEY_S) */
  [0xa] = 0x20,        /* 10 => 32 via 32 (KEY_D) */
  [0xb] = 0x21,        /* 11 => 33 via 33 (KEY_F) */
  [0xc] = 0x23,        /* 12 => 35 via 35 (KEY_H) */
  [0xd] = 0x22,        /* 13 => 34 via 34 (KEY_G) */
  [0xe] = 0x2c,        /* 14 => 44 via 44 (KEY_Z) */
  [0xf] = 0x2d,        /* 15 => 45 via 45 (KEY_X) */
  [0x10] = 0x2e,       /* 16 => 46 via 46 (KEY_C) */
  [0x11] = 0x2f,       /* 17 => 47 via 47 (KEY_V) */
  [0x12] = 0x70,       /* 18 => 112 via 170 (KEY_ISO) */
  [0x13] = 0x30,       /* 19 => 48 via 48 (KEY_B) */
  [0x14] = 0x10,       /* 20 => 16 via 16 (KEY_Q) */
  [0x15] = 0x11,       /* 21 => 17 via 17 (KEY_W) */
  [0x16] = 0x12,       /* 22 => 18 via 18 (KEY_E) */
  [0x17] = 0x13,       /* 23 => 19 via 19 (KEY_R) */
  [0x18] = 0x15,       /* 24 => 21 via 21 (KEY_Y) */
  [0x19] = 0x14,       /* 25 => 20 via 20 (KEY_T) */
  [0x1a] = 0x2,        /* 26 => 2 via 2 (KEY_1) */
  [0x1b] = 0x3,        /* 27 => 3 via 3 (KEY_2) */
  [0x1c] = 0x4,        /* 28 => 4 via 4 (KEY_3) */
  [0x1d] = 0x5,        /* 29 => 5 via 5 (KEY_4) */
  [0x1e] = 0x7,        /* 30 => 7 via 7 (KEY_6) */
  [0x1f] = 0x6,        /* 31 => 6 via 6 (KEY_5) */
  [0x20] = 0xd,        /* 32 => 13 via 13 (KEY_EQUAL) */
  [0x21] = 0xa,        /* 33 => 10 via 10 (KEY_9) */
  [0x22] = 0x8,        /* 34 => 8 via 8 (KEY_7) */
  [0x23] = 0xc,        /* 35 => 12 via 12 (KEY_MINUS) */
  [0x24] = 0x9,        /* 36 => 9 via 9 (KEY_8) */
  [0x25] = 0xb,        /* 37 => 11 via 11 (KEY_0) */
  [0x26] = 0x1b,       /* 38 => 27 via 27 (KEY_RIGHTBRACE) */
  [0x27] = 0x18,       /* 39 => 24 via 24 (KEY_O) */
  [0x28] = 0x16,       /* 40 => 22 via 22 (KEY_U) */
  [0x29] = 0x1a,       /* 41 => 26 via 26 (KEY_LEFTBRACE) */
  [0x2a] = 0x17,       /* 42 => 23 via 23 (KEY_I) */
  [0x2b] = 0x19,       /* 43 => 25 via 25 (KEY_P) */
  [0x2c] = 0x1c,       /* 44 => 28 via 28 (KEY_ENTER) */
  [0x2d] = 0x26,       /* 45 => 38 via 38 (KEY_L) */
  [0x2e] = 0x24,       /* 46 => 36 via 36 (KEY_J) */
  [0x2f] = 0x28,       /* 47 => 40 via 40 (KEY_APOSTROPHE) */
  [0x30] = 0x25,       /* 48 => 37 via 37 (KEY_K) */
  [0x31] = 0x27,       /* 49 => 39 via 39 (KEY_SEMICOLON) */
  [0x32] = 0x2b,       /* 50 => 43 via 43 (KEY_BACKSLASH) */
  [0x33] = 0x33,       /* 51 => 51 via 51 (KEY_COMMA) */
  [0x34] = 0x35,       /* 52 => 53 via 53 (KEY_SLASH) */
  [0x35] = 0x31,       /* 53 => 49 via 49 (KEY_N) */
  [0x36] = 0x32,       /* 54 => 50 via 50 (KEY_M) */
  [0x37] = 0x34,       /* 55 => 52 via 52 (KEY_DOT) */
  [0x38] = 0xf,        /* 56 => 15 via 15 (KEY_TAB) */
  [0x39] = 0x39,       /* 57 => 57 via 57 (KEY_SPACE) */
  [0x3a] = 0x29,       /* 58 => 41 via 41 (KEY_GRAVE) */
  [0x3b] = 0xe,        /* 59 => 14 via 14 (KEY_BACKSPACE) */
  [0x3d] = 0x1,        /* 61 => 1 via 1 (KEY_ESC) */
  [0x3f] = 0xdb,       /* 63 => 219 via 125 (KEY_LEFTMETA) */
  [0x40] = 0x2a,       /* 64 => 42 via 42 (KEY_LEFTSHIFT) */
  [0x41] = 0x3a,       /* 65 => 58 via 58 (KEY_CAPSLOCK) */
  [0x42] = 0x38,       /* 66 => 56 via 56 (KEY_LEFTALT) */
  [0x43] = 0x1d,       /* 67 => 29 via 29 (KEY_LEFTCTRL) */
  [0x44] = 0x36,       /* 68 => 54 via 54 (KEY_RIGHTSHIFT) */
  [0x45] = 0xb8,       /* 69 => 184 via 100 (KEY_RIGHTALT) */
  [0x46] = 0x9d,       /* 70 => 157 via 97 (KEY_RIGHTCTRL) */
  [0x47] = 0xdd,       /* 71 => 221 via 127 (KEY_COMPOSE) */
  [0x48] = 0x83,       /* 72 => 131 via 187 (KEY_F17) */
  [0x49] = 0x53,       /* 73 => 83 via 83 (KEY_KPDOT) */
  [0x4b] = 0x37,       /* 75 => 55 via 55 (KEY_KPASTERISK) */
  [0x4d] = 0x4e,       /* 77 => 78 via 78 (KEY_KPPLUS) */
  [0x4f] = 0x7e,       /* 79 => 126 via 121 (KEY_KPCOMMA) */
  [0x50] = 0xb0,       /* 80 => 176 via 115 (KEY_VOLUMEUP) */
  [0x51] = 0xae,       /* 81 => 174 via 114 (KEY_VOLUMEDOWN) */
  [0x52] = 0xa0,       /* 82 => 160 via 113 (KEY_MUTE) */
  [0x53] = 0xb5,       /* 83 => 181 via 98 (KEY_KPSLASH) */
  [0x54] = 0x9c,       /* 84 => 156 via 96 (KEY_KPENTER) */
  [0x56] = 0x4a,       /* 86 => 74 via 74 (KEY_KPMINUS) */
  [0x57] = 0xf7,       /* 87 => 247 via 188 (KEY_F18) */
  [0x58] = 0x84,       /* 88 => 132 via 189 (KEY_F19) */
  [0x59] = 0x59,       /* 89 => 89 via 117 (KEY_KPEQUAL) */
  [0x5a] = 0x52,       /* 90 => 82 via 82 (KEY_KP0) */
  [0x5b] = 0x4f,       /* 91 => 79 via 79 (KEY_KP1) */
  [0x5c] = 0x50,       /* 92 => 80 via 80 (KEY_KP2) */
  [0x5d] = 0x51,       /* 93 => 81 via 81 (KEY_KP3) */
  [0x5e] = 0x4b,       /* 94 => 75 via 75 (KEY_KP4) */
  [0x5f] = 0x4c,       /* 95 => 76 via 76 (KEY_KP5) */
  [0x60] = 0x4d,       /* 96 => 77 via 77 (KEY_KP6) */
  [0x61] = 0x47,       /* 97 => 71 via 71 (KEY_KP7) */
  [0x62] = 0x5a,       /* 98 => 90 via 190 (KEY_F20) */
  [0x63] = 0x48,       /* 99 => 72 via 72 (KEY_KP8) */
  [0x64] = 0x49,       /* 100 => 73 via 73 (KEY_KP9) */
  [0x65] = 0x7d,       /* 101 => 125 via 124 (KEY_YEN) */
  [0x67] = 0x5c,       /* 103 => 92 via 95 (KEY_KPJPCOMMA) */
  [0x68] = 0x3f,       /* 104 => 63 via 63 (KEY_F5) */
  [0x69] = 0x40,       /* 105 => 64 via 64 (KEY_F6) */
  [0x6a] = 0x41,       /* 106 => 65 via 65 (KEY_F7) */
  [0x6b] = 0x3d,       /* 107 => 61 via 61 (KEY_F3) */
  [0x6c] = 0x42,       /* 108 => 66 via 66 (KEY_F8) */
  [0x6d] = 0x43,       /* 109 => 67 via 67 (KEY_F9) */
  [0x6f] = 0x57,       /* 111 => 87 via 87 (KEY_F11) */
  [0x70] = 0x78,       /* 112 => 120 via 90 (KEY_KATAKANA) */
  [0x71] = 0x5d,       /* 113 => 93 via 183 (KEY_F13) */
  [0x72] = 0x55,       /* 114 => 85 via 186 (KEY_F16) */
  [0x73] = 0x5e,       /* 115 => 94 via 184 (KEY_F14) */
  [0x75] = 0x44,       /* 117 => 68 via 68 (KEY_F10) */
  [0x77] = 0x58,       /* 119 => 88 via 88 (KEY_F12) */
  [0x79] = 0x5f,       /* 121 => 95 via 185 (KEY_F15) */
  [0x7b] = 0xc7,       /* 123 => 199 via 102 (KEY_HOME) */
  [0x7c] = 0xc9,       /* 124 => 201 via 104 (KEY_PAGEUP) */
  [0x7d] = 0xd3,       /* 125 => 211 via 111 (KEY_DELETE) */
  [0x7e] = 0x3e,       /* 126 => 62 via 62 (KEY_F4) */
  [0x7f] = 0xcf,       /* 127 => 207 via 107 (KEY_END) */
  [0x80] = 0x3c,       /* 128 => 60 via 60 (KEY_F2) */
  [0x81] = 0xd1,       /* 129 => 209 via 109 (KEY_PAGEDOWN) */
  [0x82] = 0x3b,       /* 130 => 59 via 59 (KEY_F1) */
  [0x83] = 0xcb,       /* 131 => 203 via 105 (KEY_LEFT) */
  [0x84] = 0xcd,       /* 132 => 205 via 106 (KEY_RIGHT) */
  [0x85] = 0xd0,       /* 133 => 208 via 108 (KEY_DOWN) */
  [0x86] = 0xc8,       /* 134 => 200 via 103 (KEY_UP) */
};    


uint8_t translate_xfree86_keycode(const int key)
{
    return x_keycode_to_pc_keycode[key];
}

uint8_t translate_evdev_keycode(const int key)
{
    return evdev_keycode_to_pc_keycode[key];
}

uint8_t translate_xquartz_keycode(const int key)
{
    return xquartz_keycode_to_pc_keycode[key];
}
