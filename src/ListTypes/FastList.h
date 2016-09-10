#ifndef _FASTLIST_H_
#define _FASTLIST_H_


#include "DynamicList.h"

namespace stepflow{
	template<typename eT, template<typename lT> lT>
	class TestList : public lT<eT>
	{

	};
}
#endif