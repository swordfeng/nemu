#![crate_type = "lib"]
#![no_std]

extern {
    fn hit_good_trap() -> !;
    fn hit_bad_trap() -> !;
}

struct A {
    v: i32
}

struct B {
    v: i64
}

trait Tr {
    fn get(&self) -> i64;
    fn set(&mut self, val: i64);
}

impl Tr for A {
    fn get(&self) -> i64 {
        self.v as i64
    }
    fn set(&mut self, val: i64) {
        self.v = val as i32;
    }
}

impl Tr for B {
    fn get(&self) -> i64 {
        self.v
    }
    fn set(&mut self, val: i64) {
        self.v = val;
    }
}

fn add1<T1: Tr, T2: Tr>(a: &mut T1, b: &T2) {
    let v = a.get() + b.get();
    a.set(v);
}

fn add2(a: &mut Tr, b: &Tr) {
    let v = a.get() + b.get();
    a.set(v);
}

static C: [A; 2] = [A { v: 1 }, A { v: 2 }];

#[no_mangle]
pub fn main() {
    let a = A { v: 1 };
    let mut b = B { v: 2 };
    add1(&mut b, &a);
    add2(&mut b, &a);
    if b.v != 4 {
        unsafe { hit_bad_trap(); }
    }
    for t in &C {
        add1(&mut b, t);
    }
    if b.v != 7 {
        unsafe { hit_bad_trap(); }
    }
    unsafe { hit_good_trap(); }
}
