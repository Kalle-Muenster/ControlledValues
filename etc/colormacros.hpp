#ifndef ColorTypeCreation
#define ColorTypeCreation

 // define macros used for defining Color classes of different SurfaceFormat and ChannelDataType
 // and macros which are used within member functions on these Color typees internally

 #define iR format.index.r
 #define iG format.index.g
 #define iB format.index.b
 #define iA format.index.a

#define MergeName(name,append) name ## append

#define CreateColorTemplate(ct) MergeName(ct,ColorClass)
#define CreateChannelTemplate(ct) MergeName(ct,ChannelType)
#define CreateColorType(colt) MergeName(colt,Color)
#define CreateChannelType(c) MergeName(c,ColorChannel)

#define DefineColorStructSize(sizeconst) unsigned MergeName(__int,sizeconst)

//#define DefineColorClassWithChannelType(chn,nam) template<const unsigned f=USE_DEFAULT_SURFACE_FORMAT> union MergeName(nam)MergeName(Color)Type : CreateColorByChanneltype(chn)<f>{ MergeName(nam)MergeName(Color)Type (chn r,chn g,chn b,chn a):CreateColorByChanneltype(chn)<f>(r,g,b,a){} }
#define DefineColorWithSurfaceFormat(fmtnam,clsnam) typedef CreateColorTemplate(clsnam)<fmtnam> CreateColorType(clsnam)
#define DefineColorDataTypeFormat(fmtnam,chantyp) typedef CreateChannelTemplate(chantyp)<fmtnam> CreateChannelType(chantyp)

#elif defined(RedefinitionCheck)
 #undef RedefinitionCheck
 #ifdef Redefinition
#undef  Redefinition
#ifdef  ChannelTypeWasDefined
 #if   ( ChannelTypeWasDefined != CreateChannelType(DATATYPE_FOR_COLOR_CHANNEL) )
  #undef  ChannelTypeWasDefined
 #else
  #define Redefinition 1
 #endif
 #else
  #define ChannelTypeWasDefined CreateChannelType(DATATYPE_FOR_COLOR_CHANNEL)
  #define Redefinition 0
 #endif
#else
 #define ChannelTypeWasDefined CreateChannelType(DATATYPE_FOR_COLOR_CHANNEL)
 #define Redefinition 0
#endif

#elif defined(ColorTypeCreationCleanup)
 // undefine any macros which later are not needed anymore for using defined classes
  #undef DefineColorStructSize
  #undef ColorTypeCreationCleanup
  #undef CreateColorByChanneltype
  #undef DefineColorClassName
  #undef CreateChannelType
  #undef ColorTypeCreation
  #undef MergeName
  #undef iR
  #undef iG
  #undef iB
  #undef iA
#endif
