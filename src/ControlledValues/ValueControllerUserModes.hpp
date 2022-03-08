
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
 |  Controlled<Value>::Usermodes:                          |
 |  attachable properties for the Controlled<Value> class. |
 |  by: K. Muenster                                        |
 \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _CONTROLLED_VALUE_USERMODES_
#define _CONTROLLED_VALUE_USERMODES_

// If GLM also is included, use it!
#ifdef GLM_PLATFORM
#define MathLib glm
#else //use stdlib math.h
#include <cmath>
#define MathLib std
#endif

#ifndef M_PI
#define M_PI (3.141592653589793116)
#endif

#define PI_double M_PI
#define PI_float (3.141592741f)
#define imKreis (6.283185307179586232)

#define MIN UserModeControl<cT>::controller->MIN
#define MAX UserModeControl<cT>::controller->MAX
#define MOV UserModeControl<cT>::controller->MOV
#define SPN SpinDirect(UserModeControl<cT>::controller)
#define INV IsInverted(UserModeControl<cT>::controller)
#define CHG ChangeSpin(UserModeControl<cT>::controller)

#ifdef READONLY_PINS
#undef READONLY_PINS
#endif
#ifdef WRITEONLY_PINS
#undef WRITEONLY_PINS
#endif

BEGIN_STEPFLOW_NAMESPACE
template < typename cT >
class   UserModeControl;
ENDOF_STEPFLOW_NAMESPACE

#include "Macros/PinFunConfig.h"
#include "Macros/PinFunDefine.h"
#include "Macros/TypesDefiner.h"
#include "Macros/FlgHlpConfig.h"

BEGIN_STEPFLOW_NAMESPACE

    template<typename cT> class IController;
    template<typename cT> class FixFieldController;
    template<typename cT> class VariableController;
    template<typename cT> class PinFieldController;

    enum LockedState : uint {
        INNER      = EMPTY-1,
        SET_UNLOCK = EMPTY,
        ISNOT      = 0,
        SET_LOCKED = 1,
        OUTER      = 1,
    };

    template<typename cT>
    class UserModeControl
    {// abstract base-class for all userdefined Valuecontroller implementations
		friend class IController<cT>;
        friend class FixFieldController<cT>;
        friend class VariableController<cT>;
        friend class PinFieldController<cT>;

    private:
        static ModeCodeVal   modeCodeBias(IController<cT>* instance) {
            return instance->CustomControlMode->modeCodeBase()
                 + instance->getTypeBias(); }
    protected:
        typedef IController<cT> BASE;
        typedef cT(BASE::*controllerCheck)(ControlMode);
        IController<cT>*    controller;
        controllerCheck     func;
        unsigned char       PIN_COUNT;
        virtual void        Init(void) { PIN_COUNT = 0; };
        cT                  checkMODE( ControlMode mode )
                            { return (controller->*func)(mode); }
 
        virtual ModeCodeVal modeCodeBase() const { return 0; };
        virtual void        onActivate(bool activ) {};
        virtual uint        usrModeSize() const { return sizeof(UserModeControl); };
        virtual ulong       usrModeData() const { return (ulong)(void*)this; }

    public:
                        UserModeControl(void){ PIN_COUNT = 0; }
        virtual        ~UserModeControl(void){ }
        int             pinCount(void) { return PIN_COUNT; }
        virtual void*   Pin(void* pin, int index = 0) {
                            return controller->Pin(pin,--index);
                        }
        virtual cT      checkVALUE( cT* pVALUE ) = 0;
        void            InitFirst( void *ValueController,
                                   controllerCheck checkerFunc ) {
                            attachToController(ValueController, checkerFunc);
                            Init();
                        }
        void            attachToController( void *ValueController,
                                            controllerCheck checkerFunc ) {
                            controller = (IController<cT>*) ValueController;
                            func = checkerFunc;
                        }
        ModeCodeVal     getTypeCode() const { return modeCodeBias( controller ); }
        ModeCodeVal     getModeCode() const { return modeCodeBase() + USERMODE; }
        uint            getModeSize() const { return usrModeSize(); }
        ulong           getModeData() const { return usrModeData(); }
        virtual LockedState lockState() const { return ISNOT; }
        virtual bool lock(bool setLock) { return (bool)lockState(); }
        virtual bool lockSupport() const { return (bool)ISNOT; }

    };


    template<typename cT>
    class ClambController
        : public UserModeControl<cT>
    { DeclarePin( byte, CLAMP )
    protected:
        typedef UserModeControl<cT> BASE;
        virtual void Init(void)
                { BASE::Init(); CLAMP |= 0x01; ++PIN_COUNT; }
        virtual ModeCodeVal modeCodeBase() const {
                    return *(ModeCodeVal*)"CLMP"; }
        virtual uint        usrModeSize() const {
                    return sizeof(ClambController); }
        virtual ulong       usrModeData() const {
                    return (ulong)(void*)this; }
    public:
        virtual cT checkVALUE( cT* pVALUE ) {
            if ( CLAMP & 0x01 )
              *pVALUE = checkMODE( Clamp );
        return *pVALUE; }
    };


    template<typename cT>
    class MovingValue
    // In- or (if INVERT) Decrement the Value by MOVE every time it's checked.
        : public ClambController<cT>
    { protected:
        typedef ClambController<cT> BASE;
        virtual void Init(void) {
                   BASE::Init(); }
        virtual ModeCodeVal modeCodeBase() const {
                    return *(ModeCodeVal*)"MOVR"; }
        virtual uint        usrModeSize() const {
                    return sizeof(MovingValue); }
        virtual ulong       usrModeData() const {
                    return (ulong)(void*)this; }
    public:
        virtual cT checkVALUE( cT* pVALUE )
        {
            *pVALUE = INV ? (*pVALUE - MOV)
                          : (*pVALUE + MOV);
            return BASE::checkVALUE( pVALUE );
        }
    };

    template<typename cT>
    class Follower
    // takes a pointer to another value...
    // when that changes, the Value "tries" to
    // follow it by In-or Decrementing by MOVE...
        : public ClambController<cT> // MovingValue<cT>
    { typedef ClambController<cT> BASE;
    protected:
        cT DIR;
        PinJack<cT> jackTRGpin;
        void* Pin( void* pin, int idx ) {
            if (idx > 1) return BASE::Pin(pin, idx - 2);
            if (idx) { if (pin) { DIR = *(cT*)pin; } return &DIR;
            } else { if (pin) { jackTRGpin = (cT*)pin; }
              return jackTRGpin.ext.flg < 0
                  ?  jackTRGpin.pin.ptr
                  : &jackTRGpin.pin.var; }            
        }
        virtual void Init(void) {
            BASE::Init();
            BASE::CLAMP = false;
            PIN_COUNT+=2;
            DIR = (MAX - MIN) / 2;
        }
        virtual ModeCodeVal modeCodeBase() const {
                    return *(ModeCodeVal*)"FLWR"; }
        virtual uint        usrModeSize() const {
                    return sizeof(Follower); }
        virtual ulong       usrModeData() const {
                    return (ulong)(void*)this; }
    public:
        Follower() : BASE() {};
        cT getTRGpin(void) {
            return jackTRGpin.ext.flg < 0
                ? *jackTRGpin.pin.ptr
                :  jackTRGpin.pin.var;
        }
        void setTRGpin(cT value) {
            jackTRGpin = value;
        }
        __declspec(property(get = getTRGpin, put = setTRGpin)) cT TRG;
        virtual cT checkVALUE( cT* pVALUE ) {
            cT val = *pVALUE;
            if (TRG > val) {
                val = (TRG - val) <= MOV ? TRG : (val + MOV * (DIR-MIN)/((MAX-MIN)/2));
            } else if (TRG < val) {
                val = (val - TRG) <= MOV ? TRG : (val - MOV * (MAX-DIR)/((MAX-MIN)/2));
            } *pVALUE = val;
            return BASE::checkVALUE( pVALUE );
        }
    };

ENDOF_STEPFLOW_NAMESPACE
#include "Macros/FlgHlpConfig.h"
#undef MIN
#undef MAX
#undef MOV
#endif
