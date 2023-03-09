
 ////////////////////////////////////////////
 // internal used macros for declaring     //
 // 'pins' either readonly or writeonly    //
 // don't include these directly...        //
 // use "ControllerAccessMacros.h" instead //
 ////////////////////////////////////////////

#ifdef READONLY_PINS
  #undef PIN_JACT
 #define PIN_JACT(t,v) return v.find();
  #undef PIN_ACT
 #define PIN_ACT(t,v) return &v;
#elif defined( WRITEONLY_PINS )
  #undef PIN_JACT
 #define PIN_JACT(t,v) if (pin) v.write( *(t*)pin ); return nullptr;
  #undef PIN_ACT
 #define PIN_ACT(t,v) v = pin ? *(t*)pin : v; return nullptr;
#else
  #undef PIN_JACT
 #define PIN_JACT(t,v) if (pin) v.point( (t*)pin ); return v.find();
  #undef PIN_ACT
 #define PIN_ACT(t,v) if (pin) v = *(t*)pin; return &v;
#endif
