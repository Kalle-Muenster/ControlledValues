#include "Stepflow.h"
#include "PinPatchStruct.hpp"

template<typename UMode, typename Cntrl> static
System::IntPtr allocateAndAttach( Cntrl* controller )
{
    UMode* newmode = new UMode();
    controller->Mode( newmode );
    return System::IntPtr( newmode );
}

namespace Stepflow {
    enum class ControlMode;
    template< const unsigned bC,
              const unsigned pC > public
    ref class FilterMode {
    public: const static ControlMode Code = ControlMode( (int)( stepflow::CtrlMode::USERMODE
                          + (__BIG_ENDIAN ? 1145979202 : 1111576132)) + ( bC | (pC << 4) ) );
    };
}

template<typename ct> static
System::IntPtr setMode( Stepflow::ControlMode mode, stepflow::PinFieldController<ct>* toController ) {
    
    switch (mode) {
    case Stepflow::ControlMode::Sinus:      return allocateAndAttach<stepflow::SineFormer<ct>>(toController);
    case Stepflow::ControlMode::Pulse:      return allocateAndAttach<stepflow::PulseFormer<ct>>(toController);
    case Stepflow::ControlMode::Element:    return allocateAndAttach<stepflow::Element<ct>>(toController);
    case Stepflow::ControlMode::Follow:     return allocateAndAttach<stepflow::Follower<ct>>(toController);
    case Stepflow::ControlMode::MultiBand:  return allocateAndAttach<stepflow::MultiComp<ct,6,Preci>>(toController);
    case Stepflow::ControlMode::Delegate:   return allocateAndAttach<stepflow::DelegateWrap<ct>>(toController);
    case Stepflow::ControlMode::Stack:      return allocateAndAttach<stepflow::RampFormer<ct>>(toController);
    case Stepflow::ControlMode::PeakFinder: return allocateAndAttach<stepflow::PeakFinder<ct>>(toController);
    case Stepflow::ControlMode::Capacitor:  return allocateAndAttach<stepflow::Capacitor<ct>>(toController);
    default: {
           if ( mode == Stepflow::FilterMode<3,4>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,3,4,Preci>>(toController);
    } else if ( mode == Stepflow::FilterMode<3,6>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,3,6,Preci>>(toController);
    } else if ( mode == Stepflow::FilterMode<6,4>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,6,4,Preci>>(toController);
    } else if ( mode == Stepflow::FilterMode<6,6>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,6,6,Preci>>(toController);
    } else if ( mode == Stepflow::FilterMode<12,4>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,12,4,Preci>>(toController);
    } else if ( mode == Stepflow::FilterMode<12,6>::Code ) {
        return allocateAndAttach<stepflow::EQBandFilter<ct,12,6,Preci>>(toController);
    } else return System::IntPtr::Zero; }
    }
}

template<typename cT> void
Stepflow::ControllerBase::createMode( stepflow::PinFieldController<cT>* onController,
                                      Stepflow::ControlMode modeCode, Stepflow::TypeCode typeCode ) {   
    switch (typeCode) {
    
    case Stepflow::TypeCode::Int8:   {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Int8>*>(onController)); } break;
    case Stepflow::TypeCode::Int16:  {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Int16>*>(onController)); } break;
    case Stepflow::TypeCode::Int24:  {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Int24>*>(onController)); } break;
    case Stepflow::TypeCode::Int32:  {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Int32>*>(onController)); } break;
    case Stepflow::TypeCode::Int64:  {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Int64>*>(onController)); } break;
    case Stepflow::TypeCode::UInt8:  {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::UInt8>*>(onController)); } break;
    case Stepflow::TypeCode::UInt16: {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::UInt16>*>(onController)); } break;
    case Stepflow::TypeCode::UInt24: {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::UInt24>*>(onController)); } break;
    case Stepflow::TypeCode::UInt32: {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::UInt32>*>(onController)); } break;
    case Stepflow::TypeCode::UInt64: {umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::UInt64>*>(onController)); } break;
    case Stepflow::TypeCode::Float16:{umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Float16>*>(onController)); } break;
    case Stepflow::TypeCode::Float32:{umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Float32>*>(onController)); } break;
    case Stepflow::TypeCode::Float64:{umode = setMode( modeCode, reinterpret_cast<stepflow::PinFieldController<::Float64>*>(onController)); } break;
                   default: umode = System::IntPtr::Zero; }
}

System::Action^
Stepflow::ControllerBase::CallPin( System::IntPtr pin ) {
    System::IntPtr ptFunction( reinterpret_cast<void(*)()>((void*)pin.ToPointer()) );
    return Marshal::GetDelegateForFunctionPointer<System::Action^>( ptFunction );
}

uint
Stepflow::ControllerBase::GetVersionNumber(void)
{
    return stepflow::GetVersionNumber();
}

System::String^
Stepflow::ControllerBase::GetVersionString(void)
{
    return gcnew String( stepflow::GetVersionString() );
}
