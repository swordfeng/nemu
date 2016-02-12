/* public header */
#pragma once
#include "cpu/decode.hh"

#include "data-trans/push.h"
#include "data-trans/mov.h"
#include "data-trans/xchg.h"
#include "data-trans/pop.h"
#include "data-trans/leave.h"
#include "data-trans/cwtl.h"
#include "data-trans/cltd.h"
#include "data-trans/movsx.h"
#include "data-trans/movzx.h"
#include "data-trans/cmovcc.h"

#include "ctrl-trans/call.h"
#include "ctrl-trans/jcc.h"
#include "ctrl-trans/jmp.h"
#include "ctrl-trans/ret.h"

#include "arith/sub.h"
#include "arith/cmp.h"
#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/mul.h"
#include "arith/add.h"
#include "arith/adc.h"
#include "arith/imul.h"
#include "arith/div.h"
#include "arith/idiv.h"
#include "arith/sbb.h"

#include "logic/test.h"
#include "logic/and.h"
#include "logic/xor.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/shl.h"
#include "logic/shld.h"
#include "logic/sar.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "logic/setcc.h"

#include "string/movs.h"
#include "string/stos.h"
#include "string/cmps.h"

#include "flagctl/cld.h"
#include "flagctl/std.h"

#include "misc/lea.h"
#include "misc/nop.h"

#include "special/special.h"

#include "system/lgdt_lidt.h"
