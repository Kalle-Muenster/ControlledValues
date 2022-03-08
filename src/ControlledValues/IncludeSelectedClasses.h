/* file: IncludeSelectedClasses.h ------------------*/
#ifdef INCLUDE_SELECTED_CLASSES
//////////////////////////////////////////////////////

#ifdef WITH_PEAKFINDER_MODE
#include "AdditionalModeClasses/PeakFinder.hpp"
#endif
#ifdef WITH_CAPACITOR_MODE
#include "AdditionalModeClasses/Capacitor.hpp"
#endif
#ifdef WITH_ELEMENT_MODE
#include "AdditionalModeClasses/Element.hpp"
#endif
#ifdef WITH_JONGLEUR_MODE
#include "AdditionalModeClasses/Jongleur.hpp"
#endif
#ifdef WITH_PULS_FORM_MODE
#include "AdditionalModeClasses/PulseFormer.hpp"
#endif
#ifdef WITH_SINE_FORM_MODE
#include "AdditionalModeClasses/SineFormer.hpp"
#endif
#ifdef WITH_RAMP_FORM_MODE
#include "AdditionalModeClasses/RampFormer.hpp"
#endif
#ifdef WITH_SAW_STACK_MODE
#include "AdditionalModeClasses/SawStacker.hpp"
#endif
#ifdef WITH_BANDFILTER_MODE
#include "AdditionalModeClasses/BandFilter.hpp"
#endif
#ifdef WITH_MULTISTOP_MODE
#include "AdditionalModeClasses/MultiComp.hpp"
#endif

#else ////////////////////////////////////////////////
#ifdef INCLUDE_SELECTED_CONTROLLERS
/*--------------------------------------------------*/

#ifdef WITH_VECTOR3_CONTROLLER
#include "AdditionalModeClasses/ControlledVector3.hpp"
#endif
#ifdef WITH_POINTER_CONTROLLER
#include "AdditionalModeClasses/ControlledPointer.hpp"
#endif

//////////////////////////////////////////////////////
#endif
#endif


