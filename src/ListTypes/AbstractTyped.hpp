#ifndef _TYPED_LIST_INTERFACE_H_
#define _TYPED_LIST_INTERFACE_H_

#include "StaticList.hpp"
#include "DynamicList.hpp"

BEGIN_STEPFLOW_NAMESPACE

template<typename T>
class iContainer 
{ protected:
	T* element;
public:
	iContainer(T* containment) 
		: element(containment) {
	}
	virtual ~iContainer(void) {
		if (element)
			if(element != this)
				delete element;
	}
	T* getOut(void) {
		T* temp = element;
		element = NULL;
		return temp;
	}
	iContainer& operator =(T* wrap) {
		element = wrap;
		return *this;
	}
	T& operator *() {
		return *element;
	}
	T& operator ->() {
		return *element;
	}
	T* get(void) {
		return element;
	}
};

    template< typename containerType = size_t,
              typename pointerType = containerType*,
              typename baseType = ListBase< pointerType >
    >class AbstractTyped : public baseType
    {
    public:

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
            return (T*)&*(*(containerType*)(baseType::at(id)));
        }

        template<class T> T* as(ElmID id)
        {
            return (T*)&*(*(containerType*)(baseType::at(id)));
        }

    };

ENDOF_STEPFLOW_NAMESPACE
#endif
