#ifndef _STATICLIST_H_
#define _STATICLIST_H_

#include "ListBase.h"


namespace stepflow{

    template<typename elementType, const unsigned MAXIMUM_SIZE>
    class StaticListType
        : public ListBase<elementType>
    {
    protected:
        elementType listData[MAXIMUM_SIZE];

    public:
        typedef unsigned int ElmID;

        virtual unsigned getMaximumSize(void)
                         { return MAXIMUM_SIZE; };

        StaticListType(void)
        {
            numberOfMember = 0;
            highestSlotNumberInUse = 0;

            // check if ListMember are class-pointers or value types
            MembersArePointers = Utility::StringCompareRecursive(typeid(elementType).name(), "class ") >= 6;

            // if it's a pointer-list set each slot to NULL
            if(MembersArePointers)
            {
                Nulled = NULL;
            } else try// instantiating a variable of the ListMembertype by using it's standard constructor, for using it as NULL-sign..
            {
                Nulled = elementType();
            } catch(...)// if doesn't work: Alert!
            {
                std::cout << "ERROR: when using valuetypes as listmember, use the \"List<type,count>(type Nuller)\" - constructor !...";
                throw"LueckList: error while initializing. wrong constructor used...";
            }

            // Initialize the List's data array with the desired Nulled value
            // for signing it as unused empty...
            for(ElmID i = 0; i<MAXIMUM_SIZE; i++)
                listData[i] = Nulled;

            setDataArray(&listData[0]);
        }

        StaticListType(elementType nuller)
        {
            numberOfMember = 0;
            highestSlotNumberInUse = 0;
            MembersArePointers = Utility::StringCompareRecursive(typeid(elementType).name(), "class ") >= 6;
            for(ElmID i = 0; i<MAXIMUM_SIZE; i++)
                listData[i] = nuller;
            Nulled = nuller;
            setDataArray(&listData[0]);
        }

        virtual ~StaticListType(void)
        {
            if(MembersArePointers)
                Clear(true);
        }

        virtual void Clear(void)
        {
            numberOfMember = 0;
            if(highestSlotNumberInUse == EMPTY)
                return;

            if(MembersArePointers)
            {
                for(ElmID i = 0; i <= highestSlotNumberInUse; i++)
                    if(list[i] != Nulled)
                    {
                        size_t ptval = (size_t)listData[i];
                        delete (void*)ptval;
                        listData[i] = Nulled;
                    }
            } else
                for(ElmID i = 0; i <= highestSlotNumberInUse; i++)
                    listData[i] = Nulled;

            this->setDataArray(&listData[0]);
            highestSlotNumberInUse = EMPTY;
        }
        virtual void Clear(bool distructAll)
        {
            numberOfMember = 0;
            if(highestSlotNumberInUse == EMPTY)
                return;

            if(MemberArePointer() && distructAll)
            {
                for(ElmID i = 0; i <= highestSlotNumberInUse; i++)
                    if(list[i] != Nulled)
                    {
                        size_t ptval = (size_t)listData[i];
                        delete (void*)ptval;
                        listData[i] = Nulled;
                    }

            } else
                for(ElmID i = 0; i <= highestSlotNumberInUse; i++)
                    listData[i] = Nulled;

            this->setDataArray(&listData[0]);
            highestSlotNumberInUse = EMPTY;
        }

        virtual void foreach(void(*elmentAction)(elementType))
        {
            for(ElmID id = this->First(); id <= this->Last(); id = this->Next(id))
                elmentAction(list[id]);
            return;
        }
    };

}
#endif