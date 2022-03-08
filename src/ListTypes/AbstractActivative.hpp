/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AbstractActivative.hpp                     ||
||     Author:    Kalle                                      ||
||     Generated: 29.09.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _AbstractActivative_hpp_
#define _AbstractActivative_hpp_

#include "ListBase.hpp"

#ifdef BEGIN_STEPFLOW_NAMESPACE
BEGIN_STEPFLOW_NAMESPACE
#endif

#define MAX_FUNCTIONS_COUNT (10)

typedef unsigned int uint;

    template< typename elementType=void*, template< typename eT = elementType >class baseType = ListBase >
    class AbstractActivative : public baseType<elementType>
    {
    protected:
        void(*actions[MAX_FUNCTIONS_COUNT])(elementType);
        void(*senders[MAX_FUNCTIONS_COUNT])(elementType,void*);
        uint(*request[MAX_FUNCTIONS_COUNT])(elementType,void*);
        void* elmdats[MAX_FUNCTIONS_COUNT];

    public:
        typedef baseType<elementType> BASE;
        typedef elementType TYPE;
        typedef unsigned int ActID;
        typedef void(*Action)(elementType);
        typedef void(*Sender)(elementType,void*);
    //    typedef uint(*Request)(elementType,void*);

        typedef void(elementType::*ElmAction)(elementType);
        typedef void(elementType::*ElmSender)(elementType,void*);
    //    typedef uint(elementType::*ElmRequest)(elementType,void*);

        AbstractActivative(void) : BASE() {
            for (ActID act = 0; i < MAX_FUNCTIONS_COUNT; i++) {
                actions[act] = nullptr;
                senders[act] = nullptr;
                request[act] = nullptr;
                elmdats[act] = NULL;
            }
        };
        virtual ~AbstractActivative(void) {};

        void invokeOn(ElmID elm, ActID act) {
            ((elementType)this->)*action[act](((ListBase<elementType>*)this)->at(elm));
        }

        template<typename fp=Action>
        ActID addGlobalFunction(fp action) {
            size_t* helpers;
            switch (fp) {
                case Action: helpers = *(size_t**)&actions; break;
                case Sender: helpers = *(size_t**)&senders; break;
                case Request: helpers = *(size_t**)&request; break;
                default: throw new std::exception("unsupported signature");
            }
            for (ActID actid = 0; actid < MAX_FUNCTIONS_COUNT; actid++) {
                if(!helpers[actid]) {
                    helpers[actid] = (size_t)(void*)action;
                    return actid;
                }
            }
        }

        template<typename elmfunc = ElmAction>
        ActID addElementFunction(elmfunc elmaction) {
            size_t* helpers;
            switch (elmfunc) {
                case ElmAction: helpers = *(size_t**)&actions; break;
                case ElmSender: helpers = *(size_t**)&senders; break;
                case ElmRequest: helpers = *(size_t**)&request; break;
                default: throw new std::exception("unsupported signature");
            }
            for (ActID actid = 0; actid < MAX_FUNCTIONS_COUNT; actid++) {
                if (!helpers[actid]) {
                    helpers[actid] = (size_t)(void*)elmaction;
                    return actid;
                }
            }
        }


        Action action(uint idx) {
            return actions[idx];
        }

        void invoke( elementType element, ActID index ) {
            actions[index](element);
        }
    };

#ifdef ENDOF_STEPFLOW_NAMESPACE
       ENDOF_STEPFLOW_NAMESPACE
#endif
            
#endif
            
