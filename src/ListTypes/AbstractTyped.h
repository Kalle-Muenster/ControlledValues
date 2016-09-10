#ifndef _TYPED_LIST_INTERFACE_H_
#define _TYPED_LIST_INTERFACE_H_

#include "StaticList.h"
#include "DynamicList.h"

namespace stepflow{
	template<typename baseType, typename containerType>
	class AbstractTyped : public baseType
	{
	public:
		typedef unsigned int ElmID;

		AbstractTyped(void) : baseType()
		{}

		AbstractTyped(containerType* nuller, unsigned capacity)
		{}
		
		virtual unsigned Add(containerType element)
		{
			return baseType::Add(new containerType(element.emptyContainer()));
		}

		template<typename O> unsigned AddByType(containerType element)
		{
			return baseType::Add(new containerType(element.emptyContainer()));
		}


		template<class T> T* Get(ElmID id)
		{
			return (T*)&*(*(containerType*)(at(id)));
		}

		template<class T> T* as(ElmID id)
		{
			return (T*)&*(*(containerType*)(at(id))); 
		}

	};
}

#endif