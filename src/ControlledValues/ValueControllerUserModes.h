
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
 |  Controlled<Value>::Usermodes:                          |
 |  attachable properties for the Controlled<Value> class. |
 |  by: K. Muenster                                        |
 \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _CONTROLLED_VALUE_USERMODES_
#define _CONTROLLED_VALUE_USERMODES_

#define Circle (6.2831853071795862)

namespace stepflow
{

	template<typename T>
	class Controlled;

	typedef unsigned char ControlMode;


	//enum Controlled<float>::ControllMode : ModeID
	//{
	//	NONE = 0,		// Indicates the controller not is being active - value will stay as is..
	//	Invert = 1,		// INVERT: - The value will stay the same, but will be returned negated when getted.
	//	Clamp = 2,		// CLAMP: - The value will be clamped to MIN/MAX when set. 
	//	Damp = 3,		// DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
	//	Compress = 4,	// COMPRESS: - If the value's delta-movement becomes over the threshold MIN or lower it's inverse -MIN, it will be amplified by factor MAX (MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...  
	//	Expand = 5,		// EXPAND: - If delta-movement is between MIN and -MIN, it will be amplified by factor MAX (supposing MAX was given greater than 1).
	//	Moderate = 6,	// MODERATE: - When delta movement is between MIN and -MIN, it will be Expanded by factor MAX. If it becomes greater then MIN or lower then -MIN, it will be Compressed by factor 1/MAX.
	//	Cycle = 7,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
	//	PingPong = 8,	// PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.
	//	USERMODE		// Indicates that the value is being controlled by some other controller-implemented object.
	//};

	template<typename cType>
	class UserModeControl //abstract base-class for all userdefined Valuecontrollers... 
	{
	protected:
		typedef cType(Controlled<cType>::*controllerCheck)(ControlMode);

		Controlled<cType>*	controller;	
		controllerCheck		func;
		virtual void		Init(void) = 0;
		cType				checkMODE(ControlMode mode)
							{ return (controller->*func)(mode); }
	public:
		unsigned char	ID;
						UserModeControl(void){ ID = NULL; }
		virtual		   ~UserModeControl(void){ }
		virtual cType	checkVALUE(cType* pVALUE) = 0;
		void			InitFirst(void *ValueController, cType(Controlled<cType>::*checkerFunc)(ControlMode) )
					    { controller = (Controlled<cType>*) ValueController;
						  func = checkerFunc;
						  Init(); }
	};


	template<typename cType>
	class InvertController //Inverts The Value.
		: public UserModeControl<cType>
	{
	protected:
		virtual void Init(void)
					 { INVERT = false; }
	public:
		bool INVERT;
		virtual cType checkVALUE(cType* pVALUE)
					  {	return INVERT ? -(*pVALUE) : *pVALUE; }
	};


	template<typename cType>
	class ClambController //Clamp the Value to MIN/MAX if CLAMP=true;
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
	class MovingValue //In- or (if INVERT) Decrement the Value by MOVE every time it's checked. 
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
	//Like a normal pingpong but following a sine-curve
	//...useful for LFO's in Audio or for Movement-Automation things elsewhere
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
		}

	public:
		cType PINGPONG_VALUE;
		virtual cType checkVALUE(cType* pVALUE)
		{		
			PINGPONG_VALUE += controller->MOVE;
			PINGPONG_VALUE = PINGPONG_VALUE>Circle ? PINGPONG_VALUE - Circle : PINGPONG_VALUE;
			HALBRANGE = (controller->MAX - controller->MIN) / 2;
			*pVALUE = (glm::sin(PINGPONG_VALUE)*HALBRANGE) + (-HALBRANGE - controller->MIN);
			return  -InvertController<cType>::checkVALUE(pVALUE);
		}
	};


	template<typename cType>
	/*takes a pointer to another value...
	when that changes, the Value "tries" to
	follow it by In-or Decrementing by MOVE...  */
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
	...maybe useful for something...	 */
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