/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      PinControlled.hpp                          ||
||     Author:    Kalle                                      ||
||     Generated: 01.03.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _PinControlled_hpp_
#define _PinControlled_hpp_

BEGIN_STEPFLOW_NAMESPACE
#include "Macros/ModeExpress.h"

enum ValenceFieldState {
    VAL_EXTERN = ControllerStateFlags::MUST_DELETE,
    MIN_EXTERN = 0x10,
    MAX_EXTERN = 0x20,
    MOV_EXTERN = 0x40,
};

#define MERGE(token) token
#define IsPointer(var) ( (IControllerBase::Active.State & var ## _EXTERN) == var ## _EXTERN )
#define PassPoint(val) ( IsPointer(val) ? MERGE(vars.)val ##.p : MERGE(&vars.)val ## .v )
#define PassValue(val) ( IsPointer(val) ? MERGE(*vars.)val ##.p : MERGE(vars.)val ## .v )
#define MakeValue(pin) ( IControllerBase::Active &= ControllerStateFlags( MERGE(pin)_EXTERN ) )
#define ToPointer(pin) ( IControllerBase::Active |= ControllerStateFlags( MERGE(pin)_EXTERN ) )
#define AtIndexDo(idx,typ) ( idx < 1 ? typ(VAL) \
                           : idx < 2 ? typ(MIN) \
                           : idx < 3 ? typ(MAX) \
                           : idx < 4 ? typ(MOV):0 )

    template<typename cT> class IController;
    template<typename cT> class PinFieldController
                       : public IController<cT>
    { friend class UserModeControl<cT>;
    private:
        struct Var {
            union Val {
                ulong d;
                cT*   p;
                cT    v;
                Val() : d(0) {}
            };
            Val VAL;
            Val MIN;
            Val MAX;
            Val MOV;
            Var(void) {
                VAL.d = 0;
                MIN.d = 0;
                MAX.d = 0;
                MOV.d = 0;
            }
        } vars;
    protected:

        virtual cT checkValue( ControlMode mode ) {
            return IController<cT>::checkValue(
                PassPoint(VAL), PassPoint(MIN),
                PassPoint(MAX), PassPoint(MOV),
                mode
            );
        }

        // Internal setter..
        virtual cT SetValue(cT setter)
        {
            if (IsReadOnly(this)) return PassValue(VAL);
            else PassValue(VAL) = setter;
            return ( (IControllerBase::Active && (!CheckAtGet(this))) ?
                             checkValue(IControllerBase::controlmode) : PassValue(VAL) );
        }

        // Internal getter...
        virtual cT GetValue(void)
        {
            return ( (IControllerBase::Active && CheckAtGet(this)) ?
                          checkValue(IControllerBase::controlmode) : PassValue(VAL) );
        }

    public:
        PinFieldController(void) {
            vars.VAL.v = cT(0);
            vars.MIN.v = cT(0);
            vars.MAX.v = cT(0);
            vars.MOV.v = cT(0);
        }
        virtual ~PinFieldController(void) {
            if (!this->isValid()) {
                return;
            } if (IController<cT>::CustomControlMode) {
                delete IController<cT>::CustomControlMode;
                IController<cT>::CustomControlMode = NULL;
            }
        }
        virtual uint controllerCode() const {
            return 4;
        }
        virtual cT* getMINpt(void) {
            return PassPoint(MIN);
        }
        virtual cT* getMAXpt(void) {
            return PassPoint(MAX);
        }
        virtual cT* getMOVpt(void) {
            return PassPoint(MOV);
        }

        virtual void setMINpt(cT* val) {
            ToPointer(MIN);
            vars.MIN.p = val;
        }
        virtual void setMAXpt(cT* val) {
            ToPointer(MAX);
            vars.MAX.p = val;
        }
        virtual void setMOVpt(cT* val) {
            ToPointer(MOV);
            vars.MOV.p = val;
        }

        virtual cT* getPTR( uint idx ) {
            return idx < 1 ? PassPoint(VAL)
                 : idx < 2 ? PassPoint(MIN)
                 : idx < 3 ? PassPoint(MAX)
                 : idx < 4 ? PassPoint(MOV)
                 : &IController<cT>::GetPin<cT>( idx - 4 );
        }
        virtual void setPTR( uint idx, cT* ptr ) {
            switch (idx) {
            case Val: ToPointer(VAL); vars.VAL.p = ptr; break;
            case Min: ToPointer(MIN); vars.MIN.p = ptr; break;
            case Max: ToPointer(MAX); vars.MAX.p = ptr; break;
            case Mov: ToPointer(MOV); vars.MOV.p = ptr; break;
            }
        }

        virtual cT GetMIN(void) { return PassValue(MIN); };
        virtual cT GetMAX(void) { return PassValue(MAX); };
        virtual cT GetMOV(void) { return PassValue(MOV); };

        virtual void SetMIN(cT val) { PassValue(MIN) = val; };
        virtual void SetMAX(cT val) { PassValue(MAX) = val; };
        virtual void SetMOV(cT val) { PassValue(MOV) = val; };

        DECLPROP(cT, MIN);
        DECLPROP(cT, MAX);
        DECLPROP(cT, MOV);

        PROPLIST(cT*, PTR, ControllerVariablesCount);

        // get a raw pointer to the value as is,.. should be used just only for connectioning purposes
        // atherways the cT type cast should be used instead when just getting the actual instances value  
        virtual cT* GetPointer(void) {
            return  PassPoint(VAL);
        }
        virtual cT& GetVariable(void) {
            return  PassValue(VAL);
        }
        virtual void SetVariable( cT *var ) {
            LetPoint( ControllerVariables::Val, var );
        }
        void LetPoint( ControllerVariables index, cT* ptvar ) {
            if( ptvar ) {
                AtIndexDo(index, ToPointer);
                (&vars.VAL)[index].p = ptvar;
            } else {
                AtIndexDo(index, MakeValue);
                (&vars.VAL)[index].v = 0;
            }
        }
        virtual bool isValid(void) const {
            return ( IControllerBase::controlmode == USERMODE
                   ? IController<cT>::CustomControlMode : bool(1) );
        }

        // The primary candidate for getting actual value
        operator cT(void)
        {
            return GetValue();
        }

        // primary candidate for writing/setting the value
        cT operator =(cT set)
        {
            return SetValue(set);
        }

        // get pointer to the value by '&' reference-sign (!without causing check!) 
        cT* operator &(void)
        {
            return GetPointer();
        }

        // drop the controller and get the referenced variable dereferenced
        cT& operator *(void)
        {
            GetValue();
            return *GetPointer();
        }

        // short form of GetValue()
        cT operator()(void)
        {
            return GetValue();
        }

        bool operator ==(const IController<cT>& other) const
        {
            return !this->isValid()
                 ? !other.isValid() : other.isValid()
                 ? PassValue(VAL) == *other.GetPointer();
                 : false;
        }
        bool operator !=(const IController<cT>& other) const
        {
            return !(this->operator==(other));
        }

        bool operator ==(cT other) const
        {
            return PassValue(VAL) == other;
        }
        bool operator !=(cT other) const
        {
            return PassValue(VAL) != other;
        }
        bool operator >(cT other) const
        {
            return PassValue(VAL)  > other;
        }
        bool operator <(cT other) const
        {
            return PassValue(VAL) < other;
        }
        bool operator >=(cT other) const
        {
            return PassValue(VAL) >= other;
        }
        bool operator <=(cT other) const
        {
            return PassValue(VAL) <= other;
        }

        // perform arithmetical assignment-operator: +=
        cT operator +=(cT other)
        {
            return SetValue( GetValue() + other );
        }
        // perform arithmetical assignment-operator: -=
        cT operator -=(cT other)
        {
            return SetValue( GetValue() - other );
        }
        // perform arithmetical assignment-operator: *=
        cT operator *=(cT other)
        {
            return SetValue( GetValue() * other );
        }
        // perform arithmetical assignment-operator: /=
        cT operator /=(cT other)
        {
            return SetValue( GetValue() / other );
        }

    };

#undef IsPointer
#undef PassPoint
#undef PassValue
#undef MakeValue
#undef ToPointer
#undef AtIndexDo

#include "Macros/ModeExpress.h"
ENDOF_STEPFLOW_NAMESPACE

#endif
