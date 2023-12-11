#![no_std]
#![no_main]
#![feature(panic_info_message)]

#[macro_use]
mod console;
mod lang_items;
mod sbi;

use core::arch::global_asm;
global_asm!(include_str!("entry.asm"));

fn clear_bss() {
    
    extern "C" {
        fn sbss();
        fn ebss();
    } // 使用 range（sbss 到 ebss）迭代 BSS 段内存范围
    (sbss as usize..ebss as usize).for_each(|a| {
        unsafe { (a as *mut u8).write_volatile(0) }
    });
}

#[no_mangle]
pub fn rust_main() -> ! {
    clear_bss();
    println!("Hello, world!");
    panic!("Shutdown machine!");
}

// fn main() {
//     // println!("Hello, world!");
// }
