
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


typedef unsigned char byte;
typedef unsigned int  uint;


BEGIN_STEPFLOW_NAMESPACE

    typedef unsigned char ModeID;
    enum CtrlMode : ModeID
    {
        None = 0,       // Indicates the controller not is being active - value will stay as is..
        Clamp = 1,      // CLAMP: - The value will be clamped to MIN/MAX when set.
        Invert = 2,     // INVERT: - The value will stay the same, but will be returned negated when getted.
        Damp = 3,       // DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
        Compress = 4,   // COMPRESS: - The value will be scaled by MOV. If it's over MIN (or lower -MIN), it will bee compressed by MAX... (use MAX 100 ,MOV 75 like values for integer types for rates like 1/0.75)
        Expand = 5,     // EXPAND: - at threshold MIN expand by MAX, scale by MOV...
        Moderate = 6,   // MODERATE: - below MIN expand by MAX, above MIN compress by MAX. then scale by MOV
        Cycle = 7,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
        PingPong = 8,   // PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.
        ExperimentalA,  // -If the value's delta-movement becomes over the threshold MIN or lower it's inverse - MIN, it will be amplified by factor MAX(MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...
        ExperimentalB,  // -If delta - movement is between MIN and -MIN, it will be amplified by factor MAX(supposing MAX was given greater than 1).
        ExperimentalC,  // -When delta movement is between MIN and -MIN, it will be Expanded by factor MAX.If it becomes greater then MIN or lower then - MIN, it will be Compressed by factor 1 / MAX.
        USERMODE        // Indicates that the value is being controlled by some other controller-implemented object.
    };

ENDOF_STEPFLOW_NAMESPACE


#include "Macros/TypesDefiner.h"
#include "ValueControllerUserModes.hpp"
#include "PinJackStruct.hpp"
         #include "AdditionalTypes.h"
#include "InterfaceBase.hpp"
#include "FixFieldController.hpp"
#include "VariableController.hpp"
#include "PinFieldController.hpp"
         #include "AdditionalTypes.h"

#endif
