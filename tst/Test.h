#ifndef _PERFORMANCES_
#define _PERFORMANCES_
#include "../inc/ControlledValues.h"
#include "../inc/StepLists.h"
#include <vector>
#include <Windows.h>
#include <iostream>

//#include "half.h"

LARGE_INTEGER* getTimer(void);

#define START_CLOCK(timerName) LARGE_INTEGER* tmr = getTimer(); \
							   QueryPerformanceCounter(tmr); \
							   DWORD timerName = tmr->QuadPart

#define RETURN_CLOCK(timerName) QueryPerformanceCounter(tmr); \
								return tmr->QuadPart - timerName; \


#define BEGIN_MEASURE START_CLOCK(measure);
#define END_MEASURE  RETURN_CLOCK(measure);

template<typename E>
void copy(E src, E& dst)
{
	dst = src;
}


namespace stepflow{
	namespace tests{
		namespace performances{

			typedef unsigned char		i8;
			typedef unsigned short		i16;
			typedef unsigned			i32;
			typedef unsigned long long	i64;

			typedef char				s8;
			typedef short				s16;
			typedef int					s32;
			typedef long long			s64;

			typedef long double			d64;
			typedef double				f64;
			typedef float				f32;
	//		typedef half                f16;

			template<typename T> DWORD
				constructProband(T** object)
			{ BEGIN_MEASURE
					*object = new T();
				END_MEASURE	}

			template<typename T, typename P> DWORD
				constructProband(T** object, P parameter)
			{ BEGIN_MEASURE
					*object = new T(parameter);
				END_MEASURE }

			template<typename T, typename P1, typename P2=P1> DWORD
				constructProband(T** object, P1 para1, P2 para2)
			{ BEGIN_MEASURE
					*object = new T(para1, para2);
				END_MEASURE	}

			//template<typename T, typename P1=void*, typename P2=P1, typename P3 = P2> DWORD
			//	ConstructionMeasurement(T** object, P1 para1 = NULL, P2 para2 = NULL, P3 para3 = NULL)
			//{
			//if(para3) {
			//  BEGIN_MEASURE
			//	*object = new T(para1, para2, para3);
			//	END_MEASURE	}
			//else if(para2) {
			//  BEGIN_MEASURE
			//	 *object = new T(para1, para2);
			//	END_MEASURE	} 
			//else if(para1) {
			//  BEGIN_MEASURE
			//	 *object = new T(para1);
			//	END_MEASURE	}
			//else {
			//  BEGIN_MEASURE
			//	 *object = new T();
			//	END_MEASURE	}
			//}

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

			template<typename T, typename E, const int TESTS_COUNT>
			class TestCase : public iTestTimer
			{
			public:
				enum PHASEN
				{ construction = 0,	deletion = TESTS_COUNT + 1,
				anzahl=TESTS_COUNT+2};
				typedef T CONTAINER;
				typedef E DATA_TYPE;
				typedef void(*ElementFunc)(E, E&);
				typedef void(*PerformAction)(TestCase*, void*);
				typedef void(*IterationFunc)(T*, ElementFunc, E*);

			private:		
				unsigned long long	elementsCount;
				LARGE_INTEGER*		getTimer(void) { return &timer; }
				IterationFunc		iteration;
				const char*			testNames[PHASEN::anzahl];
				void(*              tests[PHASEN::anzahl])(TestCase*, void*);
				DWORD				amounts[PHASEN::anzahl];
				byte				currentPhase;
				bool				printResults;
				bool				testFinished;
				char				txtOutBuffer[128];
				int					testOrder[PHASEN::anzahl];
				E*					testData;
				T*					proband;
				void*				ConstructionParameterA;
				void*				ConstructionParameterB;

			public:
				TestCase(void)
				{
					proband = false;
					printResults = true;
					testFinished = false;
					currentPhase = elementsCount = 0;
					sprintf(&txtOutBuffer[0],"test not finished yet! :\\\n");
			
					for(int i = 0; i < PHASEN::anzahl; i++)
						amounts[i] = 0;

					testOrder[PHASEN::construction] = PHASEN::construction;
					testNames[PHASEN::construction] = "Constructon";
					testOrder[PHASEN::deletion] = PHASEN::deletion;
					testNames[PHASEN::deletion] = "Deletion";
				};

				virtual ~TestCase(void)
				{ 
				  if(proband)
					delete proband;
				  if(testData)
					delete testData; 
				};

				
				void SetContainerSize(unsigned long long newSize)
					 { elementsCount = newSize; }

				E* SetCheckDataBuffer(int elementsCount)
				   { return SetCheckDataBuffer(new E[elementsCount], elementsCount); }

				E* SetCheckDataBuffer(E* allocatedBlock, int elementsCount)
				   { this->elementsCount = elementsCount;
					   return testData = allocatedBlock; }

				void SetIterationFunction(IterationFunc iterFunc)
				{
					iteration = iterFunc;
				}

				template<typename CtorP1,typename CtorP2=CtorP1>
				void SetConstructionParameter(CtorP1* argumOne, CtorP2* argumTwo = NULL)
				{
					this->ConstructionParameterA = argumOne;
					this->ConstructionParameterB = argumTwo;
				}

				void AddTestFunction(PerformAction action, const char* name, int order)
				{
					if(currentPhase < PHASEN::anzahl)
					{
						testOrder[++currentPhase] = order;
						tests[order] = action;
						testNames[order] = (char*)name;
					}
				}

				template<typename CtorP1,typename CtorP2=CtorP1>
				DWORD ConstructAndRunTests(void)
				{
					int numGum  = (((CtorP1*)this->ConstructionParameterA) != NULL);
						numGum += (((CtorP2*)this->ConstructionParameterB) != NULL);
					
						switch(numGum)
						{
						case 0:
							return RunStandardConstructor();
						case 1:
							return RunConstructor(*(CtorP1*)this->ConstructionParameterA);
					/*	case 2:
							if(proband)
								amounts[testOrder[PHASEN::deletion]] = deleteProband();
							proband = false;
							BEGIN_MEASURE
								amounts[testOrder[PHASEN::construction]] = 
											                constructProband( &proband, 
															*(CtorP1*)ConstructionParameterA, 
															*(CtorP2*)ConstructionParameterB );
								Run();
								amounts[testOrder[PHASEN::deletion]] = deleteProband();
								proband = false;
								testFinished = true;
							END_MEASURE 
					*/
						}
				}

				template<typename P1>
				DWORD RunConstructor(P1 parameter)
				{ if(proband)
					amounts[testOrder[PHASEN::deletion]] = deleteProband();
				  proband = false;
				  BEGIN_MEASURE
					amounts[testOrder[PHASEN::construction]] = constructProband<T,P1>(&proband, parameter);
						Run();
					amounts[testOrder[PHASEN::deletion]] = deleteProband();
					proband = false;
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
					proband = false;
					testFinished = true;
				  END_MEASURE }

				unsigned long getElementCount(void)
				{
					return elementsCount;
				}

				DWORD GetTicksPerSecond(void)
				{
					return ticksPerSecond;
				}
				double ToMilliSeconds(DWORD ticks)
				{
					return (double)(ticks * timeFactor);
				}

				CONTAINER& getProband(void)
				{
					return *proband;
				}

				char* getResult(PHASEN phase)
				{
					if(testFinished)
					{
						sprintf( &txtOutBuffer[0], "%s: %f ms, %i ticks\n",
							     testNames[testOrder[phase]],
							     ToMilliSeconds(amounts[testOrder[phase]]),
							     amounts[testOrder[phase]] );

						if(printResults)
							printf(txtOutBuffer);
					}
					return &txtOutBuffer[0];
				}

				DWORD deleteProband(void)
				{ BEGIN_MEASURE
						delete proband;
					END_MEASURE	}

				DWORD performPhase(void)
				{ BEGIN_MEASURE
						tests[testOrder[currentPhase]](this, testData);
					END_MEASURE	}

				void Run(void)
				{ currentPhase = 0;
					while(++currentPhase < PHASEN::deletion)
						amounts[testOrder[currentPhase]] = performPhase(); }
				

				void performforeach(T* obj, void* data) {
					iteration(obj, copy<E>, (E*)data);
				}
			};
		}
	}
}
#endif
