/**************************************************\
|  ModeExpress.h - define controller builtin modes |
| which (in most case) are expression macros to be |
| placed within controller implemented check funcs |
\**************************************************/
#ifndef MODE_EXPRESSIONS_ENABLED
#define MODE_EXPRESSIONS_ENABLED

#include "FlgHlpConfig.h"

#define INVERTMOD ( IsInverted(this) ? MAX-(VAL) : (VAL) )
#define CLAMPMODE ( VAL = SpinDirect(this)*(VAL) < SpinDirect(this)*MIN \
                        ? MIN : SpinDirect(this)*(VAL) > SpinDirect(this)*MAX \
                        ? MAX : VAL )
#define DAMPFMODE ( MIN = VAL = ((MAX = (cT)(VAL - MIN)) > cT(0)) \
                                                   ? \
                     (MAX > MOV ? MIN + MOV : VAL) : (MAX < cT(0)) \
                                                          ? \
                         ((MAX < cT(-MOV)) ? (cT)(MIN - MOV) : VAL) : VAL)
#define COMPRESSM ( VAL=(VAL > MIN) \
                             ? ((VAL - MIN) * MOV) / MAX + MIN \
                             : (VAL < -MIN) \
                             ? ((VAL + MIN) * MOV) / MAX - MIN \
                       : VAL )
#define EXPANDMOD ( VAL=(VAL > MIN) \
                             ? ((VAL - MIN) / MOV) * MAX + MIN \
                             : (VAL < -MIN) \
                             ? ((VAL + MIN) / MOV) * MAX - MIN \
                       : VAL )
#define CYCLEMODE ( VAL = ( (VAL <= MIN) \
                          ?  MAX - (MIN - VAL) \
                          : (VAL >= MAX) \
                          ?  MIN + (VAL - MAX) : \
                    VAL ) + ( MOV * SpinDirect(this) ) )
#define PINGPONG(DING) VAL += (MOV * SpinDirect(DING)); \
                           if (VAL < MIN) { \
                               ChangeSpin(DING); \
                               VAL = (MIN + (MIN - VAL)); \
                           } else \
                           if (VAL > MAX) { \
                               ChangeSpin(DING); \
                               VAL = (MAX - (VAL - MAX)); \
                           } return VAL

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