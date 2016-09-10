
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

#define MERGE(name) name
#define ADD_PREFIX(typename,prefix) MERGE(prefix)typename
#define ADD_SUFFIX(typename,suffix) MERGE(typename)suffix

namespace stepflow{

    template<typename elementType> class DynamicStrait
        : public AbstractStrait<DynamicListType<elementType>> {
    };

    template<typename elementType> class DynamicList
        : public AbstractCycle<DynamicListType<elementType>, elementType>
    {
    public:
        DynamicList(void) : AbstractCycle<DynamicListType<elementType>, elementType>() {}
        DynamicList(elementType nuller, unsigned capacity) : AbstractCycle<DynamicListType<elementType>, elementType>(nuller, capacity) {}
    };



    template<typename elementType, const unsigned MAXIMUM_SIZE> class Strait
        : public AbstractStrait<StaticListType<elementType, MAXIMUM_SIZE>> {};

    template<typename elementType, const unsigned MAXIMUM_SIZE> class List
        : public AbstractCycle<StaticListType<elementType, MAXIMUM_SIZE>, elementType> {
    public:
        List(void) : AbstractCycle<StaticListType<elementType, MAXIMUM_SIZE>, elementType>() {}
        List(elementType nuller) : AbstractCycle<StaticListType<elementType, MAXIMUM_SIZE>, elementType>(nuller) {}
    };

    template<typename containerType, const unsigned MAXIMUM_SIZE> class Typed
        : public AbstractTyped <StaticListType<containerType*, MAXIMUM_SIZE>,containerType> {

    };


}
#endif
