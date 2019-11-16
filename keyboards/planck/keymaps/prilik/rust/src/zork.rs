use alloc::prelude::v1::*;

use embcrusted::{Options, Ui, Zmachine};
use qmk_sys::bindgen::{keyrecord_t, send_string, tap_code};

#[derive(Default)]
pub struct ZorkUi {
    buf: Vec<u8>,
}

impl ZorkUi {
    fn pop_char(&mut self) {
        self.buf.pop();
    }

    fn push_char(&mut self, c: u8) {
        self.buf.push(c)
    }

    fn clear(&mut self) {
        self.buf.clear()
    }
}

const ZORK_DATA: &[u8] = include_bytes!("../zork1-r88-s840726.z3");

const KEYCODE_TO_ASCII_LUT: [u8; 57] = [
    b'\0', b'\0', b'\0', b'\0', b'a', b'b', b'c', b'd', b'e', b'f', b'g', b'h', b'i', b'j', b'k',
    b'l', b'm', b'n', b'o', b'p', b'q', b'r', b's', b't', b'u', b'v', b'w', b'x', b'y', b'z', b'1',
    b'2', b'3', b'4', b'5', b'6', b'7', b'8', b'9', b'0', b'\0', b'\0', b'\0', b'\t', b' ', b'-',
    b'=', b'[', b']', b'\\', b'\0', b';', b'\'', b'`', b',', b'.', b'/',
];

impl Ui for ZorkUi {
    fn print(&self, text: &str) {
        let nullterm = format!("{}\0", text);
        unsafe {
            send_string(nullterm.as_ptr());
        }
    }

    fn print_object(&mut self, object: &str) {
        self.print(object);
    }

    fn set_status_bar(&self, left: &str, right: &str) {
        let _ = (left, right);
        // self.print(&format!("{}  -  {}", left, right));
    }

    fn get_input_buf(&mut self) -> &str {
        unsafe { core::str::from_utf8_unchecked(&self.buf).trim() }
    }
}

static mut ZORK: Option<Zmachine<ZorkUi>> = None;

pub fn zork(zork_key: u32, keycode: u16, record: &keyrecord_t) -> bool {
    use qmk_sys::bindgen::hid_keyboard_keypad_usage::*;

    fn_local_mutable_static!(static mut ENABLED: bool = false);

    if ENABLED.get() {
        let zvm = unsafe { ZORK.as_mut().unwrap() };
        if record.event.pressed {
            match keycode as u32 {
                KC_ENTER | KC_KP_ENTER => {
                    unsafe {
                        tap_code(KC_ENTER as u8);
                    }

                    zvm.ack_input();
                    let done = zvm.step();
                    zvm.ui.clear(); // clear buffer

                    if done {
                        // disable zork on exit
                        ENABLED.set(false);
                        unsafe {
                            ZORK = None;
                        }
                    }

                    return false; // eat the keypress
                }
                KC_BSPACE => zvm.ui.pop_char(),
                _ => {
                    if (keycode as u8) < 58 {
                        zvm.ui.push_char(KEYCODE_TO_ASCII_LUT[keycode as usize])
                    }
                }
            }
        }
    }

    if keycode as u32 == zork_key {
        if record.event.pressed {
            ENABLED.set(!ENABLED.get());

            if ENABLED.get() {
                // init zork
                unsafe {
                    ZORK = Some(Zmachine::new(
                        &ZORK_DATA,
                        ZorkUi::default(),
                        Options { rand_seed: 0x1337 },
                    ));
                    // run until first user input
                    ZORK.as_mut().unwrap().step();
                }
            }
        }
        return false;
    }

    true
}
