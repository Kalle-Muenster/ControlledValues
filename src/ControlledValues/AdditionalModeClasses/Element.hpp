/*
     This implements a control mode which is based uppon ||
    same logical mechanism which I used in the CD-player ||
   project for UI element controler there. it's used for ||
  handling buttons, sliders, jogdials, or Midi-Controler ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _Element_hpp_
#define _Element_hpp_

#include "../Macros/InternalAccess.h"
#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

namespace pins {
    enum Element {
        PEAK = 0, LAST = 1, CYCLED = 2,
        UNSIGNED = 3, CLAMPED = 4,
        VAR_VAL, VAR_MIN, VAR_MAX, VAR_MOV
    };
}

template<typename cT>
class Element : public ClambController<cT>
{ protected:
    typedef ClambController<cT> BASE;

    cT PIN_PEAK;
    cT PIN_LAST;
    bool PIN_UNSIGNED;
    bool PIN_ISCYCLED;

    virtual void Init(void) {
        PIN_LAST = PIN_PEAK = 0;
        PIN_ISCYCLED = false;
        PIN_UNSIGNED = false;
        PIN_COUNT += 4;
    }

    virtual ModeCodeVal modeCodeBase(void) const {
        return *(ModeCodeVal*)"ELMT";
    }
    virtual uint usrModeSize() const {
        return sizeof(Element);
    }
    virtual ulong usrModeData() const {
        return (ulong)(void*)this;
    }
public:
    virtual void* Pin( void*pin, int idx ) {
        switch ( idx ) {
        case pins::PEAK: return (void*) &PIN_PEAK;
        case pins::LAST: return (void*) &PIN_LAST;
        case pins::CYCLED: return (void*) &PIN_ISCYCLED;
        case pins::UNSIGNED: return (void*) &PIN_UNSIGNED;
        default: return BASE::Pin( pin, idx - 4 );
        }
    }

    virtual cT checkVALUE( cT* value ) {
                     cT VAL = *value;
        if( PIN_UNSIGNED && VAL < 0 )
            VAL = -VAL;

        MOV = VAL - PIN_LAST;

        if( PIN_ISCYCLED )
            MOV = MOV <= -((MAX - MIN) - 1)
                      ?
            MOV + MAX : MOV >= ((MAX - MIN) - 1)
                            ?
                  MOV - MAX : MOV;

        *value = VAL < MIN
                     ? PIN_ISCYCLED ? MAX - (MIN - VAL)
                                    : VAL
               : VAL > MAX
                     ? PIN_ISCYCLED ? MIN + (VAL - MAX)
                                    : VAL
               : VAL;

        PIN_PEAK = (MOV > 0)
                 ? *value
                 : PIN_PEAK;

        return *value = PIN_LAST = BASE::checkVALUE( value );
    }

    bool cycled(bool shouldBeCycled) {
        return PIN_ISCYCLED = shouldBeCycled;
    }

    bool forceUnsigned(bool shouldBeUnsigned) {
        return PIN_UNSIGNED = shouldBeUnsigned;
    }
};


#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif
#include "../Macros/InternalAccess.h"

#endif
