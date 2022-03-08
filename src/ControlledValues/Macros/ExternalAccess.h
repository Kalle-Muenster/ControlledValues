/***************************************************************\
|  include this file just before AND just after declaring some  |
|  new Usermode Controller. when macro WRITEONLY_PINS or        |
|  READONLY_PINS may be defined previously, the PinFunc macro   |
|  will declare pins either write only or even read only.       |
|  otherwise it declares pins readable and writable.            |
\***************************************************************/
#if    (BEGINNING_OF_NAMESPACE) == (INC_STATE)
#undef  BEGINNING_OF_NAMESPACE
#define BEGINNING_OF_NAMESPACE (__COUNTER__)
#ifndef CONTROLLER_ACCESS_ENABLED
  #include "FlgHlpConfig.h"
  #include "PinFunConfig.h"
  #define MIN BASE::controller->MIN
  #define MAX BASE::controller->MAX
  #define MOV BASE::controller->MOV
  #define SPN SpinDirect(BASE::controller)
  #define INV IsInverted(BASE::controller)
  #define CHG ChangeSpin(BASE::controller)
#define CONTROLLER_ACCESS_ENABLED
#else
   #undef WRITEONLY_PINS
   #undef READONLY_PINS
   #include "FlgHlpConfig.h"
   #include "PinFunConfig.h"
   #undef INV
   #undef SPN
   #undef CHG
   #undef MIN
   #undef MAX
   #undef MOV
 #undef CONTROLLER_ACCESS_ENABLED
#endif
#endif