
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
| file: list.h                         |
| class: List                          |
| description:                         |
|                                      |
|       a tiny list-type, very fast    |
|       at Iterating , but slower at   |
|       adding or searching elements.  |
|       by name. it provides foreach-  |
|       like iteration-style but with  |
|       advantage of ability to remove |
|       elements from the list while   |
|       cycling through it.            |
|                                      |
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _LUECKLIST_H_
#define _LUECKLIST_H_

#include "AbstractStrait.h"
#include "AbstractCycle.h"
#include "AbstractTyped.h"
#include "AbstractActivative.hpp"


BEGIN_STEPFLOW_NAMESPACE


    template<typename elmTyp> 
    class DynamicStrait 
        : public DynamicListType<elmTyp,AbstractStrait<elmTyp>>
    {};

    template<typename elmTyp> 
	class DynamicList 
        : public DynamicListType<elmTyp,AbstractCycle<elmTyp>> {
    public:
        DynamicList(void) 
			: DynamicListType<elmTyp,AbstractCycle<elmTyp>>()
		{}
        DynamicList(elmTyp nuller, unsigned capacity=4) 
            : DynamicListType<elmTyp,AbstractCycle<elmTyp>>(nuller, capacity)
		{}
    };

    template<typename containerType> 
	class DynamicTyped
        : public DynamicListType<containerType*,AbstractTyped<containerType>>
	{};



    template< const unsigned MAXIMUM_SIZE, typename elmTyp> 
	class Strait
        : public  StaticListType< MAXIMUM_SIZE ,elmTyp, AbstractStrait<elmTyp>>
	{};

    template< const unsigned MAXIMUM_SIZE,typename elmTyp>
	class List
        : public StaticListType<MAXIMUM_SIZE,elmTyp, AbstractCycle<elmTyp>> {
    public:
        List(void) 
			: StaticListType<MAXIMUM_SIZE,elmTyp,AbstractCycle<elmTyp>>()
		{}
        List(elmTyp nuller) 
			: StaticListType<MAXIMUM_SIZE,elmTyp,AbstractCycle<elmTyp>>(nuller)
		{}
    };

    template< const unsigned MAXIMUM_SIZE, typename containerType > 
	class Typed
        : public StaticListType< MAXIMUM_SIZE, AbstractTyped<containerType*,containerType>> 
    {};



#if _MSC_VER>=1900
	template< typename ET, template<typename et = ET>typename LT = ListBase> 
#else
	template< typename ET, typename LT = ListBase<ET>>
#endif
	class Vote 
		: public DynamicListType<ET,AbstractActivative<AbstractCycle<ET>>>
	{};


ENDOF_STEPFLOW_NAMESPACE
#endif
