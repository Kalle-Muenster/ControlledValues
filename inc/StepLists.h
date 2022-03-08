#ifndef STEPFLOW_LISTS_INCLUDED
#define STEPFLOW_LISTS_INCLUDED

#include "../src/ControlledValues/Macros/StepflowEmpty.h"

#ifdef   SET_STEPFLOW_NAMESPACE
 #ifndef STEPFLOW_CONTROLLED_VALUES_INCLUDED
  #if    SET_STEPFLOW_NAMESPACE != EMPTY && SET_STEPFLOW_NAMESPACE != -1
   namespace SET_STEPFLOW_NAMESPACE {}
  #endif
 #endif
 #ifndef   SET_LISTS_NAMESPACE
  #define  SET_LISTS_NAMESPACE -1
 #elif     SET_LISTS_NAMESPACE==1
  #undef   SET_LISTS_NAMESPACE
 #endif
#endif

#define INCLUDE_STEP_LISTS
#include "namespacer/namespacer.h"
#include "../src/ListTypes/Lists.hpp"
#include "namespacer/namespacer.h"

#endif
