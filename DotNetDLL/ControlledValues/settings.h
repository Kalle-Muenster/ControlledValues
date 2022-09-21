#ifndef settings_h
#define settings_h

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
#define USE_NAMESPACER (1)

#ifdef  STEPFLOW_VERSION
#define SET_VERSION_STRING STEPFLOW_VERSION
#include <versionmacro.h>
#define STEPFLOW_VERSION_STRING VERSION_STRING
#define STEPFLOW_VERSION_NUMBER VERSION_NUMBER
#else
#error 'STEPFLOW_VERSION' macro must define 'major.minor.build.revision'
#endif

// define function which controllers use for writing their log messages if debug output is enabled
#ifdef  CONSOLA_VERSION_NUMBER
#define DEFAULT_DEBUG_LOGGER Consola::StdStream::Out
#endif

// explicite include sellection:
#define STEPFLOW_MINI_CONFIG
#define WITH_ELEMENT_MODE
#define WITH_RAMP_FORM_MODE
#define WITH_PULS_FORM_MODE
#define WITH_SINE_FORM_MODE
#define WITH_BANDFILTER_MODE
#define WITH_PEAKFINDER_MODE
#define WITH_MULTISTOP_MODE
#define WITH_SAW_STACK_MODE
#define WITH_CAPACITOR_MODE

#endif
