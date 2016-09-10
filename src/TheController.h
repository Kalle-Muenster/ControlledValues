#ifndef THE_CONTROLOR
#define THE_CONTROLOR

#include "ControlledValues\ControlledValues.h"
//#include <limits>


namespace stepflow {

	template<typename cT>
	class AntiProportional : public InvertController<cT>
	{
	protected:
	//	cT* MIN, *MAX, *MOVE;
		virtual void Init(void)
		{
			InvertController<cT>::Init();
			controller->MIN = 0;
			controller->MAX = 1;
		}
	public:
		virtual cT checkVALUE(cT *VALUE)
		{ 
			return *VALUE = (InvertController<cT>::checkVALUE(VALUE) + INVERT ? controller->MAX : 0);
		}

	};

	template<typename Type>
	class TypeFloater : public AntiProportional<float>
	{
	protected:
		long SCALE;
		virtual void Init(void)
		{
			AntiProportional<float>::Init();
			SCALE = std::numeric_limits<Type>().max();
		}

	public:
		virtual float checkVALUE(float *VALUE)
		{
			return (float)(SCALE * AntiProportional<float>::checkVALUE(VALUE));
		}

	};

	
	//template<typename TheType> 
	class TheMode : public UserModeControl<data32>
	{
	protected:
		Controlled<float> theRED, theGREEN, theBLUE, theALPHA;
		ModeID INVERT;
		virtual void Init(void)
		{
			UserModeControl<data32>::Init();
			theRED.SetUserMode<TypeFloater<byte>>(0, 1, 0.5, 0);
			theGREEN.SetUserMode<TypeFloater<byte>>(0, 1, 0.5, 0);
			theBLUE.SetUserMode<TypeFloater<byte>>(0, 1, 0.5, 0);
			theALPHA.SetUserMode<TypeFloater<byte>>(0, 1, 0.5, 0);
			theRED.SetVariable(&controller->MIN.f32);
			theGREEN.SetVariable(&controller->MAX.f32);
			theBLUE.SetVariable(&controller->MOVE.f32);
			SetAntiPro(false);
		}

	public:
		
		virtual data32 checkVALUE(data32* thePointer)
		{
			thePointer->byte[0] = theALPHA;

				//theGREEN.Mode(
				//		theBLUE.Mode(
				//	theRED.Mode(INVERT)
				//					)
				//				);

			thePointer->byte[1] = theRED;
			thePointer->byte[2] = theGREEN;
			thePointer->byte[3] = theBLUE;

			return *thePointer;
		};

		void SetAntiPro(bool invert)
		{
			INVERT = invert;
			theRED.GetUserMode<TypeFloater<byte>>()->INVERT = invert;
			theGREEN.GetUserMode<TypeFloater<byte>>()->INVERT = invert;
			theBLUE.GetUserMode<TypeFloater<byte>>()->INVERT = invert;
		}

		void SetTheAlpha(float* var)
		{
			theALPHA.SetVariable(var);
		}
	};
} 



#endif

