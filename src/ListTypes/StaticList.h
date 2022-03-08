#ifndef _STATICLIST_H_
#define _STATICLIST_H_

#include "ListBase.h"
 

BEGIN_STEPFLOW_NAMESPACE

    template< const unsigned MAXIMUM_SIZE, typename elementType, class baseType=ListBase<elementType>>
    class StaticListType
        : public baseType 
    {
    protected:
        elementType listData[MAXIMUM_SIZE];

    public:
        typedef elementType TYPE;
        typedef baseType BASE;

        virtual unsigned getMaximumSize(void)
                         { return MAXIMUM_SIZE; };

        StaticListType(void)
        {
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
                BASE::Nulled = NULL;
            } else try// instantiating a variable of the ListMembertype by using it's standard constructor, for using it as NULL-sign..
            {
                BASE::Nulled = BASE::TYPE();
            } catch(...)// if doesn't work: Alert!
            {
                printf("ERROR: when using valuetypes as listmember, use the \"List<type,count>(type Nuller)\" - constructor !...");
                throw"LueckList: error while initializing. wrong constructor used...";
            }

            // Initialize the List's data array with the desired Nulled value
            // for signing it as unused empty...
            for(BASE::ElmID i = 0; i<MAXIMUM_SIZE; i++)
                listData[i] = BASE::Nulled;

            setDataArray(&listData[0]);
        }

        StaticListType(TYPE nuller)
        {
            BASE::numberOfMember = 0;
            BASE::highestSlotNumberInUse = 0;
#ifndef __STEPFLOW_TYPES__
            BASE::MembersArePointers = BASE::Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#else
            BASE::MembersArePointers = Utility::StringCompareRecursive(typeid(TYPE).name(), "class ") >= 6;
#endif
            for(ElmID i = 0; i<MAXIMUM_SIZE; i++)
                listData[i] = nuller;
            BASE::Nulled = nuller;
            setDataArray(&listData[0]);
        }

        //StaticListType(elementType elements...) 
        //    : StaticListType()
        //{
        //    list[...] = elements;
        //}

        virtual ~StaticListType(void)
        {
            if(BASE::MembersArePointers)
                Clear(true);
        }

        virtual void Clear(void)
        {
            BASE::numberOfMember = 0;
            if(BASE::highestSlotNumberInUse == EMPTY)
                return;

            if(BASE::MembersArePointers)
            {
                for(BASE::ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                    if(BASE::list[i] != BASE::Nulled)
                    {
                        size_t ptval = (size_t)listData[i];
                        delete (void*)ptval;
                        listData[i] = BASE::Nulled;
                    }
            } else
                for(BASE::ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                    listData[i] = BASE::Nulled;

            this->setDataArray(&listData[0]);
            BASE::highestSlotNumberInUse = EMPTY;
        }
        virtual void Clear(bool distructAll)
        {
            BASE::numberOfMember = 0;
            if(BASE::highestSlotNumberInUse == EMPTY)
                return;

            if(BASE::MemberArePointer() && distructAll)
            {
                for(BASE::ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                    if(BASE::list[i] != BASE::Nulled)
                    {
                        size_t ptval = (size_t)listData[i];
                        delete (void*)ptval;
                        listData[i] = BASE::Nulled;
                    }

            } else
                for(BASE::ElmID i = 0; i <= BASE::highestSlotNumberInUse; i++)
                    listData[i] = BASE::Nulled;

            this->setDataArray(&listData[0]);
            BASE::highestSlotNumberInUse = EMPTY;
        }
        virtual void foreach(void(*call)(TYPE))
        {
            for (BASE::ElmID id = First(); id != Last(); id = Next(id))
                call(BASE::list[id]);
        }

    };

ENDOF_STEPFLOW_NAMESPACE
#endif
