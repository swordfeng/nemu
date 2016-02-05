#![crate_type = "lib"]
#![no_std]

extern {
    fn good_trap() -> !;
}

#[no_mangle]
pub fn main() {
    unsafe {
        good_trap();
    }
}
