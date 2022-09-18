#ifndef STEPFLOW_NO_TYPEDEFS
#define STEPFLOW_NO_TYPEDEFS

#include <WaveLib.inl\int24bittypes.hpp>

#ifndef COMMANDLINER_ESCENTIALS_DEFINED
typedef unsigned long long int  ulong;
typedef signed   long long int  slong;
typedef unsigned int            uint;
typedef unsigned char           byte;
typedef unsigned short          word;
#define COMMANDLINER_ESCENTIALS_DEFINED 1
#endif

#define VERSION_NUMBER_PART(prt) ((prt & 0xf0)>>4)+0x30, ((prt & 0x0f)<<4)+0x30
#define CONTROLLED_VALUES_VERSIONNUMBER_BUILDER(rel,maj,min,bld) (0x##rel<<24|0x##maj<<16|0x##min<<8|0x##bld)
#define CONTROLLED_VALUES_VERSIONSTRING_BUILDER(rel,maj,min,bld) {\
	    VERSION_NUMBER_PART(0x##rel),'.',\
		VERSION_NUMBER_PART(0x##maj),'.',\
		VERSION_NUMBER_PART(0x##min),'.',\
		VERSION_NUMBER_PART(0x##bld),'\0'\
}

#define CONTROLLED_VALUES_VERSION_NUMBER CONTROLLED_VALUES_VERSIONNUMBER_BUILDER(00,00,00,02)
#define CONTROLLED_VALUES_VERSION_STRING CONTROLLED_VALUES_VERSIONSTRING_BUILDER(00,00,00,02)

BEGIN_STEPFLOW_NAMESPACE

inline unsigned GetVersionNumber(void) {
    const static unsigned versio = CONTROLLED_VALUES_VERSION_NUMBER;
    return versio;
}
inline const char* GetVersionString(void) {
	const static char versio[12] = CONTROLLED_VALUES_VERSION_STRING;
	return &versio[0];
}

enum TypeCode : uint {
    Int8    = 5,
    UInt8   = 6,
    Int16   = 7,
    UInt16  = 8,
    Int32   = 9,
    UInt32  = 10,
    UInt64  = 11,
    Int64   = 12,
    Float32 = 13,
    Float64 = 14,
    UInt24  = 24,
    Int24   = uint(-24)
};


// these both are replacement dummies for types which not have implementation yet
typedef unsigned char  mini;
typedef unsigned short half;
typedef unsigned char ControlMode;
typedef unsigned int  ModeCodeVal;

ENDOF_STEPFLOW_NAMESPACE
#endif