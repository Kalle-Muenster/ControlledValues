#ifndef _PinBelaegungs_Hpp_
#define _PinBelaegungs_Hpp_


#define Preci CONTROLER_INTERNAL_PRECISION


namespace Stepflow {

    namespace Controller {

            public enum class Pin {
                VAL, MIN, MAX, MOV
            };

            template<typename cT>
            public ref class SixBandCompPin {
                typedef stepflow::MultiComp<short,6,Preci> NATIVE;
            public:
                static const byte ByPass = (byte)NATIVE::BYPASS;
                static const double SampleRate = (double)NATIVE::RATE;
                static const cT Ratio = cT(NATIVE::MAIN_RATIO);
                static const cT OutputPeak = cT(NATIVE::OUTPUT_PEAK);
                value class  Band {
                public:
                    const int bandex;
                    Band( int index ) : bandex( index ) {}
                    property Preci Threshold {
                        Preci get(void) { return (Preci)(bandex + NATIVE::BAND_WAIGHT); }
                    }
                    property Preci GainLevel {
                        Preci get(void) { return (Preci)(bandex + NATIVE::BAND_MIXERS); }
                    }
                    property Preci Compresso {
                        Preci get(void) { return (Preci)(bandex + NATIVE::BAND_RATIOS); }
                    }
                    property uint   Frequency {
                          uint get(void) { return (uint)(bandex + NATIVE::BAND_SPLITS); }
                    }
                };
                const static Band Band0 = Band(0);
                const static Band Band1 = Band(1);
                const static Band Band2 = Band(2);
                const static Band Band3 = Band(3);
                const static Band Band4 = Band(4);
                const static Band Band5 = Band(5);
                const static cT VAL = NATIVE::INPUT_GAIN - 1;
            };

            public ref class CP6BandShort : public SixBandCompPin<stepflow::s16> {};
            public ref class CP6Band24Bit : public SixBandCompPin<Stepflow::Int24> {};
            public ref class CP6BandInt32 : public SixBandCompPin<stepflow::s32> {};
            public ref class CP6BandFloat : public SixBandCompPin<stepflow::f32> {};
            public ref class CP6BandDoubl : public SixBandCompPin<stepflow::f64> {};


            public enum class DelegateMode {
                FuncPtr = 0, CLAMP,
                VAL, MIN, MAX, MOV
            };
            public enum class ElementValue {
                PEAK = stepflow::pins::Element::PEAK,
                LAST = stepflow::pins::Element::LAST,
                CYCLED = stepflow::pins::Element::CYCLED,
                UNSIGNED = stepflow::pins::Element::UNSIGNED,
                CLAMPED = stepflow::pins::Element::CLAMPED,
                VAL, MIN, MAX, MOV
            };

            public enum class SineFormer {
                WAVE = stepflow::pins::FormControl::WAVE,
                FORM = stepflow::pins::FormControl::FORM,
                VAL, MIN, MAX, MOV
            };
            public enum class PulsFormer {
                WAVE = stepflow::pins::FormControl::WAVE,
                FORM = stepflow::pins::FormControl::FORM,
                VAL, MIN, MAX, MOV
            };
            public enum class RampFormer {
                WAVE = stepflow::pins::FormControl::WAVE,
                FORM = stepflow::pins::FormControl::FORM,
                VAL, MIN, MAX, MOV
            };
            public enum class ValueFollow {
                TARGET, DIRECT, CLAMP,
                VAL, MIN, MAX, MOV
            };

            public enum class PeakFinder {
                TRIGGER, CLAMP,
                VAL, MIN, MAX, MOV
            };





            template<const unsigned bC>
            public ref class EQFilterPin {
            public:
                value struct Band {
                public:
                    const Preci Level;
                    const Preci Gain;
                    const Preci Peak;
                    Band( Preci l, Preci g, Preci p )
                        : Level(l)
                        , Gain(g)
                        , Peak(p) {
                    }
                };
                const static Band    loPass = Band(stepflow::EQFilterPindices<bC>::LO_BAND, stepflow::EQFilterPindices<bC>::LO_GAIN, stepflow::EQFilterPindices<bC>::LO_PEAK);
                      static Band    nuBand(int num) { return Band(stepflow::EQFilterPindices<bC>::BAND(num), stepflow::EQFilterPindices<bC>::GAIN(num), stepflow::EQFilterPindices<bC>::PEAK(num)); };
                const static Band    hiPass = Band(stepflow::EQFilterPindices<bC>::HI_BAND, stepflow::EQFilterPindices<bC>::HI_GAIN, stepflow::EQFilterPindices<bC>::HI_PEAK);
                const static Preci  OutPeak = stepflow::EQFilterPindices<bC>::OUTPEAK;
                const static Preci  InpGain = stepflow::EQFilterPindices<bC>::IN_GAIN;
                const static Preci  loSplit = stepflow::EQFilterPindices<bC>::LOSPLIT;
                      static Preci  nuSplit(int num) { return (float)stepflow::EQFilterPindices<bC>::SPLT(num); }
                const static Preci  hiSplit = stepflow::EQFilterPindices<bC>::HISPLIT;
                const static byte   ByPass  = stepflow::EQFilterPindices<bC>::BY_PASS;
                const static uint   SmpRate = stepflow::EQFilterPindices<bC>::SmplFrq;
                const static size_t Invalid = stepflow::EQFilterPindices<bC>::INVALID;
            };

            public ref class EQ3BandFilterPins : public EQFilterPin<3> {
            public:
                const static Band  miBand = nuBand(2);
            };

            public ref class EQ6BandFilterPins : public EQFilterPin<6> {
            public:
                const static Preci split1 = loSplit;
                const static Preci split2 = nuSplit(2);
                const static Preci split3 = nuSplit(3);
                const static Preci split4 = nuSplit(4);
                const static Preci split5 = hiSplit;

                const static Band  loBand  = nuBand(2);
                const static Band  loMids  = nuBand(3);
                const static Band  hiMids  = nuBand(4);
                const static Band  hiBand  = nuBand(5);            
            };

            public ref class EQ12BandFilterPins : public EQFilterPin<12> {
            public:
                const static Band  band_01 = loPass;
                const static Band  band_02 = nuBand(2);
                const static Band  band_03 = nuBand(3);
                const static Band  band_04 = nuBand(4);
                const static Band  band_05 = nuBand(5);
                const static Band  band_06 = nuBand(6);
                const static Band  band_07 = nuBand(7);
                const static Band  band_08 = nuBand(8);
                const static Band  band_09 = nuBand(9);
                const static Band  band_10 = nuBand(10);
                const static Band  band_11 = nuBand(11);
                const static Band  band_12 = hiPass;

                const static Preci split_01 = loSplit;
                const static Preci split_02 = nuSplit(2);
                const static Preci split_03 = nuSplit(3);
                const static Preci split_04 = nuSplit(4);
                const static Preci split_05 = nuSplit(5);
                const static Preci split_06 = nuSplit(6);
                const static Preci split_07 = nuSplit(7);
                const static Preci split_08 = nuSplit(8);
                const static Preci split_09 = nuSplit(9);
                const static Preci split_10 = nuSplit(10);
                const static Preci split_11 = hiSplit;
            };


#define CreateStepLengthCalculationFunction(cT) \
static Stepflow:: ## cT FromFrequency( cT frequency, Preci carrier, Controlled:: ## cT ## ^ controller ) \
{ return (cT)(0.5 + ((Preci)(controller->MAX - controller->MIN) / (carrier / frequency))); }

            public ref class StepLength {
            public:
                static Stepflow::Byte FromFrequency( int frequency, Preci carrierwave, Controlled::Byte^ controller )
                {
                    return (byte)(0.5 + ((double)(controller->MAX - controller->MIN) / (carrierwave / frequency)));
                }
                static Stepflow::Int8 FromFrequency( int frequency, Preci carrierwave, Controlled::Int8^ controller )
                {
                    return (char)(0.5 + ((double)(controller->MAX - controller->MIN) / (carrierwave / frequency)));
                }

                CreateStepLengthCalculationFunction(Int16)
                CreateStepLengthCalculationFunction(Int24)
                CreateStepLengthCalculationFunction(Int32)
                CreateStepLengthCalculationFunction(Int64)
                
                CreateStepLengthCalculationFunction(UInt16)
                CreateStepLengthCalculationFunction(UInt24)
                CreateStepLengthCalculationFunction(UInt32)
                CreateStepLengthCalculationFunction(UInt64)
                
                CreateStepLengthCalculationFunction(Float32)
                CreateStepLengthCalculationFunction(Float64)

            };

#undef CreateStepLengthCalculationFunction

    }
}
#endif