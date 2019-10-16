#![no_std]
#![feature(alloc_prelude)]

#[macro_use]
extern crate alloc;

#[global_allocator]
static ALLOCATOR: qmk_sys::MallocFreeAlloc = qmk_sys::MallocFreeAlloc;

pub mod keymap;
