#ifndef settings_h
#define settings_h
//#define unsafe (1)

// native stepflow sources include as namespace 'stepflow':
#define SET_NAMESPACE stepflow
#define SET_STEPFLOW_NAMESPACE SET_NAMESPACE
#define SET_STEPLIST_NAMESPACE EMPTY
#define SET_CONTROLLERSPACE    EMPTY
#define USE_NAMESPACER (1)

#ifdef  STEPFLOW_VERSION
#define SET_VERSION_STRING STEPFLOW_VERSION
#include <versionmacro.h>
#define STEPFLOW_VERSION_STRING VERSION_STRING
#define STEPFLOW_VERSION_NUMBER VERSION_NUMBER
#else
#error 'STEPFLOW_VERSION' macro must define 'major.minor.build.revision'
#endif

// compile log output for sections which use one of
// these log macros: LOG_MESSAGE_WRITER(msg,arg) or 
// LOG_MESAGE_STREAM(msg)->Out(more)->Out(more)...
// define (1) to enable logging
// define (2) to disable logging
// comment (//) to let DEBUG decide if there should be logging
#define ENABLED_LOG_OUTPUT (2)

// set to (1) for making native stepflow 'Sinus' source
// using the 'glm' math library sinus instead of std c:
#define USE_GLM_SINUS (0)

// control modes which use internaly an additional,
// different data type will be implemented using 'float'
#define CONTROLER_INTERNAL_PRECISION WORD_PRCISION

// any controllers with threading support:
// will wait THREAD_WAITCYCLE_TIME milliseconds for getting locked by distinct thread:
#define THREAD_WAITCYCLE_TIME 2
// will call function THREAD_WAITCYCLE_FUNC(ms) for doing such  timeout cycle
#define THREAD_WAITCYCLE_FUNC(ms) System::Threading::Thread::CurrentThread->Sleep(ms)


#ifndef ENABLED_LOG_OUTPUT
#ifdef  _DEBUG
#define ENABLED_LOG_OUTPUT (1)
#else
#define ENABLED_LOG_OUTPUT (0)
#endif
#endif

#if     ENABLED_LOG_OUTPUT == 1
#define LOG_MESSAGE_WRITER(message,argument) Stepflow::MessageLogger::logMessage( message, argument )
#define LOG_MESSAGE_STREAM(output) Stepflow::MessageLogger::Stream( output )
#include <MessageLogger.hpp>
#else 
#define LOG_MESSAGE_WRITER(message,argument)
#define LOG_MESSAGE_STREAM(output)
#endif

#endif
