#ifndef ControlledPointer_included
#define ControlledPointer_included

#ifdef BEGIN_STEPFLOW_NAMESPACE
BEGIN_STEPFLOW_NAMESPACE
#endif

template<class pT, const unsigned cE>
class EventTrigger;

    template<class O> // where 'O' may be any kind of object pointer
    class ControlledPtr
        : public VariableController<size_t>
    {
    protected:
        virtual size_t checkValue(byte mode) {
            if (mode < USERMODE)
                CustomControlMode->checkVALUE(_Value);
            return VariableController<size_t>::checkValue(mode);
        }
        virtual size_t GetValue(void) {
            return GetUserMode<EventTrigger<size_t,5>>()->RefCount(*_Value);
        }
    public:
        typedef O* TYPE;
        virtual uint controllerCode() const { return 3; }
        virtual ControlledPtr* SetUp(O* min, O* max=NULL, int mov = sizeof(O), O* val=NULL) {
            SetUserMode<EventTrigger<size_t, 5>>(size_t(min), size_t(max!=NULL?max:min), size_t(val!=NULL?val:min), mov);
            Active = false;
            Mode(USERMODE);
            SetCheckAtGet(false);
            *_Value = size_t(val);
            return this;
        }
        void SetEventHandler(int pindex, O* (*triggeredFunction) (O*) ) {
            GetPin<O*(*)(O*)>(pindex) = triggeredFunction;
        }

        O* operator()(void) {
            return (O*) GetValue();
        }
        O* operator()(int advancer) {
            return (O*) SetValue( *_Value + (MOVE * advancer) );
        }

        operator O*(void) {
            return (O*)*_Value;
        }
        O* operator =(O* other) {
            return (O*) SetValue( size_t( other ) );
        }
        ControlledPtr& operator &(void) {
            ControlledPtr* controllercopy = new ControlledPtr();
            controllercopy->SetUp((O*)MIN, (O*)MAX, MOVE, (O*)GetValue());
            controllercopy->ControllerActive.State = this->ControllerActive.State;
            return *(ControlledPtr*)(void*)controllercopy;
        }
        O& operator *(void) {
            return  *(O*)*_Value;
        }
        O* operator->(void) {
            return (O*)*_Value;
        }
        O* operator+(int other) {
            return (O*)(*_Value + (MOVE * other));
        }
        O* operator+=(int other) {
            return operator()(other);
        }
        O* operator++() {
            return operator()(1);
        }
        O* operator++(int) {
            size_t temp = *_Value;
            SetValue(temp + MOVE);
            return (O*)temp;
        }
        O* operator-(int other) {
            return (O*)(*_Value - (MOVE * other));
        }
        O* operator-=(int other) {
            return operator()(-other);
        }
        O* operator--() {
            return operator()(-1);
        }
        O* operator--(int) {
            size_t temp = *_Value;
            SetValue(temp - MOVE);
            return (O*)temp;
        }
        O* operator[](int idx) {
            O* This = this;
            SetMIN(MIN);
            this += idx;
            O* That = this;
            this = This;
            return That;
        }

        bool active(int activate = EMPTY) {
            if (activate != EMPTY)
                Active = activate ? true : false;
            return (bool)Active;
        }
    };



/////////////////////////////////////////////////////////////
// Usermode: EventTrigger
// Used by ControlledPtr controller. implements a small
// 'object internal event system' which is used to trigger
// destruction on pointer lost and/or to reset pointer
// back into it's allowed range when setting it to some
// 'too far away' address was tried.


template<class pT, const unsigned cE>
class EventTrigger;

#ifdef ENDOF_STEPFLOW_NAMESPACE
ENDOF_STEPFLOW_NAMESPACE
#endif


template<class pT>
pT isOverTheTop(pT trigger) {
    printf("Pointer at range maximum: %p", trigger);
    return trigger;
}
template<class pT>
pT isBeneathZero(pT trigger) {
    printf("Pointer at nearly null: %p", trigger);
    return trigger;
}
template<class pT>
pT isGoneLost(pT lastReference) {
    printf("All pointer's references where droped..\nwill delete object at address '%p' pointed by last reference dropped", lastReference);
    delete (void*)lastReference;
    return 0;
}


#define READONLY_PINS
#include "../Macros/InternalAccess.h"

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW_NAMESPACE
#endif

    template<class pT, const unsigned cE>
    class EventTrigger : public UserModeControl< size_t >
    {
    public:
        enum PinValue { BeneathZero = 0, OverTheTop = 1, GoneLost = cE-1 };
        typedef pT(*EVENT)(pT);

    protected:
        typedef UserModeControl<size_t> BASE;
        pT(*Event[cE])(pT);
        int REFCOUNTER;
        virtual unsigned modeCodeBase(void) const {
                             return *(unsigned*)"EVNT";
                         }
        virtual unsigned usrModeSize() const {
                             return sizeof(EventTrigger);
                         }
        virtual void Init(void) {
            REFCOUNTER = 1;
            Event[BeneathZero] = &isBeneathZero;
            Event[OverTheTop] = &isOverTheTop;
            Event[GoneLost] = &isGoneLost;
            PIN_COUNT += cE;
        }

    public:
        AddPinArrayAccess(pT(*)(pT), cE, Event)

        size_t RefCount(size_t current) {
            REFCOUNTER++;
            return current;
        }
        virtual size_t checkVALUE(size_t* pVALUE) {
            if ( (((pT)*pVALUE) < MIN) || (((pT)*pVALUE) > MAX) ) {
                REFCOUNTER--;
            } else if (((pT)*pVALUE) == MAX) {
                *pVALUE = (size_t)Event[OverTheTop]((pT)*pVALUE);
            } else if (((pT)*pVALUE) == MIN ) {
                *pVALUE = (size_t)Event[BeneathZero]((pT)*pVALUE);
            }
            if (!REFCOUNTER)
                *pVALUE = (size_t)Event[GoneLost]((pT)MIN);
            return *pVALUE;
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW_NAMESPACE
#endif
#include "../Macros/InternalAccess.h"
#endif

