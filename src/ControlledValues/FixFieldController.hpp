/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ControlledValue.hpp                        ||
||     Author:    Kalle                                      ||
||     Generated: 18.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _FixFieldControler_hpp_
#define _FixFieldControler_hpp_

#if defined(__GNUC__)||defined(__MINGW32__)||defined(__MINGW64__)
#include "Macros/StepflowEmpty.h"
#include "ControlledValues.hpp"
#endif

BEGIN_STEPFLOW_NAMESPACE
#include "Macros/ModeExpress.h"

class IControllerBase;
template<typename cT> class IController;
template<typename cT> class FixFieldController
                   : public IController<cT>
{ friend class UserModeControl<cT>;
protected:

    static bool IsControllerValid( const FixFieldController* checkling ) {
        return ( checkling->controlmode >= USERMODE ? checkling->CustomControlMode != NULL : true );
    }

    //the controlling-function
    virtual cT checkValue(ControlMode mode)
    {
        switch (mode)
        {
        case Invert:   return INVERTMOD;
        case Clamp:    return CLAMPMODE;
        case Damp:     return DAMPFMODE;
        case Compress: return COMPRESSM;
        case Expand:   return EXPANDMOD;
        case Moderate: return VAL = (VAL > MIN)
             ? ((VAL - MIN) / MAX) * MOV + MIN
              : (VAL <-MIN)
             ? ((VAL + MIN) / MAX) * MOV - MIN
              : (VAL * MAX) / MOV;
        case Cycle:    return CYCLEMODE;
        case PingPong: PINGPONG(this);

            // EXPERIMENTALs: //
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
            return MOV = VAL = (((VAL - MOV) > MIN) || ((VAL - MOV) < -MIN))
                 ? MOV + ((VAL - MOV)*((cT)1 / MAX))
                 : MOV + ((VAL - MOV)*MAX);
        default:
            return CustomControlMode
                 ? CustomControlMode->checkVALUE(&VAL)
                 : VAL;
        }
    }

    // Internal setter..
    virtual cT SetValue(cT setter)
    {
        if (IsReadOnly(this)) return VAL;
        else VAL = setter;
        return ((this->Active && (!CheckAtGet(this))) ?
            checkValue(controlmode) : VAL);
    }

    // Internal getter...
    virtual cT GetValue(void)
    {
        return ((this->Active && CheckAtGet(this)) ?
            this->checkValue(this->controlmode) : VAL);
    }

    virtual void* Pin(void* value, int index)
    {
        if (value != nullptr) {
            cT* val = (cT*)value;
            switch (index) {
            case 0: VAL = *val; return &VAL;
            case 1: MIN = *val; return &MIN;
            case 2: MAX = *val; return &MAX;
            case 3: MOV = *val; return &MOV;
            default: throw new std::exception("pindex out of range", index);
            }
        }
        else {
            switch (index) {
            case 0: return &VAL;
            case 1: return &MIN;
            case 2: return &MAX;
            case 3: return &MOV;
            default: throw new std::exception("pindex out of range", index);
            }
        }
    }

    void* detachUserMode(void)
    {
        void* detached = this->CustomControlMode;
        this->CustomControlMode = NULL;
        this->controlmode = None;
        return detached;
    }

public:
    typedef cT TYPE;
    virtual uint controllerCode() const { return 0; }

    //If set "true", the Value will be checked and updated
    //every time it's red or written. ...useful for Clamping or Inverting
    //If set to "false", it's Value won't be checked and won't be updated
    //unless you call "Check()" on it manually. ...usefull for MovingValues
    //like pingpong or cycle. it than can be updated once per frame in an
    //update-loop for example, to let its Value stay the same during the
    //whole frame-cycle.
    cT VAL, MIN, MAX, MOV;

    //constructor...
    FixFieldController(void) : IController<cT>()
    {
        VAL = MIN = MAX = MOV = 0;
        this->AddFlags( MUST_DELETE );
    }

    //Destructor...
    virtual ~FixFieldController(void)
    {
        if (!isValid()) {
            return;
        }
    }

    virtual bool isValid(void) const
    {
        return IsControllerValid(this);
    }

    virtual cT* getMINpt(void) {
        return &MIN;
    }
    virtual cT* getMAXpt(void) {
        return &MAX;
    }
    virtual cT* getMOVpt(void) {
        return &MOV;
    }

    //Set MIN and also reset the value itself to that given MIN.
    virtual void SetMIN(cT val)
    {
        MIN = val;
    }
    //Set new MAX and also set the value to this new MAX.
    virtual void SetMAX(cT val)
    {
        MAX = val;
    }

    //assign 'val' to MOV and set the controller active (moving) !
    virtual void SetMOV(cT val)
    {
        MOV = val;
    }

    virtual cT GetMIN(void)
    {
        return MIN;
    }
    //Set new MAX and also set the value to this new MAX.
    virtual cT GetMAX(void)
    {
        return MAX;
    }

    //assign 'val' to MOV and set the controller active (moving) !
    virtual cT GetMOV(void)
    {
        return MOV;
    }

    //get a raw pointer to the value;
    virtual cT* GetPointer(void)
    {
        return &VAL;
    }

    virtual void SetVariable(cT *var)
    {
        VAL = *var;
    }
    virtual cT& GetVariable(void) {
        return VAL;
    }

    //read the value by implicit cast
    operator cT(void)
    {
        return GetValue();
    }

    //write the value
    cT operator =(cT set)
    {
        return SetValue(set);
    }

    // get pointer to the value by '&' reference-sign
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
        return !this->isValid() ? !other.isValid()
             :  other.isValid() ? VAL == *other.GetPointer()
                                : false;
    }
    bool operator !=(const IController<cT>& other) const
    {
        return !(this->operator==(other));
    }

    bool operator ==(cT other) const
    {
        return VAL == other;
    }
    bool operator !=(cT other) const
    {
        return VAL != other;
    }
    bool operator >(cT other) const
    {
        return VAL > other;
    }
    bool operator <(cT other) const
    {
        return VAL < other;
    }
    bool operator >=(cT other) const
    {
        return VAL >= other;
    }
    bool operator <=(cT other) const
    {
        return VAL <= other;
    }

    // perform arithmetical assignment-operator: +=
    cT operator +=(cT other)
    {
        return SetValue(GetValue() + other);
    }
    // perform arithmetical assignment-operator: -=
    cT operator -=(cT other)
    {
        return SetValue(GetValue() - other);
    }
    // perform arithmetical assignment-operator: *=
    cT operator *=(cT other)
    {
        return SetValue(GetValue() * other);
    }
    // perform arithmetical assignment-operator: /=
    cT operator /=(cT other)
    {
        return SetValue(GetValue() / other);
    }


#ifdef _TYPEINFO_
    const char* toString()
    {
        return this->controlmode < USERMODE
            ? typeid(this).name()
            : (const char*)typeid(this->CustomControlMode).name();
    }
#endif

};

#include "Macros/ModeExpress.h"
ENDOF_STEPFLOW_NAMESPACE
#endif
