#![crate_type = "lib"]
#![no_std]

extern {
    fn hit_good_trap() -> !;
    fn hit_bad_trap() -> !;
}

#[no_mangle]
pub fn main() {
    call(|| {
        call2(&|| {
            unsafe {
                hit_good_trap();
            }
        });
    });
}

fn call<T>(func: T) where T: Fn() {
    func();
}

fn call2(func: &Fn()) {
    func();
}
