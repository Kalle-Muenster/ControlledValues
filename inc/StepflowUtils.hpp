#ifndef STEPFLOW_UTILITIES_INCLUDED
#define STEPFLOW_UTILITIES_INCLUDED

#ifdef COMPILE_SNAPSHOT_UTILS
#define SNAPSHOT_UTILS_API __declspec(dllexport)
#else
#define SNAPSHOT_UTILS_API __declspec(dllimport)
#endif

//#ifndef SET_STEPFLOW_NAMESPACE
#define SET_NAMESPACE stepflow
#define SET_STEPFLOW_NAMESPACE SET_NAMESPACE
//#define SET_CONTROLLERSPACE    values
//#endif


//#define STDLIB_LISTTYPES



#include <thread>
#define THREAD_WAITCYCLE_TIME 2
#define THREAD_WAITCYCLE_FUNC(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms)) 


#include "../src/ControlledValues/utils/ControllerDump.hpp"
#include "../src/ControlledValues/utils/SnapShooter.hpp"


#endif
