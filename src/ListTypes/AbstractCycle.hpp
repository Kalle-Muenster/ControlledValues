#ifndef _CYCLABLE_LIST_INTERFACE_H_
#define _CYCLABLE_LIST_INTERFACE_H_

#include "StaticList.hpp"
#include "DynamicList.hpp"

#ifdef LISTTYPE_STANDARD_ITERATION
#undef LISTTYPE_STANDARD_ITERATION
#endif
#define LISTTYPE_STANDARD_ITERATION for( ElmID ID=StartCycle(); ID==Cycle(); ID=Next(ID) )


BEGIN_STEPFLOW_NAMESPACE

   #if _MSC_VER>=1900
    template< typename elementType = void*, 
              template< typename eT = elementType >typename baseType = ListBase
	>class AbstractCycle : public baseType<elementType>
   #else
	template< typename elementType = void*, typename baseType = ListBase<elementType>
	>class AbstractCycle : public baseType
   #endif
    {
    protected:
        volatile bool           CyclingActive;
        volatile int            CyclingCounter;
        volatile unsigned       CycleCheck;
        unsigned int            currentPosition;

    public:
        typedef elementType           TYPE;
      #if _MSC_VER>=1900
        typedef baseType<elementType> BASE;
      #else
		typedef baseType              BASE;
      #endif
        typedef unsigned int          ElmID;

        AbstractCycle(void) : BASE()
        { CyclingActive = false; CycleCheck = currentPosition = 0; }
        AbstractCycle(TYPE nuller) : BASE(nuller)
        { CyclingActive = false; CycleCheck = currentPosition = 0; }
        AbstractCycle(TYPE nuller, unsigned capacity) : BASE(nuller, capacity)
        { CyclingActive = false; CycleCheck = currentPosition = 0; }
        AbstractCycle(TYPE elements...) : BASE(elements...)
        { CyclingActive = false; CycleCheck = currentPosition = 0; }

        // Activate Cycle-Style-Iteration:
        // Iteration will start at given offset-index ID
        // and the list will behave like offsetID where it's First element
        // when iteration again reaches offsetID, CycleMode will turn off itself
        virtual int StartCycle( int offsetID = -1 )
        {
            if (offsetID < 0)
                offsetID = currentPosition;

            CycleCheck = Next(offsetID - 1);
            CyclingActive = true;
            CyclingCounter = 0;
            return CycleCheck;
        }

        // returns the Next member's ID - the next used slot-number in the list
        virtual ElmID Next(int current)
        {
            //return if list is empty.
            if(BASE::numberOfMember == 0)
                return EMPTY;

            //fast forward until finding next used slot or reaching it's end
            while(BASE::list[++current] == BASE::Nulled);

            //If in cycle-mode jump to first if reached end...
            if(CyclingActive)
            {
                CycleCheck = (current = current >= (int)this->getMaximumSize() ? 0 : current);

                if(++CyclingCounter >= this->numberOfMember)
                {
                    CyclingActive = false;
                    CycleCheck = EMPTY;
                }
            }

            //and return it's position.
            return currentPosition = (ElmID)current;
        }

        // returns the previous member's ID - the first used slot-number found before [current]
        int virtual Prev( int current )
        {
            // check if reached start-address
            if( current <= 0 )
            {
                if( CyclingActive )
                    current = BASE::Last() + 1;
                else
                    return EMPTY;
            }

            //rewind the list until finding a used slot or reaching it's start-address
            while( BASE::list[--current] == BASE::Nulled );

            //if in Cycle-mode, jump to Last() if reached First()
            if( CyclingActive )
            {
                CycleCheck = current;
                if( ++CyclingCounter >= BASE::numberOfMember )
                {
                    CyclingActive = false;
                    CycleCheck = EMPTY;
                }
            }

            //return the found position.
            return currentPosition = (ElmID)current;
        }

        // if cycling active, Cycle is used as Last/First where used normally
        virtual ElmID Cycle(void)
        { return CyclingActive ? CycleCheck : EMPTY_(ElmID); }


        virtual ElmID Find(TYPE element) 
		{ LISTTYPE_STANDARD_ITERATION
              if (element == list[ID])
                  return ID;
          return EMPTY_(ElmID); }

        virtual void foreach(void(*call)(TYPE))
        { LISTTYPE_STANDARD_ITERATION
             call(BASE::list[ID]); }

        template<typename Carrier,typename Conditioner>
        void whileEachAre( bool(Carrier::*checkIfIs)(TYPE,Conditioner), Conditioner isItThis = NULL )
		{ ElmID ID = StartCycle(currentPosition);
            while ((((Carrier*)this)->*checkIfIs)(BASE::list[ID], isItThis))
                ID = Next(ID); }

        template<typename Conditioner = void*>
        TYPE find( bool(*function)(TYPE,Conditioner), Conditioner comparer = NULL )
        { LISTTYPE_STANDARD_ITERATION {
                if ( function( BASE::list[ID], comparer ) )
                    return BASE::list[ID];
          } return BASE::Nulled; }

        template<typename CarrierClass, typename Conditioner=void*>
        TYPE find( bool(CarrierClass::*method)(TYPE, Conditioner), Conditioner compare=NULL )
        { LISTTYPE_STANDARD_ITERATION {
                if ( (((CarrierClass*)this)->*method)(BASE::list[ID],compare) )
                    return BASE::list[ID];
          } return BASE::Nulled; }

    };

ENDOF_STEPFLOW_NAMESPACE

#endif
