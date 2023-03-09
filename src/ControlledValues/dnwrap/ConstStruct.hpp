#ifndef _ConstStruct_Hpp_
#define _ConstStruct_Hpp_


using namespace System::Runtime::InteropServices;

    typedef System::SByte           Int8;
    typedef System::Byte            UInt8;
    typedef System::UInt16          UInt16;
    typedef System::Int16           Int16;
    typedef stepflow::UINT_24BIT    UInt24;
    typedef stepflow::INT_24BIT     Int24;
    typedef System::UInt32          UInt32;
    typedef System::Int32           Int32;
    typedef System::UInt64          UInt64;
    typedef System::Int64           Int64;
    typedef half_float::half        Float16;
    typedef System::Single          Float32;
    typedef System::Double          Float64;


namespace Stepflow
{
    template<typename cT> ref class ValueController;
    template<typename mT,typename nT> ref class StructController;

    

    typedef System::SByte           Int8;
    typedef System::Byte            UInt8;
    typedef System::UInt16          UInt16;
    typedef System::Int16           Int16;
    typedef System::UInt32          UInt32;
    typedef System::Int32           Int32;
    typedef System::UInt64          UInt64;
    typedef System::Int64           Int64;
    typedef System::Single          Float32;
    typedef System::Double          Float64;

    public enum class ControllerVariable {
        VAL = stepflow::ControllerVariables::Val,
        MIN = stepflow::ControllerVariables::Min,
        MAX = stepflow::ControllerVariables::Max,
        MOV = stepflow::ControllerVariables::Mov
    };

    public enum class ControlMode {
        None = stepflow::CtrlMode::None,
        Invert = stepflow::CtrlMode::Invert,
        Clamp = stepflow::CtrlMode::Clamp,
        Damp = stepflow::CtrlMode::Damp,
        Compress = stepflow::CtrlMode::Compress,
        Expand = stepflow::CtrlMode::Expand,
        Moderate = stepflow::CtrlMode::Moderate,
        Cycle = stepflow::CtrlMode::Cycle,
        PingPong = stepflow::CtrlMode::PingPong,
        Delegate = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1129207110 : 1179995715),
        Sinus = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1162758483 : 1397313093),
        Pulse = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1514948944 : 1346980954),
        Stack = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1768256068 : 1148347753),
        Element = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1414351941 : 1162628436),
        Filter3Band4Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (3 | (4 << 4)),
        Filter3Band6Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (3 | (6 << 4)),
        Filter6Band4Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (6 | (4 << 4)),
        Filter6Band6Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (6 | (6 << 4)),
        Filter12Band4Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (12 | (4 << 4)),
        Filter12Band6Pole = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1145979202 : 1111576132) + (12 | (6 << 4)),
        Follow = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1381452870 : 1179408210),
        MultiBand = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1953264973 : 1299541108),
        PeakFinder = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1262568784 : 1346715979),
        Capacitor = stepflow::CtrlMode::USERMODE + (__BIG_ENDIAN ? 1381257283 : 1129337938),
    };

    public enum class TypeCode : int {
        Int8    = (int)System::TypeCode::SByte,
        Int16   = (int)System::TypeCode::Int16,
        Int24   = (int)stepflow::TypeCode::Int24,
        Int32   = (int)System::TypeCode::Int32,
        Int64   = (int)System::TypeCode::Int64,
        UInt8   = (int)System::TypeCode::Byte,
        UInt16  = (int)System::TypeCode::UInt16,
        UInt24  = (int)stepflow::TypeCode::UInt24,
        UInt32  = (int)System::TypeCode::UInt32,
        UInt64  = (int)System::TypeCode::UInt64,
        Float16 = (int)stepflow::TypeCode::Float16,
        Float32 = (int)System::TypeCode::Single,
        Float64 = (int)System::TypeCode::Double,
    };

    public value struct ValenceFieldState
    {
    private:  byte* data;
    internal: ValenceFieldState(char* state) {
        data = (byte*)state;
    }
    public:
        [System::FlagsAttribute]
        enum class Flags : byte {
            VAL = stepflow::ValenceFieldState::VAL_EXTERN,
            MIN = stepflow::ValenceFieldState::MIN_EXTERN,
            MAX = stepflow::ValenceFieldState::MAX_EXTERN,
            MOV = stepflow::ValenceFieldState::MOV_EXTERN
        };
        bool operator[](ValenceFieldState::Flags index) {
            return (*data & (byte)index) > 0;
        }
        void Clear() {
            *data &= (byte)( Flags::VAL | Flags::MIN
                           | Flags::MAX | Flags::MOV );
        }
    };

    public value struct  Spin {
    public: const static Spin UP = Spin(Int8(1));
    public: const static Spin DOWN = Spin(Int8(-1));

    private: Int8 direction;
    public: static operator bool( Spin cast ) {
        return cast.direction > 0;
    };
    public: static operator float( Spin cast ) {
        return (float)cast.direction;
    }
    public: static operator double( Spin cast ) {
        return (double)cast.direction;
    }
    public: static operator Int8( Spin cast ) {
        return cast.direction;
    }
    public: Spin(bool up) : direction( up ? 1 : -1 ) {};
    public: Spin(Int8 dir) : direction( dir ) {  direction = direction > 0 ? 1 : -1; };
    public: static operator Spin (bool cast) {
        return Spin(cast);
    }

    public: static bool operator ==(Spin This, Spin that) {
        return This.direction == that.direction;
    }
    public: static bool operator !=(Spin This, Spin that) {
        return This.direction != that.direction;
    }
    public: static Spin operator !(Spin This) {
        return This.direction * DOWN.direction;
    }
    generic<class T> where T : System::IConvertible
    T operator *(T self) {
        return (T)((System::IConvertible^)(self->ToDouble(System::Globalization::CultureInfo::CurrentCulture) * this->direction))->ToType(T::typeid, System::Globalization::CultureInfo::CurrentCulture);
    }
    public: static float operator *( float that, Spin self ) {
        return that * self.direction;
    }
    public: static int operator *( int that, Spin self ) {
        return that * self.direction;
    }
    public: static double operator *( double that, Spin self ) {
        return that * self.direction;
    }
    public: Spin% Change() {
        this->direction *= DOWN.direction;
        return *this;
    }
    };
}


#endif
