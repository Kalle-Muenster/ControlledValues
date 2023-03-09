/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      PulseTrigger.hpp                           ||
||     Author:    Kalle                                      ||
||     Generated: 25.08.2019                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _PulsFormer_hpp_
#define _PulsFormer_hpp_


#include "../Macros/InternalAccess.h"
#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

    /*
    Approach wich generates pulse waves by abstracting samples
    from a sawstack which the base implemented Cycle mode produces
    by checking that sawstack if actually above or beneath a FLUSH
    point threshold value which is defined by a FORM parameter as
    distance to the waves center/0db value. So each time the cycle
    sawstack passes the actual FLUSH point value, a phase switch is
    applied to the output, so its generating a pulse wave at least.
    */

#if ! ( defined(_RampFormer_hpp_) || defined(_SineFormer_hpp_) )
    namespace pins {
       enum FormControl { WAVE = 0, FORM = 1 };
    }
#endif

    template<typename cT>
    class PulseFormer : public UserModeControl<cT>
    { DeclareTwoPins( cT, WAVE, cT, FORM )
    protected:
        typedef UserModeControl<cT> BASE;
        virtual void Init(void) {
            BASE::Init();
            WAVE = *BASE::controller->GetPointer();
            FORM = ( (MAX - MIN) / 2 ) + MIN;
            BASE::PIN_COUNT += 2;
        }
        virtual ModeCodeVal modeCodeBase(void) const {
            return *(ModeCodeVal*)"PILZ";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(PulseFormer);
        }
        virtual ulong    usrModeData() const {
            return (ulong)(void*)this;
        }
    public:
        virtual cT checkVALUE( cT* pVALUE ) {
         // VALUE is pulse wave level at ACTUAL sample position:
         // at first assign TO it: the last sample's 'sawstack' level (which still should sit on the WAVE pin).
           *pVALUE = WAVE;
         // then check for the NEXT, new sawstack sample by the builtin 'Cycle' mode, and store the NEW value on the WAVE pin.
            WAVE = BASE::checkMODE( Cycle );
         // from that sawstack wave's actual value a new sample is abstracted by assigning to VAL: (depending on WAVE is currently
         // above or below FORM) either value of MAX or value of MIN, shifted by actual distance of FORM value to 0db (center)
            cT db0 = cT( ((MAX - MIN) / 2) + MIN );
            return *pVALUE = ( WAVE > FORM )
                 ? ( FORM < db0) ? cT(MAX - (db0 - FORM)) : MAX
                 : ( FORM > db0) ? cT(MIN + (FORM - db0)) : MIN;
        // ...and then VAL is returned as the pulse waves NEXT sample value.
        }
    };

#include "../Macros/InternalAccess.h"
#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif
#endif
