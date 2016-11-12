#ifndef _STEPFLOW_LIST_BASE_
#define _STEPFLOW_LIST_BASE_

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
| file: AbstractListBase.h             |
| class: List                          |
| description:                         |
|     Abstract base-class for most     |
|     Stepflows list and collection    |
|     types.                           |
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <limits>

#ifndef EMPTY
#define EMPTY (unsigned)(-1) //(4294967295u)
#endif
#define LIST_ELEMENTS(type) typedef type ElementType; typedef void(*ElementAction)(ElementType); protected: ElementType *list; ElementType Nulled
#define BloedesDing delete temp

namespace stepflow{



    template<typename eT>
    class ListBase
    {
        typedef unsigned int    ElmID;

    public:

        LIST_ELEMENTS(eT);
        typedef void(ListBase<eT>::*ListAction)(ElementType);

    protected:
        int                     numberOfMember;
        bool                    MembersArePointers;
        ElmID                   highestSlotNumberInUse;
        void                    setDataArray(eT* pData)
                                { list = pData; }
        // following functions will only be declared if this file will
        // be included standalone. (If included as part of the complete
        // Stepflow framework, the versions in Utility.h will be used)
#ifndef __STEPFLOW_TYPES__
        class Utility {
        // recursive string comparison... returns -1 if strings are equal,
        // else returns the number of character at which they where different.
        public:
            static unsigned StringCompareRecursive(const char* A, const char* B)
                            { return cmprStrngsRec(A, B, 0); }
        private:
            static int      cmprStrngsRec(const char* A, const char* B, int position)
                            { return (A[position] == B[position]) ?
                                           (A[position] > 0) ?
                              cmprStrngsRec(A, B, ++position) : -1 : position; }
    };//end of Utility
#endif
    public:
        ElementType const &  getSlotNuller(void) const
                             { return Nulled; }
        bool                 MemberArePointer(void)
                             { return MembersArePointers; }
        virtual unsigned     getMaximumSize(void) = 0;

        // empties the list and destruct's all
        // contained member if they where pointer.
        virtual void Clear(bool=true) = 0;

        // Add a member to the list and return the memberID
        // (slot-number) at which the object has been added to...
        virtual ElmID Add(ElementType member)
        {
            ElmID FirstEmptySlotFound = EMPTY;
            ElmID counter = 0;
            ElmID index = 0;
            try
            {   //check's if the object has been added to the list already.
                while(counter < numberOfMember)
                {
                    // return if list already contain the object.
                    if(list[index] == member)
                        return index;
                    else // mark the first empty slot found.
                        if(FirstEmptySlotFound == EMPTY)
                        {
                            if(list[index] == Nulled)
                                FirstEmptySlotFound = index;
                            else
                                counter++;
                        }
                    index++;
                }
                // if a Free slot was found before, add the object there and return it's index.
                if(FirstEmptySlotFound != EMPTY)
                {
                    list[FirstEmptySlotFound] = member;
                    numberOfMember++;
                    return FirstEmptySlotFound;
                }
                // if no empty slot was found: go ahead finding one..
                else for(index = numberOfMember; index < this->getMaximumSize(); index++)
                {
                    if(list[index] == Nulled)
                    {
                        list[index] = member;
                        numberOfMember++;
                        return (highestSlotNumberInUse = index);
                    }
                }
                //if maximum count on members reached !!
                throw "Liste voll!..\n";
            } catch(char* exeption)
            {
                std::cout << "Exception in StepList<";
                std::cout << typeid(ElementType).name();
                std::cout << "> ... ";
                std::cout << exeption;
            }
            return EMPTY;
        }

        //returns the member at List[index].
        virtual ElementType& operator[](ElmID index)
        {
            return list[index];
        }
        //returns the member at(index).
        virtual ElementType& at(ElmID index)
        {
            return list[index];
        }

        //retrieve's a pointer to the ListMember.at(index)...
        virtual ElementType* getPointerToIndex(ElmID index)
        {
            return &list[index];
        }

        // returns count on elements.
        virtual int Count(void)
        {
            return numberOfMember;
        }

        // return the ID of the first member in the list..
        virtual ElmID First(void)
        {
            //list-slot indexes can't be negative, so getting the next of [-1] will be the first...
            return Next(-1);
        }

        // return the ID of the last member at the end of the list..
        virtual ElmID Last(void)
        {
            //return a "peak"-value, witch's updated every time an objects is Added or Removed
            return highestSlotNumberInUse;
        }

        // returns the Next member's ID - the next used slot-number in the list
        virtual ElmID Next(int current)
        {
            //return if list is empty.
            if(numberOfMember == 0)
                return EMPTY;

            //fast forward until finding next used slot or reaching it's end
            while(list[++current] == Nulled);

            //and return it's position.
            return (ElmID)current;
        }

        // returns the previous member's ID - the first used slot-number found before [current]
        int virtual Prev(int current)
        {
            // check if reached start-address
            if(current <= 0)
                return EMPTY;

            //rewind the list until finding a used slot or reaching it's start-address
            while(list[--current] == Nulled);

            //return the found position.
            return (ElmID)current;
        }

        // remove's the member at slot-number [ListMemberID]...
        // and return it
        virtual ElementType Remove(ElmID id)
        {
            //check if slot at "id" is in use..
            if(list[id] != Nulled)
            {
                //save element to return it later..
                ElementType removed = list[id];

                //erase the pointer,
                list[id] = Nulled;

                //if it's the last element:
                if(id == highestSlotNumberInUse) //decrease the peak-value...
                    highestSlotNumberInUse = Prev(id); // to it's preceding element.

                // and decrease the member-count by 1.
                --numberOfMember;

                return removed;
            }
            return Nulled;
        }

        // remove's the given member from the list, if it's contained in it..
        // and return it
        virtual ElementType Remove(ElementType member)
        {
            for(ElmID ID = First(); ID <= Last(); ID = Next(ID))
            {
                if(list[ID] == member)
                {
                    highestSlotNumberInUse = (ID == highestSlotNumberInUse) ? Prev(ID) : highestSlotNumberInUse;

                    list[ID] = Nulled;
                    --numberOfMember;
                    return member;
                }
            }
            return Nulled;
        }

        // remove's the element at slot-number [ListMemberID] and calls it's destructor...
        virtual void Distruct(ElmID id)
        {
            if(list[id] != Nulled)
            {
                if(id == highestSlotNumberInUse)
                    highestSlotNumberInUse = id != First() ? Prev(id) : highestSlotNumberInUse;

                if(MemberArePointer())
                {
                    size_t ptval = (size_t)list[id];
                    delete (void*)ptval;
                }

                list[id] = Nulled;

                --numberOfMember;
            }
        }

        // Execute ElementAction once per contained element
        virtual void foreach(ElementAction elementAction) = 0;

        void forEach(void* argum,void(*elefantAction)(void*,eT))
        {
            for(ElmID id = First(); id <= highestSlotNumberInUse;
                id = Next(id)) elefantAction(argum, list[id]);
        }

        template<typename CallersClass=TYPE>
        void forEachOn(void(CallersClass::*performOnCaller)(eT))
        {
            for(ElmID id = First(); id <= highestSlotNumberInUse;
                id = Next(id)) (((CallersClass*)this)->*performOnCaller)(list[id]);
        }

        template<typename Caller, typename Argum>
        void PerformBy(void(Caller::*func)(ElementType, Argum), Argum arg)
        {
            for(ElmID id = First(); id <= Last(); id = Next(id))
                (((Caller*)this)->*func)(list[id], arg);
        }
    };

}
#endif