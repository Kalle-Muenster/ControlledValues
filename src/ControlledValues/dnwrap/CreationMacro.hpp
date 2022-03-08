/* 
/  CreationMacro.hpp
/  DefineControlledType(typ)
/  macro wich generates a wrapper class for instanciating
/  wrapper objects which can hold a native ControlledValue 
/  instance which can control a variable of type 'typ'
*/

#ifndef DefineControlledType
#define DefineControlledType( typ ) ref class typ \
    : public ValueController< Stepflow::##typ > { \
public: \
    typedef Stepflow::##typ TYPE; \
    typ() : ValueController<TYPE>() {} \
    typ( ControlMode m ) : ValueController<TYPE>(m) {} \
    typ( TYPE min, TYPE max, TYPE mov, ControlMode mod ) \
        : ValueController<TYPE>( min, max, mov, mod ) {} \
    typ( TYPE% refVar) : ValueController<TYPE>( refVar ) {} \
    virtual Stepflow::TypeCode GetTypeCode(void) override { \
        return Stepflow::TypeCode::##typ; \
    } \
    virtual stepflow::ModeCodeVal GetModeCode(void) override { \
        return (stepflow::ModeCodeVal)this->Mode; } \
    typ^ Clone( void ) { \
        byte state = Get()->getConfiguration(); \
        typ^ derNeue = gcnew typ(); \
        Get()->Active = false; \
        derNeue->Get()->SetUp( \
            this->MIN, this->MAX, this->VAL, this->MOV, \
            stepflow::CtrlMode::None ); \
        derNeue->Mode = this->Mode; \
        derNeue->Get()->SetFlags( state ); \
        this->Get()->SetFlags( state ); \
        return derNeue; \
    } \
    typ^ Refer( void ) { \
        typ^ derAndere = gcnew typ(); \
        derAndere->nativ = IntPtr(Get()); \
        return derAndere; \
    } \
    static operator TYPE( Controlled::##typ^ This ) { \
        return This->VAL; \
    } \
    TYPE operator()(){ return this->VAL; } \
    typ^ operator()(TYPE set){ this->VAL = set; return this; } \
}
#endif
