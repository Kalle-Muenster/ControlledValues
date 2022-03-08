/***************************************************************\
|  include this file just before AND just after declaring some  |
|  new Usermode Controller. when macro WRITEONLY_PINS or        |
|  READONLY_PINS may be defined previously, the PinFunc macro   |
|  will declare pins either write only or even read only.       |
|  otherwise it declares pins readable and writable.            |
\***************************************************************/
#ifndef CONTROLLER_ACCESS_ENABLED
  #include "FlgHlpConfig.h"
  #include "PinFunConfig.h"
  #define MIN BASE::controller->MIN
  #define MAX BASE::controller->MAX
  #define MOV BASE::controller->MOV
  #define INV IsInverted(BASE::controller)
#define CONTROLLER_ACCESS_ENABLED
#else
   #undef WRITEONLY_PINS
   #undef READONLY_PINS
   #include "PinFunConfig.h"
   #include "FlgHlpConfig.h"
   #undef MIN
   #undef MAX
   #undef MOV
   #undef INV
 #undef CONTROLLER_ACCESS_ENABLED
#endif

