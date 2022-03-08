
 ////////////////////////////////////////////
 // internal used macros for declaring     //
 // 'pins' either readonly or writeonly    //
 // don't include these directly...        //
 // use "ControllerAccessMacros.h" instead //
 ////////////////////////////////////////////

#ifdef READONLY_PINS
  #undef PIN_ACT
 #define PIN_ACT(t,v) return &v;
#elif defined( WRITEONLY_PINS )
  #undef PIN_ACT
 #define PIN_ACT(t,v) v = pin ? *(t*)pin : v; return nullptr;
#else
  #undef PIN_ACT
 #define PIN_ACT(t,v) v = pin ? *(t*)pin : v; return &v;
#endif
