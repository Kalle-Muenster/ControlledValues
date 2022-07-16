/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      BandSplit.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 12.02.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _BandSplit_hpp_
#define _BandSplit_hpp_


#ifdef  THREAD_WAITCYCLE_FUNC
#define THREADSAFE_CONTROLLER 

#if defined(_DEBUG) && ENABLED_LOG_OUTPUT < 2
#define log_wait_state(cyc) LOG_MESSAGE_STREAM(__FUNCTION__)->Out("(): not got lock! retry in ")->Out(cyc*THREAD_WAITCYCLE_TIME)->Out(" milliseconds!\n");
#define log_lock_state(msg) LOG_MESSAGE_STREAM(__FUNCTION__)->Out("(): ")->Out(msg)->Out(" lock state is ")->Out(locked)->Out("\n");
#else
#define log_lock_state(msg)
#define log_wait_state(cyc)
#endif
#define do_wait_cycles(cycles) { log_wait_state(cycles) THREAD_WAITCYCLE_FUNC(cycles*THREAD_WAITCYCLE_TIME); }
#define enter_locked_scope(cond,incr) do if( this->locked cond 0 ) { incr##this->locked;
#define close_locked_scope(waittimer) } else do_wait_cycles(waittimer) while(true);
#define outer_locked_scope enter_locked_scope(>=,++) log_lock_state("outer")
#define inner_locked_scope enter_locked_scope(<=,--) log_lock_state("inner")
#else
#define log_lock_state(msg)
#define do_wait_cycles(cycles)
#define enter_locked_scope(cond,incr)
#define close_locked_scope(waittimer)
#define outer_locked_scope
#define inner_locked_scope
#endif



#define READONLY_PINS
#include <ControllerAccess.h>
#include <enumoperators.h>

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW( VALUES )
#endif

#define ConversionFactor stm
#define ByPass (BASE::CLAMP & 0x01)
#define SPLIT (BANDS-1)
#define LASTS (POLES-1)
#define DELAY (LASTS-1)

template<
    typename cT,
    const unsigned BANDS = 3,
    const unsigned POLES = 4,
    typename prcT = double
> class BandSplit 
    : public ClambController<cT>
{
    using BASE = ClambController<cT>;
    using SELF = BandSplit;
    using CALL = void(SELF::*)(void);

public:
    const static prcT typEps; //= std::numeric_limits<prcT>::epsilon();

private:
    void(BandSplit::*invalidation)(void);

    volatile int update;
    struct State {
        prcT  cut[SPLIT];          // cutoff freqs
        prcT  pol[SPLIT][POLES];  // filter poles
        prcT  dls[LASTS];        // delay buffer
    } state;
    prcT      srt;

    prcT      prepareCutoff( prcT frqInHz ) {
        return prcT( 2.0 * MathLib::sin( M_PI * (frqInHz / srt) ) );
    }
protected:
    prcT         stm;
    volatile int locked;
    prcT         bnd[BANDS];
    virtual prcT getSplitPoint( int idx ) = 0;
    virtual void onParameterUpdate(void) = 0;
    void         parametersDirty() { update = true; }

    // call this from within the checkVALUE() function of filter implementations
    // which derive from this abstract 'BandSplit' controller. it's got passed
    // current sample value (converted to the implemented precision type) and
    // will perform band split at distinct frequencies then, which the inheriting 
    // filter implementation can define via implementing the virtual method 
    // 'getSplitPoint()'. the input signal is stored to 'BAND' count on split 
    // bands then, which can be accessed via the 'bnd[]' levels array later 
    // and which consists of each frequency bands individual level amounts.
    // These levels so can be used for any further proccessings like analysis
    // or for further fx processing on just individual frequency bands. the
    // summ of all bnd[] level values should match input signal 1:1 (but POLES-1
    // count on samples delayed - means a 4-POL filter would implecitely cause
    // a dealy of three samples at the output..) for acting as an equilizer
    // the bands can be easily remixed back amplified individually. Actual samples
    // so could have individual bass/midd and trebbil amplifications applied. 
    cT doBandSplit(prcT inputSumm)
    {
        inner_locked_scope
        
        const int DL = DELAY;

        // dequeue (take out) delayed (first in) dry input sample from the delay (fifo) queue
        // for later processing actual full sum input in synch with (delayed) filtered signal 
    	const prcT delayedInp = state.dls[DL];

        if (!ByPass) {
            parameterUpdate();

            const int SP = SPLIT;
            const int LP = LASTS;

            // process all filter poles per frequency split point
            for ( int f = 0; f < SP; ++f ) {
                state.pol[f][0] += (state.cut[f] * ( inputSumm - state.pol[f][0]) ) + typEps;
                for ( int p = 1; p < POLES; ++p ) {
                    state.pol[f][p] += ( state.cut[f] * ( state.pol[f][p-1] - state.pol[f][p] ) );
                } bnd[f] = state.pol[f][LP];
            }

            //// store each band level to it's related bnd[] pin:
            prcT lastLowPass = bnd[0];
            for ( int num = 1; num < SP; ++num ) {
                const prcT lpInverted = -lastLowPass;
                bnd[num] = lpInverted + (lastLowPass = bnd[num]);
            } bnd[SP] = delayedInp - lastLowPass;

            /* store each band level to it's related bnd[] pin:
            prcT lastLowPass = bnd[0];
            for (int num = 1; num < SP; ++num) {
                const prcT nextLowPass = bnd[num];
                bnd[num] = delayedInp - (lastLowPass + (delayedInp - nextLowPass));
                lastLowPass = nextLowPass;
            } bnd[SP] = delayedInp - lastLowPass; */
        }

        // enqueue the latest input sample that came in (which we didn't had touched
        // yet in the fx processing) to the delay buffer, for later being able using
        // it in synch with filtered band signals which the filter produces as output
        // Or (if BYPASSed) to just returning a plain, untouched, not filtered signal
        // without loosing synch to the filtered output signal (even if this may be
        // deactivated over some time). if input would bypass also the delay line in
        // such case, the filter then in result would 'eat' all samples from the input
        // signal which actually are in progress within the filters pole state buffers
        // with each time getting de- and re-activated again via it's 'bypass' switch. 
        for( int d = DL; d > 0; --d) {
            state.dls[d] = state.dls[ d-1 ];
        } state.dls[0] = inputSumm;
        locked = 0;

        // return value is the untouched original sample from input DELAY frames ago
        // which can be used by the calling Inheriting Filter controller for passing
        // through unfiltered, bypassed signal but with delay correction applied to. 
        return (cT)(delayedInp * ConversionFactor);
    close_locked_scope( 2 ) }

    // onStateReset() is called when controller instance
    // is signaling 'Activate' on SetActive(true) calls
    // It will set the implementing filter into 'ByPass'
    // state when SetActive(false) is called rather then
    // letting the controller to get completly dectivated
    // ...so calling SetActive(false) on the controller
    // will set it ByPassed but not deactivated to keep
    // the filters caused latency of count on poles-1
    // samples staying intact. otherwise, the filter on
    // deactivation would 'eat' these last samples passed
    int onStateReset(void) { 
        do if ( locked >= 0 ) {
              ++locked;
           log_lock_state("outer")
           bool active = (bool)BASE::controller->Active;
           if ( active ) {
                BASE::CLAMP = BASE::CLAMP &~ 0x01;
                update = true;
           } else {
                BASE::CLAMP = BASE::CLAMP | 0x01;
           } return true;
        } else do_wait_cycles(2) while( locked < 0 );
        return 0;
    }

    virtual void Init(void) { locked = 1;
        invalidation = &BandSplit::parametersDirty;
        srt = (srt<1000 || srt>1000000) ? 44100 : srt;
        stm = std::numeric_limits<cT>().is_integer
            ? std::numeric_limits<cT>().max()
            : 1.0;
        BASE::Init();
        BASE::PIN_COUNT += (BANDS+2);
        for (int i = 0; i < BANDS; ++i)
            bnd[i] = 0;
        BASE::controller->SetCheckAtGet(false);
        BASE::controller->Active &= DEACTIVATED;
        locked = 1;
        update = true;
        resetState();
    }

public:

    virtual void* Pin(void* pin, int idx) {
        outer_locked_scope
          if (idx < BANDS) {
                --locked; PIN_ACT(prcT, bnd[idx])
          } else {
              update = true;
              switch (idx - BANDS) {
                case 0: { --locked; PIN_ACT(uint, srt) }
                case 1: { --locked; PIN_ACT(size_t, invalidation) }
               default: { --locked; return BASE::Pin(pin, idx - (BANDS + 2)); }
              }
          }
        close_locked_scope(5)
    }

#ifdef THREADSAFE_CONTROLLER
    virtual LockedState lockState() const {
        return (LockedState) ( locked>0
             ?  LockedState::OUTER
                             : locked<0
             ?  LockedState::INNER
             :  LockedState::ISNOT );
    }
    virtual bool lock( bool setState ) {
       do if(locked == 0 || locked == 127) {
       locked = setState ? 127 : 0; 
       return locked == 127;
       close_locked_scope(3)
    }
    virtual bool lockSupport() const {
        return true;
    }
#endif

    // does a full reset of the state buffers.
    // All cutoff efficients will be recalculated
    // from the values of actual set split frequency
    // paremeters in relation to the streams sample
    // rate. Also all band levels as well as band
    // peaks and each band's delay buffers will be
    // reset back to 0db
    bool resetState() {
        bool aktive = onStateReset();
        if ( locked > 0 )
        if ( aktive ) { log_lock_state("outer")
            for (int i = 0; i < SPLIT; ++i) {
                state.cut[i] = prepareCutoff( getSplitPoint(i) );
                for (int p = 0; p < POLES; ++p)
                    state.pol[i][p] = 0;
            } for (int i = 0; i < LASTS; ++i) {
                state.dls[i] = 0;
            } for (int b = 0; b < BANDS; ++b) {
                bnd[b] = 0;
            } update = false;
        } return ( (locked = locked > 0? 0 
                           : locked ) == 0 )
              && ( !update );
    }
    // parameter update applies just within idle
    // and returns false when updating maybe was 
    // not possible and there still changed pins 
    // need update calls being applied 
    bool parameterUpdate() {
        if ( update ) {
            for ( int i = ( locked * locked * SPLIT );
                  i < SPLIT && locked == 0; ++i ) {
                state.cut[i] = prepareCutoff( getSplitPoint(i) );
            } if( locked == 0 )
                onParameterUpdate();
            update = false;
        } return (locked == 0) || (!update);
    }
    // set new sample rate at which sample data
    // is assumed comming in then (or just get 
    // actual set samplerate by not passing a
    // samplerate as 'rate' parameter)
    uint SampleRate( uint rate = EMPTY_(uint) ) {
        if ( enum_utils::is_val(rate) ) {
            if ( rate != srt ) {
                srt = rate;
                update = true;
            }
        } return srt;
    }
};

template<typename cT,const unsigned BANDS,const unsigned POLES,typename prcT>
const prcT BandSplit<cT, BANDS, POLES, prcT>::typEps = std::numeric_limits<prcT>::epsilon();


#undef ByPass
#undef LASTS
#undef DELAY
#undef SPLIT

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW( VALUES )
#endif

#include <ControllerAccess.h>
#endif
