/* public header */
#pragma once
#include "cpu/decode.hh"

#include "data-trans/push.h"
#include "data-trans/mov.h"
#include "data-trans/xchg.h"
#include "data-trans/pop.h"
#include "data-trans/leave.h"

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

#include "logic/test.h"
#include "logic/and.h"
#include "logic/xor.h"
#include "logic/or.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/sar.h"

#include "misc/lea.h"

#include "special/special.h"

/*
#include "arith/add.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "stack/leave.h"
#include "stack/ret.h"
#include "string/rep.h"
#include "misc/misc.h"
*/
