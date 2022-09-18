#ifndef _DYNAMIC_LIST_
#define _DYNAMIC_LIST_

#include "ListBase.h"


BEGIN_STEPFLOW_NAMESPACE

template<typename elementType, typename baseType = ListBase<elementType>>
class DynamicListType
    : public baseType // ListBase<elementType>
{
protected:
    unsigned int  MAXIMUM_SIZE;

public:
    typedef elementType TYPE;
    typedef void(*ElementAction)(TYPE);
    typedef void(DynamicListType::*ListAction)(TYPE);
    typedef baseType BASE;//ListBase<baseType::TYPE> BASE;
    typedef unsigned ElmID;

    virtual unsigned getMaximumSize(void)
                     { return MAXIMUM_SIZE; };

    DynamicListType(ElmID capacity)
    {
        MAXIMUM_SIZE = capacity;
        setDataArray(new TYPE[MAXIMUM_SIZE]);
        
        BASE::numberOfMember = 0;
        BASE::highestSlotNumberInUse = 0;
        
        // check if ListMember are class-pointers or value types
#ifndef __STEPFLOW_TYPES__
            BASE::MembersArePointers = BASE::Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#else
            BASE::MembersArePointers = Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#endif

        // if it's a pointer-list set each slot to NULL
        if(BASE::MembersArePointers)
        {
            BASE::Nulled = TYPE(NULL);
        }
        else try// instantiating a variable of the ListMembertype by using it's standard constructor, for using it as NULL-sign..
        {
            if (typeid(TYPE).hash_code() == typeid(ElmID).hash_code()) {
                BASE::Nulled = TYPE((const char*)(size_t)EMPTY_(ElmID));
            }
            else {
                BASE::Nulled = TYPE();
            }
        }
        catch(...)// if doesn't work: Alert!
        {
            printf("ERROR: when using valuetypes as listmember, use the \"List<type,count>(type Nuller)\" - constructor !...");
            throw std::exception("StepList: error while initializing. - you should try to use another constructor...");
        }

        // Initialize the List's data array with the desired Nulled value
        // for signing it as unused empty...
        for(int i=0;i<MAXIMUM_SIZE;i++)
            BASE::list[i] = BASE::Nulled;
    }

    DynamicListType(TYPE nuller, unsigned capacity)
    {
        MAXIMUM_SIZE = capacity;
		byte* l = (byte*)(new TYPE[MAXIMUM_SIZE]);
        setDataArray((TYPE*)&l[0]);

        BASE::numberOfMember = 0;
        BASE::highestSlotNumberInUse = 0;
#ifndef __STEPFLOW_TYPES__
            BASE::MembersArePointers != BASE::Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#else
            BASE::MembersArePointers = Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#endif

        for(int i=0;i<MAXIMUM_SIZE;i++)
            BASE::list[i] = nuller;
        BASE::Nulled = nuller;
    }

    DynamicListType(void) 
		: DynamicListType::DynamicListType(4)
	{};

	virtual ~DynamicListType(void)
	{
		if (BASE::MemberArePointer())
			Clear(true);
		delete BASE::list;
	}



	// re-allocate by new size
    int Enlarge(unsigned newSize)
    {
        if(newSize <= MAXIMUM_SIZE)
            return MAXIMUM_SIZE - newSize;
        TYPE* neu = new TYPE[newSize];
        for(ElmID id = 0; id < MAXIMUM_SIZE; id++)
            neu[id] = BASE::list[id];
        for(ElmID id = MAXIMUM_SIZE; id < newSize; id++)
            neu[id] = BASE::Nulled;
        int tempCount = this->Count();
        int tempLast = this->Last();
        Clear(false);
        BASE::highestSlotNumberInUse = tempLast;
        BASE::numberOfMember = tempCount;
        delete BASE::list;
        setDataArray(neu);
        return newSize - MAXIMUM_SIZE;
    }

    // reorders the list so it won't contain any gaps nomore
    // and returns the list's new size of bytes in memory
    int Compact(void)
    {
        if(MAXIMUM_SIZE>BASE::numberOfMember)
        {
			const unsigned typeSize = sizeof(TYPE);
            byte* neu = (byte*)(new TYPE[BASE::numberOfMember]);
			byte* alt = (byte*)BASE::list;
			byte* nul = &(byte&)BASE::Nulled;

            ElmID cmpct=0;
            for(ElmID id=this->First();id<=this->Last();id=this->Next(id))
			{
                if(id>cmpct)
                {
					memcpy(&alt[cmpct*typeSize], &alt[id*typeSize], typeSize);
                 //   BASE::list[cmpct] = BASE::list[id];
					if (MemberArePointer())
						*(TYPE*)&alt[id*typeSize] = BASE::Nulled;
                 //   BASE::list[id] = BASE::Nulled;
                    id=cmpct;
                }
				memcpy(&neu[cmpct*typeSize], &alt[cmpct*typeSize], typeSize);
            //    neu[cmpct] = BASE::list[cmpct];
                cmpct++;
            }

			setDataArray((TYPE*)&neu[0]);            
            BASE::highestSlotNumberInUse=(BASE::numberOfMember-1);
            delete (TYPE*)&alt[0];
			return sizeof(TYPE)*(MAXIMUM_SIZE = BASE::numberOfMember);
        }
    }

    virtual ElmID Add(TYPE member)
    {
        if(BASE::numberOfMember >= getMaximumSize())
            MAXIMUM_SIZE += Enlarge( BASE::numberOfMember + ( BASE::numberOfMember > 10 
															? BASE::numberOfMember / 10 
								                            : 1 )
			                          );
        return ListBase<TYPE>::Add(member);
    }

    // empties the list and destruct's all
    // contained member if they where pointer.
    virtual void Clear()
    {
        BASE::numberOfMember = 0;
        if(BASE::highestSlotNumberInUse == EMPTY_(ElmID))
            return;

        if(BASE::MembersArePointers)
        {
            for(ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                if(BASE::list[i] != BASE::Nulled)
                {
                    size_t ptval = *(size_t*)&BASE::list[i];
                    delete (void*)ptval;
                    BASE::list[i] = BASE::Nulled;
                }
        } else
            for(ElmID i = 0; i<= BASE::highestSlotNumberInUse; i++)
                BASE::list[i] = BASE::Nulled;

        BASE::highestSlotNumberInUse = EMPTY_(ElmID);
    }

    virtual void Clear(bool distroyAllElements)
    {
        BASE::numberOfMember = 0;
        if(BASE::highestSlotNumberInUse == EMPTY_(ElmID))
            return;

        if(BASE::MemberArePointer() && distroyAllElements)
        {
            for(ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                if(BASE::list[i] != BASE::Nulled)
                {
					size_t ptval = *(size_t*)&BASE::list[i];
                    delete (void*)ptval;
                    BASE::list[i] = BASE::Nulled;
                }
        }
        else for(ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                BASE::list[i] = BASE::Nulled;

        BASE::highestSlotNumberInUse = EMPTY_(ElmID);
    }

    virtual void foreach(void(*call)(TYPE))
    {
        for (ElmID id = First(); id != Last(); id = Next(id))
            call(BASE::list[id]);
    }
};

ENDOF_STEPFLOW_NAMESPACE
#endif
