/*
 * File:   Jongleur.hpp
 * Author: kalle
 *
 * Created on 14. Oktober 2017, 18:23
 */

#ifndef JONGLEUR_HPP
#define JONGLEUR_HPP

#ifdef   BEGIN_STEPFLOW_NAMESPACE
#include "../Macros/InternalAccess.h"
         BEGIN_STEPFLOW_NAMESPACE
#endif

    template<typename cT, unsigned char NUMBER_OF_PINS>
    class Jongleur
//  It takes NUMBER_OF_PINS pointers to other values...
//  and starts Moving the first one until it reaches the second's value.
//  then it switches the pointers and continues,- now Moving the second one,
//  until it reaches the third, then switches the pointer and goes on moving the third
//  when reached the last PIN it switch's back to the first....
//  ...maybe useful for something...
        : public Follower<cT>
    {
    protected:
        typedef Follower<cT> BASE;
        cT* pPIN[NUMBER_OF_PINS];
        char currentPIN;
        virtual void Init(void)
        {
            BASE::Init();
            currentPIN = 0;
            PIN_COUNT += NUMBER_OF_PINS;
        }
    public:
        virtual void* Pin(void*pin, int idx) {
            if (idx > NUMBER_OF_PINS) {
                return BASE::Pin(pin, --idx);
            } else
            if (idx < NUMBER_OF_PINS) {
                return pPIN[idx] = (cT*)pin;
            }//if(idx == NUMBER_OF_PINS)
                currentPIN = *(char*)pin;
            return &currentPIN;
        }
        virtual cT checkVALUE(cT* pVALUE)
        {
            int lastpin = currentPIN;
            BASE::pTARGET = pPIN[currentPIN];

            *pVALUE = *pVALUE < *BASE::pTARGET ? *pVALUE + MOV
                    : *(pVALUE = pPIN[currentPIN++]) + MOV;

            if(currentPIN == NUMBER_OF_PINS)
                currentPIN = 0;

            if(lastpin != currentPIN)
                pPIN[lastpin] = BASE::pTARGET;

            *pVALUE = *BASE::pTARGET;

            return ClambController<cT>::checkVALUE(pVALUE);
        }

        virtual ~Jongleur(void)
        {
            delete[] pPIN;
        }

        virtual void SetTarget(cT* targetValues)
        {
            for(unsigned i = 0; i<NUMBER_OF_PINS; i++)
                pPIN[i] = &targetValues[i];
        }

        virtual void SetTarget(cT** targetValues)
        {
            for(unsigned i = 0; i<NUMBER_OF_PINS; i++)
                pPIN[i] = targetValues[i];
        }

        virtual void SetTarget(cT* targetValue, char pinNumber)
        {
            pPIN[pinNumber] = targetValue;
        }
    };

#ifdef   ENDOF_STEPFLOW_NAMESPACE
         ENDOF_STEPFLOW_NAMESPACE
#include "../Macros/InternalAccess.h"
#endif


#endif /* JONGLEUR_HPP */

