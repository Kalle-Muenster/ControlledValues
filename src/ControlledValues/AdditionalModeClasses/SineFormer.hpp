/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      SineOscillator.hpp                         ||
||     Author:    Autogenerated                              ||
||     Generated: 19.12.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _SineFormer_hpp_
#define _SineFormer_hpp_


#ifdef BEGIN_STEPFLOW_NAMESPACE
#include "..\..\inc\ControllerAccess.h"
       BEGIN_STEPFLOW( VALUES )
#endif

#if  ! ( defined(_RampFormer_hpp_) || !!defined(_PulsFormer_hpp_) )
           namespace pins {
           enum FormControl { WAVE = 0, FORM = 1 };
       }
#endif

    template<typename cT>
    class SineFormer
        // Sine wave generator with 'FORM' parameter which morphes the generated
        // output between a rgular Sine wave (at FORM = 0DB) to different kind of
        // wave forms:
        // - as closer FORM gets to (MIN) the more triangle characteristics the sine will gain
        // - as closer FORM gets to (MAX) the more impulse characteristics the sine will gain
        // the generated wave form gains.
        : public UserModeControl<cT>
    {
        DeclareTwoPins(cT, WAVE, cT, FORM)
    protected:
        typedef UserModeControl<cT> BASE;
        virtual void Init(void)
        {
            WAVE = 0;
            BASE::Init();
            BASE::PIN_COUNT += 2;
            FORM = (MAX-MIN)/2;
        }
        virtual uint        usrModeSize() const {
            return sizeof(SineFormer);
        };
        virtual ModeCodeVal modeCodeBase(void) const {
            return *(ModeCodeVal*)"SINE";
        }
        virtual ulong       usrModeData(void) const {
            return (ulong)(void*)this;
        }
    public:
        virtual cT checkVALUE(cT* pVALUE)
        {
            cT halbRange = (MAX - MIN) / 2;

            WAVE += (MAX * MOV) / (MAX - MIN);
            WAVE  = WAVE >= halbRange
                  ? cT( WAVE - halbRange )
                  : WAVE;

            *pVALUE = (cT)( ( MathLib::sin(
                         (WAVE*imKreis) / halbRange
                                      ) * halbRange
                                    ) + ((halbRange * -1) - MIN)
                                  );

            // morph it by FORM parameter from sawstack (MIN) over a plain
            // sine (at 0DB) to (at MAX) an impulse peak which leaves range
            // against +inf and returns back valid again from below -inf.
            // The actual point where +/-infinity is crossed, will always
            // lay 'between' two actual sample values of concrete levels.
            if (FORM != halbRange) {
                double mix = ( (double) (FORM - MIN) / halbRange ) - 1.0;
                   *pVALUE = ( *pVALUE * (1.0 - MathLib::abs(mix)) )
                           - ( ((WAVE + WAVE) + MIN) * mix );
            }
            return *pVALUE;
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
#include "..\..\inc\ControllerAccess.h"
       ENDOF_STEPFLOW( VALUES )
#endif
#endif
