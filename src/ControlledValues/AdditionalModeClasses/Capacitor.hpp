/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      Condenser.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _Capacitor_hpp_
#define _Capacitor_hpp_

#include "../Macros/InternalAccess.h"

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

    // A 'Capacitor' wich stores assigned values by forwarding any assignments to it as 'addition to' a reservoir
    // buffer instead. the value itself will stay as is, as was before,... as long the reservior not is fully
    // charged (to MAX value). When the reservior is fully charged at least, it then 'flushes' to the value's own
    // main variable and the value than durantly stays at MAX level, - by also changing the controller's processing
    // direction to make any incomming assignments now being forwarded to the reservior by 'subtracting from' it
    // instead. As soon the reservior value is sunken below MIN value again, it again flushes to the main variable
    // and turns direction back to 'addition' again then.

    // when MOV is set to some value different then 0, MOV acts as a durant input value which makes the
    // value moving automatically each time it is reed. It then behaves like if it always got a MOV
    // assigned to it just befor each time it is reed. (or like: each reed operation performs a MOV
    // assignment before, implicitely) So the value begins oscilating between MIN and MAX by just reeding it.
    // The controller can be used as an oscillator so, which is following a pulse wave where oscillation
    // frequency can be controlled by changing the value of MOV.

    // pulswidth can be controlled by centering the the wave by giving MIN / MAX values not symetrically to
    // each other. means setting up the controller by MIN = -1 and MAX = +0.5 values would lead to a wave
    // form which has shorter, but in relaton to the MAX phase, higher leveled MIN phase and a longer, but
    // in relation to the MIN phase, lower leveled MAX phase (but causes, when changes are not applied per
    // oscillation cycle but frame (or sample) wise instead, some 'glitching' arround that logically expected
    // exact zero crossing possition may happen. a full oscyllation cycle at set frequency value expectedly should hit
    // - so when doing puls width modulation via constantly reducing MIN / MAX values by an lfo for example
    // such oscyllator would begin swimming slightly

    template<typename cT>
    class Capacitor
        : public UserModeControl<cT>
    { DeclarePin( cT, BUF )
    protected:
        typedef UserModeControl<cT> BASE;
        virtual void Init(void) {
            BASE::Init();
            BUF = *BASE::controller->GetPointer();
            BASE::PIN_COUNT++;
        }
        virtual  ModeCodeVal modeCodeBase(void) const {
            return *(ModeCodeVal*)"CPTR";
        }
        virtual uint usrModeSize() const {
            return sizeof(Capacitor);
        }
        virtual ulong usrModeData() const {
            return (ulong)(void*)this;
        }
    public:
        virtual cT checkVALUE(cT* pVALUE) {

            cT val = *pVALUE;
            val = cT( ( val != MAX && val != MIN) ? (MAX-val)*(MAX-val) < (val-MIN)*(val-MIN) ? MAX : MIN : val );
            BUF = ( val < MAX ? INV ? cT(BUF-(MOV*((MIN * -1)/MAX))) : cT(BUF+(MOV*((MIN * -1)/MAX)))
                  : val > MIN ? INV ? cT(BUF+(MOV*((MAX * -1)/MIN))) : cT(BUF-(MOV*((MAX * -1)/MIN)))
           :BUF   );

            if (BUF > MAX) {
                BUF = MAX - (BUF - MAX);
                val = INV ? MIN : MAX;
            } else
            if (BUF < MIN) {
                BUF = MIN + (BUF - MIN);
                val = INV ? MAX : MIN;
            } return *pVALUE = val;
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif
#include "../Macros/InternalAccess.h"
#endif
