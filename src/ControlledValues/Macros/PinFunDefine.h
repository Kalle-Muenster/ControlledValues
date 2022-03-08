
///////////////////////////////////////////////
// internal used macros for declaring member //
// variables or methodes defined on Usermode //
// classes as being indexed 'pins' which are //
// accessible via  'GetPin(index)' function. //
//                                           //
//  ...don't include this directly - instead //
// use "ControllerAccessMacros.h" therefore  //
///////////////////////////////////////////////

#ifndef PIN_FUNCTIONS_DEFINED
#define PIN_FUNCTIONS_DEFINED

#define DeclarePin(pinType,pinName) protected: \
pinType pinName; public: \
virtual void* Pin(void*pin, int idx) { \
    if (idx) return BASE::Pin(pin,idx-1); \
    PIN_ACT(pinType,pinName) \
}

#define AddTwoPinAccessors(T1,V1,T2,V2) virtual \
void* Pin(void*pin,int idx) { \
    if(idx>1) return BASE::Pin(pin,idx-2); \
    else if (idx) { PIN_ACT(T2,V2) \
    } else { PIN_ACT(T1,V1) } \
}

#define DeclareTwoPins(pin1Typ,pin1Nam,pin2Typ,pin2Nam) \
protected: pin1Typ pin1Nam; pin2Typ pin2Nam; public: \
AddTwoPinAccessors(pin1Typ,pin1Nam,pin2Typ,pin2Nam);

#define ThereAreFourPins(pT1,pN1,pT2,pN2,pT3,pN3,pT4,pN4) \
protected: pT1 pN1; pT2 pN2; pT3 pN3; pT4 pN4; public: \
virtual void* Pin(void*pin,int idx) { \
    switch(idx) { \
        case 0: PIN_ACT(pT1,pN1) \
        case 1: PIN_ACT(pT2,pN2) \
        case 2: PIN_ACT(pT3,pN3) \
        case 3: PIN_ACT(pT4,pN4) \
        default: return BASE::Pin(pin,idx-4); \
    } \
}

#define AddPinArrayAccess(type,count,name) \
virtual void* Pin(void*pin,int idx) { \
    if(idx>=count) return BASE::Pin(pin,idx-count); \
    else { PIN_ACT( type, name[idx] ) } \
}

#define DeclarePinArray(pinType,pinCount,pinName) \
protected: pinType pinName[pinCount]; public: \
AddPinArrayAccess(pinType,pinCount,pinName)

#define DeclareJackPin( pinTyp, pinNam ) protected: \
 PinJack<pinTyp> MERGE(pinNam)JACK; public: \
 pinTyp ADD_PREFIX(pinNam,get)(void) { return ADD_SUFFIX(pinNam,JACK).ext ? *MERGE(pinNam)JACK.pin.ptr : MERGE(pinNam)JACK.pin.var;  } \
 void ADD_PREFIX(pinNam,set)(pinTyp v) { if ( ADD_SUFFIX(pinNam,JACK).ext ) *MERGE(pinNam)JACK.pin.ptr = v; \
                                         else ADD_SUFFIX(pinNam,JACK).pin.val = v; } \
__declspec(property(get = ADD_PREFIX(pinNam,get), put = ADD_PREFIX(pinNam,set) )) pinTyp pinNam; \
virtual void* Pin(void*pin, int idx) { \
    if(idx) return BASE::Pin(pin,idx-1); \
    if(pin) pinNam = (pinTyp*)pin; \
    return  pinNam; \
}

#define DeclareJackPinArray( pinTyp, pinNam, pinCnt ) protected: \
 PinJack<pinTyp> MERGE(pinNam)JACK[pinCnt]; public: \
 pinTyp ADD_PREFIX(pinNam,get)(int num) { return ADD_SUFFIX(pinNam,JACK[num]).ext ? *MERGE(pinNam)JACK[num].pin.ptr : MERGE(pinNam)JACK[num].pin.var;  } \
 void ADD_PREFIX(pinNam,set)(int num,pinTyp v) { if ( ADD_SUFFIX(pinNam,JACK[num]).ext ) *MERGE(pinNam)JACK[num].pin.ptr = v; \
                                                 else ADD_SUFFIX(pinNam,JACK[num]).pin.val = v; } \
__declspec(property(get = ADD_PREFIX(pinNam,get), put = ADD_PREFIX(pinNam,set) )) pinTyp pinNam[pinCnt]; \
virtual void* Pin(void*pin, int idx) { \
    if(idx>=pinCnt) return BASE::Pin(pin,idx-pinCnt); \
    if(pin) pinNam[idx] = (pinTyp*)pin; \
    return  pinNam[idx]; \
}

#endif