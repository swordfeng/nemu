/* public header */
#pragma once
#include "cpu/decode.hh"

#include "data-trans/push.h"
#include "data-trans/mov.h"
#include "data-trans/xchg.h"

#include "ctrl-trans/call.h"
#include "ctrl-trans/jcc.h"

#include "arith/sub.h"
#include "arith/cmp.h"
#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/mul.h"

#include "logic/test.h"

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
