#ifndef STEPFLOW_MACROS_INCLUDED
#define STEPFLOW_MACROS_INCLUDED

#define MERGE(name) name
#define ADD_PREFIX(typename,prefix) MERGE(prefix)typename
#define ADD_SUFFIX(typename,suffix) MERGE(typename)suffix

#define EMPTYTYPE(t) ((t)-1)
#define EMPTY EMPTYTYPE(unsigned)

#endif