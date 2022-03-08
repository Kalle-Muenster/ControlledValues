/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      SawStacker.hpp                             ||
||     Author:    Autogenerated                              ||
||     Generated: 07.02.2021                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _SawStacker_hpp_
#define _SawStacker_hpp_

#include "..\..\inc\ControllerAccess.h"

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

    enum SawStackerSpin { UP = 1, DOWN = -1 };
    template<typename cT>
    class SawStacker : public STEPFLOW(VALUES,UserModeControl<cT>)
    { DeclareTwoPins(cT,WAVE,cT,FORM)
    protected:
        typedef STEPFLOW(VALUES,UserModeControl<cT>) BASE;
        virtual unsigned modeCodeBase(void) const {
            return *(unsigned*)"SawS";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(SawStacker);
        }
        virtual ulong usrModeData() const {
            return (ulong)(void*)this;
        }
        virtual void Init(void) {
            BASE::Init();
            FORM = ((MAX - MIN) / 2) + MIN;
            WAVE = ((MAX - MIN) / 2) + MIN;
            PIN_COUNT += 2;
        }
    public:
        virtual cT checkVALUE(cT* pVALUE) {
            *pVALUE = WAVE;
            WAVE = checkMODE(CtrlMode::Cycle);
            float frm = float((float)(float)FORM / ((MAX - MIN) / 2));
            return *pVALUE = cT((2.0f + ((frm*2.0f) * MathLib::abs(float((float)WAVE / ((MAX - MIN) / 2)))) * WAVE) / (2+frm*2));
        }
        SawStackerSpin spinDirection(SawStackerSpin spin = SawStackerSpin(0)) {
            if ( spin != 0 ) {
                if( spin != SPN )
                    spin = CHG;
            } else  spin = SPN;
            return  spin;
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif

#include "..\..\inc\ControllerAccess.h"
#endif
