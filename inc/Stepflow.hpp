#ifndef HAVE_CONTROLLED_VALUES
#define HAVE_CONTROLLED_VALUES (1)


// native stepflow sources are included within *namespace*:
#ifndef SET_STEPFLOW_NAMESPACE
#define SET_STEPFLOW_NAMESPACE stepflow
#define SET_STEPLIST_NAMESPACE lists
#define SET_CONTROLLERSPACE    EMPTY
#endif

#if 1
// if '1' use default configuration
#define DEFAULT_CONFIGURATION (1)
#ifdef  WITH_STEPLIST_TYPES
#undef  WITH_STEPLIST_TYPES
#define WITH_STEPLIST_TYPES (1)
#else
#define WITH_STEPLIST_TYPES (0)
#endif
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

// if it seem to be a dotnet framework managed (CLR) project
#ifdef _MANAGED
#define THREAD_WAITCYCLE_FUNC( ms ) \
System::Threading::Thread::CurrentThread->Sleep( ms )

#ifndef DEFAULT_DEBUG_LOGGER
#define DEFAULT_DEBUG_LOGGER Console::WriteLine
#endif

#if defined(_DEBUG) && defined(ENABLED_LOG_OUTPUT)
#define log_wait_state(cyc) DEFAULT_DEBUG_LOGGER("{0}(): not got lock! retry in {1} milliseconds!\n",__FUNCTION__,(cyc*THREAD_WAITCYCLE_TIME));
#define log_lock_state(msg) DEFAULT_DEBUG_LOGGER("{0}(): {1} lock state is {2}\n",__FUNCTION__,msg,locked);
#endif

// if it seem to be a Qt framework managed (CPP) project
#elif defined(QT_VERSION)
#include <QThread>
#define THREAD_WAITCYCLE_FUNC(ms) \
QThread::currentThread()->msleep( ms )
#else

// if no framework management can be detected (CPP):
#include <chrono>
#include <thread>
#define THREAD_WAITCYCLE_FUNC(ms) \
std::this_thread::sleep_for( std::chrono::milliseconds( ms ) )

#if defined(_DEBUG) && defined(ENABLED_LOG_OUTPUT)
#define log_wait_state(cyc) std::cout << __FUNCTION__ << "(): not got lock! retry in " << (cyc*THREAD_WAITCYCLE_TIME) << " milliseconds!\n";
#define log_lock_state(msg) std::cout << __FUNCTION__ << "(): " << msg << " lock state is " << locked << "\n";
#endif

#endif
#endif
#endif

#undef DEFAULT_CONFIGURATION
#if    WITH_STEPLIST_TYPES
#include "StepLists.h"
#undef WITH_STEPLIST_TYPES
#endif
#include "ControlledValues.h"
#endif
