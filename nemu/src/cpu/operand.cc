#include "cpu/decode.hh"
#include "decode.tcc"

Operand::Operand(): type(opt_undefined), sreg(0xff) {}

uint32_t Operand::getSignedValue() {
   uint32_t ret = getUnsignedValue();
   return signed_extend(ret, size);
}

uint32_t Operand::getUnsignedValue() {
   uint32_t ret = 0;
   switch (type) {
   case opt_register:
       ret = reg_read_index(reg_index, size);
       break;
   case opt_address:
       ret = swaddr_read(address, size, sreg);
       break;
   case opt_immediate:
       ret = immediate;
       break;
   case opt_register_cr:
       ret = reg_cr_read_index(reg_index);
       break;
   case opt_register_seg:
       ret = cpu.sr[reg_index].sel;
       break;
   case opt_undefined:
       panic("operand undefined");
   }
   return ret;
}

swaddr_t Operand::getAddress() {
    Assert(type == opt_address, "operand is not from memory address");
    return address;
}

void Operand::setValue(uint32_t v) {
    switch (type) {
    case opt_undefined:
        panic("operand undefined");
    case opt_immediate:
        panic("set value to imediate");
    case opt_register:
        if (size == 1) reg_b(reg_index) = static_cast<uint8_t>(v);
        else if (size == 2) reg_w(reg_index) = static_cast<uint16_t>(v);
        else reg_l(reg_index) = v;
        break;
    case opt_address:
        swaddr_write(address, size, sreg, v);
        break;
    case opt_register_cr:
        reg_cr_set(reg_index, v);
        break;
    case opt_register_seg:
        cpu.sr[reg_index].sel = v;
        sreg_load(reg_index);
        break;
    }
}

string Operand::suffix() {
    switch (size) {
    case 1:
        return "b";
    case 2:
        return "w";
    case 4:
        return "l";
    }
    panic("wrong size");
    return "";
}
