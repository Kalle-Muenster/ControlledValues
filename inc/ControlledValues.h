// this includes ControlledValue types
#ifndef STEPFLOW_CONTROLLED_VALUES_INCLUDED
#define STEPFLOW_CONTROLLED_VALUES_INCLUDED

#include "../src/ControlledValues/Macros/StepflowEmpty.h"

#ifndef STEPFLOW_MINI_CONFIG
#define INCLUDE_ADDITIONAL_CLASSES
#else
#define INCLUDE_SELECTED_CLASSES
#endif

#ifdef   SET_STEPFLOW_NAMESPACE
 #ifndef STEPFLOW_LISTS_INCLUDED
  #if    SET_STEPFLOW_NAMESPACE != EMPTY && SET_STEPFLOW_NAMESPACE != -1
   namespace SET_STEPFLOW_NAMESPACE {}
   #ifndef SET_CONTROLLERSPACE
    #define SET_CONTROLLERSPACE EMPTY
   #elif SET_CONTROLLERSPACE==1
    #undef SET_CONTROLLERSPACE
   #endif
  #endif
 #endif
#endif


#define  INCLUDE_CONTROLLED_VALUES
#include "namespacer/namespacer.h"
#include "../src/ControlledValues/ControlledValues.hpp"
#include "namespacer/namespacer.h"

#endif
