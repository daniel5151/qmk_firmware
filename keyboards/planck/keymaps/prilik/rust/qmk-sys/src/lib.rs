#![no_std]
#![feature(alloc_prelude)]
#![feature(alloc_error_handler)]
#![allow(clippy::empty_loop)]

// #[macro_use]
extern crate alloc;

pub mod bindgen;

mod malloc;
mod panic;

pub use malloc::MallocFreeAlloc;
