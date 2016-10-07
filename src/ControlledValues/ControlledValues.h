
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
    typedef byte ModeID;
    //enum ControlMode : ModeID
    //{
    //  NONE = 0,       // Indicates the controller not is being active - value will stay as is..
    //  Invert = 1,     // INVERT: - The value will stay the same, but will be returned negated when getted.
    //  Clamp = 2,      // CLAMP: - The value will be clamped to MIN/MAX when set.
    //  Damp = 3,       // DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
    //  Compress = 4,   // COMPRESS: - If the value's delta-movement becomes over the threshold MIN or lower it's inverse -MIN, it will be amplified by factor MAX (MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...
    //  Expand = 5,     // EXPAND: - If delta-movement is between MIN and -MIN, it will be amplified by factor MAX (supposing MAX was given greater than 1).
    //  Moderate = 6,   // MODERATE: - When delta movement is between MIN and -MIN, it will be Expanded by factor MAX. If it becomes greater then MIN or lower then -MIN, it will be Compressed by factor 1/MAX.
    //  Cycle = 7,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
    //  PingPong = 8,   // PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.
    //  USERMODE        // Indicates that the value is being controlled by some other controller-implemented object.
    //};

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