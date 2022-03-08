/***************************************************************\
|  include this file just before AND just after declaring some  |
|  new Usermode Controller in that by SET_STEPFLOW_NAMESPACE    |
|  defined namespace. If macros WRITEONLY_PINS or READONLY_PINS |
|  may be defined previously, the PinFunc macro will declare    |
|  pins either 'write only' or even 'read only'. If not defined |
|  any pins will be declared readable and writable by default.  |
\***************************************************************/
#ifdef  STEPFLOW
//#include "../src/ControlledValues/Macros/StepflowEmpty.h"
//#define INC_STATE  ( MERGE(SPACE_)MERGE(__COUNTER__)_ID )
#define INC_STATE __COUNTER__
#ifndef BEGINNING_OF_NAMESPACE
#define BEGINNING_OF_NAMESPACE (INC_STATE)

BEGIN_STEPFLOW( VALUES )

#include "../src/ControlledValues/Macros/ExternalAccess.h"
#define SPACEINCOUNTER (BEGINNING_OF_NAMESPACE+1)
#elif SPACEINCOUNTER > INC_STATE
#include "../src/ControlledValues/Macros/ExternalAccess.h"

ENDOF_STEPFLOW( VALUES )

#undef BEGINNING_OF_NAMESPACE
#endif
#endif