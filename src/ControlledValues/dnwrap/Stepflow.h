/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ControlledValues.hpp                       ||
||     Author:    Kalle                                      ||
||     Generated: 16.02.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _ControlledValues_hpp_
#define _ControlledValues_hpp_

// explicite include sellection:
#define STEPFLOW_MINI_CONFIG
#define WITH_ELEMENT_MODE
#define WITH_RAMP_FORM_MODE
#define WITH_PULS_FORM_MODE
#define WITH_SINE_FORM_MODE
#define WITH_BANDFILTER_MODE
#define WITH_PEAKFINDER_MODE
#define WITH_MULTISTOP_MODE
#define WITH_SAW_STACK_MODE
#define WITH_CAPACITOR_MODE

#include "..\..\..\inc\ControlledValues.h"

#include "ConstStruct.hpp"
#include "CreationMacro.hpp"
#include "MessageLogger.hpp"

#include <typeinfo>
#ifdef USE_GLM_SINUS
#if    USE_GLM_SINUS
#include <glm.hpp>
#endif
#endif



using namespace System;
using namespace System::Runtime::InteropServices;

namespace Stepflow {
    ref class Int24Controller;
    ref class UInt24Controller;
}

#include <ControllerAccess.h>
namespace stepflow {

    template<typename cT>
    class DelegateWrap
        : public stepflow::MovingValue<cT>
    {     friend Stepflow::ValueController<cT>;
    protected:
        typedef cT(DelegateWrap<cT>::*BaseModeFunction)( Stepflow::ControlMode );
        typedef cT(*DelegateFunction)( cT%, cT%, cT%, cT% );
        typedef MovingValue<cT> BASE;

        DelegateFunction FUNCPTR;
        BaseModeFunction BASEPTR;

        virtual uint modeCodeBase( void ) const {
            return *(uint*)"FUNC";
        }
        virtual unsigned usrModeSize() const {
            return sizeof( DelegateWrap );
        }
        cT baseCallFunc( Stepflow::ControlMode mod ) {
            return checkMODE( stepflow::CtrlMode( mod ) );
        }
    public:
        virtual void* Pin( void* pin, int idx ) {
            return idx>1 ? BASE::Pin( pin, idx-2 ) :
                   idx ? (void*)&BASEPTR : (void*)&FUNCPTR;
        }
        virtual void Init( void ) {
            PIN_COUNT += 2;
            BASEPTR = GetBaseCallFunction();
            if( std::numeric_limits<cT>::is_integer ) {
                MOV = 1;
                MIN = std::numeric_limits<cT>().min();
                MAX = std::numeric_limits<cT>().max();
            } else {
                MOV = std::numeric_limits<cT>().epsilon();
                MIN = -1;
                MAX = 1;
            }
        }
        virtual cT checkVALUE( cT* value ) {
            return FUNCPTR( *value, MIN, MAX, MOV );
        }
        void SetDelegateFunction( DelegateFunction func ) {
            FUNCPTR = func;
        }
        BaseModeFunction GetBaseCallFunction( void ) {
            return &DelegateWrap<cT>::baseCallFunc;
        }
    };

    class DelegateInt24Wrap
        : public stepflow::MovingValue<::Int24>
    {
        friend Stepflow::Int24Controller;
        typedef ::Int24         NT;
        typedef Stepflow::Int24 MT;

    protected:
        typedef MovingValue<NT> BASE;
        typedef NT(DelegateInt24Wrap::*BaseModeFunction)( Stepflow::ControlMode );
        typedef MT(*DelegateFunction)( MT%, MT%, MT%, MT% );

        DelegateFunction FUNCPTR;
        BaseModeFunction BASEPTR;

        virtual uint modeCodeBase(void) const {
            return *(uint*)"FUNC";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(DelegateInt24Wrap);
        }
        NT baseCallFunc( Stepflow::ControlMode mod ) {
            return checkMODE( stepflow::CtrlMode( mod ) );
        }

    public:
        virtual void* Pin( void* pin, int idx ) {
            return idx > 1 ? BASE::Pin(pin, idx - 2) :
                idx ? (void*)&BASEPTR : (void*)&FUNCPTR;
        }
        virtual void Init(void) {
            PIN_COUNT += 2;
            BASEPTR = GetBaseCallFunction();
            controller->SetMIN( INT24_MIN );
            controller->SetMAX( INT24_MAX );
            controller->SetMOV( 1 );
        }
        virtual NT checkVALUE( NT* value ) {
            return (NT)FUNCPTR( *(MT*)value,
                *(MT*)controller->getMINpt(),
                *(MT*)controller->getMAXpt(),
                *(MT*)controller->getMOVpt()
            );
        }
        void SetDelegateFunction( DelegateFunction func ) {
            FUNCPTR = func;
        }
        BaseModeFunction GetBaseCallFunction(void) {
            return &DelegateInt24Wrap::baseCallFunc;
        }
    };

    class DelegateUInt24Wrap
        : public stepflow::MovingValue<::UInt24>
    {
        friend Stepflow::UInt24Controller;
        typedef ::UInt24         NT;
        typedef Stepflow::UInt24 MT;

    protected:
        typedef MovingValue<NT> BASE;
        typedef NT(DelegateUInt24Wrap::* BaseModeFunction)( Stepflow::ControlMode );
        typedef MT(*DelegateFunction)( MT%, MT%, MT%, MT% );

        DelegateFunction FUNCPTR;
        BaseModeFunction BASEPTR;

        virtual uint modeCodeBase(void) const {
            return *(uint*)"FUNC";
        }
        virtual unsigned usrModeSize() const {
            return sizeof(DelegateUInt24Wrap);
        }
        NT baseCallFunc( Stepflow::ControlMode mod ) {
            return checkMODE( stepflow::CtrlMode( mod ) );
        }

    public:
        virtual void* Pin( void* pin, int idx ) {
            return idx > 1 ? BASE::Pin(pin, idx - 2) :
                idx ? (void*)&BASEPTR : (void*)&FUNCPTR;
        }
        virtual void Init(void) {
            PIN_COUNT += 2;
            BASEPTR = GetBaseCallFunction();
            controller->SetMIN( UINT24_MIN );
            controller->SetMAX( UINT24_MAX );
            controller->SetMOV( 1 );
        }
        virtual NT checkVALUE( NT* value ) {
            return (NT)FUNCPTR( *(MT*)value,
                *(MT*)BASE::controller->getMINpt(),
                *(MT*)BASE::controller->getMAXpt(),
                *(MT*)BASE::controller->getMOVpt()
            );
        }
        void SetDelegateFunction( DelegateFunction func ) {
            FUNCPTR = func;
        }
        BaseModeFunction GetBaseCallFunction(void) {
            return &DelegateUInt24Wrap::baseCallFunc;
        }
    };
}
#include <ControllerAccess.h>

#undef MIN
#undef MAX
#undef MOV

namespace Stepflow {

    public ref class ControllerBase abstract
    {
    private protected:
        IntPtr nativ;
        IntPtr umode;

    internal:
        template< typename cT >
        void createController( void ) {
            nativ = System::IntPtr( new stepflow::PinFieldController<cT>() );
        }
        stepflow::IControllerBase* controller(void) {
            return (stepflow::IControllerBase*)nativ.ToPointer();
                                                       }
        template< typename cT > void createMode(
            stepflow::PinFieldController<cT>* onController,
            Stepflow::ControlMode modeCode, Stepflow::TypeCode typeCode
                                                );
        void* mod(void) {
            return umode.ToPointer();
                           }

    protected:
        ControllerBase()
            : nativ( IntPtr::Zero )
            , umode( IntPtr::Zero ) {
        }
        virtual ~ControllerBase() {
            if( nativ != IntPtr::Zero ) {
                delete nativ.ToPointer();
                nativ = IntPtr::Zero;
            }
        }

    public:
        static uint    GetVersionNumber( void );
        String^        GetVersionString( void );
        Action^        CallPin( IntPtr pin );
        virtual bool   Check( void ) abstract;
        virtual void   Step( void ) abstract;
        virtual void   SetCheckAtGet( void ) abstract;
        virtual void   SetCheckAtSet( void ) abstract;
        virtual IntPtr GetPin( int pindex ) abstract;
        virtual void   SetPin( int pindex, IntPtr pt ) abstract;
        virtual IntPtr GetPin( Enum^ pinum ) abstract;
        virtual IntPtr GetTarget( void ) abstract;
        virtual void   SetTarget( IntPtr ) abstract;
        virtual IntPtr GetPointer( ControllerVariable ) abstract;
        virtual void   LetPoint( ControllerVariable, IntPtr ) abstract;
        IntPtr         GetHandle( void ) { return nativ; };
        IntPtr         GetModeHandle(void) { return umode; }
        virtual stepflow::ModeCodeVal GetModeCode() abstract;
        virtual Stepflow::TypeCode GetTypeCode() abstract;
        property ValenceFieldState ValenceField {
         public: ValenceFieldState get( void ) {
          return ValenceFieldState(
              &controller()->Active.State
                                    );
          };
        }
        void SetReadOnly( bool readonly ) {
            controller()->SetReadOnly( readonly );
        }
        bool SetLocked( bool shouldBeLocked ) {
            return controller()->SetLocked( shouldBeLocked );
        }
        bool IsLocked( void ) {
            return controller()->IsLocked();
        }
        property bool Active {
            bool get( void ) { return controller()->IsActive(); }
            void set( bool value ) { controller()->SetActive( value ); }
        }
        property Spin Invert {
            Spin get( void ) { return Spin( !controller()->HasFlags( stepflow::IS_INVERTED ) ); }
            void set( Spin inversive ) {
                if ( inversive == Spin::DOWN ) controller()->AddFlags( stepflow::IS_INVERTED );
                else controller()->RemFlags( stepflow::IS_INVERTED );
            }
        }
    };

    public ref class Int24Controller abstract
        : public ControllerBase
    {
        typedef Stepflow::Int24 MT;
        typedef ::Int24         NT;

    public:
        delegate MT BaseModeDelegate( ControlMode );
        delegate MT UserModeDelegate( MT%, MT%, MT%, MT% );

    protected:
        UserModeDelegate^ checkVALUE;
        stepflow::DelegateInt24Wrap::BaseModeFunction ModenativeCheckCall;
        virtual MT checkMODE( ControlMode mode ) {
            return MT( (Get()->GetUserMode<stepflow::DelegateInt24Wrap>()->*ModenativeCheckCall)( mode ) );
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
                ModenativeCheckCall = Get()->GetPin<stepflow::DelegateInt24Wrap::BaseModeFunction>( 1 );
            }
        }

    public:
        Int24Controller( void ) : ControllerBase() {
            nativ = IntPtr(new stepflow::PinFieldController<NT>());
        }
        Int24Controller( MT% reference ) : Int24Controller() {
            Get()->SetUp( 0, 0, NT( Stepflow::Int24::operator int(reference) ), 0, stepflow::CtrlMode::None );
            pin_ptr<MT> p = &reference;
            Get()->SetVariable( (NT*)p );
        }
        Int24Controller( ControlMode mode ) : Int24Controller() {
            Get()->SetUp( 0, 0, 0, 0, stepflow::CtrlMode::None );
            Mode = mode;
        }
        Int24Controller( MT min, MT max, MT mov, ControlMode mod ) : Int24Controller() {
            Get()->SetUp( (NT)min, (NT)max, (NT)0, (NT)mov, stepflow::CtrlMode::None );
            Mode = mod;
        }

        virtual void SetUp( MT min, MT max, MT mov, MT val, ControlMode mod ) {
            if( mod > ControlMode::PingPong ) {
                Get()->SetMIN( *(NT*)&min );
                Get()->SetMAX( *(NT*)&max );
                Get()->SetMOV( *(NT*)&mov );
                *Get()->GetPointer() = *(NT*)&val;
            }
            else {
                Get()->SetUp(
                    *(NT*)&min, *(NT*)&max,
                    *(NT*)&val, *(NT*)&mov,
                byte(mod) );
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get( void ) {
                return (ControlMode)Get()->Mode();
            }
            void set( ControlMode value ) {
                if ( value <= ControlMode::PingPong ) {
                    Get()->Mode( stepflow::CtrlMode( (uint)value ) );
                } else createMode<NT>( Get(), value, GetTypeCode() );
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

        virtual bool Check( void ) override {
            return Get()->Check();
        }
        virtual void Step( void ) override {
            Get()->Step();
        }
        virtual void SetCheckAtGet( void ) override {
            Get()->SetCheckAtGet( true );
        }
        virtual void SetCheckAtSet( void ) override {
            Get()->SetCheckAtGet( false );
        }

        property UserModeDelegate^ AttachedDelegate {
            UserModeDelegate^ get( void ) {
                if( Mode == ControlMode::Delegate ) {
                    return (UserModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr( Get()->GetPin<stepflow::DelegateInt24Wrap::DelegateFunction>( 0 ) ),
                        UserModeDelegate::typeid );
                } else return nullptr;
            }
            void set( UserModeDelegate^ value ) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateInt24Wrap>()->SetDelegateFunction(
                    (stepflow::DelegateInt24Wrap::DelegateFunction)
                    Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                ); FetchBaseModeCall();
            }
        }

        static operator MT( Int24Controller^ This ) {
            return MT( NT( *This->Get() ) );
        }
        virtual void SetPin( Enum^ pinum, MT% val ) {
            pin_ptr<MT> p = &val;
            Get()->SetPin( *(NT*)p, ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture ) );
        }
        virtual void SetPin( int index, System::IntPtr ptr ) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>( *typed, index );
        }
        virtual IntPtr GetPin( int pindex ) override {
            return IntPtr( &Get()->GetPin<NT>(pindex) );
        }
        virtual IntPtr GetPin( Enum^ pinnum ) override {
            return GetPin( ((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture ) );
        }
        virtual IntPtr GetTarget( void ) override {
            return IntPtr( Get()->GetPointer() );
        }
        virtual void SetTarget( IntPtr target ) override {
            Get()->SetVariable( (NT*)target.ToPointer() );
        }
        virtual void SetTarget( MT% target ) {
            pin_ptr<MT> p = &target;
            Get()->SetVariable( (NT*)p );
        }
        virtual IntPtr GetPointer( ControllerVariable index ) override {
            return IntPtr(
                Get()->getPTR( stepflow::ControllerVariables( index ) )
            );
        }
        virtual void LetPoint( ControllerVariable varnam, IntPtr target ) override {
            Get()->LetPoint(
                stepflow::ControllerVariables(varnam), (NT*)target.ToPointer()
            );
        }
    };


    public ref class UInt24Controller abstract
        : public ControllerBase
    {
        typedef Stepflow::UInt24 MT;
        typedef ::UInt24         NT;

    public:
        delegate MT BaseModeDelegate( ControlMode );
        delegate MT UserModeDelegate( MT%, MT%, MT%, MT% );

    protected:
        UserModeDelegate^ checkVALUE;
        stepflow::DelegateUInt24Wrap::BaseModeFunction ModenativeCheckCall;
        virtual MT checkMODE( ControlMode mode ) {
            return MT( (Get()->GetUserMode<stepflow::DelegateUInt24Wrap>()->*ModenativeCheckCall)( mode ) );
        }
    internal:
        stepflow::PinFieldController<NT>* Get() {
            return (stepflow::PinFieldController<NT>*)ControllerBase::controller();
        }
        void Set( stepflow::PinFieldController<NT>* ntiv ) {
            nativ = IntPtr(ntiv);
        }
        void FetchBaseModeCall( void ) {
            if( Mode == ControlMode::Delegate ) {
                ModenativeCheckCall = Get()->GetPin<stepflow::DelegateUInt24Wrap::BaseModeFunction>( 1 );
            }
        }

    public:
        UInt24Controller( void ) : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<NT>() );
        }
        UInt24Controller( MT% reference ) : UInt24Controller() {
            Get()->SetUp( 0, 0, (stepflow::i16)reference, 0, byte(stepflow::CtrlMode::None) );
            pin_ptr<MT> p = &reference;
            Get()->SetVariable( (NT*)p );
        }
        UInt24Controller( ControlMode mode ) : UInt24Controller() {
            Get()->SetUp( 0, 0, 0, 0, stepflow::CtrlMode::None );
            Mode = mode;
        }
        UInt24Controller( MT min, MT max, MT mov, ControlMode mod )
            : UInt24Controller() {
            Get()->SetUp( (stepflow::i24&)min, (stepflow::i24&)max,
                       0, (stepflow::i24&)mov, stepflow::CtrlMode::None
            );
            Mode = mod;
        }

        virtual void SetUp( MT min, MT max, MT mov, MT val, ControlMode mod ) {
            if( mod > ControlMode::PingPong ) {
                Get()->MIN = reinterpret_cast<NT&>( min );
                Get()->MAX = reinterpret_cast<NT&>( max );
                Get()->MOV = reinterpret_cast<NT&>( mov );
                *Get()->GetPointer() = reinterpret_cast<NT&>( val );;
            }
            else {
                Get()->SetUp(
                    (stepflow::i24&)min,
                    (stepflow::i24&)max,
                    (stepflow::i24&)val,
                    (stepflow::i24&)mov,
                byte(mod) );
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get(void) {
                return (ControlMode)Get()->Mode();
            }
            void set(ControlMode value) {
                if (value <= ControlMode::PingPong) {
                    Get()->Mode(stepflow::CtrlMode((uint)value));
                } else createMode<NT>( Get(), value,
                                       GetTypeCode() );
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

        property UserModeDelegate^ AttachedDelegate {
            UserModeDelegate^ get(void) {
                if (Mode == ControlMode::Delegate) {
                    return (UserModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr(Get()->GetPin<stepflow::DelegateUInt24Wrap::DelegateFunction>( 0 ) ),
                        UserModeDelegate::typeid );
                }
                else return nullptr;
            }
            void set(UserModeDelegate^ value) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateUInt24Wrap>()->SetDelegateFunction(
                    (stepflow::DelegateUInt24Wrap::DelegateFunction)
                    Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                ); FetchBaseModeCall();
            }
        }

        static operator MT( UInt24Controller^ This ) {
            return MT( NT( *This->Get() ) );
        }
        virtual void SetPin( Enum^ pinum, MT% val ) {
            pin_ptr<MT> p = &val;
            Get()->SetPin( *(NT*)p, ((IConvertible^)pinum)->ToInt32(
                                    System::Globalization::CultureInfo::CurrentCulture )
            );
        }
        virtual void SetPin( int index, System::IntPtr ptr ) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>( *typed, index );
        }
        virtual IntPtr GetPin( int pindex ) override {
            return IntPtr( &Get()->GetPin<NT>( pindex ) );
        }
        virtual IntPtr GetPin( Enum^ pinnum ) override {
            return GetPin( ((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture) );
        }
        virtual IntPtr GetTarget( void ) override {
            return IntPtr( Get()->GetPointer() );
        }
        virtual void SetTarget( IntPtr target ) override {
            Get()->SetVariable( (NT*)target.ToPointer() );
        }
        virtual void SetTarget( MT% target ) {
            pin_ptr<MT> p = &target;
            Get()->SetVariable( (NT*)p );
        }
        virtual IntPtr GetPointer( ControllerVariable index ) override {
            return IntPtr( Get()->getPTR( stepflow::ControllerVariables(index) ) );
        }
        virtual void LetPoint( ControllerVariable varnam, IntPtr target ) override {
            Get()->LetPoint( stepflow::ControllerVariables(varnam), (NT*)target.ToPointer() );
        }
    };

    template<typename cT> public 
    ref class ValueController
     : public ControllerBase
    {
    public:
        delegate cT BaseModeDelegate( ControlMode );
        delegate cT UserModeDelegate( cT%, cT%, cT%, cT% );
       
    protected:
        UserModeDelegate^ checkVALUE;
        cT (stepflow::DelegateWrap<cT>::*ModenativeCheckCall) (ControlMode);
        virtual cT checkMODE( ControlMode mode ) {
            return (Get()->GetUserMode<stepflow::DelegateWrap<cT>>()->*ModenativeCheckCall)( mode );
        }

    internal:
        stepflow::PinFieldController<cT>* Get() {
            return (stepflow::PinFieldController<cT>*)ControllerBase::controller();
        }
        void Set( stepflow::PinFieldController<cT>* ntiv ) {
            nativ = IntPtr( ntiv );
        }
        void FetchBaseModeCall(void) {
            if (Mode == ControlMode::Delegate) {
                ModenativeCheckCall = Get()->GetPin<stepflow::DelegateWrap<cT>::BaseModeFunction>( 1 );
            }
        }

    public:
        ValueController( void ) : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<cT>() );
        }
        ValueController( cT% reference ) : ValueController() {
            Get()->SetUp( 0, 0, reference, 0, stepflow::CtrlMode::None );
            pin_ptr<cT> p = &reference;
            Get()->SetVariable( p );
        }
        ValueController( ControlMode mode ) : ValueController() {
            Get()->SetUp( 0, 0, 0, 0, stepflow::CtrlMode::None );
            Mode = mode;
        }
        ValueController( cT min, cT max, cT mov, ControlMode mod ) : ValueController() {
            Get()->SetUp( min, max, 0, mov, stepflow::CtrlMode::None );
            Mode = mod;
        }

        virtual void SetUp( cT min, cT max, cT mov, cT val, ControlMode mod ) {
            if( mod > ControlMode::PingPong ) {
                Get()->MIN = min;
                Get()->MAX = max;
                Get()->MOV = mov;
               *Get()->GetPointer() = val;
            } else {
                Get()->SetUp( min, max, val, mov, byte(mod) );
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get( void ) {
                return (ControlMode)Get()->Mode();
            }
            void set( ControlMode value ) {
                if( value <= ControlMode::PingPong ) {
                    Get()->Mode( stepflow::CtrlMode( (uint)value ) );
                } else createMode<cT>( Get(), value, GetTypeCode() );
            }
        }

        virtual property cT VAL {
            cT get(void) { return *Get(); }
            void set(cT value) { *Get() = value; }
        }
        virtual property cT MIN {
            cT get(void) { return Get()->MIN; }
            void set(cT value) { Get()->MIN = value; }
        }
        virtual property cT MAX {
            cT get(void) { return Get()->MAX; }
            void set(cT value) { Get()->MAX = value; }
        }
        virtual property cT MOV {
            cT get(void) { return Get()->MOV; }
            void set(cT value) { Get()->MOV = value; }
        }

        virtual bool Check( void ) override {
            return Get()->Check();
        }
        virtual void Step( void ) override {
            Get()->Step();
        }
        virtual void SetCheckAtGet( void ) override {
            Get()->SetCheckAtGet( true );
        }
        virtual void SetCheckAtSet( void ) override {
            Get()->SetCheckAtGet( false );
        }

        property UserModeDelegate^ AttachedDelegate {
            UserModeDelegate^ get( void ) {
                if ( Mode == ControlMode::Delegate ) {
                    return (UserModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr( Get()->GetPin<stepflow::DelegateWrap<cT>::DelegateFunction>( 0 ) ),
                        UserModeDelegate::typeid                                      );
                } else return nullptr; }
            void set( UserModeDelegate^ value ) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateWrap<cT>>()->SetDelegateFunction(
                    (stepflow::DelegateWrap<cT>::DelegateFunction)
                    Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                ); FetchBaseModeCall();
            }
        }

        static operator cT( ValueController^ This ) {
            return *This->Get();
        }
        virtual void SetPin( Enum^ pinum, cT% val ) {
            pin_ptr<cT> p = &val;
            Get()->SetPin( *p, ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture ) );
        }
        virtual void SetPin( int index, System::IntPtr ptr ) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>( *typed, index );
        }
        virtual IntPtr GetPin( int pindex ) override {
            return IntPtr( &Get()->GetPin<cT>( pindex ) );
        }
        virtual IntPtr GetPin( Enum^ pinnum ) override {
            return GetPin( ((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture) );
        }
        virtual IntPtr GetTarget( void ) override {
            return IntPtr( Get()->GetPointer() );
        }
        virtual void SetTarget( IntPtr target ) override {
            Get()->SetVariable( (cT*)target.ToPointer() );
        }
        virtual void SetTarget( cT% target ) {
            pin_ptr<cT> p = &target;
            Get()->SetVariable( p );
        }
        virtual IntPtr GetPointer( ControllerVariable index ) override {
            return IntPtr( Get()->getPTR( stepflow::ControllerVariables(index) ) );
        }
        virtual void LetPoint( ControllerVariable varnam, IntPtr target ) override {
            Get()->LetPoint( stepflow::ControllerVariables( varnam ), (cT*)target.ToPointer() );
        }

    };


    public ref class Controlled : public ControllerBase
    {
    public:
        DefineControlledType( Byte );
        DefineControlledType( Int8 );
        DefineControlledType( Int16 );
        DefineControlledType( Int32 );
        DefineControlledType( Int64 );
        DefineControlledType( UInt16 );
        DefineControlledType( UInt32 );
        DefineControlledType( UInt64 );
        DefineControlledType( Float32 );
        DefineControlledType( Float64 );

#undef MIN
#undef MAX
#undef MOV

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
            virtual Stepflow::TypeCode GetTypeCode( void ) override {
                return Stepflow::TypeCode::Int24;
            }
            virtual stepflow::ModeCodeVal GetModeCode( void ) override {
                return (stepflow::ModeCodeVal)this->Mode;
            }
            Int24^ Clone( void ) {
                byte state = Get()->getConfiguration();
                Int24^ clone = gcnew Int24();
                Get()->Active = false;
                clone->Get()->SetUp(
                    Get()->MIN, Get()->MAX, *Get(),
                    Get()->MOV, stepflow::CtrlMode::None
                );
                clone->Mode = this->Mode;
                clone->Get()->SetFlags( state );
                this->Get()->SetFlags( state );
                return clone;
            }
            Int24^ Refer( void ) {
                Int24^ other = gcnew Int24();
                other->nativ = IntPtr(Get());
                return other;
            }
            static operator TYPE( Controlled::Int24^ This ) {
                return *(TYPE*)This->Get()->getVALpt();
            }
            TYPE operator()() { return VAL; }
            Int24^ operator()( TYPE set ) { VAL = set; return this; }
        };

        // Unsigned 24bit controlled integer type
        ref class UInt24
            : public UInt24Controller {
        public:
            typedef Stepflow::UInt24 TYPE;
            UInt24() : UInt24Controller() {}
            UInt24( ControlMode m ) : UInt24Controller( m ) {}
            UInt24( TYPE min, TYPE max, TYPE mov, ControlMode mod )
                : UInt24Controller( min, max, mov, mod ) {}
            UInt24( TYPE% refVar ) : UInt24Controller( refVar ) {}
            virtual Stepflow::TypeCode GetTypeCode( void ) override {
                return Stepflow::TypeCode::UInt24;
            }
            virtual stepflow::ModeCodeVal GetModeCode( void ) override {
                return stepflow::ModeCodeVal( this->Mode );
            }
            UInt24^ Clone( void ) {
                byte state = Get()->getConfiguration();
                UInt24^ clone = gcnew UInt24();
                Get()->Active = false;
                clone->Get()->SetUp(
                    Get()->MIN, Get()->MAX,
                    *Get(), Get()->MOV,
                    stepflow::CtrlMode::None
                );
                clone->Mode = this->Mode;
                clone->Get()->SetFlags( state );
                this->Get()->SetFlags( state );
                return clone;
            }
            UInt24^ Refer( void ) {
                UInt24^ other = gcnew UInt24();
                other->nativ = IntPtr( Get() );
                return other;
            }
            static operator TYPE( Controlled::UInt24^ This ) {
                return *(TYPE*)This->Get()->getVALpt();
            }
            TYPE operator()() { return VAL; }
            UInt24^ operator()( TYPE set ) {
                VAL = set;
                return this;
            }
        };
         
        Controlled() {};
    };
} //end of Stepflow

//#include "Konstrukt24.hpp"

#endif