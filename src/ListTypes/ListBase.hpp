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
#include <typeinfo>
//#include <stdio.h>

#ifdef LISTTYPE_STANDARD_ITERATION
#undef LISTTYPE_STANDARD_ITERATION
#endif
#define LISTTYPE_STANDARD_ITERATION for(ElmID ID=First();ID<=highestSlotNumberInUse;ID=Next(ID))

#ifdef BEGIN_STEPFLOW_NAMESPACE
       BEGIN_STEPFLOW_NAMESPACE
#endif

    typedef unsigned int ElmID;
    template<typename eT>
    class ListBase
    {    
    public:
        typedef unsigned int    ElmID;
        typedef ListBase        BASE;
        typedef eT              TYPE;
        typedef void(*          ElementAction )(TYPE);
        

    protected: 
        TYPE*              list;
        TYPE               Nulled;
        int                numberOfMember;
        bool               MembersArePointers;
        ElmID              highestSlotNumberInUse;
        void               setDataArray( eT* pData )
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
        TYPE const &         getSlotNuller(void) const
                             { return Nulled; }
        bool                 MemberArePointer(void)
                             { return MembersArePointers; }
        virtual unsigned     getMaximumSize(void) = 0;

        // empties the list and destruct's all
        // contained member if they where pointer.
        virtual void Clear(bool=true) = 0;

        // Add a member to the list and return the memberID
        // (slot-number) at which the object has been added to...
        virtual ElmID Add( TYPE& member )
        {
            ElmID FirstEmptySlotFound = EMPTY;
            ElmID counter = 0;
            ElmID index = 0;

				//check's if the object has been added to the list already.
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
                else for( index = numberOfMember; index < this->getMaximumSize(); index++ )
                {
                    if( list[index] == Nulled )
                    {
                        list[index] = member;
                        numberOfMember++;
                        return ( highestSlotNumberInUse = index );
                    }
                }
                //if maximum count on members reached !!
                throw std::exception("Liste voll!..\n");
        }

		void setSlotNuller(const TYPE& newNuller)
		{
			const unsigned long end = getMaximumSize();
			for (unsigned long i = 0; i < end; i++)
				if (list[i] == Nulled)
					list[i] = newNuller;
			Nulled = newNuller;
		}

        //returns the member at List[index].
        virtual TYPE& operator[](ElmID index)
        {
            return list[index];
        }
        //returns the member at(index).
        virtual TYPE& at(ElmID index)
        {
            return list[index];
        }

        //retrieve's a pointer to the ListMember.at(index)...
        virtual TYPE* getPointerToIndex(ElmID index)
        {
			return (TYPE*)((byte*)list + (sizeof(TYPE) * index));;
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
        virtual TYPE RemoveAt(ElmID id)
        {
            //check if slot at "id" is in use..
            if(list[id] != Nulled)
            {
                //save element to return it later..
                TYPE removed = list[id];

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
        virtual TYPE Remove(TYPE& member)
        {
            LISTTYPE_STANDARD_ITERATION
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
                    size_t pointerValue = *(size_t*)&list[id];
                    delete (TYPE*)pointerValue;
                }

                list[id] = Nulled;

                --numberOfMember;
            }
        }


        virtual ElmID Find(TYPE& element)
        { LISTTYPE_STANDARD_ITERATION
            if (element == list[ID])
                return ID;
          return EMPTY_(ElmID); }

        // Execute ElementAction once per contained element
        virtual void foreach(ElementAction elementAction) = 0;

        void forEach(void(*blindParameterPaste)(TYPE,void*), void* parameter)
        { LISTTYPE_STANDARD_ITERATION 
            blindParameterPaste(list[ID], parameter); }

        template<typename ComponentClass> // perform on each carrier elements connected component 
        void forEachOn(void(ComponentClass::*triggerMethod)(TYPE))
        { LISTTYPE_STANDARD_ITERATION 
            (((CallersClass*)this)->*triggerMethod)(list[ID]); }

        template<typename Argum, typename Caller> // perform for each child element by a method on parent 
        void PerformBy(Argum arg, void(Caller::*typedParameterMethod)(TYPE, Argum))
        { LISTTYPE_STANDARD_ITERATION
            (((Caller*)this)->*typedParameterMethod)(list[ID], arg); }

        template<typename Carrier,typename ArgType>
        unsigned forEachCount( bool(Carrier::*conditionalMethod)(TYPE,ArgType), ArgType arg=NULL )
        { unsigned statistics = 0;
          LISTTYPE_STANDARD_ITERATION
            statistics += (((Carrier*)this)->*conditionalMethod)(list[ID], arg);
          return statistics; }

        template<typename ObjectClass> // passing each element as arguments to a method on some object
        void foreachOnObject( ObjectClass* obj, void(ObjectClass::*call)(TYPE) )
        { LISTTYPE_STANDARD_ITERATION
            (obj->*call)(list[ID]); }

        template<typename SomeObject,typename Arg, Arg...>
        void performOnObject(SomeObject* obj, void(SomeObject::*func)(TYPE,Arg...),Arg arg...)
        { LISTTYPE_STANDARD_ITERATION
            (obj->*func)(list[ID],arg...); }

        template<typename ReturnType, typename FromObject>
        ReturnType fromEachReturn(FromObject* obj, ReturnType(FromObject::*collect)(TYPE))
        { ReturnType collector = ReturnType();
          LISTTYPE_STANDARD_ITERATION
            collector += (obj->*collect)( list[ID] );
          return collector; }

    };
    
#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW_NAMESPACE
#endif

#endif
