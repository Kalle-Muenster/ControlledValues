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


// PinJack - implements the 'Pin' mechanism (the 'PinFieldController' also provides
// this for providing fix, indexable, enumerable accesspoints to variables/functions
// which are not nessessarly being either pointer or value... these always supporting
// both mechanisms same time. a 'Pin' in the first place is valuetype variable declared
// on a fix position, defined as a class member (so it's not a compositional thing. 
// it's compiletime hardcoded as class member variable - not removable ore addable to
// any kind of dynamically changable member lists.
// ...but it provides a mechanism which makes possible 'joining' a 'Pin' with another
// 'Pin' which can be declared on other object. joining means: as soon a 'Pin' gets
// assingned a pointer to it, which point to an address of another 'Pin' variable on
// another object, then the pin changes it's type (with that asignment), stops being
// a value type and turns to being a pointer type then - and will point that address of the other 
// distant 'Pin' instance then - but still supports assigning values to it further then, same as
// like it did when it was a value type before. the assigner/caller cannot determeine 
// if the Pin where it asigns value to actually maybe forwards asignments to some other Pin
// located on another controller on another object mayby. it's effect is almost same like 
// circumstances a uninon typed data structure also inflicts. (in a union, there may be 
// several filds declared and asignable.. but for real assigning to one of these fields -
// makes all the variables inside which share the same memory to holding the same value so.
// same situation comes up as soon two Pins had been joind. both will contain same value,
// stored at one same location in memory. so no calls or any kind processing is needed then
// for transporting values between two joined pins - they will have same value, ...always.
// like two atoms orbitally bound to each other . both having an owned electron each - but its one same 
// electron, within both of the atoms orbitals - or two electrons of two atoms but both at same time in one same place
template<typename pT>
struct PinJack {

    union Jack {
        ulong dat;
        pT*   ptr;
        pT    var;
        Jack() { dat = 0; }
    } pin;

    union Trick {
        ulong ptr;
        slong flg;
        Trick() { ptr = 0; }
    } ext;

    PinJack& operator =( pT* assignPointer ) {
        pin.ptr = assignPointer;
        external( assignPointer );
        return *this;
    }

    PinJack& operator =( pT assignValue ) {
        write( assignValue );
        return *this;
    }

    operator pT (void) {
        return read();
    }

    operator pT* (void) {
        return ext.flg < 0 ? pin.ptr : &pin.var;
    }

    template< typename U = stepflow::UserModeControl<pT> > U* umode(void) {
        return reinterpret_cast<U*>((void*)(ext.ptr & 0x7FFFFFFFFFFFFFFF));
    }

    bool external( void ) { return ext.flg < 0; }
    bool external( bool set ) {
        ext.ptr = set
     ? (ext.ptr | 0x8000000000000000)
     : (ext.ptr & 0x7FFFFFFFFFFFFFFF);
        return ext.flg < 0;
    }

    PinJack() {}
    PinJack( ulong umode ) {
        ext.ptr = umode;
    }
    PinJack( const PinJack& copy ) {
        pin = copy.pin;
        ext = copy.ext;
    }
    virtual void write( pT value ) {
        if (ext.flg < 0) *pin.ptr = value;
        else pin.var = value;
    }
    virtual pT read( void ) {
        return ext.flg < 0 ? *pin.ptr : pin.var;
    }

    __declspec(property(get = read, put = write)) pT value;
};

typedef unsigned char ControlMode;
typedef unsigned int  ModeCodeVal;

ENDOF_STEPFLOW_NAMESPACE
#endif