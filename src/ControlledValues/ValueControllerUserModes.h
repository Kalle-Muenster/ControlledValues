
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
 |  Controlled<Value>::Usermodes:                          |
 |  attachable properties for the Controlled<Value> class. |
 |  by: K. Muenster                                        |
 \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _CONTROLLED_VALUE_USERMODES_
#define _CONTROLLED_VALUE_USERMODES_

#define Circle (6.2831853071795862)

// If GLM also is included, use it!
#ifdef GLM_PLATFORM
#define MathLib glm
#else // use the standard math.h
#define MathLib std
#endif

namespace stepflow
{
    template<typename T>
    class Controlled;

    typedef unsigned char ControlMode;

    template<typename cType>
    // abstract base-class for all userdefined Valuecontrollers...
    class UserModeControl
    {
    protected:
        typedef cType(Controlled<cType>::*controllerCheck)(ControlMode);

        Controlled<cType>*  controller;
        controllerCheck     func;
        virtual void        Init(void) = 0;
        cType               checkMODE(ControlMode mode)
                            { return (controller->*func)(mode); }
    public:
        unsigned char   ID;
                        UserModeControl(void){ ID = NULL; }
        virtual        ~UserModeControl(void){ }
        virtual cType   checkVALUE(cType* pVALUE) = 0;
        void            InitFirst(void *ValueController, cType(Controlled<cType>::*checkerFunc)(ControlMode) )
                        { controller = (Controlled<cType>*) ValueController;
                          func = checkerFunc;
                          Init(); }
    };


    template<typename cType>
    // Inverts The Value.
    class InvertController
        : public UserModeControl<cType>
    {
    protected:
        virtual void Init(void)
                     { INVERT = false; }
    public:
        bool INVERT;
        virtual cType checkVALUE(cType* pVALUE)
                      { return INVERT ? -(*pVALUE) : *pVALUE; }
    };


    template<typename cType>
    // Clamp the Value to MIN/MAX if CLAMP=true
    class ClambController
        : public InvertController<cType>
    {
    protected:
        virtual void Init(void)
        {
            InvertController<cType>::Init();
            CLAMP = true;
        }

    public:
        bool CLAMP;
        virtual cType checkVALUE(cType* pVALUE)
        {
            *pVALUE = *pVALUE<controller->MIN ? controller->MIN : *pVALUE>controller->MAX ? controller->MAX : *pVALUE;
            return InvertController<cType>::checkVALUE(pVALUE);
        }
    };


    template<typename cType>
    // In- or (if INVERT) Decrement the Value by MOVE every time it's checked.
    class MovingValue
        : public ClambController<cType>
    {
    protected:
        virtual void Init(void)
        {
            ClambController<cType>::Init();
        }

    public:
        virtual cType checkVALUE(cType* pVALUE)
        {
            *pVALUE = INVERT ? (*pVALUE - controller->MOVE) : (*pVALUE + controller->MOVE);
            return CLAMP ? ClambController<cType>::checkVALUE(pVALUE) : *pVALUE;
        }
    };


    template<typename cType>
    // Like a normal pingpong but following a sine-curve
    // ...useful for LFO's in Audio or for Movement-Automation things elsewhere
    class SineControlled
        : public MovingValue<cType>
    {
    protected:
        cType HALBRANGE;
        virtual void Init(void)
        {
            InvertController<cType>::Init();
            PINGPONG_VALUE = 0;
            CLAMP = false;
            INVERT = true;
            controller->MOVE = (controller->MOVE / (controller->MAX - controller->MIN)) * Circle;
        }

    public:
        cType PINGPONG_VALUE;
        virtual cType checkVALUE(cType* pVALUE)
        {
            PINGPONG_VALUE += controller->MOVE;
            PINGPONG_VALUE = PINGPONG_VALUE>Circle ? PINGPONG_VALUE - Circle : PINGPONG_VALUE;
            HALBRANGE = (controller->MAX - controller->MIN) / 2;
            *pVALUE = (MathLib::sin(PINGPONG_VALUE)*HALBRANGE) + (-HALBRANGE - controller->MIN);
            return  -InvertController<cType>::checkVALUE(pVALUE);
        }
    };

    template<typename cType>
    // takes a pointer to another value...
    // when that changes, the Value "tries" to
    // follow it by In-or Decrementing by MOVE...
    class Follower
        : public MovingValue<cType>
    {
    protected:
        cType* pTARGET;
        virtual void Init(void)
        {
            InvertController<cType>::Init();
            CLAMP = false;
            INVERT = true;
        }

    public:
        virtual cType checkVALUE(cType* pVALUE)
        {
            cType dist = *pTARGET - *pVALUE;
            if(dist < controller->MOVE && dist > -controller->MOVE)
                return *pVALUE = *pTARGET;

            INVERT = *pTARGET < *pVALUE ? true : false;
            return MovingValue<cType>::checkVALUE(pVALUE);
        }
        virtual void SetTarget(cType* targetValue)
        {
            pTARGET = targetValue;
        }

    };


    template<typename cType>
    /*If it's Value is Set higher than MAX it will be MOVE
    if it's Set lower than MIN it will be -MOVE
    if it's set to something between MIN/MAX it will not change.
    ..useful for implementing functions using Threshold-Values... */
    class Trigger
        : public MovingValue<cType>
    {
    protected:
        cType* pTARGET;
        virtual void Init(void)
        {
            MovingValue<cType>::Init();
        }

    public:

        virtual cType checkVALUE(cType* pVALUE)
        {
            *pVALUE = *pTARGET >= controller->MAX ? controller->MOVE : *pTARGET <= controller->MIN ? -controller->MOVE : *pVALUE;
            return InvertController<cType>::checkVALUE(pVALUE);
        }
        void SetTarget(cType* targetValue)
        {
            pTARGET = targetValue;
        }
    };


    template<typename cType, unsigned char NUMBER_OF_PINS>
    /*It takes NUMBER_OF_PINS pointers to other values...
    and starts Moving the first one until it reaches the second's value.
    then it switches the pointers and continues,- now Moving the second one,
    until it reaches the third, then switches the pointer and goes on moving the third
    when reached the last PIN it switch's back to the first....
    ...maybe useful for something...     */
    class Jongleur
        : public Follower<cType>
    {
    protected:
        char currentPIN;
        virtual void Init(void)
        {
            Follower<cType>::Init();
            currentPIN = 0;
        }

    public:
        cType* pPIN[NUMBER_OF_PINS];

        virtual cType checkVALUE(cType* pVALUE)
        {
            int lastpin = currentPIN;
            pTARGET = pPIN[currentPIN];
            *pVALUE = *pVALUE < *pTARGET ? *pVALUE + controller->MOVE : *(pVALUE = pPIN[currentPIN++]) + controller->MOVE;
            if(currentPIN == NUMBER_OF_PINS)
                currentPIN = 0;
            if(lastpin != currentPIN)
                pPIN[lastpin] = pTARGET;
            *pVALUE = *pTARGET;
            return ClambController<cType>::checkVALUE(pVALUE);
        }

        virtual ~Jongleur(void)
        {
            delete[] pPIN;
        }

        virtual void SetTarget(cType* targetValues)
        {
            for(unsigned i = 0; i<NUMBER_OF_PINS; i++)
                pPIN[i] = &targetValues[i];
        }

        virtual void SetTarget(cType** targetValues)
        {
            for(unsigned i = 0; i<NUMBER_OF_PINS; i++)
                pPIN[i] = targetValues[i];
        }

        virtual void SetTarget(cType* targetValue, char pinNumber)
        {
            pPIN[pinNumber] = targetValue;
        }
    };


    template<typename cType>
    /*takes NUMBER_OF_PINS + one TARGET pointers.. calculates the pin's average value
    and scale's it by TARGET... */
    class Calculator
        : public Jongleur<cType, 3>
    {
    protected:
        virtual void Init(void)
        {
            Jongleur<cType, 3>::Init();
            CLAMP = INVERT = false;
        }
    public:
        virtual cType checkVALUE(cType* pVALUE)
        {
            *pVALUE = (((*pPIN[0] + *pPIN[1] + *pPIN[2]) / 3) * (*pTARGET));
            return ClambController<cType>::checkVALUE(pVALUE);
        }

        virtual void SetTarget(cType* targetPointer)
        {
            pTARGET = targetPointer;
        }
        virtual void SetPins(cType* pinPtrA, cType* pinPtrB, cType* pinPtrC)
        {
            pPIN[0] = pinPtrA;
            pPIN[1] = pinPtrB;
            pPIN[2] = pinPtrC;
        }
    };

}//end stepflow
#endif