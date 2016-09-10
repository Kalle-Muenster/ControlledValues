
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
|  Controlled<Value>:                    |
|  a Variable value which is controlled  |
|  by a simple controlling-mechanism     |
|  it provides:                          |
|                                        |
|        -Invert                         |
|        -Clamp                          |
|        -Compress                       |
|        -Expand                         |
|        -Moderate                       |
|        -Cycle                          |
|        -PingPong                       |
|                                        |
|  it also support attaching UserDefined |
|  Mode-Properties which can be derived  |
|  from the UserMode<ValueType> class... |
|                                        |
|  by: Kalle Muenster                    |
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _CONTROLLED_VALUES_
#define _CONTROLLED_VALUES_

#ifndef NULL
#define NULL (0)
#endif

#include <limits>

#include "ValueControllerUserModes.h"
#include "ValueController.h"
#include "ControlledVector3.h"


namespace stepflow{

	template<typename cT>
	class AntiProportional : public InvertController < cT >
	{
	protected:
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
	class TypeFloater : public AntiProportional < float >
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
}
#endif