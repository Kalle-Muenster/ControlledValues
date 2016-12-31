/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      Element.hpp                                ||
||     Author:    Kalle Muenster                             ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _Element_hpp_
#define _Element_hpp_

#include <ControlledValues.h>

namespace stepflow
{
	template<typename cT>
    class Element : public UserModeControl<cT>
    {
	public:
		cT PEAK;
	protected:
		byte EXTRAS;
		cT LAST;
		virtual void Init(void)
		{
			EXTRAS = EMPTYTYPE(byte);
			PEAK = 0;
		}
	public:
		virtual cT checkVALUE(cT* value)
		{
			bool Cycled;
			if (EXTRAS>'c' && *value < 0)
				*value = -*value;
			controller->MOVE = *value - LAST;
			if (Cycled = (EXTRAS == 'c' || EXTRAS == 'U'))
				controller->MOVE = controller->MOVE <= -((controller->MAX - controller->MIN) - 1)
							 ? controller->MOVE + controller->MAX 
				                 : controller->MOVE >= ((controller->MAX - controller->MIN) - 1)
							 ? controller->MOVE - controller->MAX : controller->MOVE;

			LAST = *value < controller->MIN ? Cycled ? controller->MAX - (controller->MIN - *value) : controller->MIN
				 : *value > controller->MAX ? Cycled ? controller->MIN + (*value - controller->MAX) : controller->MAX
				 : *value;

			PEAK = (controller->MOVE > 0) ? LAST : PEAK;
			return *value = LAST;
		}
		bool cycled(char shouldBeCycled = -23)
		{
			if (shouldBeCycled != -23)
				EXTRAS = EXTRAS > 'c' ? (shouldBeCycled ? 'U' : 'u') : (shouldBeCycled ? 'c' : '\0');
			return (EXTRAS == 'c' || EXTRAS == 'U');
		}
		bool uForced(char shouldBeUforced = -23)
		{
			if (shouldBeUforced != -23)
				EXTRAS = cycled() ? (shouldBeUforced ? 'U' : 'c') : (shouldBeUforced ? 'u' : '\0');
			return EXTRAS > 'c';
		}
    };

}//end of stepflow

#endif
