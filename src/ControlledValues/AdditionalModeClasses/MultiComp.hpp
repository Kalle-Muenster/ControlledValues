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
       BEGIN_STEPFLOW( VALUES )
#endif

#define THRSHS(band) cmp[band].getMIN()
#define GAIN(band) cmp[band].getMAX()
#define InputConversion ( BASE::stm / inp.read() )
#define ConversionFactor BASE::stm
#define ByPass BASE::CLAMP

    namespace pins {
        template<const unsigned bandcount>
        struct MultiBandPin {
           const static uint INPUT_GAIN  = 0u;
           static uint MIXERS(int bndidx) { return uint(INPUT_GAIN+bndidx); }
           const static uint OUTPUT_GAIN  = bandcount + 1u;
           static uint WAIGHT(int bndidx) { return uint(OUTPUT_GAIN+bndidx); }
           const static uint OUTPUT_PEAK = (OUTPUT_GAIN+bandcount) + 1u;  
           static uint RATIOS(int bndidx) { return uint(OUTPUT_PEAK+bndidx); }
           static uint SPLITS(int sltidx) { return uint(OUTPUT_PEAK+bandcount+sltidx); }
           // first pin in BandSplit (base class)
           const static uint BAND_LEVELS = (OUTPUT_PEAK+bandcount+bandcount)-1u; 
           static uint LEVELS(int bndidx) { return uint(BAND_LEVELS+(bndidx-1)); }
           // from here pins are located in base classes:
           const static uint SAMPLE_RATE = (BAND_LEVELS+bandcount)+1u;
           const static uint INVALIDATOR = SAMPLE_RATE + 1u;
           const static uint BYPASS      = SAMPLE_RATE + 2u;
           const static uint THRESHOLD   = SAMPLE_RATE + 4u; // ontroller MIN 
           const static uint COMPRESSO   = SAMPLE_RATE + 5u; // ontroller MAX 
           const static uint COMPRESSI   = SAMPLE_RATE + 6u; // ontroller MOV 
        };
    }

    template<typename cT> class VariableController;
    template<typename cT,const uint BANDS,typename pT = double>
    class MultiComp : public BandSplit<cT,BANDS,4,pT>
    {
    public:
        virtual void* Pin( void* pin, int pindex ) {
            if ( pindex ) {
                if ( pindex < pins::MultiBandPin<BANDS>::OUTPUT_GAIN ) {
                    const int bandex = (pindex % BANDS)-1;
                    if(pin) cmp[bandex].PTR[2] = (pT*)pin;
                    return cmp[bandex].getMAXpt();
                } else if ( pindex == pins::MultiBandPin<BANDS>::OUTPUT_GAIN ) {
                    if(pin) oup = (pT*)pin;
                    return (pT*)oup; 
                } else if ( pindex < pins::MultiBandPin<BANDS>::OUTPUT_PEAK ) {
                    const int bandex = (pindex % BANDS)-1;
                    if(pin) cmp[bandex].PTR[1] = (pT*)pin;
                    return cmp[bandex].getMINpt();
                } else if ( pindex == pins::MultiBandPin<BANDS>::OUTPUT_PEAK ) {
                    if(pin) peak = *(cT*)pin;
                    return (cT*)&peak; 
                } else if ( pindex < pins::MultiBandPin<BANDS>::SPLITS(1) ) {
                    const int bandex = (pindex % BANDS)-1;
                    if(pin) cmp[bandex].PTR[3] = (pT*)pin;
                    return cmp[bandex].getMOVpt();              
                } else if ( pindex < pins::MultiBandPin<BANDS>::LEVELS(1) ) {
                    const int splidex = (pindex % (BANDS-1))-1;
                    if(pin) frq[splidex] = *(cT*)pin;
                    return &frq[splidex];
                } else
                    return BASE::Pin( pin, pindex-(pins::MultiBandPin<BANDS>::BAND_LEVELS+1) );
            } else {
                if(pin) inp = (pT*)pin;
                return (pT*)inp;
            }
        }
    protected:
        typedef BandSplit<cT, BANDS, 4, pT> BASE;
        cT                     peak;
        PinJack<pT>            inp;
        PinJack<pT>            oup;
        PinFieldController<pT> cmp[BANDS];
        uint                   frq[BANDS-1];

        virtual pT getSplitPoint(int idx) {
            return (pT)frq[idx];
        }
        virtual void Init(void) {
            if( std::numeric_limits<cT>().is_integer ) {
                MIN = std::numeric_limits<cT>().max();
                MAX = std::numeric_limits<cT>().max()/cT(3);
            } else {
                MIN = cT(1.0);
                MAX = cT(1.0/3.0);
            } MOV = cT(0);
            BASE::sampleRate(44100);
            for (int i = 0; i < BANDS; ++i) {
                cmp[i].SetUp(pT(0.75*MIN),pT(MAX),0,pT(MIN)/0.75f,CtrlMode::Compress);
                cmp[i].SetVariable( &BASE::bnd[i] );
                cmp[i].SetCheckAtGet( true );
                //cmp[i].LetPoint( ControllerVariables::Min, controller->getMINpt() );
                //cmp[i].LetPoint( ControllerVariables::Max, controller->getMAXpt() );

                cmp[i].Active = true;
                if( i < BANDS-1 )
                    frq[i] = ( (BASE::sampleRate()/2.0) / (BANDS-1) ) * (i+1);
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
            cT val = BASE::doBandSplit( ((pT)*pVALUE)
                                       / InputConversion
                                        );
            if (!ByPass) {
             // mix sample from (individually compressed) band levels
                pT mix = 0;
                for( int b = 0; b < BANDS; ++b ) {
                    mix += cmp[b];
                } val = ( mix * oup ) * BASE::stm;
            } // store found peak level
            peak = val < 0
                 ? cT( maxOf( cT(val * -1), peak ) )
                 : cT( maxOf( val, peak ) );
            // return the processed sample
            return (*pVALUE = val);
        }
        virtual void onActivate(bool active) {
            BASE::resetState();
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

#undef InputConversion
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
