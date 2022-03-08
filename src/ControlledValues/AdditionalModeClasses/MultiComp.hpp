/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      MultiComp.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 12.02.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _MultiBandCompressor_hpp_
#define _MultiBandCompressor_hpp_

#include "BandSplit.hpp"
#define READONLY_PINS
#include <ControllerAccess.h>

#ifdef BEGIN_STEPFLOW_NAMESPACE
BEGIN_STEPFLOW(VALUES)
#endif


#define THRSHS(band) cmp[band].getMIN()
#define GAIN(band) cmp[band].getMAX()
#define InGainConversion MIN
#define ConversionFactor stm
#define SPLITS (BANDS-1)
#define ByPass CLAMP

    template<typename cT> class VariableController;
    template<typename cT,const uint BANDS,typename pT = double>
    class MultiComp : public BandSplit<cT,BANDS,4,pT>
    {
    public:
        enum PinName {
           BAND_WAIGHT = 0, // inflicts thresholds and relation between own and side reductivity (e.g. multiplies 'the other' band ratios)
           BAND_MIXERS = BANDS,
           BAND_RATIOS = (BANDS*2),  //
           BAND_SPLITS = (BANDS*3),
           RATE = BAND_SPLITS + (BANDS - 1),
           BYPASS = RATE + 2, // CLAMP
           INPUT_GAIN = BYPASS + 2, // MIN
           MAIN_RATIO, // MAX ?
           OUTPUT_PEAK // MOV  ?
        };
        virtual void* Pin(void* pin, int pindex) {
            const int bandex = pindex % BANDS;
            switch (pindex-bandex) {
            case PinName::BAND_WAIGHT: return &cmp[bandex].GetPin<pT>(1);
            case PinName::BAND_MIXERS: return &cmp[bandex].GetPin<pT>(2);
            case PinName::BAND_RATIOS: return &cmp[bandex].GetPin<pT>(3);
            case PinName::BAND_SPLITS: if (bandex >= SPLITS) return &frq[bandex];
            default: return BASE::Pin( pin, pindex-RATE );
            }
        }
    protected:
        typedef BandSplit<cT, BANDS, 4, pT> BASE;
        PinFieldController<pT> cmp[BANDS];
        PinJack<pT> inp;
        uint frq[SPLITS];

        virtual pT getSplitPoint(int idx) {
            return (pT)frq[idx];
        }
        virtual void Init(void) {
            if (std::numeric_limits<cT>().is_integer) {
                MIN = std::numeric_limits<cT>().max();
                MAX = std::numeric_limits<cT>().max()/3;
            } else {
                MIN = cT(1);
                MAX = cT(1.0/3.0);
            } MOV = cT(0);
            SampleRate(44100);
            for (int i = 0; i < BANDS; ++i) {
                cmp[i].SetUp(pT(0.75*MIN),pT(MAX),0,pT(MIN)/0.75f,CtrlMode::Compress);
                cmp[i].SetVariable( &bnd[i] );
                cmp[i].SetCheckAtGet( true );
                //cmp[i].LetPoint( ControllerVariables::Min, controller->getMINpt() );
                //cmp[i].LetPoint( ControllerVariables::Max, controller->getMAXpt() );

                cmp[i].Active = true;
                if( i < SPLITS )
                    frq[i] = ( (SampleRate()/2.0) / (BANDS-1) ) * (i+1);
            } BASE::PIN_COUNT += BANDS;
            BASE::Init();
        }
        virtual void onParameterUpdate(void) {
        //    for (int i = 0; i < BANDS; ++i) {
        //        cmp[i].SetMin()
        //    }
        }
        virtual cT checkVALUE(cT* pVALUE) {
            // split input and get bypassed, delayed signal
            cT val = doBandSplit( ((pT)*pVALUE)
                                 / InGainConversion
                                  );
            if (!ByPass) {
             // mix sample from (individually compressed) band levels
                pT mix = 0;
                for( int b = 0; b < BANDS; ++b ) {
                    mix += cmp[b];
                } val = ( mix * ConversionFactor );
            } // store found peak level
            MOV = val < 0
                ? maxOf(-val, MOV)
                : maxOf(val, MOV);
            // return the processed sample
            return (*pVALUE = val);
        }
        virtual void onActivate(bool active) {
            resetState();
        }
        virtual unsigned modeCodeBase(void) const {
            return *(unsigned*)"Mult";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(MultiComp);
        }
        virtual ulong usrModeData() const {
            return (ulong)(void*)this;
        }
    };

#undef InGainConversion
#undef ConversionFactor
#undef THRSHS
#undef GAIN
#undef SPLITS
#undef ByPass

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif

#include <ControllerAccess.h>
#endif
