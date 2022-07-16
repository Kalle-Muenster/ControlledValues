/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFilter.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 28.07.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFilters_hpp_
#define _WaveFilters_hpp_

#define FILTER_BANDS 3
#define FILTER_POLES 6

#include "BandSplit.hpp"

/* EQ Bands Filter implementation:

(n-bands, each k-poles)

 where n describes count on FILTER_BANDS:
    so n-band cuttoffs (f1 to fn) will be calculated with each sample input

 where k describes count on FILTER_POLES:
    so the poles will cause a latency of k-1 samples at the output! 


- low-band: f1 output  (lowshelf level: f1)
- fn-bands: in between (bandpass level: fn + (input - (f1 out + (f2 out + (... + (fn out))))
- hig-band: fn output (highshelf level: input - fn)

  pin 0 to n     : parameter for adjusting each bands output level
  pin n          : parameter for adjusting input signal before split
  pin n to 2n    : measured peak level returns per each output band
  pin 2n to 3n   : the effective output levels returned for each band
  pin 3n to 4n-1 : individual split frequencies between bands (in Hz) 
  pin 4n         : playback sample rate (if not set, defaults to 44100 Khz)
  pin 4n+1       : dirt thrower (aknowledge the filter to re-read pined values when pins plugged in/out).    
  pin 4n+3       : Bypass (deactivates band processing, but keeps output delay of n-1 samples acurate.)

most pins need to have 'resetState()'
which need filter-reset in any case for being changed.
these can NOT be changed 'on-the-fly' during processing
(as long thread safe access isn't enabled via defining
THREAD_WAITCYCLE_FUNC before including this controller)

peakvalue:
pin3 will always hold highest output level
detected during progress and will reset to
zero on filter state reset by 'resetState()' */


#define READONLY_PINS
#include "../Macros/InternalAccess.h"
#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif
#define GAINS            bC+1
#define SPLIT            bC-1
#define ByPass           (BASE::CLAMP & 0x01)
#define Compression      (BASE::CLAMP & 0x80)
#define PreCompress      ( (BASE::CLAMP & 0x82) == 0x82 )
#define PostCmpress      ( (BASE::CLAMP & 0x80) && (!(BASE::CLAMP & 0x02)) )
#define ConversionFactor BASE::stm
#define PIN_INVALID      void(*INVALID)(void) 
#define FILTER_PINS      ( GAINS + GAINS + SPLIT )
#define InGainConversion ( BASE::stm / gain[EQFilterPindices<bC>::IN_GAIN].read() )

//#define PINPROP(Ident,Typus,Range)  __declspec(property(get=get##Ident,put=set##Ident)) Typus Ident[Range];

/*
namespace pins {
enum DreiBandEQ {
    LO_GAIN, // precision type (ptr)
    MI_GAIN, // precision type (ptr)
    HI_GAIN, // precision type (ptr)
    IN_GAIN, // precision type (?

    LO_PEAK, // precision type
    MI_PEAK, // precision type
    HI_PEAK, // precision type
    OUTPEAK, // precision type

    LoSplit,
    HiSplit,

    LO_BAND, // precission type
    MI_BAND, // precision type
    HI_BAND, // precision type

    SmplFrq, // uint type

    // one bool pin between not used
    BY_PASS = SmplFrq+2, // boolean
    PEAKVAL = BY_PASS+4 // sample type
};
}
*/

template<const unsigned bC>
struct EQFilterPindices {
    const static int LO_GAIN = 0;
    static int GAIN(int n) { return int(n - 1); }
    const static int HI_GAIN = bC - 1;
    const static int IN_GAIN = bC;

    const static int LO_PEAK = bC + 1;
    static int PEAK(int n) { return int(n + bC); }
    const static int HI_PEAK = bC + bC;
    const static int OUTPEAK = HI_PEAK + 1;

    const static int LOSPLIT = OUTPEAK + 1;
    static int SPLT(int n) { return int(n + OUTPEAK); }
    const static int HISPLIT = LOSPLIT + (bC - 1);

    const static int LO_BAND = HISPLIT + 1;
    static int BAND(int n) { return int(n + HISPLIT); };
    const static int HI_BAND = LO_BAND + bC;

    const static int SmplFrq = HI_BAND + 1;
    const static size_t INVALID = SmplFrq+1;

    const static int BY_PASS = INVALID + 1;
    const static int CpThrsh = BY_PASS + 1;
    const static int CmprOne = CpThrsh + 1;
    const static int CpMinus = CmprOne + 1;
};

template< typename cT, const unsigned bC = FILTER_BANDS, const unsigned pC = FILTER_POLES, typename prcT = double >
class EQBandFilter : public BandSplit<cT,bC,pC,prcT>
{ public:
    typedef BandSplit<cT, bC, pC, prcT> BASE;

protected:
    struct FilterSplitPin : public PinJack<prcT> {
        FilterSplitPin() : PinJack<prcT>() {}
        FilterSplitPin( ulong umode ) : PinJack<prcT>( umode ) {}
        FilterSplitPin( const FilterSplitPin& copy ) : PinJack<prcT>() { pin.var = copy.pin.var; ext.ptr = copy.ext.ptr; }
        FilterSplitPin& operator =( prcT* point ) {
            this->pin.ptr = point;
            this->external( true );
            return *this;
        }
        FilterSplitPin& operator =( prcT set ) {
            if ( this->ext.flg < 0 )  *this->pin.ptr = set;
            else this->pin.var = set;
            return *this;
        }
        virtual void write( prcT value ) override {
            PinJack<prcT>::write( value );
            PinJack<prcT>::umode<EQBandFilter>()->parametersDirty();
        }
    };
    struct FilterPeaktPin : public PinJack<prcT> {
        FilterPeaktPin& operator =( prcT* point ) {
            this->pin.ptr = point;
            this->external( true );
            return *this;
        }
        FilterPeaktPin& operator =( prcT set ) {
            this->write( set );
            return *this;
        }
        void set( prcT value ) {
            const prcT newv = MathLib::abs( value );
            const prcT oldv = this->read();
            this->write( maxOf( newv, oldv ) );
        }
    };
    PinJack<prcT>  gain[GAINS];
    FilterPeaktPin peak[GAINS];
    FilterSplitPin splt[bC-1];
    virtual void* Pin( void* pin, int idx ) {
      outer_locked_scope
        if( idx < EQFilterPindices<bC>::LO_PEAK) {
            if (pin) gain[idx] = (prcT*)pin;
            --this->locked;
            return (prcT*)gain[idx];
        } else
        if( idx < (EQFilterPindices<bC>::LOSPLIT) ) {
            idx -= EQFilterPindices<bC>::LO_PEAK;
            if (pin) peak[idx] = (prcT*)pin;
            --this->locked;
            return (prcT*)peak[idx];
        } else
        if( idx < EQFilterPindices<bC>::LO_BAND ) {
            idx -= EQFilterPindices<bC>::LOSPLIT;
            if (pin) splt[idx] = (prcT*)pin;
            --this->locked;
            return (prcT*)splt[idx];
        } else {
            --this->locked;
        return BASE::Pin( pin, idx - FILTER_PINS ); }
      close_locked_scope(2)
    }
    virtual ModeCodeVal modeCodeBase(void) const {
        return (bC | pC << 4) + *(ModeCodeVal*)"BAND";
    }
    virtual uint usrModeSize() const {
        return sizeof(EQBandFilter);
    }
    virtual ulong usrModeData() const {
        return (ulong)(void*)this;
    }
    virtual prcT getSplitPoint( int idx ) {
        return splt[idx];
    }
    virtual void onParameterUpdate( void ) {
    }
    virtual void Init( void ) {
        for ( int i = 0; i < GAINS; ++i ) {
            gain[i] = 1.0f;
            peak[i] = 0.0f;
        } BASE::PIN_COUNT += FILTER_PINS;
        int f = ((44100 / 4) / SPLIT);
        for ( int i = 1; i < SPLIT; ++i ) {
            splt[i] = FilterSplitPin( (ulong)(void*)this );
            splt[i] = prcT( f * i );
        } splt[0] = FilterSplitPin( (ulong)(void*)this );
        splt[0] = prcT( f / 10 );
        BASE::SampleRate( 44100 );
        MIN = cT( std::numeric_limits<cT>::max() * 0.5f );
        MAX = 100;
        MOV = cT( 100.0f / 3.0f );
        BASE::Init();
        BASE::CLAMP = 0x82;
    }
    virtual cT checkVALUE( cT* pVALUE ) {
        cT val = BASE::doBandSplit( // split input at split frequencies
            prcT( PreCompress ? BASE::checkMODE( Compress ) : *pVALUE )
             / InGainConversion );
        inner_locked_scope
            if( !ByPass ) { // If not ByPassed: mix output...
                prcT out = 0;
                for( int b = 0; b < bC; ++b ) {
                    const prcT ampl = BASE::bnd[b] * gain[b].value;
                    out += ampl;
                    // ..., store peak level per each band
                    peak[b].set( ampl );
                } // ...and store peak level of output summ
                peak[bC].set( out ); 
                if ( PostCmpress ) {
                    *pVALUE = cT( out * MIN );
                    val = BASE::checkMODE( Compress );
                } else {
                    val = cT( out * ConversionFactor );
                }
            } this->locked = 0;
            return *pVALUE = val;
        close_locked_scope(2)
    }
    virtual void onActivate( bool active ) {
        BASE::resetState();
    }
public:
    prcT getGain( int idx ) {
        return  gain[idx].value;
    }
    void setGain( prcT val, int idx ) {
      outer_locked_scope
        gain[idx].value = val;
        return;
      close_locked_scope(2)
    }
    prcT getPeak(int idx) {
        return  peak[idx].value;
    }
    void setPeak(prcT val, int idx) {
      outer_locked_scope
        peak[idx].value = val;
        return;
      close_locked_scope(2)
    }
    prcT getSplit( int idx ) {
        return splt[idx].value;
    }
    void setSplit( prcT val, int idx ) {
      outer_locked_scope
        splt[idx].value = val;
        return;
      close_locked_scope(2)
    }
    PROPLIST( prcT, Gain, GAINS );
    PROPLIST( prcT, Peak, GAINS );
    PROPLIST( prcT,Split, SPLIT );
};

#undef close_locked_scope
#undef enter_locked_scope
#undef log_lock_state
#ifdef log_wait_state
#undef log_wait_state
#endif
#undef InGainConversion
#undef ConversionFactor
#undef PINPROP
#undef ByPass
#undef outer_locked_scope
#undef inner_locked_scope
#undef PIN_INVALID
#undef FILTER_PINS
#ifdef THREADSAFE_CONTROLLER
#undef THREADSAFE_CONTROLLER
#undef do_wait_cycles
#endif

#include "../Macros/InternalAccess.h"
#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW(VALUES)
#endif
#endif

