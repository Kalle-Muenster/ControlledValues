/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      DreiEcker.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 08.08.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _RampFormer_hpp_
#define _RampFormer_hpp_

#include "../Macros/InternalAccess.h"
#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

#if  ! ( defined(_SineFormer_hpp_) || defined(_PulsFormer_hpp_) )
       namespace pins {
           enum FormControl { WAVE = 0, FORM = 1 };
       }
#endif

    template<typename cT>
    class RampFormer : public UserModeControl<cT>
    { DeclareTwoPins(cT,WAVE,cT,FORM)
    protected:
        typedef UserModeControl<cT> BASE;
        virtual void Init(void) {
            BASE::Init();
            FORM = ((MAX-MIN)/2)+MIN;
            WAVE = ((MAX-MIN)/4)+MIN;
            BASE::PIN_COUNT += 2;
        }
        virtual unsigned modeCodeBase(void) const {
            return *(unsigned*)"Drei";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(RampFormer);
        }
        virtual ulong usrModeData() const {
            return (ulong)(void*)this;
        }
    public:
        virtual cT checkVALUE(cT* pVALUE) {
            // VALUE is triangle wave level at ACTUAL sample position. let's discard it and:
            // assign the ACTUAL sawstack level (which still should sit on the WAVE pin) to it.
            *pVALUE = WAVE;
            // then check for the NEXT sawstack samples level by doing a base mode 'Cycle' step.
            WAVE = BASE::checkMODE( Cycle );
            // The new Triangle sample got level assigned , which (regarding on sawstack level
            //   being actually above the FORM parameter or below the FORM parameter) is either:
            // - That point in the full range which is proportional to the full range, in the same way like
            //   the NEXT sawstack sample level is proportional to the LOWER half range (range below FORM)
            // - That point in the full range which is proportional to the full range, in the same way like
            //   the NEXT sawstack sample level is ANTI proportional to the UPPER half range (above FORM).
            if ( WAVE > FORM ) {
                 cT urg = MAX - FORM;
                *pVALUE = MIN + (cT)(((CONTROLER_INTERNAL_PRECISION)(urg - (WAVE - FORM)) / urg)*(MAX - MIN));
            } else {
                *pVALUE = MIN + (cT)(((CONTROLER_INTERNAL_PRECISION)(WAVE - MIN) / (FORM - MIN))*(MAX - MIN));
            } return BASE::checkMODE( Clamp );
        }
    };

#include "../Macros/InternalAccess.h"
#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif
#endif
