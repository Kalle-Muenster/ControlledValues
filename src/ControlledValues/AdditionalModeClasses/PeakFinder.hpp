/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      PeakFinder.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _PeakFinder_hpp_
#define _PeakFinder_hpp_

#ifdef BEGIN_STEPFLOW_NAMESPACE
#include "../Macros/InternalAccess.h"
BEGIN_STEPFLOW_NAMESPACE
#endif

    template<typename cT>
    cT TheTrigger(cT value) {
        return value;
    }

    template<typename cT>
    class PeakFinder
        : public ClambController<cT>
    {
    public:
        typedef cT(*OnNewPeakFoundFunc)(cT);
    protected:
        typedef ClambController<cT> BASE;
        virtual ModeCodeVal modeCodeBase(void) const { return *(ModeCodeVal*)"PEAK"; }
        virtual unsigned usrModeSize() const { return sizeof(PeakFinder); }
        virtual ulong usrModeData() const { return (ulong)(void*)this; }
        DeclarePin( OnNewPeakFoundFunc, TRIGGER )
    protected:
        virtual void Init(void) {
            BASE::Init();
            BASE::CLAMP=false;
            TRIGGER=&TheTrigger;
            PIN_COUNT++;
        }
    public:
        virtual cT checkVALUE(cT* pVALUE)
        {
            cT peak = *pVALUE;
            if (peak > MOV) {
                if (peak > MAX) {
                    peak = TRIGGER( BASE::checkVALUE(pVALUE) );
                    MAX = peak;
                }
            }
            else if ( peak < MOV) {
                if (peak < MIN) {
                    peak = TRIGGER( BASE::checkVALUE(pVALUE) );
                    MIN = peak;
                }
            }
            MOV = peak;
            return peak;
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW_NAMESPACE
#include "../Macros/InternalAccess.h"
#endif
#endif
