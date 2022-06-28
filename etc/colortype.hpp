// file: colortype.hpp
#include "colorformat.hpp"
#include "colormacros.hpp"

#ifndef USE_DEFAULT_SURFACE_FORMAT
#define USE_DEFAULT_SURFACE_FORMAT BGRA
#endif

#ifndef DATATYPE_FOR_COLOR_CHANNEL
#define DATATYPE_FOR_COLOR_CHANNEL byte
#endif

#ifndef STRUCTSIZE_FOR_TYPED_COLOR
#define STRUCTSIZE_FOR_TYPED_COLOR 32
#endif

#ifndef MAXIMUMSIZE_OF_CHANNELTYPE
#define MAXIMUMSIZE_OF_CHANNELTYPE 255
#endif

#ifndef CLASS_NAME_FOR_TYPED_COLOR
#define CLASS_NAME_FOR_TYPED_COLOR
#endif

#define MCS MAXIMUMSIZE_OF_CHANNELTYPE
#define CHANNEL_TYPE CreateChannelTemplate( CLASS_NAME_FOR_TYPED_COLOR )
#define COLOR_TYPE CreateColorTemplate( CLASS_NAME_FOR_TYPED_COLOR )
#define COLOR_USED_CHANNEL_DATA DATATYPE_FOR_COLOR_CHANNEL
#define COLOR_SIZE_TYPE DefineColorStructSize( STRUCTSIZE_FOR_TYPED_COLOR )

#ifdef  COLOR_USED_CHANNEL_DATA
 #ifdef CHAN
   #if    defined(CHAN) && defined(DATATYPE_FOR_COLOR_CHANNEL)
   #undef CHAN
   #endif
 #endif
#ifndef CHAN
typedef COLOR_USED_CHANNEL_DATA CHANNEL_TYPE;
#define CHAN CreateChannelType( DATATYPE_FOR_COLOR_CHANNEL )


#ifdef BEGIN_NAMESPACE
       BEGIN_NAMESPACE
#endif

typedef DATATYPE_FOR_COLOR_CHANNEL CHAN;
template<const unsigned FORMAT>
union COLOR_TYPE;
template<const unsigned FORMAT>
bool is_not(COLOR_TYPE<FORMAT>);

template<const unsigned FORMAT>
struct CHANNEL_TYPE {
    typedef DATATYPE_FOR_COLOR_CHANNEL TYPE;
    static const ColorFormat format;
};

template<const unsigned FORMAT>
const ColorFormat CHANNEL_TYPE<FORMAT>::format = ColorFormat{ ColorFormatCode(FORMAT) };

template<>
struct CHANNEL_TYPE<ARGB> {
    CHAN A; CHAN R;
    CHAN G; CHAN B;
};
template<>
struct CHANNEL_TYPE<BGRA> {
    CHAN B; CHAN G;
    CHAN R;  CHAN A;
};
template<>
struct CHANNEL_TYPE<ABGR> {
    CHAN A; CHAN B;
    CHAN G; CHAN R;
};
template<>
struct CHANNEL_TYPE<RGBA> {
    CHAN R; CHAN G;
    CHAN B; CHAN A;
};


template<const unsigned FORMAT=USE_DEFAULT_SURFACE_FORMAT>
union COLOR_TYPE {
    typedef CHAN CHAN_TYPE;

    const static ColorFormat   format;
    static unsigned s_transparenceKey;

    CHANNEL_TYPE<FORMAT>           Ch;
    CHAN                        ch[4];
    COLOR_SIZE_TYPE             value;

    COLOR_TYPE(void)
        : COLOR_TYPE(0)
    {}
    COLOR_TYPE( COLOR_SIZE_TYPE Value )
        : value(Value)
    {}
    COLOR_TYPE( CHAN r, CHAN g, CHAN b, CHAN a = EMPTY_(CHAN) ) {
        Ch.R = r;
        Ch.G = g;
        Ch.B = b;
        Ch.A = a;
    }
    COLOR_TYPE( const COLOR_TYPE& copy ) {
        value = copy.value;
    }

    int idxR(void) {
        return iR;
    }
    int idxG(void) {
        return iG;
    }
    int idxB(void) {
        return iB;
    }
    int idxA(void) {
        return iA;
    }


    // these should return always normal float values
    float getR(void) {
        return ((float)ch[iR])/MCS;
    }
    float getG(void) {
        return ((float)ch[iG])/MCS;
    }
    float getB(void) {
        return ((float)ch[iB])/MCS;
    }
    float getA(void) {
        return ((float)ch[iA])/MCS;
    }

    // these assignment functions asume values always normal
    // maybe implement either assertion that input IS normal
    // or even limitation TO normal if passed level too high
    // otherwise passing abnormal value causage is undefined
    COLOR_TYPE& setR(float val) {
        Ch.R = CHAN(val * MCS);
        return *this;
    }
    COLOR_TYPE& setG(float val) {
        Ch.G = CHAN(val * MCS);
        return *this;
    }
    COLOR_TYPE& setB(float val) {
        Ch.B = CHAN(val * MCS);
        return *this;
    }
    COLOR_TYPE& setA(float val) {
        Ch.A = CHAN(val * MCS);
        return *this;
    }

    // these needs testing..
    COLOR_TYPE operator +(COLOR_TYPE That) {
        COLOR_TYPE mix;
        mix.ch[0] = ((ch[0] + That.ch[0]) / 2);
        mix.ch[1] = ((ch[1] + That.ch[1]) / 2);
        mix.ch[2] = ((ch[2] + That.ch[2]) / 2);
        mix.ch[3] = ((ch[3] + That.ch[3]) / 2);
        return mix;
    }
    COLOR_TYPE operator -(COLOR_TYPE That) {
        COLOR_TYPE mix;
        mix.ch[iA] = ((this->ch[iA] + That.ch[iR] + That.ch[iG] + That.ch[iB]) / 4);
        mix.ch[iR] = ((this->ch[iR] + That.ch[iA] + That.ch[iG] + That.ch[iB]) / 4);
        mix.ch[iG] = ((this->ch[iG] + That.ch[iA] + That.ch[iR] + That.ch[iB]) / 4);
        mix.ch[iB] = ((this->ch[iB] + That.ch[iA] + That.ch[iR] + That.ch[iG]) / 4);
        return mix;
    }
    COLOR_TYPE operator *(COLOR_TYPE That) {
        COLOR_TYPE mix; int chan;
        chan = ((ch[0] + That.ch[0]));
        mix.ch[0] = chan > MCS ? MCS : chan;
        chan = ((ch[1] + That.ch[1]));
        mix.ch[1] = chan > MCS ? MCS : chan;
        chan = ((ch[2] + That.ch[2]));
        mix.ch[2] = chan > MCS ? MCS : chan;
        chan = ((ch[3] + That.ch[3]));
        mix.ch[3] = chan > MCS ? MCS : chan;
        return mix;
    }
    COLOR_TYPE operator /(COLOR_TYPE That) {
        COLOR_TYPE mix; int chan;
        chan = ch[iA] + ((That.ch[That.format.index.r] + That.ch[That.iG] + That.ch[That.iB]) / 3);
        mix.ch[iA] = chan > MCS ? MCS : chan;
        chan = ch[iR] + ((That.ch[That.iA] + That.ch[That.iG] + That.ch[That.iB]) / 3);
        mix.ch[iR] = chan > MCS ? MCS : chan;
        chan = ch[iG] + ((That.ch[That.iA] + That.ch[That.iR] + That.ch[That.iB]) / 3);
        mix.ch[iG] = chan > MCS ? MCS : chan;
        chan = ch[iB] + ((That.ch[That.iA] + That.ch[That.iR] + That.ch[That.iG]) / 3);
        mix.ch[iB] = chan > MCS ? MCS : chan;
        return mix;
    }
    COLOR_TYPE operator !(void)
    {
        Color inverse;
        //inverse.ch[iA] = MCS - ch[iA];
        inverse.ch[iR] = MCS - ch[iR];
        inverse.ch[iG] = MCS - ch[iG];
        inverse.ch[iB] = MCS - ch[iB];
        return inverse;
    }
    bool operator <(COLOR_TYPE other)
    {
        return (ch[0] + ch[1] + ch[2] + ch[3]) < (other.ch[0] + other.ch[1] + other.ch[2] + other.ch[3]);
    }
    bool operator >(COLOR_TYPE other)
    {
        return (ch[0] + ch[1] + ch[2] + ch[3]) > (other.ch[0] + other.ch[1] + other.ch[2] + other.ch[3]);
    }
    bool operator ==(COLOR_TYPE other)
    {
        return value == other.value;
    }
    bool operator !=(COLOR_TYPE other)
    {
        return !(operator==(other));
    }
    COLOR_TYPE& operator =(COLOR_TYPE& other) {
        value = other.value;
        return *this;
    }

    operator bool() {
        return is_val(*this);
    }

    static void setTransparenceKey(COLOR_TYPE key) {
        s_transparenceKey = key.value;
    }
};

template<>
const ColorFormat COLOR_TYPE<RGBA>::format = ColorFormat{ RGBA };
template<>
const ColorFormat COLOR_TYPE<ARGB>::format = ColorFormat{ ARGB };
template<>
const ColorFormat COLOR_TYPE<BGRA>::format = ColorFormat{ BGRA };
template<>
const ColorFormat COLOR_TYPE<ABGR>::format = ColorFormat{ ABGR };

template<const unsigned FORMAT>
unsigned COLOR_TYPE<FORMAT>::s_transparenceKey = 0;

template<const unsigned FORMAT>
inline bool is_not(COLOR_TYPE<FORMAT> checkForVisibility) {
    bool invisible = ( !checkForVisibility.Channel.A)
                   || ( checkForVisibility.value == COLOR_TYPE<FORMAT>::s_transparenceKey );
    return invisible;
}

#ifdef DEFAULT_COLORCLASS_DEFINED
#ifdef CLASS_NAME_FOR_TYPED_COLOR
//DefineColorClassWithChannelType(COLOR_USED_TYPE,CLASS_NAME_FOR_TYPED_COLOR);
DefineColorWithSurfaceFormat(USE_DEFAULT_SURFACE_FORMAT,CLASS_NAME_FOR_TYPED_COLOR);
//typedef MergeName(CLASS_NAME_FOR_TYPED_COLOR)ColorClass<USE_DEFAULT_SURFACE_FORMAT> MergeName(CLASS_NAME_FOR_TYPED_COLOR)Color;
//typedef Color##CLASS_NAME_FOR_TYPED_COLOR<USE_DEFAULT_SURFACE_FORMAT> Color;
#undef CLASS_NAME_FOR_TYPED_COLOR
#endif
#else
DefineColorWithSurfaceFormat(USE_DEFAULT_SURFACE_FORMAT, CLASS_NAME_FOR_TYPED_COLOR);
DefineColorDataTypeFormat(USE_DEFAULT_SURFACE_FORMAT, CLASS_NAME_FOR_TYPED_COLOR);
typedef ColorClass<USE_DEFAULT_SURFACE_FORMAT> Color;
typedef ChannelType<USE_DEFAULT_SURFACE_FORMAT> Cannel;
#define DEFAULT_COLORCLASS_DEFINED
#undef  CLASS_NAME_FOR_TYPED_COLOR
#endif

#ifdef ENDOF_NAMESPACE
       ENDOF_NAMESPACE
#endif


#ifdef USE_SURFACEFORMAT
#undef USE_SURFACEFORMAT
#endif
#ifdef DATATYPE_FOR_COLOR_CHANNEL
#undef DATATYPE_FOR_COLOR_CHANNEL
#endif
#ifdef CHANNEL_TYPE
#undef CHANNEL_TYPE
#endif
#ifdef COLOR_TYPE
#undef COLOR_TYPE
#endif
#ifdef COLOR_SIZE_TYPE
#undef COLOR_SIZE_TYPE
#endif
#ifdef COLOR_USED_CHANNEL_DATA
#undef COLOR_USED_CHANNEL_DATA
#endif
#ifdef STRUCTSIZE_FOR_TYPED_COLOR
#undef STRUCTSIZE_FOR_TYPED_COLOR
#endif
#ifdef MAXIMUMSIZE_OF_CHANNELTYPE
#undef MAXIMUMTYPE_OF_CHANNELTYPE
#endif
#endif
#endif
//#ifdef CHAN
//#undef CHAN
//#endif

#define  ColorTypeCreationCleanup
#include "colormacros.hpp"

