#ifndef _STRAITLIST_
#define _STRAITLIST_

#include "StaticList.hpp"
#include "DynamicList.hpp"


BEGIN_STEPFLOW_NAMESPACE

template<typename eT = void*>
class BaseType;

#if _MSC_VER>=1900
template< typename elementType = void*,
          template< typename eT = elementType >typename baseType = ListBase
>class AbstractStrait 
	: public baseType<elementType>
#else
template< typename elementType = void*, typename baseType = ListBase<elementType>
>class AbstractStrait
	: public baseType
#endif

{};

ENDOF_STEPFLOW_NAMESPACE
#endif
