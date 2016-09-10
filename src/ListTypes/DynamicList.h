#ifndef _DYNAMIC_LIST_
#define _DYNAMIC_LIST_ 

#include "ListBase.h"

namespace stepflow{

template<typename elementType>                     
class DynamicListType
	: public ListBase<elementType>
{
protected:
	typedef unsigned int ElmID;
	ElmID				 MAXIMUM_SIZE;
	
public:
	typedef elementType ElementType;
	typedef void(*ElementAction)(elementType);
	typedef void(DynamicListType::*ListAction)(elementType);

	virtual unsigned getMaximumSize(void)
					 { return MAXIMUM_SIZE; };

	DynamicListType(ElmID capacity) 
	{
		MAXIMUM_SIZE = capacity;
		setDataArray(new elementType[MAXIMUM_SIZE]);
		
		numberOfMember = 0;
		highestSlotNumberInUse = 0;

		// check if ListMember are class-pointers or value types
		MembersArePointers = Utility::StringCompareRecursive(typeid(elementType).name(), "class ") >= 6;

		// if it's a pointer-list set each slot to NULL
		if(MembersArePointers)
		{
			Nulled = (elementType)NULL;
		}
		else try// instantiating a variable of the ListMembertype by using it's standard constructor, for using it as NULL-sign..
		{
			Nulled = elementType();
		}
		catch(...)// if doesn't work: Alert!
		{	
			std::cout << "ERROR: when using valuetypes as listmember, use the \"List<type,count>(type Nuller)\" - constructor !...";
			throw"StepList: error while initializing. - you should try to use another constructor...";
		}

		// Initialize the List's data array with the desired Nulled value
		// for signing it as unused empty...
		for(int i=0;i<MAXIMUM_SIZE;i++)
			list[i] = Nulled;	
	}
	
	DynamicListType(elementType nuller, unsigned capacity)
	{
		MAXIMUM_SIZE = capacity;
		setDataArray(new elementType[MAXIMUM_SIZE]);
		
		numberOfMember = 0;
		highestSlotNumberInUse = 0;
		MembersArePointers = Utility::StringCompareRecursive(typeid(elementType).name(), "class ") >= 6;

		for(int i=0;i<MAXIMUM_SIZE;i++)
			list[i] = nuller;	 
		Nulled = nuller;
	}
	
	DynamicListType(void) : DynamicListType::DynamicListType(4) {};

	int Enlarge(unsigned newSize)
	{
		if(newSize <= MAXIMUM_SIZE)
			return MAXIMUM_SIZE - newSize;
		elementType* neu = new elementType[newSize];
		for(unsigned id = 0; id < MAXIMUM_SIZE; id++)
			neu[id] = list[id];
		for(unsigned id = MAXIMUM_SIZE; id < newSize; id++)
			neu[id] = Nulled;
		int tempCount = this->Count();
		int tempLast = this->Last();
		Clear(false);
		highestSlotNumberInUse = tempLast;
		numberOfMember = tempCount;
		delete list;
		setDataArray(neu);
		return newSize - MAXIMUM_SIZE;
	}


	virtual ~DynamicListType(void)
	{
		if(MemberArePointer())
			Clear(true);
			
		delete list;
	}

	virtual ElmID Add(ElementType member)
	{
		if(numberOfMember + 1 >= getMaximumSize())
			MAXIMUM_SIZE += Enlarge(numberOfMember + (numberOfMember > 10 ? numberOfMember / 10 : 1));

		return ListBase<elementType>::Add(member);
	}
	
	// empties the list and destruct's all 
	// contained member if they where pointer.
	virtual void Clear()
	{
		numberOfMember = 0;
		if(highestSlotNumberInUse == EMPTY)
			return;

		if(MembersArePointers)
		{
			for(int i = 0; i <= highestSlotNumberInUse; i++)
				if(list[i] != Nulled)
				{
					size_t ptval = (size_t)list[i];
					delete (void*)ptval;
					list[i] = Nulled;
				}
		} else 
			for(int i = 0; i<= highestSlotNumberInUse; i++)
				list[i] = Nulled;

		highestSlotNumberInUse = EMPTY;
	}

	virtual void Clear(bool distroyAllElements)
	{
		numberOfMember = 0;
		if(highestSlotNumberInUse == EMPTY)
			return;

		if(MemberArePointer() && distroyAllElements)
		{
			for(int i = 0; i <= highestSlotNumberInUse; i++)
				if(list[i] != Nulled)
				{
					size_t ptval = (size_t)list[i];
					delete (void*)ptval;
					list[i] = Nulled;
				}
		}
		else
			for(int i = 0; i <= highestSlotNumberInUse; i++)
				list[i] = Nulled;  

		highestSlotNumberInUse = EMPTY;
	}

	virtual void foreach(ElementAction elementAction)
	{
		for(ElmID id = this->First(); id <= this->Last(); id = this->Next(id))
			elementAction(list[id]);
		return;
	}
};

}//end of stepflow
#endif
