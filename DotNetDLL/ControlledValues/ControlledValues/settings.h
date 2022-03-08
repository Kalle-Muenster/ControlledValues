
//#define unsafe (1)

#ifdef SET_STEPFLOW_NAMESPACE
#undef SET_STEPFLOW_NAMESPACE
#endif
#ifdef SET_STEPLIST_NAMESPACE
#undef SET_STEPLIST_NAMESPACE
#endif
#ifdef SET_CONTROLLERSPACE
#undef SET_CONTROLLERSPACE
#endif

// native stepflow sources include as namespace 'stepflow':
#define SET_NAMESPACE stepflow
#define SET_STEPFLOW_NAMESPACE SET_NAMESPACE
#define SET_STEPLIST_NAMESPACE EMPTY
#define SET_CONTROLLERSPACE    EMPTY

// should logoutput be enabled? 
// set to 1: always enable
// set to 2: always disable
// out-comment or 0: debug decides
#define ENABLED_LOG_OUTPUT (2)

// set to (1) for making native stepflow 'Sinus' source
// using the 'glm' math library sinus instead of std c:
#define USE_GLM_SINUS (0)

// control modes which use internaly an additional,
// different data type will be implemented to choose 
// precission best matching actual target platforms
// machine types word size 
#define CONTROLER_INTERNAL_PRECISION WORD_PRCISION

// any controllers with threading support:
// will wait for THREAD_WAITCYCLE_TIME ms when doing a waitcycle:
#define THREAD_WAITCYCLE_TIME 2
// will call function THREAD_WAITCYCLE_FUNC(ms) for doing a WaitCycle
#define THREAD_WAITCYCLE_FUNC(ms) System::Threading::Thread::CurrentThread->Sleep(ms)


#ifndef ENABLED_LOG_OUTPUT
#define ENABLED_LOG_OUTPUT  (0)
#endif

#if      ENABLED_LOG_OUTPUT == 0
#undef   ENABLED_LOG_OUTPUT
#ifdef   _DEBUG
#define  ENABLED_LOG_OUTPUT (1)
#else
#define  ENABLED_LOG_OUTPUT (2)
#endif
#endif

#if      ENABLED_LOG_OUTPUT < 2
#define  LOG_MESSAGE_WRITER(message,argument) Stepflow::MessageLogger::logMessage(message,argument)
#define  LOG_MESSAGE_STREAM(output) Stepflow::MessageLogger::Stream(output)
#else
#define  LOG_MESSAGE_WRITER(message,argument)
#define  LOG_MESSAGE_STREAM(output)
#endif