#ifndef HAVE_CONTROLLED_VALUES
#define HAVE_CONTROLLED_VALUES (1)


// native stepflow sources are included within *namespace*:
#define SET_STEPFLOW_NAMESPACE stepflow
#define SET_STEPLIST_NAMESPACE lists
#define SET_CONTROLLERSPACE    EMPTY


#if 1
// if '1' use default configuration
#define DEFAULT_CONFIGURATION (1)
#define WITH_STEPLIST_TYPES (0)
#else
// if '0' just include *minimal* set of sourcecode:
#define STEPFLOW_MINI_CONFIG
// and from the below listed parts include these:
// (for using a subset, comment out unwanted parts)
#define WITH_PEAKFINDER_MODE
#define WITH_CAPACITOR_MODE
#define WITH_ELEMENT_MODE
#define WITH_JONGLEUR_MODE
#define WITH_PULS_FORM_MODE
#define WITH_SINE_FORM_MODE
#define WITH_RAMP_FORM_MODE
#define WITH_SAW_STACK_MODE
#define WITH_BANDFILTER_MODE
#define WITH_MULTISTOP_MODE
#define WITH_VECTOR3_CONTROLLER
#define WITH_POINTER_CONTROLLER
// for also including the lists part:
#define WITH_STEPLIST_TYPES (1)
#endif

// algorythim which the *Sinus* Control Mode should use
// set this to (1) to use glm lib's sinus implementation
// (need to have glm lib headers in the include path)
#define USE_GLM_SINUS (0)

// control modes which maybe internaly use additional
// floating point data type will be implemented using:
#if defined( __x86_64__) || defined(_WIN64)
#define CONTROLER_INTERNAL_PRECISION double
#elif defined(_M_IX86) || defined(__x86__)
#define CONTROLER_INTERNAL_PRECISION float
#endif

// enable threadsafe features for controllers which support it:
// (e.g. define it EMPTY to disable any threadsafe features )
#ifndef  THREAD_WAITCYCLE_TIME
#define  THREAD_WAITCYCLE_TIME 2
#endif
#if defined(THREAD_WAITCYCLE_TIME) 
   #if THREAD_WAITCYCLE_TIME == EMPTY

// if threadsafe features are disabled at all:
#undef THREAD_WAITCYCLE_TIME
 #else

// if it seem to be a managed (.Net) CLR project
#ifdef _MANAGED
#define THREAD_WAITCYCLE_FUNC(ms) \
System::Threading::Thread::CurrentThread->Sleep( ms )

// if it seem to be a Qt managed project
#elif defined(QT_VERSION)
#define THREAD_WAITCYCLE_FUNC(ms) \
QThread::currentThread()->msleep( ms )
#else

// if not in any (known) framework managed project:
#define THREAD_WAITCYCLE_FUNC(ms) \
std::this_thread::sleep_for(std::chrono::milliseconds( ms ))
#endif

#endif
#endif

#undef DEFAULT_CONFIGURATION
#if    WITH_STEPLIST_TYPES
#include "StepLists.h"
#endif
#undef WITH_STEPLIST_TYPES

#endif