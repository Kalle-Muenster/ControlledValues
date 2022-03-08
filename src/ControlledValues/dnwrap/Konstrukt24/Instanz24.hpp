#ifndef _Value24_hpp_
#define _Value24_hpp_

namespace Stepflow {
    
    ref class Controlled;
    ref class ControllerBase;
    ref class Int24Controller;
    ref class UInt24Controller;

    public ref class Controlled
        : public ControllerBase {
    public:

        // Signed 24bit controlled integer type
        ref class Int24
            : public Int24Controller {
        public:
            typedef Stepflow::Int24 TYPE;
            Int24() : Int24Controller() {}
            Int24(ControlMode m) : Int24Controller(m) {}
            Int24(TYPE min, TYPE max, TYPE mov, ControlMode mod)
                : Int24Controller(min, max, mov, mod) {}
            Int24(TYPE% refVar) : Int24Controller(refVar) {}
            virtual Stepflow::TypeCode GetTypeCode(void) override {
                return Stepflow::TypeCode::Int24;
            }
            virtual stepflow::ModeCodeVal GetModeCode(void) override {
                return (stepflow::ModeCodeVal)this->Mode;
            }
            Int24^ Clone(void) {
                byte state = Get()->getConfiguration();
                Int24^ clone = gcnew Int24();
                Get()->Active = false;
                clone->Get()->SetUp(
                    Get()->MIN, Get()->MAX, *Get(), Get()->MOV,
                    stepflow::CtrlMode::None);
                clone->Mode = this->Mode;
                clone->Get()->SetFlags(state);
                this->Get()->SetFlags(state);
                return clone;
            }
            Int24^ Refer(void) {
                Int24^ other = gcnew Int24();
                other->nativ = IntPtr(Get());
                return other;
            }
            static operator TYPE(Controlled::Int24^ This) {
                return *(TYPE*)This->Get()->getVALpt();
            }
            TYPE operator()() { return VAL; }
            Int24^ operator()(TYPE set) { VAL = set; return this; }
        };

        // Unsigned 24bit controlled integer type
        ref class UInt24
            : public UInt24Controller {
        public:
            typedef Stepflow::UInt24 TYPE;
            UInt24() : UInt24Controller() {}
            UInt24(ControlMode m) : UInt24Controller(m) {}
            UInt24(TYPE min, TYPE max, TYPE mov, ControlMode mod)
                : UInt24Controller(min, max, mov, mod) {}
            UInt24(TYPE% refVar) : UInt24Controller(refVar) {}
            virtual Stepflow::TypeCode GetTypeCode(void) override {
                return Stepflow::TypeCode::UInt24;
            }
            virtual stepflow::ModeCodeVal GetModeCode(void) override {
                return (stepflow::ModeCodeVal)this->Mode;
            }
            UInt24^ Clone(void) {
                byte state = Get()->getConfiguration();
                UInt24^ clone = gcnew UInt24();
                Get()->Active = false;
                clone->Get()->SetUp(
                    Get()->MIN, Get()->MAX, *Get(), Get()->MOV,
                    stepflow::CtrlMode::None);
                clone->Mode = this->Mode;
                clone->Get()->SetFlags(state);
                this->Get()->SetFlags(state);
                return clone;
            }
            UInt24^ Refer(void) {
                UInt24^ other = gcnew UInt24();
                other->nativ = IntPtr(Get());
                return other;
            }
            static operator TYPE(Controlled::UInt24^ This) {
                return *(TYPE*)This->Get()->getVALpt();
            }
            TYPE operator()() { return VAL; }
            UInt24^ operator()(TYPE set) { VAL = set; return this; }
        };


    };
}

#endif

