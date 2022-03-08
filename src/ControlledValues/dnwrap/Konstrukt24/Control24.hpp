#ifndef _Control24_hpp_
#define _Control24_hpp_

namespace Stepflow
{

    public ref class Int24Controller abstract
        : public ControllerBase
    {
        typedef Stepflow::Int24 MT;
        typedef ::Int24         NT;

    public:
        delegate MT bModeDelegate(ControlMode);
        delegate MT uModeDelegate(MT%, MT%, MT%, MT%);

    protected:
        uModeDelegate^ checkVALUE;
        stepflow::DelegateInt24Wrap::BaseModeFunction checkMODENativeCall;
        virtual MT checkMODE(ControlMode mode) {
            return MT((Get()->GetUserMode<stepflow::DelegateInt24Wrap>()->*checkMODENativeCall)(mode));
        }

    internal:
        stepflow::PinFieldController<NT>* Get() {
            return (stepflow::PinFieldController<NT>*)ControllerBase::controller();
        }
        void Set(stepflow::PinFieldController<NT>* ntiv) {
            nativ = IntPtr(ntiv);
        }
        void FetchBaseModeCall(void) {
            if (Mode == ControlMode::Delegate) {
                checkMODENativeCall = Get()->GetPin<stepflow::DelegateInt24Wrap::BaseModeFunction>(1);
            }
        }

    public:
        Int24Controller(void) : ControllerBase() {
            nativ = IntPtr(new stepflow::PinFieldController<NT>());
        }
        Int24Controller(MT% reference) : Int24Controller() {
            Get()->SetUp(0, 0, NT(Stepflow::Int24::operator int(reference)), 0, stepflow::CtrlMode::None);
            pin_ptr<MT> p = &reference;
            Get()->SetVariable((NT*)p);
        }
        Int24Controller(ControlMode mode) : Int24Controller() {
            Get()->SetUp(0, 0, 0, 0, stepflow::CtrlMode::None);
            Mode = mode;
        }
        Int24Controller(MT min, MT max, MT mov, ControlMode mod) : Int24Controller() {
            Get()->SetUp((NT)min, (NT)max, (NT)0, (NT)mov, stepflow::CtrlMode::None);
            Mode = mod;
        }

        virtual void SetUp(MT min, MT max, MT mov, MT val, ControlMode mod) {
            if (mod > ControlMode::PingPong) {
                Get()->SetMIN(*(NT*)&min);
                Get()->SetMAX(*(NT*)&max);
                Get()->SetMOV(*(NT*)&mov);
                *Get()->GetPointer() = *(NT*)&val;
            }
            else {
                Get()->SetUp(*(NT*)&min, *(NT*)&max, *(NT*)&val, *(NT*)&mov, byte(mod));
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get(void) {
                return (ControlMode)Get()->Mode();
            }
            void set(ControlMode value) {
                if (value <= ControlMode::PingPong) {
                    Get()->Mode(stepflow::CtrlMode((uint)value));
                }
                else createMode<NT>(Get(), value, GetTypeCode());
            }
        }

        virtual property MT VAL {
            MT get(void) { return *(MT*)Get()->getVALpt(); }
            void set(MT value) { Get()->SetVAL((NT*)&value); }
        }
        virtual property MT MIN {
            MT get(void) { return *(MT*)Get()->getMINpt(); }
            void set(MT value) { Get()->MIN = *(NT*)&value; }
        }
        virtual property MT MAX {
            MT get(void) { return *(MT*)Get()->getMAXpt(); }
            void set(MT value) { Get()->MAX = *(NT*)&value; }
        }
        virtual property MT MOV {
            MT get(void) { return *(MT*)Get()->getMOVpt(); }
            void set(MT value) { Get()->MOV = *(NT*)&value; }
        }

        virtual bool Check(void) override {
            return Get()->Check();
        }
        virtual void Step(void) override {
            Get()->Step();
        }
        virtual void SetCheckAtGet(void) override {
            Get()->SetCheckAtGet(true);
        }
        virtual void SetCheckAtSet(void) override {
            Get()->SetCheckAtGet(false);
        }

        property uModeDelegate^ AttachedDelegate {
            uModeDelegate^ get(void) {
                if (Mode == ControlMode::Delegate) {
                    return (uModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr(Get()->GetPin<stepflow::DelegateInt24Wrap::DelegateFunction>(0)),
                        uModeDelegate::typeid);
                }
                else return nullptr;
            }
            void set(uModeDelegate^ value) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateInt24Wrap>()->SetDelegateFunction(
                    (stepflow::DelegateInt24Wrap::DelegateFunction)
                    Marshal::GetFunctionPointerForDelegate(checkVALUE).ToPointer()
                ); FetchBaseModeCall();
            }
        }

        static operator MT(Int24Controller^ This) {
            return MT(NT(*This->Get()));
        }
        virtual void SetPin(Enum^ pinum, MT% val) {
            pin_ptr<MT> p = &val;
            Get()->SetPin(*(NT*)p, ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture));
        }
        virtual void SetPin(int index, System::IntPtr ptr) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>(*typed, index);
        }
        virtual IntPtr GetPin(int pindex) override {
            return IntPtr(&Get()->GetPin<NT>(pindex));
        }
        virtual IntPtr GetPin(Enum^ pinnum) override {
            return GetPin(((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture));
        }
        virtual IntPtr GetTarget(void) override {
            return IntPtr(Get()->GetPointer());
        }
        virtual void SetTarget(IntPtr target) override {
            Get()->SetVariable((NT*)target.ToPointer());
        }
        virtual void SetTarget(MT% target) {
            pin_ptr<MT> p = &target;
            Get()->SetVariable((NT*)p);
        }
        virtual IntPtr GetPointer(ControllerVariable index) override {
            return IntPtr(Get()->getPTR(stepflow::ControllerVariables(index)));
        }
        virtual void LetPoint(ControllerVariable varnam, IntPtr target) override {
            Get()->LetPoint(stepflow::ControllerVariables(varnam), (NT*)target.ToPointer());
        }
    };


    public ref class UInt24Controller abstract
        : public ControllerBase
    {
        typedef Stepflow::UInt24 MT;
        typedef ::UInt24         NT;

    public:
        delegate MT bModeDelegate(ControlMode);
        delegate MT uModeDelegate(MT%, MT%, MT%, MT%);

    protected:
        uModeDelegate^ checkVALUE;
        stepflow::DelegateUInt24Wrap::BaseModeFunction checkMODENativeCall;
        virtual MT checkMODE(ControlMode mode) {
            return MT((Get()->GetUserMode<stepflow::DelegateUInt24Wrap>()->*checkMODENativeCall)(mode));
        }

    internal:
        stepflow::PinFieldController<NT>* Get() {
            return (stepflow::PinFieldController<NT>*)ControllerBase::controller();
        }
        void Set(stepflow::PinFieldController<NT>* ntiv) {
            nativ = IntPtr(ntiv);
        }
        void FetchBaseModeCall(void) {
            if (Mode == ControlMode::Delegate) {
                checkMODENativeCall = Get()->GetPin<stepflow::DelegateUInt24Wrap::BaseModeFunction>(1);
            }
        }

    public:
        UInt24Controller(void) : ControllerBase() {
            nativ = IntPtr(new stepflow::PinFieldController<NT>());
        }
        UInt24Controller(MT% reference) : UInt24Controller() {
            Get()->SetUp(0, 0, (stepflow::i16)reference, 0, byte(stepflow::CtrlMode::None));
            pin_ptr<MT> p = &reference;
            Get()->SetVariable((NT*)p);
        }
        UInt24Controller(ControlMode mode) : UInt24Controller() {
            Get()->SetUp(0, 0, 0, 0, stepflow::CtrlMode::None);
            Mode = mode;
        }
        UInt24Controller(MT min, MT max, MT mov, ControlMode mod) : UInt24Controller() {
            Get()->SetUp((stepflow::i24&)min, (stepflow::i24&)max, 0, (stepflow::i24&)mov, stepflow::CtrlMode::None);
            Mode = mod;
        }

        virtual void SetUp(MT min, MT max, MT mov, MT val, ControlMode mod) {
            if (mod > ControlMode::PingPong) {
                Get()->MIN = *(NT*)&min;
                Get()->MAX = *(NT*)&max;
                Get()->MOV = *(NT*)&mov;
                *Get()->GetPointer() = *(NT*)&val;
            }
            else {
                Get()->SetUp((stepflow::i24&)min, (stepflow::i24&)max, (stepflow::i24&)val, (stepflow::i24&)mov, byte(mod));
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get(void) {
                return (ControlMode)Get()->Mode();
            }
            void set(ControlMode value) {
                if (value <= ControlMode::PingPong) {
                    Get()->Mode(stepflow::CtrlMode((uint)value));
                }
                else createMode<NT>(Get(), value, GetTypeCode());
            }
        }

        virtual property MT VAL {
            MT get(void) { return *(MT*)Get()->getVALpt(); }
            void set(MT value) { Get()->SetVAL((NT*)&value); }
        }
        virtual property MT MIN {
            MT get(void) { return *(MT*)Get()->getMINpt(); }
            void set(MT value) { Get()->MIN = *(NT*)&value; }
        }
        virtual property MT MAX {
            MT get(void) { return *(MT*)Get()->getMAXpt(); }
            void set(MT value) { Get()->MAX = *(NT*)&value; }
        }
        virtual property MT MOV {
            MT get(void) { return *(MT*)Get()->getMOVpt(); }
            void set(MT value) { Get()->MOV = *(NT*)&value; }
        }

        virtual bool Check(void) override {
            return Get()->Check();
        }
        virtual void Step(void) override {
            Get()->Step();
        }
        virtual void SetCheckAtGet(void) override {
            Get()->SetCheckAtGet(true);
        }
        virtual void SetCheckAtSet(void) override {
            Get()->SetCheckAtGet(false);
        }

        property uModeDelegate^ AttachedDelegate {
            uModeDelegate^ get(void) {
                if (Mode == ControlMode::Delegate) {
                    return (uModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr( Get()->GetPin<stepflow::DelegateUInt24Wrap::DelegateFunction>(0) ),
                        uModeDelegate::typeid);
                }
                else return nullptr;
            }
            void set(uModeDelegate^ value) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateUInt24Wrap>()->SetDelegateFunction(
                    (stepflow::DelegateUInt24Wrap::DelegateFunction)
                    Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                ); FetchBaseModeCall();
            }
        }

        static operator MT(UInt24Controller^ This) {
            return MT(NT(*This->Get()));
        }
        virtual void SetPin(Enum^ pinum, MT% val) {
            pin_ptr<MT> p = &val;
            Get()->SetPin(*(NT*)p, ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture));
        }
        virtual void SetPin(int index, System::IntPtr ptr) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>(*typed, index);
        }
        virtual IntPtr GetPin(int pindex) override {
            return IntPtr(&Get()->GetPin<NT>(pindex));
        }
        virtual IntPtr GetPin(Enum^ pinnum) override {
            return GetPin(((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture));
        }
        virtual IntPtr GetTarget(void) override {
            return IntPtr(Get()->GetPointer());
        }
        virtual void SetTarget(IntPtr target) override {
            Get()->SetVariable((NT*)target.ToPointer());
        }
        virtual void SetTarget(MT% target) {
            pin_ptr<MT> p = &target;
            Get()->SetVariable((NT*)p);
        }
        virtual IntPtr GetPointer(ControllerVariable index) override {
            return IntPtr(Get()->getPTR(stepflow::ControllerVariables(index)));
        }
        virtual void LetPoint(ControllerVariable varnam, IntPtr target) override {
            Get()->LetPoint(stepflow::ControllerVariables(varnam), (NT*)target.ToPointer());
        }
    };

}
#endif


