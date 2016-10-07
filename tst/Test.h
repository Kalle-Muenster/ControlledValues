#ifndef _PERFORMANCES_
#define _PERFORMANCES_

#include <Windows.h>
#include <iostream>



LARGE_INTEGER*
getTimer(void);

// Helper macros for scoped measurements. don't use directly, 
// use BEGIN_MEASURE and END_MEASURE macros instead.
#define START_CLOCK(timerName) LARGE_INTEGER* tmr = getTimer(); \
                               QueryPerformanceCounter(tmr); \
                               DWORD timerName = tmr->QuadPart

#define RETURN_CLOCK(timerName) QueryPerformanceCounter(tmr); \
                                return tmr->QuadPart - timerName; \

// begins a scoped measurement in class which
// implements iTestTimer interface
#define BEGIN_MEASURE START_CLOCK(measure);

// closes the scope and return it's amount.
#define END_MEASURE  RETURN_CLOCK(measure);


// std::string but will implicit cast to const char*
struct String : public std::string
{
    String(void) 
        : std::string() {};
    String(const char* init)
        : std::string(init) {};
    String(std::basic_string<char>& copy)
        : std::string(copy) {};

    operator const char*(void)
    { return this->c_str(); }

    String operator +(String& other)
    { String tmp(this->c_str());
      tmp.append(other);
      return tmp; }

    String operator +(const char* other)
    {
        String tmp(this->c_str());
        tmp.append(other);
        return tmp;
    }

    String operator +(char other)
    {
        char buf[2] = { other, '\0' };
        String tmp(this->c_str());
        tmp.append(&buf[0]);
        return tmp;
    }
};



// function that will by used when
// performing "foreach" loops:
template<typename E>
void copy(E src, E& dst)
{
    dst = src;
}


namespace stepflow{
    namespace Tests{
    
            //unsigned data types
            typedef unsigned char       i8;
            typedef unsigned short      i16;
            typedef unsigned            i32;
            typedef unsigned long long  i64;

            //signed data type
            typedef char                s8;
            typedef short               s16;
            typedef int                 s32;
            typedef long long           s64;

            //fpu types
            typedef long double         d64;
            typedef double              f64;
            typedef float               f32;

            
            
            //construction wrappers:
            template<typename T> DWORD
                constructProband(T** object)
            { BEGIN_MEASURE
                    *object = new T();
                END_MEASURE }

            template<typename T, typename P> DWORD
                constructProband(T** object, P parameter)
            { BEGIN_MEASURE
                    *object = new T(parameter);
                END_MEASURE }

            template<typename T, typename P1, typename P2 = P1> DWORD
                constructProband(P1 para1, P2 para2)
            { BEGIN_MEASURE
                    *object = new T(para1, para2);
                END_MEASURE }

    /*      template<typename T, typename P1, typename P2, typename P3 = P2> DWORD
                constructProband(T** object, P1 para1, P2 para2, P3 para3)
            {
                BEGIN_MEASURE
                    *object = new T(para1, para2, para3);
                END_MEASURE }
    */
            // Interface which adds a high precision tick counter 
            // to a class, so you can define scopes by BEGIN_MEASURE 
            // and END_MEASURE in it. Such scope will return the amount
            // on ticks when the code inside has been executed.  
            class iTestTimer
            {
            protected:
                static DWORD ticksPerSecond;
                static double timeFactor;
                static bool isInitialized;
                LARGE_INTEGER timer;

            public:
                static void InitializeTimer(void);
                static unsigned getTicksPerSecond(void)
                                { return ticksPerSecond; }
            };

            // A test case for objects with up to 2 construction parameters.
            // choosable number of func pointers can be assigned to it, which
            // then will be invoked on the object while measuring and logging 
            // the amounts on execution time. 
            template<typename T, typename E, const int TESTS_COUNT>
            class TestCase : public iTestTimer
            {
            public:
                enum PHASEN
                     { construction = 0, deletion = TESTS_COUNT + 1,
                       anzahl=TESTS_COUNT+2 };
                typedef T CONTAINER;
                typedef E DATA_TYPE;
                typedef void(*ElementFunc)(E, E&);
                typedef void(*PerformAction)(TestCase*, void**);
                typedef void(*IterationFunc)(T*, ElementFunc, E*);
                typedef void(*PerformDoubleAction)(TestCase*, void*, void*);

            private:        
                unsigned long long  elementsCount;
                LARGE_INTEGER*      getTimer(void) { return &timer; }
                IterationFunc       iteration;
                void(*              tests[PHASEN::anzahl])(TestCase*, void**);
                DWORD               amounts[PHASEN::anzahl];
                String              testNames[PHASEN::anzahl];
                int                 testOrder[PHASEN::anzahl];
                byte                currentPhase;
                bool                printResults;
                bool                testFinished;
                char                txtOutBuffer[128];
                E*                  testData;
                T*                  proband;
                void*               CtorPrmtr[3];
                bool                dontReInstanciate;

            public:
                TestCase(bool atOnce=false)
                {
                    proband = NULL;
                    printResults = true;
                    testFinished = false;
                    currentPhase = elementsCount = 0;
                    sprintf_s<128>( txtOutBuffer,"test not finished yet! :\\\n");
            
                    for(int i = 0; i < PHASEN::anzahl; i++)
                        amounts[i] = 0;

                    testOrder[PHASEN::construction] = PHASEN::construction;
                    testNames[PHASEN::construction] = "Constructon";
                    testOrder[PHASEN::deletion] = PHASEN::deletion;
                    testNames[PHASEN::deletion] = "Deletion";

                    SetConstructionParameter<void*>();

                    if(dontReInstanciate = atOnce)
                        proband = new CONTAINER();
                };

                virtual ~TestCase(void)
                { 
                  if(proband)
                  {
                      delete proband;
                      proband = NULL;
                  }
                  if(testData)
                      delete testData;
                };

                // functions for configuring the test
                ///////////////////////////////////////////////////////
                void SetContainerSize(unsigned long long newSize)
                     { elementsCount = newSize; }

                // allocate a new test data block 
                E* SetCheckDataBuffer(int elementsCount)
                { 
                    SetCheckDataBuffer(new E[elementsCount], elementsCount);
                    for(int i = 0; i < elementsCount; i++)
                        testData[i] = 0;
                    return testData;
                }

                // set an already allocated data block to be used as test data.
                E* SetCheckDataBuffer(E* allocatedBlock, int elementsCount)
                   { this->elementsCount = elementsCount;
                       return testData = allocatedBlock; }

                // Get current testphase while test is running.
                const char* getCurrentPhase(void)
                {
                    return testNames[testOrder[currentPhase]];
                }

                // get the testData block that's used.
                E* getTestData(void)
                {
                    return testData;
                }

                // detach the testData block if you will need it later.
                // If you don't detach before getting the results,
                // it will be deleted within test case distruction.         
                E* detachData(void)
                {
                    if(testFinished) {
                        E* temp = testData;
                        testData = NULL;
                        return temp; 
                    } else return testData;
                }

                // Set function that will iterate once through all the data
                void SetIterationFunction(IterationFunc iterFunc)
                {
                    iteration = iterFunc;
                }

                // Set up to three construction parameters 
                template< typename CtorP1, 
                          typename CtorP2 = CtorP1, 
                          typename CtorP3 = CtorP2 >
                void SetConstructionParameter( CtorP1* argumOne = nullptr, 
                                               CtorP2* argumTwo = nullptr, 
                                               CtorP3* argumTri = nullptr ) {
                    CtorPrmtr[0] = argumOne;
                    CtorPrmtr[1] = argumTwo;
                    CtorPrmtr[2] = argumTri;
                }

                // add a function to be performed on the object.
                void AddTestFunction(PerformAction action, const char* name, int order)
                {
                    if(currentPhase < PHASEN::anzahl)
                    {
                        testOrder[++currentPhase] = order;
                        tests[order] = action;
                        testNames[order] = (char*)name;
                    }
                }

                CONTAINER& getProband(void)
                {
                    return *proband;
                }

                // don't run proband's con-/de-structors
                bool dontConstruct(void)
                {
                    return dontReInstanciate;
                }

                // size of testData buffer
                unsigned long getElementCount(void)
                {
                    return elementsCount;
                }

                // speed of the clock 
                DWORD GetTicksPerSecond(void)
                {
                    return ticksPerSecond;
                }

                // clock-ticks to milliseconds
                double ToMilliSeconds(DWORD ticks)
                {
                    return (double)(ticks * timeFactor);
                }


                // starting tests:
                //////////////////////////////////////////////////////
                template<typename CtorP1>
                DWORD ConstructAndRunTests(void)
                {
                    if(((CtorP1*)CtorPrmtr[0]) == nullptr)
                        return RunStandardConstructor();
                    else
                        return RunConstructor(*(CtorP1*)CtorPrmtr[0]);
                }

                template<typename CtorP1, typename CtorP2 = CtorP1>
                DWORD RunTest(void)
                {
                    if(((CtorP2*)CtorPrmtr[1] == nullptr))
                        return RunConstructor(*(CtorP1*)CtorPrmtr[0]);
                    else
                        return RunConstructor(*(CtorP1*)CtorPrmtr[0], *(CtorP2*)CtorPrmtr[1]);
                }

                template<typename CtorP1, typename CtorP2 = CtorP1, typename CtorP3 = CtorP2>
                DWORD RunTest(void)
                {
                    if(((CtorP3*)CtorPrmtr[2] == nullptr))
                        return RunConstructor(*(CtorP1*)CtorPrmtr[0], *(CtorP2*)CtorPrmtr[1]);
                    else
                        return RunConstructor(*(CtorP1*)CtorPrmtr[0], *(CtorP2*)CtorPrmtr[1], *(CtorP3*)CtorPrmtr[2]);
                }
    

                DWORD RunConstructorless(void)
                { BEGIN_MEASURE
                    Run();
                if(dontReInstanciate)
                    amounts[testOrder[PHASEN::deletion]] = deleteInContainer();
                else
                    amounts[testOrder[PHASEN::deletion]] = deleteProband();
                proband = NULL;
                    testFinished = true; 
                  END_MEASURE }

                DWORD RunStandardConstructor(void)
                { if(proband)
                    amounts[testOrder[PHASEN::deletion]] = deleteProband();
                  proband = false;
                  BEGIN_MEASURE
                    amounts[testOrder[PHASEN::construction]] = constructProband<T>(&proband);
                    Run();
                    amounts[testOrder[PHASEN::deletion]] = deleteProband();
                    proband = NULL;
                    testFinished = true;
                  END_MEASURE }

                template<typename P1>
                DWORD RunConstructor(P1 parameter)
                {
                    if(proband)
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                    proband = false;
                    BEGIN_MEASURE
                        amounts[testOrder[PHASEN::construction]] = constructProband(&proband, parameter);
                        Run();
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                        proband = NULL;
                        testFinished = true;
                    END_MEASURE
                }
                
                template<typename P1,typename P2>
                DWORD RunConstructor(P1 p1,P2 p2)
                {
                    if(proband)
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                    proband = false;
                    BEGIN_MEASURE
                        amounts[testOrder[PHASEN::construction]] = constructProband(&proband,p1,p2);
                        Run();
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                        proband = NULL;
                        testFinished = true;
                    END_MEASURE
                }
                
                template<typename P1, typename P2, typename P3>
                DWORD RunConstructor(P1 p1, P2 p2, P3 p3)
                {
                    if(proband)
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                    proband = false;
                    BEGIN_MEASURE
                        amounts[testOrder[PHASEN::construction]] = constructProband(&proband,p1,p2,p3);
                        Run();
                        amounts[testOrder[PHASEN::deletion]] = deleteProband();
                        proband = NULL;
                        testFinished = true;
                    END_MEASURE
                }

                
                // Measurement functions:
                ////////////////////////////////////////////////////
                
                // perform and measure the next testphase 
                DWORD performPhase(void)
                { BEGIN_MEASURE
                        tests[testOrder[currentPhase]](this, (void**)&testData);
                  END_MEASURE }

                //measure deleting the probant:
                DWORD deleteProband(void)
                { BEGIN_MEASURE
                        delete proband;
                  END_MEASURE }

                // call's and measures destruction of a probant 
                // which would be wrapped in a test-container 
                DWORD deleteInContainer(void)
                { BEGIN_MEASURE
                        proband->~CONTAINER();
                  END_MEASURE }

                // runs and measures all test phasen ordered by testOrtder-values:          
                void Run(void)
                {
                    currentPhase = 0;
                    while(++currentPhase < PHASEN::deletion)
                        amounts[testOrder[currentPhase]] = performPhase();
                }

                //calls the setable itaration function
                void performforeach(T* obj, void* data)
                {
                    iteration(obj, copy<E>, (E*)data);
                }

                // get test's results when finished:
                char* getResult(PHASEN phase)
                {
                    if(testFinished)
                    {
                        sprintf_s<128>( txtOutBuffer, "%s: %f ms, %i ticks\n",
                                 testNames[testOrder[phase]].c_str(),
                                 ToMilliSeconds(amounts[testOrder[phase]]),
                                 amounts[testOrder[phase]] );

                        if(printResults)
                            printf(txtOutBuffer);
                    }
                    return &txtOutBuffer[0];
                }
            };
        }
    }
#endif
