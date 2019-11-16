use alloc::prelude::v1::*;

use qmk_sys::bindgen::{
    keyrecord_t, planck_ez_keycodes, send_char, send_unicode_hex_string, tap_code,
};

const WIDETEXT: u32 = planck_ez_keycodes::EZ_SAFE_RANGE as u32 + 1;
const TAUNTEXT: u32 = planck_ez_keycodes::EZ_SAFE_RANGE as u32 + 2;
const UC_SHRG: u32 = planck_ez_keycodes::EZ_SAFE_RANGE as u32 + 3;
const ZORK_KEY: u32 = planck_ez_keycodes::EZ_SAFE_RANGE as u32 + 4;

#[no_mangle]
pub unsafe extern "C" fn process_record_user_rs(keycode: u16, record: *const keyrecord_t) -> bool {
    process_record_user(keycode, &*record)
}

fn process_record_user(keycode: u16, record: &keyrecord_t) -> bool {
    if !super::zork::zork(ZORK_KEY, keycode, record) {
        return false;
    }

    if !tauntext(keycode, record) {
        return false;
    }

    if !widetext(keycode, record) {
        return false;
    }

    if !shrug(keycode, record) {
        return false;
    }

    true
}

fn tauntext(keycode: u16, record: &keyrecord_t) -> bool {
    use qmk_sys::bindgen::hid_keyboard_keypad_usage::*;

    fn_local_mutable_static!(static mut ENABLED: bool = false);

    if ENABLED.get() {
        if record.event.pressed {
            if keycode as u32 != KC_SPACE {
                unsafe {
                    tap_code(KC_CAPSLOCK as u8);
                }
            }
        }
    }

    match keycode as u32 {
        TAUNTEXT => {
            if record.event.pressed {
                ENABLED.set(!ENABLED.get());
            }
            return false;
        }
        _ => {}
    }

    true
}

fn widetext(keycode: u16, record: &keyrecord_t) -> bool {
    use qmk_sys::bindgen::hid_keyboard_keypad_usage::*;

    fn_local_mutable_static!(static mut ENABLED: bool = false);
    fn_local_mutable_static!(static mut FIRST: bool = false);

    if ENABLED.get() {
        if record.event.pressed {
            match keycode as u32 {
                KC_A..=KC_0 | KC_SPACE..=KC_SLASH => {
                    if FIRST.get() {
                        FIRST.set(false);
                    } else {
                        unsafe { send_char(b' ') }
                    }
                }
                KC_ENTER => FIRST.set(true),
                KC_BSPACE => {
                    // backspace char
                    unsafe {
                        send_char(b'\x08');
                    }
                }
                _ => {}
            }
        }
    }

    match keycode as u32 {
        WIDETEXT => {
            if record.event.pressed {
                ENABLED.set(!ENABLED.get());
            }
            return false;
        }
        _ => {}
    }

    true
}

fn shrug(keycode: u16, record: &keyrecord_t) -> bool {
    match keycode as u32 {
        UC_SHRG => {
            if record.event.pressed {
                let mut s = "¯\\_(ツ)_/¯"
                    .chars()
                    .map(|c| format!("{:04X}", c as u32))
                    .collect::<Vec<_>>()
                    .join(" ");
                s.push('\0');
                unsafe {
                    send_unicode_hex_string(s.as_ptr());
                }
            }
            return false;
        }
        _ => {}
    }

    true
}
