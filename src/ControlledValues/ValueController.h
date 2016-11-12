#ifndef _VALUE_CONTROLLER_
#define _VALUE_CONTROLLER_

namespace stepflow
{
    typedef unsigned char ModeID;

//#ifndef __STEPFLOW_TYPES__
    enum CtrlMode : ModeID
    {
        None = 0,       // Indicates the controller not is being active - value will stay as is..
        Invert = 1,     // INVERT: - The value will stay the same, but will be returned negated when getted.
        Clamp = 2,      // CLAMP: - The value will be clamped to MIN/MAX when set.
        Damp = 3,       // DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
        Compress = 4,   // COMPRESS: - If the value's delta-movement becomes over the threshold MIN or lower it's inverse -MIN, it will be amplified by factor MAX (MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...
        Expand = 5,     // EXPAND: - If delta-movement is between MIN and -MIN, it will be amplified by factor MAX (supposing MAX was given greater than 1).
        Moderate = 6,   // MODERATE: - When delta movement is between MIN and -MIN, it will be Expanded by factor MAX. If it becomes greater then MIN or lower then -MIN, it will be Compressed by factor 1/MAX.
        Cycle = 7,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
        PingPong = 8,   // PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.
        ExperimentalA,
        ExperimentalB,
        ExperimentalC,
        USERMODE        // Indicates that the value is being controlled by some other controller-implemented object.
    };
//#endif

    template<typename ctrlType> class Controlled
    {
    protected:
        // pointer to the value under control;
        ctrlType* _Value;

        /* TODO: Implement this:
        enum ControllerFlags : unsigned char
        { MustDelete = 0x1, CheckAtGet = 0x2 };
        ControllerFlags flags;
        */ //instead of:
        bool MustDelete;
        bool CheckAtGet;

        ModeID _mode;
        char userModesCount;
        UserModeControl<ctrlType>* UserMode;

        //the controlling-function
        virtual ctrlType checkValue(ControlMode mode)
        {
            switch(mode)
            {
            case 0:// OFF - value will be let as it is..
                return *_Value;

            /* STILL VALUES */

            // INVERT:
            case CtrlMode::Invert://- the value will stay the same, but when
                // it's requested it will be returned negated when GET!.
                return mode == Invert ? -(*_Value) : *_Value;

            // CLAMP:
            case CtrlMode::Clamp:// - the value will be clamp to MIN/MAX when it's being SET!...
                *_Value = *_Value<MIN ? MIN : *_Value>MAX ? MAX : *_Value;
                break;

            /* DYNAMICS: */
            case CtrlMode::Damp:
                MAX = *_Value - MIN;

                                 MIN = *_Value = (MAX > 0)
                                                      ?
                  (MAX > MOVE ? MIN + MOVE : *_Value) : (MAX < 0)
                                                             ?
                      ((MAX < -MOVE) ? MIN - MOVE : *_Value) : *_Value;
                break;

            // COMPRESS:
            case CtrlMode::Compress://  - if the value's delta-movement becomes greater than MIN,
                // it will be damped/Amplified by factor MAX (MAX from 0 to 1 will effect reduction,
                // MAX greater 1 effect's amplification)...
                *_Value = (((*_Value>MIN) || (*_Value < -MIN)
                        ? (*_Value / MAX) * MOVE
                        :  *_Value)* MAX) / MOVE; break;

            // EXPAND:
            case CtrlMode::Expand://  - if delta-movement will stay below MIN,
                // it will be effected by factor MAX...
                *_Value = (((*_Value > MIN) || (*_Value < -MIN)
                        ? (*_Value * MAX) / MOVE
                        :  *_Value)/ MAX) * MOVE; break;

            // MODERATE:
            case CtrlMode::Moderate://- When delta is greater MIN or lower -MIN, it will be Compressed by 1/MAX.
                // when delta is between MIN and -MIN, it will be Expanded by MAX...

                *_Value = (*_Value > MIN) || (*_Value < -MIN)
                        ? (*_Value / MAX) * MOVE
                        : (*_Value * MAX) / MOVE; break;


            /* MOTIVATORS: */

            // CYCLE:
            case CtrlMode::Cycle:// - every time the value is checked, it will move by adding MOVE to it...
                // if it get's greater MAX, it will reset to MIN...
                            *_Value = ((*_Value < MIN)
                                                ?
                          MAX - (MIN - *_Value) : (*_Value >= MAX)
                                                           ?
                                     MIN + (*_Value - MAX) : *_Value) + MOVE; break;
            // PINGPONG:
            case CtrlMode::PingPong: //- every time it's checked, will move by MOVE.
                // when reaches MAX or MIN, MOVE changes it's sign. so the movement will change direction.
                                        *_Value = (*_Value <= MIN)
                                                           ?
                (MIN + (MIN - *_Value)) + (MOVE = (-MOVE)) : (*_Value >= MAX)
                                                                      ?
                           (MAX - (*_Value - MAX)) + (MOVE = (-MOVE)) : (*_Value + MOVE); break;

            /* EXPERIMENTALs: */
            case CtrlMode::ExperimentalA:
                MOVE = *_Value = ((*_Value - MOVE) > MIN) || ((*_Value - MOVE) < -MIN) ?
                                                           MOVE + (*_Value - MOVE)*MAX : *_Value; break;

            case CtrlMode::ExperimentalB:
                MOVE = *_Value = (((*_Value - MOVE) < MIN) || ((*_Value - MOVE) > -MIN)) ?
                                                           MOVE + ((*_Value - MOVE)*MAX) : *_Value; break;

            case CtrlMode::ExperimentalC:
                MOVE = *_Value = (((*_Value - MOVE) > MIN) || ((*_Value - MOVE) < -MIN)) ?
                                                  MOVE + ((*_Value - MOVE)*(1.0f / MAX)) : MOVE + ((*_Value - MOVE)*MAX); break;

            // USERMODE
            default://  - implement whatever you able thinking about
                // and attach it to the controller...
                if(UserMode != NULL)
                    return UserMode->checkVALUE(_Value);
            }
            return *_Value;
        }

        // Internal setter..
        virtual ctrlType SetValue(ctrlType setter)
        {
            *_Value = setter;
            return (ControllerActive && !CheckAtGet) ?
                                  checkValue(Mode()) : setter;
        }

        // Internal getter...
        virtual ctrlType GetValue(void)
        {
            return (ControllerActive && CheckAtGet) ?
                                 checkValue(Mode()) : *_Value;
        }

    public:
        //If set "true", the Value will be checked and updated
        //every time it's red or written. ...useful for Clamping or Inverting
        //If set to "false", it's Value won't be checked and won't be updated
        //unless you call "Check()" on it manually. ...usefull for MovingValues
        //like pingpong or cycle. it than can be updated once per frame in an
        //update-loop for example, to let its Value stay the same during the
        //whole frame-cycle.
        bool ControllerActive;
        ctrlType MIN, MAX, MOVE;

        typedef ctrlType TYPE;

        //The selectable modes...
        enum ControllMode : ModeID
        {
            NONE     = CtrlMode::None,       // Indicates the controller not is being active - value will stay as is..
            Invert   = CtrlMode::Invert,     // INVERT: - The value will stay the same, but will be returned negated when getted.
            Clamp    = CtrlMode::Clamp,      // CLAMP: - The value will be clamped to MIN/MAX when set.
            Damp     = CtrlMode::Damp,       // DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
            Compress = CtrlMode::Compress,   // COMPRESS: - If the value's delta-movement becomes over the threshold MIN or lower it's inverse -MIN, it will be amplified by factor MAX (MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...
            Expand   = CtrlMode::Expand,     // EXPAND: - If delta-movement is between MIN and -MIN, it will be amplified by factor MAX (supposing MAX was given greater than 1).
            Moderate = CtrlMode::Moderate,   // MODERATE: - When delta movement is between MIN and -MIN, it will be Expanded by factor MAX. If it becomes greater then MIN or lower then -MIN, it will be Compressed by factor 1/MAX.
            Cycle    = CtrlMode::Cycle,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
            PingPong = CtrlMode::PingPong,   // PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.
            ExperimentalA = CtrlMode::ExperimentalA,
            ExperimentalB = CtrlMode::ExperimentalB,
            ExperimentalC = CtrlMode::ExperimentalC,
            USERMODE = CtrlMode::USERMODE      // Indicates that the value is being controlled by some other controller-implemented object.
        };

        //constructor...
        Controlled(void)
        {
            _Value = new ctrlType();
            MustDelete = true;
            userModesCount = ControllMode::USERMODE-1;
            ControllerActive = false;
            CheckAtGet = true;
            UserMode = NULL;
        }

        //Destructor...
        virtual ~Controlled(void)
        {
            if(UserMode)
            {
                delete UserMode;
                UserMode = NULL;
            }
            if(_Value)
            {
                if(MustDelete)
                    delete _Value;
                _Value = NULL;
            }
        }

        //Set MIN and reset the value itself to the new given MIN.
        void SetMIN(ctrlType val)
        {
            *_Value = MIN = val;
        }
        //Set MAX and reset the value to MAX.
        void SetMAX(ctrlType val)
        {
            *_Value = MAX = val;
        }

        //Set MOVE and set the Controller Active !
        void SetMOVE(ctrlType val)
        {
            MOVE = val;
            ControllerActive = true;
            checkValue(_mode);
        }

        //get a raw pointer to the value;
        ctrlType* GetPointerToValue(void)
        {
            return _Value;
        }

        //if set <true> the value will be checked
        //when it is getted, and not when it's been set.
        void SetCheckAtGet(bool shouldCheckedAtGet)
        {
        //  flags = shouldCheckdAtGet ? flags | ControllerFlags::ChackAtGet : flags & ~ControllerFlags::ChackAtGet;
            this->CheckAtGet = shouldCheckedAtGet;
        }

        //returns the UserMode-Class instance if there's one attached...
        template<typename Umode> Umode* GetUserMode(void)
        {
            return (Umode*)UserMode;
        }

        //template-function for setting the mode to an Usermode-class-type.
        //it will instantiate and attach the UserMode to the controller...
        template<typename Umode> Umode& SetUserMode(ctrlType initial)
        {
            if(UserMode)
            {
                delete UserMode;
                UserMode = NULL;
                userModesCount--;
            }

            UserMode = new Umode();
            UserMode->InitFirst(this,&Controlled::checkValue);
            *_Value = initial;

            UserMode->ID = ++userModesCount;
            Mode(UserMode);
            return *(Umode*)UserMode;
        }

        //template-function for setting the mode to an Usermode-class-type.
        //it will instantiate, initialize and attach the UserMode to the controller...
        template<typename Umode> Umode& SetUserMode(ctrlType min, ctrlType max, ctrlType initial, ctrlType move)
        {
            MIN = min;
            MAX = max;
            MOVE = move;

            if(UserMode)
            {
                delete UserMode;
                UserMode = NULL;
                --userModesCount;
            }
            if(!UserMode)
            {
                userModesCount++;
                UserMode = new Umode();
                UserMode->InitFirst(this, &Controlled::checkValue);
                *_Value = initial;
                UserMode->ID = userModesCount;
                Mode(UserMode);
            }
            return *(Umode*)UserMode;
        }

        //Checks the Value and return it.  (even if Controller is deactivated)
        //... use it to perform updating manually and not to be called automatically,
        //every time the value is "getted" when in a "MovingValue-Mode"...
        ctrlType Check(void)
        {
            ControllerActive = false;
            return checkValue(_mode);
        }

        //???
        virtual ControllMode Mode(bool mode)
        {
            return (ControllMode)(_mode = mode ? 1 : 0);
        }

        //set the controller's mode to one of the build-in modes
        virtual ControllMode Mode(ModeID modeID)
        {
            if(modeID <= Controlled::USERMODE)
                _mode = modeID;
            else
            {
                if(&UserMode[modeID - Controlled::USERMODE])
                    _mode = modeID;
            }
            return (ControllMode)_mode;
        }

        // get current active mode (when called without parameter)
        // or set one of the build-in modes (when parameter given)
        virtual ControllMode Mode(ControllMode mode = ControllMode::NONE)
        {
            switch(mode)
            {
            case ControllMode::NONE:
                break;

            case ControllMode::Damp:
                MIN = *_Value;
            case ControllMode::Invert:
            case ControllMode::Clamp:
            case ControllMode::Compress:
            case ControllMode::Expand:
            case ControllMode::Moderate:
            case ControllMode::ExperimentalA:
            case ControllMode::ExperimentalB:
            case ControllMode::ExperimentalC:
                CheckAtGet = false;
                _mode = mode;
                break;

            case ControllMode::Cycle:
            case ControllMode::PingPong:
            case ControllMode::USERMODE:
            default:
                CheckAtGet = true;
                _mode = mode;
                break;
            }
            return (ControllMode)_mode;
        }

        //set a UserControlMode-instance as controller-mode
        virtual ControllMode Mode(UserModeControl<ctrlType>* mode)
        {
            if(_mode != mode->ID)
            {
                UserMode = mode;
                _mode = UserMode->ID;
            }
            return (ControllMode)_mode;
        }



        //Initializes a Controlled<ValueType> with MIN,MAX,Initial,MOVE and Mode...
        virtual Controlled* SetUp(ctrlType min, ctrlType max, ctrlType initial, ctrlType move, ControllMode mode)
        {
            return SetUp(min, max, initial, move, (unsigned char)mode);
        }
        //Initializes a Controlled<ValueType> with MIN,MAX,Initial,MOVE and Mode...
        virtual Controlled* SetUp(ctrlType min, ctrlType max, ctrlType initial, ctrlType move, unsigned char mode)
        {
            *_Value = initial;
            MAX = max;
            MIN = min;
            MOVE = mode==ControllMode::PingPong? (move*2):move;
            Mode(mode);
            return this;
        }

        //Initializes a Controlled<ValueType> with MIN,MAX,Initial,MOVE and set's as Mode a given UserMode-Instance
        virtual Controlled* SetUp(ctrlType min, ctrlType max, ctrlType initial, ctrlType move, UserModeControl<ctrlType> *mode)
        {
            SetUp(min, max, initial, move, ControllMode::Clamp);
            mode->InitFirst(this, &Controlled::checkValue);
            if(mode->ID < 0)
                mode->ID = ++userModesCount;
            Mode(mode);
            return this;
        }
        //Initializes a Controlled<ValueType> with MIN, MAX and MOVE, sets it to MODE, and uses VALUE as the variable to be controlled.
        virtual Controlled* SetUp(ctrlType min, ctrlType max, ctrlType move, ControllMode MODE, ctrlType& VALUE)
        {
            SetUp(min, max, VALUE, move, (ModeID)MODE);
            SetVariable(&VALUE);
            return this;
        }
        //resets the pointer to the value an deletes it. and takes the given pointer as new Controlling-Target
        virtual void SetVariable(ctrlType *var)
        {
            if(MustDelete)
                delete _Value;

            _Value = var;
            MustDelete = false;
        }

        //read the value
        virtual operator ctrlType(void)
        {
            return GetValue();
        }

        //write the value
        virtual ctrlType operator =(ctrlType set)
        {
            return SetValue(set);
        }

        // perform arithmetical assignment-operator: +=
        ctrlType operator +=(ctrlType other)
        {
            return SetValue(GetValue() + other);
        }// perform arithmetical assignment-operator: -=
        ctrlType operator -=(ctrlType other)
        {
            return SetValue(GetValue() - other);
        }// perform arithmetical assignment-operator: *=
        ctrlType operator *=(ctrlType other)
        {
            return SetValue(GetValue() * other);
        }// perform arithmetical assignment-operator: /=
        ctrlType operator /=(ctrlType other)
        {
            return SetValue(GetValue() / other);
        }
    };
} // end namspace stepflow
#endif
