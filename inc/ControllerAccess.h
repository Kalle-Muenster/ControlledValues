/***************************************************************\
|  include this file just before AND just after declaring some  |
|  new Usermode Controller. when macro WRITEONLY_PINS or        |
|  READONLY_PINS may be defined previously, the PinFunc macro   |
|  will declare pins either write only or even read only.       |
|  otherwise it declares pins readable and writable.            |
\***************************************************************/
#include "../src/ControlledValues/Macros/InternalAccess.h"
