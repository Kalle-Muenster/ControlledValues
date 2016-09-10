#ifndef _CYCLABLE_LIST_INTERFACE_H_
#define _CYCLABLE_LIST_INTERFACE_H_

#include "StaticList.h"
#include "DynamicList.h"

namespace stepflow{
	template<typename baseType, typename elementType>
	class AbstractCycle : public baseType
	{
	protected:
		volatile bool           CyclingActive;
		volatile int            CyclingCounter;
		volatile unsigned       CycleCheck;

	public:
		typedef unsigned int ElmID;

		AbstractCycle(void) : baseType()
		{ CyclingActive = false; CycleCheck = 0; }
		AbstractCycle(elementType nuller) : baseType(nuller)
		{ CyclingActive = false; CycleCheck = 0; }
		AbstractCycle(elementType nuller, unsigned capacity) : baseType(nuller, capacity)
		{ CyclingActive = false; CycleCheck = 0; }
		
		// Activate Cycle-Style-Iteration: 
		// Iteration will start at given offset-index ID
		// and the list will behave like offsetID where it's First element
		// when iteration again reaches offsetID, CycleMode will turn off itself
		virtual int StartCycle(int offsetID)
		{
			CycleCheck = Next(offsetID - 1);
			CyclingActive = true;
			CyclingCounter = 0;
			return CycleCheck;
		}

		// returns the Next member's ID - the next used slot-number in the list
		virtual ElmID Next(int current)
		{
			//return if list is empty.
			if(baseType::numberOfMember == 0)
				return EMPTY;

			//fast forward until finding next used slot or reaching it's end
			while(list[++current] == Nulled);

			//If in cycle-mode jump to first if reached end...
			if(CyclingActive)
			{
				CycleCheck = (current = current >= this->getMaximumSize() ? 0 : current);

				if(++CyclingCounter >= this->numberOfMember)
				{
					CyclingActive = false;
					CycleCheck = EMPTY;
				}
			}

			//and return it's position.
			return (ElmID)current;
		}

		// returns the previous member's ID - the first used slot-number found before [current]
		int virtual Prev(int current)
		{
			// check if reached start-address 
			if(current <= 0)
			{
				if(CyclingActive)
					current = Last() + 1;
				else
					return EMPTY;
			}

			//rewind the list until finding a used slot or reaching it's start-address 
			while(list[--current] == Nulled);

			//if in Cycle-mode, jump to Last() if reached First() 
			if(CyclingActive)
			{
				CycleCheck = current;
				if(++CyclingCounter >= numberOfMember)
				{
					CyclingActive = false;
					CycleCheck = EMPTY;
				}
			}

			//return the found position.
			return (ElmID)current;
		}

		// if cycling active, Cycle is used as Last/First where used normally
		virtual ElmID Cycle(void)
		{
			return CyclingActive ? CycleCheck : EMPTY;
		}

	};
}

#endif