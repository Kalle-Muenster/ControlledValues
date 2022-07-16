/**************************************************\
|  ModeExpress.h - define controller builtin modes |
| which (in most case) are expression macros to be |
| placed within controller implemented check funcs |
\**************************************************/
#ifndef MODE_EXPRESSIONS_ENABLED
#define MODE_EXPRESSIONS_ENABLED

#include "FlgHlpConfig.h"

#define INVERTMOD ( IsInverted(this) ? cT(MAX-VAL) : cT(VAL) )
#define CLAMPMODE ( VAL = SpinDirect(this)*(VAL) < SpinDirect(this)*MIN \
                        ? MIN : SpinDirect(this)*(VAL) > SpinDirect(this)*MAX \
                        ? MAX : VAL )
#define DAMPFMODE ( MIN = VAL = ((MAX = cT(VAL - MIN)) > cT(0)) \
                                                   ? \
                     (MAX > MOV ? cT(MIN + MOV) : VAL) : (MAX < cT(0)) \
                                                          ? \
                         ((MAX < cT(MOV * -1)) ? cT(MIN - MOV) : VAL ) : VAL )
#define COMPRESSM ( VAL=(VAL > MIN) \
                             ? cT(((VAL - MIN) * MOV) / MAX + MIN) \
                             : (VAL <  (MIN * -1)) \
                             ? cT(((VAL + MIN) * MOV) / MAX - MIN) \
                       : VAL )
#define EXPANDMOD ( VAL=(VAL > MIN) \
                             ? cT(((VAL - MIN) / MOV) * MAX + MIN) \
                             : (VAL < (MIN * -1)) \
                             ? cT(((VAL + MIN) / MOV) * MAX - MIN) \
                       : VAL )
#define CYCLEMODE ( VAL = ( (VAL <= MIN) \
                          ? cT(MAX - (MIN - VAL)) \
                          : (VAL >= MAX) \
                          ? cT(MIN + (VAL - MAX)) : \
                    VAL ) + cT( MOV * SpinDirect(this) ) )
#define PINGPONG(DING) VAL += (MOV * SpinDirect(DING)); \
                           if (VAL < MIN) { \
                               ChangeSpin(DING); \
                               VAL = (MIN + (MIN - VAL)); \
                           } else \
                           if (VAL > MAX) { \
                               ChangeSpin(DING); \
                               VAL = (MAX - (VAL - MAX)); \
                           }

#else // disables on a secondary 'closing' include
#undef MODE_EXPRESSIONS_ENABLED
#include "FlgHlpConfig.h"

#undef INVERTMOD
#undef CLAMPMODE
#undef DAMPFMODE
#undef COMPRESSM
#undef EXPANDMOD
#undef CYCLEMODE
#undef PINGPONG

#endif