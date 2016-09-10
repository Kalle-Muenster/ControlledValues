
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

#include "AbstractCyclableList.h"

namespace stepflow{
	

template<typename eT, const unsigned MAXIMUM_SIZE>
class _declspec(dllexport) List : public AbstractCyclableList<eT> 
{//A List of variables of type <ListMember> , with a maximum amount of <MAXIMUM_SIZE>
private:
	ElementType           listData[MAXIMUM_SIZE];
	
public:
	// Constructs a new list-instance..
	List(void)
	{
		numberOfMember = 0;
		highestSlotNumberInUse = 0;

		// check if ListMember are class-pointers or value types
		MembersArePointers = Utility::StringCompareRecursive(typeid(ElementType).name(), "class ") >= 6;

		// if it's a pointer-list set each slot to NULL
		if(MembersArePointers)
			Nulled = NULL;
		else try// instantiating a variable of the ListMembertype by using it's standard constructor, for using it as NULL-sign..
		{
			Nulled = ElementType();
		}
		catch(...)// if doesn't work: Alert!
		{	
			std::cout<<"ERROR: when using valuetypes as listmember, use the \"List<type,count>(type Nuller)\" - constructor !...";
			throw"LueckList: error while initializing. wrong constructor used...";
		}

		// Initialize the List's data array with the desired Nulled value
		// for signing it as unused empty...
		for(ElmID i=0;i<MAXIMUM_SIZE;i++)
			listData[i] = Nulled;

		list = &listData[0];
	}

	//Construct a new List instance and 
	//initialize it's data-array to the given value...
	List(ElementType Nuller)
	{
		numberOfMember = 0;
		highestSlotNumberInUse = 0;
		MembersArePointers = Utility::StringCompareRecursive(typeid(ElementType).name(), "class ") >= 6;
		for(ElmID i = 0; i<MAXIMUM_SIZE; i++)
			listData[i] = Nuller;	 
		Nulled = Nuller;
		list = &listData[0];
	}

	// destructor
	virtual ~List(void)
	{
		if(MembersArePointers)
			Clear();
	}

	// empties the list and destruct's all 
	// contained member if they where pointer.
	virtual void Clear(void)
	{
		if(MembersArePointers)
		    {for(ElmID i = 0; i<highestSlotNumberInUse; i++)
				if (list[i] != Nulled)
					{ delete (ElementType*)&list[i]; list[i] = Nulled; }
		}
		else
			for(ElmID i = 0; i<highestSlotNumberInUse; i++)
				list[i] = Nulled;   
	}

	virtual void foreach(void(*elmentAction)(ElementType))
	{
		for(ElmID id = this->First(); id <= this->Last(); id = this->Next(id))
			elmentAction(list[id]);
		return;
	}
};


}//end stepflow
#endif
