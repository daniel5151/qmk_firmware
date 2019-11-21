#![no_std]
#![feature(alloc_prelude)]

#[macro_use]
extern crate alloc;

#[global_allocator]
static ALLOCATOR: qmk_sys::malloc::MallocFreeAlloc = qmk_sys::malloc::MallocFreeAlloc;

/// This is very danger.
macro_rules! fn_local_mutable_static {
    (static mut $name:ident: $type:ty = $val:expr) => {
        #[allow(non_snake_case)]
        let mut $name = {
            static mut THEVALUE: $type = $val;
            struct $name;
            impl $name {
                pub fn get(&self) -> $type {
                    unsafe { THEVALUE }
                }
                pub fn set(&mut self, val: $type) {
                    unsafe {
                        THEVALUE = val;
                    }
                }
            }
            $name
        };
    };
}

pub mod keymap;
mod zork;
