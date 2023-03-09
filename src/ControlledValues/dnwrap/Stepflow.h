/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ControlledValues.hpp                       ||
||     Author:    Kalle                                      ||
||     Generated: 16.02.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _ControlledValues_hpp_
#define _ControlledValues_hpp_

#include "ProjectHeader.hpp"
#include <Stepflow.hpp>

#using   <System.Core.Dll>
#include "ConstStruct.hpp"
#include "CreationMacro.hpp"
#include "MessageLogger.hpp"
#include <typeinfo>

using namespace System;
using namespace System::Runtime::InteropServices;


#include <ControllerAccess.h>
namespace stepflow {

    template<typename N, typename M = N> class DB;
    template<typename T> using DelegateFunction = T(*)(T%, T%, T%, T%);
    template<typename M, template<typename wtN, typename wtM = M> typename W = DB, typename N = M>
        using BaseModeFunction = N(W<N,M>::*)( Stepflow::ControlMode );

    template<typename N, typename M > class DB
        : public stepflow::MovingValue<N>
    {
    protected: typedef DB<N,M> WRAP;
        typedef stepflow::MovingValue<N> BASE;
        typedef N(DB::*BaseModeFunction)( Stepflow::ControlMode );
         DelegateFunction<M> FUNCPTR;
         BaseModeFunction    BASEPTR;

         virtual uint modeCodeBase(void) const {
             return *(uint*)"FUNC";
         }
         N baseCallFunc( Stepflow::ControlMode mod ) {
             return BASE::checkMODE( stepflow::CtrlMode(mod) );
         }
    public:
        virtual void* Pin( void* pin, int idx ) {
            return idx > 1 ? BASE::Pin( pin, idx - 2 ) :
                idx ? (void*)&BASEPTR : (void*)&FUNCPTR;
        }
        virtual void Init( void ) {
            BASE::PIN_COUNT += 2;
            BASEPTR = GetBaseCallFunction();
            if (std::numeric_limits<N>::is_integer) {
                MOV = N(1);
                MIN = std::numeric_limits<N>::min();
                MAX = std::numeric_limits<N>::max();
            }
            else {
                MOV = std::numeric_limits<N>::epsilon();
                MIN = N(-1.0f);
                MAX = N(1.0f);
            }
        }
        void SetDelegateFunction( DelegateFunction<M> func ) {
            FUNCPTR = func;
        }
        BaseModeFunction GetBaseCallFunction( void ) {
            return &DB::baseCallFunc;
        }
    };

    template<typename cT>
    class DelegateWrap
        : public DB<cT,cT>
    {     friend Stepflow::ValueController<cT>;
    typedef DB<cT,cT> BASE;
    protected:

        virtual unsigned usrModeSize() const {
            return sizeof(DelegateWrap<cT>);
        }

    public:

        virtual cT checkVALUE( cT* value ) {
            return BASE::FUNCPTR( *value, MIN, MAX, MOV );
        }

    };

    template<typename nT,typename mT>
    class DelegateMode
        : public DB<nT,mT>
    {     friend Stepflow::StructController<mT,nT>;
    typedef DB<nT,mT> BASE;
    protected:

        virtual unsigned usrModeSize() const {
            return sizeof(DelegateMode<nT,mT>);
        }

    public:

        virtual nT checkVALUE( nT* value ) {
            return nT( BASE::FUNCPTR(
                *reinterpret_cast<mT*>(value),
                *reinterpret_cast<mT*>(BASE::controller->getMINpt()),
                *reinterpret_cast<mT*>(BASE::controller->getMAXpt()),
                *reinterpret_cast<mT*>(BASE::controller->getMOVpt())
            ) );
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
        void* mod( void ) {
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
        virtual Stepflow::TypeCode    GetTypeCode() abstract;
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
                if ( inversive == Spin::DOWN )
                     controller()->AddFlags( stepflow::IS_INVERTED );
                else controller()->RemFlags( stepflow::IS_INVERTED );
            }
        }
    };

    template<typename MT,typename NT>
    public ref class StructController abstract
        : public ControllerBase
    {
        typedef stepflow::BaseModeFunction<MT,stepflow::DelegateMode,NT> BaseModeFunc;
    public:
        delegate MT BaseModeDelegate( ControlMode );
        delegate MT UserModeDelegate( MT%, MT%, MT%, MT% );

    protected:
        UserModeDelegate^ checkVALUE;
        BaseModeFunc      ModenativeCheckCall;

        virtual MT checkMODE( ControlMode mode ) {
            return MT( ( Get()->GetUserMode<stepflow::DelegateMode<NT,MT>>()->*ModenativeCheckCall)( mode ) );
        }

    internal:
        stepflow::PinFieldController<NT>* Get() {
            return (stepflow::PinFieldController<NT>*)ControllerBase::controller();
        }
        void Set( stepflow::PinFieldController<NT>* ntiv ) {
            nativ = IntPtr( ntiv );
        }
        void FetchBaseModeCall( void ) {
            if( Mode == ControlMode::Delegate ) {
                ModenativeCheckCall = Get()->GetPin<BaseModeFunc>( 1 );
            }
        }

    public:
        StructController( void ) : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<NT>() );
        }
        StructController( MT% reference ) : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<NT>() );
            Get()->SetUp( NT(0), NT(0), reinterpret_cast<NT&>(reference),
                          NT(0), byte( stepflow::CtrlMode::None ) );
            pin_ptr<MT> p = &reference;
            Get()->SetVariable( (NT*)p );
        }
        StructController( ControlMode mode ) : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<NT>() );
            Get()->SetUp( NT(0), NT(0), NT(0), NT(0), byte(stepflow::CtrlMode::None) );
            Mode = mode;
        }
        StructController( MT min, MT max, MT mov, ControlMode mod )
            : ControllerBase() {
            nativ = IntPtr( new stepflow::PinFieldController<NT>() );
            Get()->SetUp(
                reinterpret_cast<NT&>(min),
                reinterpret_cast<NT&>(max), NT(0),
                reinterpret_cast<NT&>(mov),
                byte( stepflow::CtrlMode::None ) );
            Mode = mod;
        }

        virtual void SetUp( MT min, MT max, MT mov, MT val, ControlMode mod ) override {
            if( mod > ControlMode::PingPong ) {
                Get()->MIN = reinterpret_cast<NT&>(min);
                Get()->MAX = reinterpret_cast<NT&>(max);
                Get()->MOV = reinterpret_cast<NT&>(mov);
                *Get()->GetPointer() = reinterpret_cast<NT&>(val);
            } else {
                Get()->SetUp(
                    reinterpret_cast<NT&>(min),
                    reinterpret_cast<NT&>(max),
                    reinterpret_cast<NT&>(val),
                    reinterpret_cast<NT&>(mov),
                    byte( (stepflow::CtrlMode)mod )
                );
            } Mode = mod;
        }

        property ControlMode Mode {
            ControlMode get( void ) {
                return (ControlMode)Get()->Mode();
            }
            void set( ControlMode mode ) {
                if( mode <= ControlMode::PingPong ) {
                    Get()->Mode( stepflow::CtrlMode( (uint)mode ) );
                } else this->createMode<NT>( Get(), mode, GetTypeCode() );
            }
        }

        virtual property MT VAL {
            MT get( void ) { return reinterpret_cast<MT&>( *Get() ); }
            void set( MT value ) { *Get() = reinterpret_cast<NT&>( value ); }
        }
        virtual property MT MIN {
            MT get( void ) { return *reinterpret_cast<MT*>( Get()->getMINpt() ); }
            void set( MT value ) { Get()->MIN = reinterpret_cast<NT&>( value ); }
        }
        virtual property MT MAX {
            MT get(void) { return *reinterpret_cast<MT*>( Get()->getMAXpt() ); }
            void set(MT value) { Get()->MAX = reinterpret_cast<NT&>( value ); }
        }
        virtual property MT MOV {
            MT get(void) { return *reinterpret_cast<MT*>( Get()->getMOVpt() ); }
            void set(MT value) { Get()->MOV = reinterpret_cast<NT&>( value ); }
        }

        virtual bool Check(void) override {
            return Get()->Check();
        }
        virtual void Step(void) override {
            Get()->Step();
        }
        virtual void SetCheckAtGet( void ) override {
            Get()->SetCheckAtGet( true );
        }
        virtual void SetCheckAtSet( void ) override {
            Get()->SetCheckAtGet( false );
        }

        property UserModeDelegate^ AttachedDelegate {
            UserModeDelegate^ get(void) {
                if( Mode == ControlMode::Delegate ) {
                    return (UserModeDelegate^)Marshal::GetDelegateForFunctionPointer(
                        IntPtr( Get()->GetPin<stepflow::DelegateFunction<MT>>( 0 ) ),
                        UserModeDelegate::typeid                                     );
                } else return nullptr;
            }
            void set( UserModeDelegate^ value ) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateMode<NT,MT>>()->SetDelegateFunction( 
                    (stepflow::DelegateFunction<MT>) Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                                                                                          );
                FetchBaseModeCall();
            }
        }

        static operator MT( StructController^ This ) {
            return reinterpret_cast<MT&>( *This->Get() );
        }
        virtual void SetPin( Enum^ pinum, MT% val ) {
            // pin_ptr<MT> p = &val;
            Get()->SetPin( ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture ),
                reinterpret_cast<NT&>( val )
            );
               // *(NT*)p );
        }
        virtual void SetPin( int index, System::IntPtr ptr ) override {
           // ulong* typed = (ulong*)ptr.ToPointer();
           // Get()->SetPin<ulong>( index, *typed );
            Get()->SetPin<ulong>( index, *(ulong*)ptr.ToPointer() );
        }
        virtual IntPtr GetPin( int pindex ) override {
            return IntPtr( &Get()->GetPin<NT>( pindex ) );
        }
        virtual IntPtr GetPin( Enum^ pinnum ) override {
            return GetPin( ((IConvertible^)pinnum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture )
            );
        }
        virtual IntPtr GetTarget( void ) override {
            return IntPtr( Get()->GetPointer() );
        }
        virtual void SetTarget( IntPtr target ) override {
            Get()->SetVariable( (NT*)target.ToPointer() );
        }
        virtual void SetTarget( MT% target ) {
            interior_ptr<MT> pt( &target );
            Get()->SetVariable( (NT*)&pt );
           //  pin_ptr<MT> p = &target;
           // Get()->SetVariable( (NT*)p );
        }
        virtual IntPtr GetPointer( ControllerVariable index ) override {
            return IntPtr( Get()->getPTR( stepflow::ControllerVariables( index ) ) );
        }
        virtual void LetPoint( ControllerVariable varnam, IntPtr target ) override {
            Get()->LetPoint( stepflow::ControllerVariables(varnam), (NT*)target.ToPointer() );
        }
    };

    template<typename cT> public 
    ref class ValueController
     : public ControllerBase
    {
        typedef stepflow::BaseModeFunction<cT,stepflow::DB,cT> BaseModeFunc;
    public:
        typedef  cT MT,NT;
        delegate MT BaseModeDelegate( ControlMode );
        delegate MT UserModeDelegate( MT%, MT%, MT%, MT% );
       
    protected:
        UserModeDelegate^ checkVALUE;
        BaseModeFunc      ModenativeCheckCall;

        virtual cT checkMODE( ControlMode mode ) {
            return ( ((stepflow::DelegateWrap<cT>*)Get()->GetUserMode())->*ModenativeCheckCall )( mode );
        }

    internal:
        stepflow::PinFieldController<cT>* Get() {
            return (stepflow::PinFieldController<cT>*)ControllerBase::controller();
        }
        void Set( stepflow::PinFieldController<cT>* ntiv ) {
            nativ = IntPtr( ntiv );
        }
        void FetchBaseModeCall( void ) {
            if( Mode == ControlMode::Delegate ) {
                ModenativeCheckCall = Get()->GetPin<BaseModeFunc>( 1 );
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

        virtual void SetUp( cT min, cT max, cT mov, cT val, ControlMode mod ) override {
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
                        IntPtr( Get()->GetPin<stepflow::DelegateFunction<cT>>( 0 ) ),
                        UserModeDelegate::typeid                                     );
                } else return nullptr; }
            void set( UserModeDelegate^ value ) {
                checkVALUE = value;
                Mode = ControlMode::Delegate;
                Get()->GetUserMode<stepflow::DelegateWrap<cT>>()->SetDelegateFunction(
                     (stepflow::DelegateFunction<cT>)Marshal::GetFunctionPointerForDelegate( checkVALUE ).ToPointer()
                                                                                   );
                FetchBaseModeCall();
            }
        }

        static operator cT( ValueController^ This ) {
            return *This->Get();
        }
        virtual void SetPin( Enum^ pinum, cT% val ) {
            pin_ptr<cT> p = &val;
            Get()->SetPin( ((IConvertible^)pinum)->ToInt32(
                System::Globalization::CultureInfo::CurrentCulture ), *p );
        }
        virtual void SetPin( int index, System::IntPtr ptr ) override {
            ulong* typed = (ulong*)ptr.ToPointer();
            Get()->SetPin<ulong>( index, *typed );
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
        
        DefineControlledType( Int8 );
        DefineControlledType( Int16 );
        DefineControlledImpl( Int24 );
        DefineControlledType( Int32 );
        DefineControlledType( Int64 );

        DefineControlledType( UInt8 );
        DefineControlledType( UInt16 );
        DefineControlledImpl( UInt24 );
        DefineControlledType( UInt32 );
        DefineControlledType( UInt64 );

        DefineControlledImpl( Float16 );
        DefineControlledType( Float32 );
        DefineControlledType( Float64 );

#undef MIN
#undef MAX
#undef MOV

        Controlled() {};
    };

} //end of Stepflow

//#include "Konstrukt24.hpp"

#endif