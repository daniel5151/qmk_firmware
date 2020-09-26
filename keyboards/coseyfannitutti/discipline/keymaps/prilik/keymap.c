/* Copyright 2019 COSEYFANNITUTTI
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "process_unicode.h"
#define ___X___ XXXXXXX // KC_NO

enum prilik_keycodes {
    _DUMMY = SAFE_RANGE,

    WIDETXT, // w i d e t e x t   f o r   a   w i d e   b o y
    TAUNTXT, // FoR ThE UlTiMaTe sHiTpOsTiNg eXpErIeNcE

    UC_HELP, // URL for QMK unicode help
    UC_SHRG,              // shrug       - ¯\_(ツ)_/¯
#define UC_100  X(E_100)  // hundo       - 💯
#define UC_BBB  X(E_BBB)  // dat B       - 🅱️
#define UC_CLAP X(E_CLAP) // clap        - 👏
#define UC_EYES X(E_EYES) // shifty eyes - 👀
#define UC_GRIM X(E_GRIM) // grimmace    - 😬
#define UC_THNK X(E_THNK) // thinking    - 🤔
#define UC_UGHH X(E_UGHH) // UGHHHHH     - 😩
#define UC_SWET X(E_SWET) // sweat smile - 😅
};

enum unicode_names {
    E_100,
    E_BBB,
    E_CLAP,
    E_EYES,
    E_GRIM,
    E_THNK,
    E_UGHH,
    E_SWET,
};
const uint32_t PROGMEM unicode_map[] = {
    [E_BBB]  = 0x1f171,
    [E_100]  = 0x1f4af,
    [E_EYES] = 0x1f440,
    [E_CLAP] = 0x1f44f,
    [E_GRIM] = 0x1f62c,
    [E_THNK] = 0x1f914,
    [E_UGHH] = 0x1f629,
    [E_SWET] = 0x1f605,
};

enum prilik_layers {
    _QWERTY,
    _ACTIONS,
    _MEMES,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_65_ansi_2_right_mods(
        KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,  \
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_HOME, \
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          KC_UP,   KC_PGDN, \
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_LEFT, KC_DOWN, KC_RGHT  \
    ),
    [_ACTIONS] = LAYOUT_65_ansi_2_right_mods(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_MUTE, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS, _______, KC_END,  \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, KC_VOLU, \
        _______, _______, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_MNXT, KC_MPLY, _______,          _______, KC_VOLD, \
        _______, _______, _______,                            _______,                            MO(2),   _______, _______, _______, _______  \
    ),
    [_MEMES] = LAYOUT_65_ansi_2_right_mods(
        ___X___, UC_100,  ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, \
        ___X___, ___X___, ___X___, UC_EYES, ___X___, UC_THNK, ___X___, UC_UGHH, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, ___X___, \
        TAUNTXT, ___X___, UC_SHRG, ___X___, ___X___, UC_GRIM, ___X___, ___X___, ___X___, ___X___, ___X___, UC_SWET,          ___X___, ___X___, \
        ___X___, ___X___, ___X___, UC_CLAP, ___X___, UC_BBB,  ___X___, ___X___, ___X___, ___X___, UC_HELP, ___X___,          ___X___, ___X___, \
        UC_M_OS, UC_M_WC, UC_M_LN,                            WIDETXT,                            ___X___, ___X___, ___X___, ___X___, ___X___  \
    ),
    /*
    [X] = LAYOUT_65_ansi_2_right_mods(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, \
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______  \
    ),
    */
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static struct {
        bool on;
        bool first;
    } w_i_d_e_t_e_x_t = {false, false};

    if (w_i_d_e_t_e_x_t.on) {
        if (record->event.pressed) {
            switch (keycode) {
                case KC_A...KC_0:
                case KC_MINUS...KC_SLASH:
                case KC_SPC:
                    if (w_i_d_e_t_e_x_t.first) {
                        w_i_d_e_t_e_x_t.first = false;
                    } else {
                        send_char(' ');
                    }
                    break;
                case KC_ENT:
                    w_i_d_e_t_e_x_t.first = true;
                    break;
                case KC_BSPC:
                    send_char('\b'); // backspace
                    break;
            }
        }
    }

    static bool tAuNtTeXt = false;

    if (tAuNtTeXt) {
        if (record->event.pressed) {
            if (keycode != KC_SPC) {
                register_code(KC_CAPS);
                unregister_code(KC_CAPS);
            }
        }
    }

    switch (keycode) {
        /* z e s t y   m e m e s */
        case WIDETXT:
            if (record->event.pressed) {
                w_i_d_e_t_e_x_t.on = !w_i_d_e_t_e_x_t.on;
                w_i_d_e_t_e_x_t.first = true;
            }
            return false;
        case TAUNTXT:
            if (record->event.pressed) {
                tAuNtTeXt = !tAuNtTeXt;
                // when it's turned on, toggle caps lock (makes first letter lowercase)
                if (tAuNtTeXt) {
                    register_code(KC_CAPS);
                    unregister_code(KC_CAPS);
                }
            }
            return false;

        /* Unicode */
        case UC_HELP:
            if (record->event.pressed) {
                SEND_STRING("https://beta.docs.qmk.fm/using-qmk/software-features/feature_unicode");
            }
            return false;
        case UC_SHRG: // ¯\_(ツ)_/¯
            if (record->event.pressed) {
                send_unicode_hex_string("00AF 005C 005F 0028 30C4 0029 005F 002F 00AF");
            }
            return false;

        default:
            return true; //Process all other keycodes normally
    }
}
