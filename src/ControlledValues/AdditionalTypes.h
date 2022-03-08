// Include all pre implemented controller and mode classes if
// 'INCLUDE_ADDITIONAL_CLASSES' macro is defined just before.
///////////////////////////////////////////////////////////////

#ifndef   ADDITIONAL_CLASSES_INCUDED
 #ifdef   INCLUDE_ADDITIONAL_MODES
  #define INCLUDE_ADDITIONAL_CONTROLLERS
  #undef  INCLUDE_ADDITIONAL_MODES
 #else
  #ifdef    INCLUDE_SELECTED_MODES
   #undef   INCLUDE_SELECTED_MODES
   #define  INCLUDE_SELECTED_CONTROLLERS
   #include "IncludeSelectedClasses.h"
   #undef   INCLUDE_SELECTED_CONTROLLERS
   #define  ADDITIONAL_CLASSES_INCUDED
  #endif
 #endif
 #ifdef   INCLUDE_ADDITIONAL_CLASSES
  #define INCLUDE_ADDITIONAL_MODES
 #else
  #ifdef    INCLUDE_SELECTED_CLASSES
   #define  INCLUDE_SELECTED_MODES
   #include "IncludeSelectedClasses.h"
   #undef   INCLUDE_SELECTED_CLASSES
  #endif
 #endif
#endif

#ifdef INCLUDE_ADDITIONAL_MODES


/////////////////////////////////////////////////////////////
// Usermode classes:


  #include "AdditionalModeClasses/Element.hpp"
  #include "AdditionalModeClasses/PeakFinder.hpp"
  #include "AdditionalModeClasses/Capacitor.hpp"
  #include "AdditionalModeClasses/PulseFormer.hpp"
  #include "AdditionalModeClasses/SineFormer.hpp"
  #include "AdditionalModeClasses/Jongleur.hpp"
  #include "AdditionalModeClasses/BandFilter.hpp"
  #include "AdditionalModeClasses/MultiComp.hpp"
  #include "AdditionalModeClasses/RampFormer.hpp"
  #include "AdditionalModeClasses/SawStacker.hpp"

//........................................................../


#undef INCLUDE_ADDITIONAL_CLASSES
#endif
#ifdef INCLUDE_ADDITIONAL_CONTROLLERS


/////////////////////////////////////////////////////////////
// Controller classes:

  #include "AdditionalModeClasses/ControlledVector3.hpp"
  #include "AdditionalModeClasses/ControlledPointer.hpp"

//........................................................../


#undef  INCLUDE_ADDITIONAL_CONTROLLERS
#define ADDITIONAL_CLASSES_INCUDED
#endif
//#endif
//#endif
