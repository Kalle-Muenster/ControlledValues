
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

#include "AbstractStrait.hpp"
#include "AbstractCycle.hpp"
#include "AbstractTyped.hpp"
#include "AbstractActivative.hpp"


BEGIN_STEPFLOW( LISTS )

enum ITERATION_DIRECTION : uint {
    FORWARD = 0,
    BACKWARD = 0x1,
};

template<
    const ITERATION_DIRECTION ITERATION,
    typename ElmType,
    typename ListClass = ListBase<ElmType>
> struct StepIterator {
protected:
    friend ListClass;
    typedef ElmType TYPE;
    ElmID ID;
    ListClass* list;
    StepIterator( ListClass* lst )
        : ID( ITERATION&BACKWARD
            ? lst->Last()
            : lst->First() )
         , list(lst) {
    }
public:
    ElmType& operator*(void) {
        return list->at(ID);
    }
    ElmType& operator->(void) {
        if (list->memberArePointer) {
            return list->at(ID);
        } else {
            return &list->at(ID);
        }
    }
    ListClass& operator&(void) {
        return *list;
    }
    ElmType& operator ++(void) {
        ID = list->Next(ID);
        return (*list)[ID];
    }
    ElmType& operator --(void) {
        ID = list->Prev(ID);
        return (*list)[ID];
    }
    ElmType& operator ++(int) {
        ElmType* current = &(*list)[ID];
        ID = list->Next(ID);
        return *current;
    }
    ElmType& operator --(int) {
        ElmType* current = &(*list)[ID];
        ID = list->Prev(ID);
        return *current;
    }
    virtual void Delete(void) {
        list->Distruct(ID);
    }
    virtual ElmType Remove(void) {
        return list->Remove((*list)[ID]);
    }
    virtual bool Step() {
        if (ITERATION & BACKWARD) {
            ID = list->Prev(ID);
        } else {
            ID = list->Next(ID);
        } return ID != EMPTY_(ElmID);
    }
    operator ElmID(void) {
        return ID;
    }
    operator TYPE& (void) {
        return list->at(ID);
    }
};

    template<typename elmTyp> 
    class DynamicStrait 
        : public DynamicListType<elmTyp,AbstractStrait<elmTyp>>
    { public:
        DynamicStrait(void)
            : DynamicListType<elmTyp,AbstractStrait<elmTyp>>()
        {}
        DynamicStrait(const elmTyp& nuller, unsigned capacity = 4)
            : DynamicListType<elmTyp,AbstractStrait<elmTyp>>(nuller, capacity)
        {}
         
        template<const ITERATION_DIRECTION DIRECTION> struct Iterator
            : public StepIterator<DIRECTION,elmTyp,DynamicStrait> {
        protected:
            friend class DynamicStrait<elmTyp>;
            Iterator(DynamicStrait* straight) : StepIterator<DIRECTION,elmTyp,DynamicStrait>(straight) {}
        };

        template<const ITERATION_DIRECTION DIRECTION>
        Iterator<DIRECTION> getIterator(ElmID startElement = EMPTY_(ElmID)) {
            Iterator<DIRECTION> it(this);
            if (startElement != EMPTY_(ElmID))
                it.ID = startElement;
            return it;
        }
    };



    template<typename elmTyp> 
	class DynamicList 
        : public DynamicListType<elmTyp,AbstractCycle<elmTyp>> {
    public:
        enum ITERATOR_MODE : uint {
            STRAIGHT = 0,
            FORWARD = ITERATION_DIRECTION::FORWARD,
            BACKWARD = ITERATION_DIRECTION::BACKWARD,
            CYCLE = 0x2,
        };
        template<const ITERATOR_MODE ITERATION> struct Iterator
            : public StepIterator<(ITERATION_DIRECTION)ITERATION, elmTyp, DynamicList> {
        protected:
            friend class DynamicList<elmTyp>;
            Iterator( DynamicList* lst )
                : StepIterator<(ITERATION_DIRECTION)ITERATION, elmTyp, DynamicList>(lst) {
                ID = list->currentPosition;
            }
        public:
            virtual elmTyp Remove(void) override {
                elmTyp taken = list->Remove((*list)[ID]);
                --list->CyclingCounter;
                return taken;
            }
            virtual void Delete(void) override {
                list->Distruct(ID);
                --list->CyclingCounter;
            }
            virtual bool Step() override {
                if (ITERATION & BACKWARD) {
                    ID = list->Prev(ID);
                } else {
                    ID = list->Next(ID);
                } return ITERATION & CYCLE
                       ? ID == list->Cycle() 
                       : ID != EMPTY_(ElmID);
            }
        };
        DynamicList(void) 
			: DynamicListType<elmTyp,AbstractCycle<elmTyp>>()
		{}
        DynamicList(const elmTyp& nuller, unsigned capacity=4) 
            : DynamicListType<elmTyp,AbstractCycle<elmTyp>>(nuller, capacity)
		{}
        template<const ITERATOR_MODE ITERATION>
        Iterator<ITERATION> getIterator( ElmID startElement = EMPTY_(ElmID) ) {
            if ( ITERATION & CYCLE ) {
                StartCycle(*(int*)&startElement);
            } return Iterator<ITERATION>(this);
        }
    };

    template<typename containerType> 
	class DynamicTyped
        : public DynamicListType<containerType*,AbstractTyped<containerType>>
	{};



    template< const unsigned MAXIMUM_SIZE, typename elmTyp> 
	class Strait
        : public  StaticListType< MAXIMUM_SIZE ,elmTyp, AbstractStrait<elmTyp>>
	{ public:
        virtual void foreach(void(*elementAction)(elmTyp)) {
            for( ElmID ID = First(); ID <= Last(); ID = Next(ID) )
                elementAction( BASE::list[ID] );
        }
        template<const ITERATION_DIRECTION DIRECTION> struct Iterator : public StepIterator<DIRECTION,elmTyp,Strait>{
            protected:
                friend class Strait<MAXIMUM_SIZE, elmTyp>;
                Iterator(Strait* straight) : StepIterator<DIRECTION, elmTyp, Strait>(straight) {}
        };
        template<const ITERATION_DIRECTION DIRECTION> Iterator<DIRECTION> getIterator(ElmID startElement = EMPTY_(ElmID)) {
            Iterator<DIRECTION> it(this);
            if (startElement != EMPTY_(ElmID))
                it.ID = startElement;
            return it;
        }
    };

    template< const unsigned MAXIMUM_SIZE,typename elmTyp>
	class List
        : public StaticListType<MAXIMUM_SIZE,elmTyp, AbstractCycle<elmTyp>> {
    public:
        enum ITERATION_MODE : uint {
            STRAIGHT = 0,
            FORWARD = ITERATION_DIRECTION::FORWARD,
            BACKWARD = ITERATION_DIRECTION::BACKWARD,
            CYCLE = 0x02
        };
        List(void) 
			: StaticListType<MAXIMUM_SIZE,elmTyp,AbstractCycle<elmTyp>>()
		{}
        List(elmTyp nuller) 
			: StaticListType<MAXIMUM_SIZE,elmTyp,AbstractCycle<elmTyp>>(nuller)
		{}
        template<const ITERATION_MODE MODE> struct Iterator
            : public StepIterator<(ITERATION_DIRECTION)MODE, elmTyp, List> {
        protected:
            friend class List<MAXIMUM_SIZE, elmTyp>;
            Iterator(List* lst) : StepIterator<(ITERATION_DIRECTION)MODE, elmTyp, List>(lst) {
                ID = list->currentPosition;
            }
        public:
            virtual elmTyp Remove(void) override {
                elmTyp taken = list->Remove((*list)[ID]);
                --list->CyclingCounter;
                return taken;
            }
            virtual void Delete(void) override {
                list->Distruct(ID);
                --list->CyclingCounter;
            }
            virtual bool Step() override {
                if (MODE & BACKWARD) {
                    ID = list->Prev(ID);
                } else {
                    ID = list->Next(ID);
                } return MODE & CYCLE
                    ? ID == list->Cycle()
                    : ID != EMPTY_(ElmID);
            }
        };
        template<const ITERATION_MODE MODE> Iterator<MODE> getIterator(ElmID startElement = EMPTY_(ElmID)) {
            if( MODE & CYCLE ) {
                StartCycle(*(int*)&startElement);
            } return Iterator<MODE>(this);
        }
    };

    template< const unsigned MAXIMUM_SIZE, typename containerType > 
	class Typed
        : public StaticListType< MAXIMUM_SIZE, AbstractTyped<containerType*,containerType>> 
    {};



#if _MSC_VER>=1900
	template< typename ET, template<typename et = ET>typename LT = ListBase> 
#else
	template< typename ET, typename LT = ListBase<ET>>
#endif
	class Vote 
		: public DynamicListType<ET,AbstractActivative<AbstractCycle<ET>>>
	{ public:
        virtual void foreach(void(*call)(ET)) {
           for( ElmID ID = StartCycle(); ID == Cycle(); ID = Next(ID) )
                call( BASE::list[ID] );
        }
    };


ENDOF_STEPFLOW( LISTS )
#endif
