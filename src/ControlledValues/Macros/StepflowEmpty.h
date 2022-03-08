#ifndef DONT_INCLUDE_DEFINES
#define DONT_INCLUDE_DEFINES

#ifndef EMPTY
#define EMPTY_(t) ((t)-1)
#define EMPTY 0xFFFFFFFF
#endif

#ifndef DECLPROP
#define DECLPROP(type,name)       __declspec(property(get=Get##name,put=Set##name)) type name
#define PROPLIST(type,name,size)  __declspec(property(get=get##name,put=set##name)) type name[size]
#endif

#define MERGE(name) name
#define ADD_SUFFIX(name,suffix) MERGE(name)suffix
#define ADD_PREFIX(name,prefix) MERGE(prefix)name

#ifndef minOf
#define minOf(a,b) (a < b ? a : b)
#endif
#ifndef maxOf
#define maxOf(a,b) (a > b ? a : b)
#endif

#endif
