/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      InterfaceControled.hpp                     ||
||     Author:    Kalle                                      ||
||     Generated: 25.01.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _BaseInterface_hpp_
#define _BaseInterface_hpp_

#if defined(__GNUC__)||defined(__MINGW32__)||defined(__MINGW64__)
#include "Macros/StepflowEmpty.h"
#include "ControlledValues.hpp"
#define ADVANCED_TEMPLATE_SUPPORT (__cplusplus >= 201103L)
#elif defined(_MSC_VER)
#define ADVANCED_TEMPLATE_SUPPORT (_MSC_VER >= 1900)
#else
#define ADVANCED_TEMPLATE_SUPPORT (0)
#endif


BEGIN_STEPFLOW_NAMESPACE

typedef unsigned char ModeID;
template<typename cT> class IController;

enum ControllerVariables
    : byte {
    Val, Min, Max, Mov,
    ControllerVariablesCount
};

enum ControllerStateFlags
    : char {
    DEACTIVATED = -128,
    MUST_DELETE = 0x01,
    CHECKED_GET = 0x02,
    IS_READONLY = 0x04,
    IS_INVERTED = 0x08,
};

#include "Macros/ModeExpress.h"

    class IControllerBase;
    struct ControllerState {
        friend class IControllerBase;
    public:

        char State;
        operator ControllerStateFlags(void) const {
            return ControllerStateFlags(State);
        }
        ControllerStateFlags operator &=(ControllerStateFlags That) {
            return ControllerStateFlags(State = (State & ~(char)That));
        }
        ControllerStateFlags operator |=(ControllerStateFlags That) {
            return ControllerStateFlags(State = (State | (char)That));
        }
        ControllerStateFlags operator =(ControllerStateFlags setter) {
            return ControllerStateFlags(State = *(char*)&setter);
        }
        ControllerStateFlags operator =(int setter) {
            return ControllerStateFlags(State = *(char*)&setter);
        }
        bool operator =(bool setter) {
            if((State >= 0) != setter) {
                State = char(setter
                      ? (State &~(char)DEACTIVATED)
                      : (State | (char)DEACTIVATED));
            } return setter;
        }
        operator bool(void) const {
            return (State >= 0);
        }
    };

    class IControllerBase
    {
    protected:
        ModeID controlmode;

    public:
        ControllerState Active;

        void SetFlags(int setflags) {
            Active.State = (ControllerStateFlags)setflags;
        }
        void AddFlags(ControllerStateFlags addflags) {
            Active |= addflags;
        }
        void RemFlags(ControllerStateFlags removals) {
            Active &= removals;
        }
        bool HasFlags(ControllerStateFlags contains) {
            return (Active.State & contains) == contains;
        }

        // magic number identifying type of the controller instance
        // and as part of the type code needed for determining size
        // of instance + attached mode instance (which differs in
        // fact of different structural layout) and it's needed for
        // adjusting pointers to the controlled value and to also
        // the MIN/MAX/MOV helper variables a controller allocates  
        virtual uint controllerCode(void) const = 0;
        // Try achieving a lock state which will prevent actually
        // processed section from being executed by other treads
        // then the current one on which SetLocked() is called.
        // if the actually attached control mode instance doesn't
        // supports any 'threadsafe' features it will return 'false'
        // if the actually attached control mode instance does
        // indeed support 'threadsafe' features, it will return
        // true if actually no other thread maybe executes any critical
        // lockable code sections, and it will lock that actually
        // progressed section for the actuall current thread for either:
        //  - until the locking thread reached end of critical section
        //  within progressing
        //  - until the locking thread releases the lock state by calling
        //  progress reached so it will be safe progressing the section
        // 
        virtual bool SetLocked(bool lock) = 0;
        virtual bool IsLocked(void) const = 0;

        virtual void SetActive(bool activate) { Active = activate; };
        virtual ModeCodeVal getModeCode(void) const { return controlmode; };
        virtual uint        getTypeSize(void) const = 0;
        virtual ModeCodeVal getTypeCode(void) const = 0;
        virtual uint        getModeSize(void) const { return controlmode < USERMODE ? 0 : EMPTY_(uint); }
        virtual ulong       getModeData(void) const { return controlmode < USERMODE ? 0 : EMPTY_(ulong); }
        bool         IsActive(void) const { return Active.operator bool(); };

        ///////////////////////////////////////////////////
        /// depricated,... use AddFlags/HasFlaggs instead

        ControllerStateFlags getConfiguration(void) const {
            return Active;
        }
        // if set <true> the value will be checked
        // on getting the value, not on setting it.
        void SetCheckAtGet(bool shouldCheckedAtGet) {
            if (shouldCheckedAtGet)
                Active |= CHECKED_GET;
            else
                Active &= CHECKED_GET;
        }
        // Sets the value not writable
        void SetReadOnly(bool shouldBeReadOnly) {
            if (shouldBeReadOnly)
                Active |= IS_READONLY;
            else
                Active &= IS_READONLY;
        }
    };

    template<typename cT>
    class IController
        : public IControllerBase {
    friend class UserModeControl<cT>;
    public:

        IController(void) {
            CustomControlMode = NULL;
            controlmode = None;
            SetFlags( DEACTIVATED
                    | CHECKED_GET );
        }

        virtual ~IController(void) {
            if (! this->isValid() ) {
                return;
            } if ( CustomControlMode ) {
                delete CustomControlMode;
                CustomControlMode = NULL;
            }
        }

        virtual bool isValid(void) const {
            return ( this != nullptr )
                && ( controlmode == USERMODE
                   ? CustomControlMode != NULL
                   : true );
        }

        virtual void SetActive( bool activate )
        {
            if( activate != Active ) {
                IControllerBase::SetActive( activate );
                if( CustomControlMode )
                    CustomControlMode->onActivate( activate );
            }
        }

        virtual ModeCodeVal getModeCode(void) const override {
            return CustomControlMode && controlmode >= USERMODE
                 ? CustomControlMode->getModeCode()
                 : ModeCodeVal( controlmode );
        }
        virtual uint getTypeSize(void) const override {
            return sizeof(cT);
        }
        virtual uint getModeSize(void) const override {
            uint size = IControllerBase::getModeSize();
            if ( is_not(size) ) {
                size = CustomControlMode->getModeSize();
            } return size;
        }
        virtual ulong getModeData(void) const override {
            ulong adr = IControllerBase::getModeData();
            if ( is_not(adr) ) {
                adr = CustomControlMode->getModeData();
                const_cast<IController<cT>*>(this)->CustomControlMode = NULL;
                return adr;
            } else return 0;
        }
        virtual ModeCodeVal getTypeCode(void) const override {
            if ( CustomControlMode && controlmode >= USERMODE)
                return CustomControlMode->getTypeCode();
            else return getTypeBias();
        }
        virtual bool SetLocked( bool lock ) {
            if( CustomControlMode ) {
                return CustomControlMode->lock( lock );
            } return false;
        }
        virtual bool IsLocked( void ) const {
            if( CustomControlMode ) {
                return (int)CustomControlMode->lockState() > 0;
            } return false;
        }

        void ReInitialize(void)
        {
            if ( isValid() ) {
                SetVariable( GetPointer() );
                AddFlags( DEACTIVATED );
                if( CustomControlMode )
                    CustomControlMode->InitFirst(
                        (void*)this, &IController::checkValue
                );
            }
        }

        void SwapOtherMode(IController<cT>* other)
        {
            cT min = GetMIN();
            cT max = GetMAX();
            cT mov = GetMOV();
            CtrlMode mode = Mode();
            UserModeControl<cT>* priorMode = (UserModeControl<cT>*)detachUserMode();
            ControllerStateFlags priorState = ControllerStateFlags(getConfiguration());
            cT* priorValue = GetPointer();
            bool mustdelete = MustDelete(this);
            if( other->controllerCode() == 1 ) {
                RemFlags( MUST_DELETE );
                SetVariable( other->GetPointer() );
            }
            SetUp( other->GetMIN(), other->GetMAX(), *other->GetPointer(),
                   other->GetMOV(), (UserModeControl<cT>*)other->detachUserMode()
                    );
            SetFlags(ControllerStateFlags(other->getConfiguration()));
            if (other->controllerCode() == 1) {
                other->RemFlags( MUST_DELETE );
                other->SetVariable(priorValue);
            } else if (mustdelete)
                AddFlags( MUST_DELETE );

            other->SetFlags(priorState);
            other->SetUp( min, max, *priorValue, mov, priorMode );
            if (other->controllerCode() == 1 && (!mustdelete))
                other->RemFlags( MUST_DELETE );
        }

        // get a controller variable by index
        cT getVAR( ControllerVariables idx ) {
            switch (idx) {
            case Val: return GetValue();
            case Min: return MIN;
            case Max: return MAX;
            case Mov: return MOV;
            default: return GetPin<cT>( idx );
            }
        }

        // set controller variable at 'idx' to value 'val'
        void setVAR( ControllerVariables idx, cT val) {
            switch (idx) {
            case Val: SetValue(val); break;
            case Min: *getMINpt() = val; break;
            case Max: *getMAXpt() = val; break;
            case Mov: *getMOVpt() = val; break;
            default: SetPin<cT>( val, (uint)idx );
                break;
            }
        }

        // get pointer to the (checked) value 
        virtual cT* getVALpt(void) { GetValue(); return GetPointer(); }
        //get a pointer to the MIN value.
        virtual cT* getMINpt(void) = 0;
        //get a pointer to the MAX value.
        virtual cT* getMAXpt(void) = 0;
        //get a pointer to the MOV value.
        virtual cT* getMOVpt(void) = 0;

        //Get the value.
        virtual cT GetVAL(void) { return GetValue(); }
        //Get the MIN boundery.
        virtual cT GetMIN(void) { return *getMINpt(); }
        //Get the MAX boundary.
        virtual cT GetMAX(void) { return *getMAXpt(); }
        //Get the modifier.
        virtual cT GetMOV(void) { return *getMOVpt(); }

        //Set the value.
        virtual void SetVAL(cT val) { SetValue( val ); }
        //Set the MIN boundary.
        virtual void SetMIN(cT val) { *getMINpt() = val; }
        //Set the MAX boundary.
        virtual void SetMAX(cT val) { *getMAXpt() = val; }
        //Set the modifier.
        virtual void SetMOV(cT val) { *getMOVpt() = val; }

        //Set the value to the value pointed by 'val'.
        virtual void SetVAL(cT* val) { SetValue(*val); }

        // declare 'MIN' property
        DECLPROP(cT, MIN);
        // declare 'MAX' property
        DECLPROP(cT, MAX);
        // declare 'MOV' property
        DECLPROP(cT, MOV);

        // declare VAR[] property-array {VAL,MIN,MAX,MOV}
        PROPLIST(cT, VAR, ControllerVariablesCount);

        //get a raw pointer to the controller instance
        virtual IController* GetController(void) const {
            return const_cast<IController*>(this);
        }
        // get a raw pointer to the variable which is controled (!without doing check) 
        virtual cT* GetPointer(void) = 0;
        // get a reference to the variable which is controlled (!without doing check)
        virtual cT& GetVariable(void) = 0;
        // from now on, this controller controls that variable pointed by 'var'.
        virtual void SetVariable( cT *var ) = 0;



        //Initializes a Controlled<ValueType> with MIN,MAX,Initial,MOVE and Mode...
        IController* SetUp(cT min, cT max, cT initial, cT move, unsigned char mode)
        {
            controlmode = None;
            VAR[Min] = min;
            VAR[Max] = max;
            VAR[Mov] = move;
            VAR[Val] = initial;
            Mode(mode);
            return this;
        }
        IController* SetUp(cT min, cT max, cT val, cT mov, CtrlMode mod)
        {
            return SetUp(min, max, val, mov, (unsigned char)mod);
        }
        template<class uMode = UserModeControl<cT>>
        IController* SetUp(cT min, cT max, cT val, cT mov)
        {
            SetUserMode<uMode>(min, max, val, mov);
            return this;
        }
#if ADVANCED_TEMPLATE_SUPPORT
        template<template<typename uModCtTy = cT>class uMode = UserModeControl>
        IController* SetUp(cT min, cT max, cT val, cT mov)
        {
            return SetUp<uMode<cT>>(min, max, val, mov);
        }
#endif
        //set the controller's mode to one of the build-in modes
        ModeCodeVal Mode( ModeID modeID )
        {
            if ( modeID < USERMODE ) {
                controlmode = setModeSafeTypeMaxima( modeID );
                return ModeCodeVal( controlmode );
            } else {
                controlmode = setModeSafeTypeMaxima( modeID );
                return CustomControlMode->getModeCode();
            }
        }
        // get current active mode (when called without parameter)
        // or set one of the build-in modes (when parameter given)
        ModeCodeVal Mode( CtrlMode mode = None ) {
            if ( mode )
                controlmode = setModeSafeTypeMaxima( mode );
            return controlmode < USERMODE
                 ? ModeCodeVal( controlmode )
                 : CustomControlMode->getModeCode();
        }
        //set a UserControlMode-instance as controller-mode
        ModeCodeVal Mode( UserModeControl<cT>* mode )
        {
            if( CustomControlMode ) {
                if( CustomControlMode != mode ) {
                    delete CustomControlMode;
                    controlmode = None;
                } else
                    controlmode = USERMODE;
            } if ( mode ) {
                CustomControlMode = mode;
                CustomControlMode->InitFirst( (void*)this, &IController::checkValue );
                controlmode = USERMODE;
                return CustomControlMode->getModeCode();
            } else
                return ModeCodeVal( controlmode = None );
        }

        //set an already constructed UserControlMode instance as used controller mode
        template< template< typename UcT = cT > class Umode = UserModeControl>
        ModeCodeVal Mode(Umode<cT>* mode)
        {
            if ( CustomControlMode ) {
                if (CustomControlMode != mode) {
                    delete CustomControlMode;
                    controlmode = None;
                }
                else
                    controlmode = USERMODE;
            } if (mode) {
                CustomControlMode = mode;
                CustomControlMode->InitFirst((void*)this, &IController::checkValue);
                controlmode = USERMODE;
                return CustomControlMode->getModeCode();
            }
            else
                return ModeCodeVal( controlmode = None );
        }

        // gets the UserMode-Class instance if there's one attached...
        // ControledValueVariable.GetUserMode<Umode<cT>>() <- the type of
        // the controlled variable must be known for getting the mode instance
        template<typename uMode>
        uMode* GetUserMode(void) const {
            return (uMode*)CustomControlMode;
        }
#if ADVANCED_TEMPLATE_SUPPORT
        // more 'modern' (msvc19.0+/c++11+) version getting a UserMode template
        // with no need knowing the concrete controlled value's type at runtime
        // (e.g. 'ControledValueVariable.GetUserMode<Umode>()' here just 'mode'
        // type of the controller instance is known, but not the concrete 'type'
        // of the value that is controlled at least.
        template<template<typename uModCtTy = cT>class uMode = UserModeControl>
        uMode<cT>* GetUserMode(void) const {
            return GetUserMode<uMode<cT>>();
        }
#endif
        // Set control mode to some user defined mode-class.
        // it will instantiate and attach a new UserMode instance
        // to the controller. UserMode must be given type speciffic
        // by explicitely specifying the concrete type of the value
        // which is being controlled by the controller at runtime:
        // ControlledVariable.SetUserMode<SineController<float>>()
        template<typename uMode = UserModeControl<cT>>
        uMode* SetUserMode( cT val )
        {
            if( CustomControlMode ) {
                delete CustomControlMode;
                CustomControlMode = NULL;
            }
            CustomControlMode = new uMode();
            CustomControlMode->InitFirst(
                (void*)this, &IController::checkValue
            );
            *GetPointer() = val;
            controlmode = USERMODE;
            return (uMode*)CustomControlMode;
        }
#if ADVANCED_TEMPLATE_SUPPORT
        // Same, but can instanciate some distinct UserMode controller
        // without knowing concrete type of the value to be controlled
        // at runtime actually. (c++11+ or msvc19.0+ compilance needed)
        template<template<typename uModCrTy = cT>class uMode = UserModeControl>
        uMode<cT>* SetUserMode( cT val )
        {
            return SetUserMode<uMode<cT>>( val );
        }
#endif
        //template-function for setting the mode to an Usermode-class-type.
        //it will instantiate, initialize and attach the UserMode to the controller...
        template<class uMode = UserModeControl<cT>>
        uMode* SetUserMode( cT min, cT max, cT val, cT move )
        {
            if( CustomControlMode ) {
                delete CustomControlMode;
                CustomControlMode = NULL;
            }
            controlmode = None;
            VAR[Min] = min;
            VAR[Max] = max;
            VAR[Mov] = move;
            VAR[Val] = val;
            CustomControlMode = new uMode();
            CustomControlMode->InitFirst((void*)this, &IController::checkValue);
            controlmode = USERMODE;
            return (uMode*)CustomControlMode;
        }

#if ADVANCED_TEMPLATE_SUPPORT
        template<template<typename uModCrTy = cT>class uMode = UserModeControl>
        uMode<cT>* SetUserMode(cT min, cT max, cT val, cT move)
        {
            return SetUserMode<uMode<cT>>(min, max, val, move);
        }
#endif
        // Checks the Value (even if Controller is deactivated) and returns
        // the controllers actual active state.
        // ... use it to perform updating manually and not automatically
        // each time the value is "getted" when in a "MovingValue-Mode"...
        bool Check(void)
        {
            bool currentState = Active;
            AddFlags( DEACTIVATED );
            checkValue( controlmode );
            return currentState;
        }

        // performs a 'Step' to the value if actually in a MovingValue mode..
        // if not, it does almost same like Check() but without deactivating
        void Step(void) {
            checkValue( controlmode );
        }

        // retreive a reference from anywhere in the controller
        // 'index' here describes inheritance-depth when accessing
        // variables which are declared in Usermode class hirarchy
        template<typename P>
        P& GetPin(int pindex)
        {
            if (CustomControlMode != NULL) {
                return *(P*)CustomControlMode->Pin(nullptr, pindex);
            } else
                return *(P*)Pin(nullptr, pindex);
        }

        template<typename P>
        P& SetPin(P& pinVal, uint pindex = EMPTY)
        {
           // P* address;
            if (CustomControlMode != NULL) {
                return *(P*)CustomControlMode->Pin(
                    (void*)&pinVal,
                      (int)(pindex != EMPTY
                          ? pindex : 0)
                );
            } else return *(P*)Pin( (void*)&pinVal, pindex );
        }


        //read the value by implicit cast
        operator cT(void) {
            return GetValue();
        }
        //write the value
        cT operator =(cT set) {
            return SetValue(set);
        }
        // get pointer to the value by '&' reference-sign
        cT* operator &(void) const {
            return GetPointer();
        }
        // drop the controller and get the referenced variable dereferenced
        cT& operator *(void) {
            GetValue();
            return *GetPointer();
        }
        // short form of GetValue()
        cT operator()(void) {
            return GetValue();
        }


        bool operator ==( const IController& other ) const {
            return !this->isValid()
                 ? !other.isValid() : other.isValid()
                 ? *this->GetPointer() == *other.GetPointer()
                 : false;
        }
        bool operator !=( const IController& other ) const {
            return !(this->operator==(other));
        }
        bool operator ==( cT other ) const {
            return *GetPointer() == other;
        }
        bool operator !=( cT other ) const {
            return *GetPointer() != other;
        }
        bool operator >( cT other ) const {
            return *GetPointer() > other;
        }
        bool operator <( cT other ) const {
            return *GetPointer() < other;
        }
        bool operator >=( cT other ) const {
            return *GetPointer() >= other;
        }
        bool operator <=( cT other ) const {
            return *GetPointer() <= other;
        }

        // perform arithmetical assignment-operator: +=
        cT operator +=( cT other ) {
            return SetValue( GetValue() + other );
        }
        // perform arithmetical assignment-operator: -=
        cT operator -=( cT other ) {
            return SetValue( GetValue() - other );
        }
        // perform arithmetical assignment-operator: *=
        cT operator *=( cT other ) {
            return SetValue( GetValue() * other );
        }
        // perform arithmetical assignment-operator: /=
        cT operator /=( cT other ) {
            return SetValue( GetValue() / other );
        }

    protected:

        ModeCodeVal getTypeBias( void ) const {
            int t = (!std::numeric_limits<cT>().is_integer)
              ? 100 : std::numeric_limits<cT>().is_signed ? 10 : 1;
            return ((sizeof(cT) * t) << 4) | controlmode;
        }

        virtual cT checkValue( ControlMode mode ) {
            return checkValue( GetPointer(), getMINpt(), getMAXpt(), getMOVpt(), mode );
        }

        virtual cT checkValue(cT* val, cT* min, cT* max, cT* mov, ControlMode mod = EMPTY_(ControlMode))
        {
            ControlMode mode = mod != EMPTY_(ControlMode)
                             ? mod : this->controlmode;
            if (!mode)
                return *val;

            #define MIN mIN
            #define MAX mAX
            #define MOV mOV

            cT& VAL = *val;  cT& MOV = *mov;
            cT& MIN = *min;  cT& MAX = *max;

            switch (mode)
            {
            case Invert:   return INVERTMOD;
            case Clamp:    return CLAMPMODE;
            case Damp:     return DAMPFMODE;
            case Compress: return COMPRESSM;
            case Expand:   return EXPANDMOD;
            case Moderate: return VAL = (VAL > MIN)
                ? (cT)(((VAL - MIN) / MAX) * MOV + MIN)
                : (VAL < (cT)-MIN)
                ? (cT)(((VAL + MIN) / MAX) * MOV - MIN)
                : (VAL * MAX) / MOV;
            case Cycle:    return CYCLEMODE;
            case PingPong: PINGPONG( this );

                /* EXPERIMENTALs: */
            case ExperimentalA://  - if the value's delta-MOVment becomes greater than MIN,
                               // it will be damped/Amplified by factor MAX (MAX from 0 to 1 will effect reduction,
                               // MAX greater 1 effect's amplification)...
                return MOV = VAL = ((VAL - MOV) > MIN) || ((VAL - MOV) < -MIN)
                    ? MOV + (VAL - MOV)*MAX
                    : VAL;
            case ExperimentalB://  - if delta-MOVment will stay below MIN,
                               // it will be effected by factor MAX...
                return MOV = VAL = (((VAL - MOV) < MIN) || ((VAL - MOV) > -MIN))
                    ? MOV + ((VAL - MOV)*MAX)
                    : VAL;
            case ExperimentalC://- When delta is greater MIN or lower -MIN, it will be Compressed by 1/MAX.
                               // when delta is between MIN and -MIN, it will be Expanded by MAX...
                return MOV = VAL = ( ((VAL - MOV) > MIN) || ((VAL - MOV) < -MIN))
                    ? MOV + ((VAL - MOV)*(cT(1) / MAX))
                    : MOV + ((VAL - MOV)*MAX);
            default:
                return CustomControlMode
                    ? CustomControlMode->checkVALUE(val)
                    : VAL;
            }
            #undef MIN
            #undef MAX
            #undef MOV
        }

        virtual void* Pin( void* val, int index )
        {
            switch (index) {
            case 0: return GetPointer();
            case 1: return getMINpt();
            case 2: return getMAXpt();
            case 3: return getMOVpt();
            default: return nullptr;
            }
        }


        CtrlMode setModeSafeTypeMaxima( ModeID mode ) {
            switch (mode) {
            case None: break;
            case Damp: VAR[Min] = *GetPointer(); // should set the MIN value !
            case Invert:
            case Clamp:
            case Compress:
            case Expand:
            case Moderate:
            case ExperimentalA:
            case ExperimentalB:
            case ExperimentalC:
                RemFlags( CHECKED_GET );
                break;
            case PingPong:
                if (std::numeric_limits<cT>().is_integer) {
                    if (VAR[Min] == (cT)std::numeric_limits<cT>().min())
                        VAR[Min] = cT(1) + std::numeric_limits<cT>().min();
                    if (VAR[Max] == (cT)std::numeric_limits<cT>().max())
                        VAR[Max] = std::numeric_limits<cT>().max() - cT(1);
                }
            case Cycle:
            case USERMODE:
            default:
                AddFlags( CHECKED_GET );
                break;
            } return CtrlMode(mode);
        }

        virtual cT SetValue(cT) = 0;
        virtual cT GetValue(void) = 0;

        UserModeControl<cT>* CustomControlMode;
    };

template<typename T> using InterfaceControled = IController<T>;

#include "Macros/ModeExpress.h"
ENDOF_STEPFLOW_NAMESPACE
#endif
