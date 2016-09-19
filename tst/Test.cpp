#include "Test.h"

namespace stepflow{ namespace tests{ namespace performances{
const int SIZE = 8192;
}}}

LARGE_INTEGER localTimer = LARGE_INTEGER();
LARGE_INTEGER* getTimer(void)
{ return &localTimer; }

bool stepflow::tests::performances::iTestTimer::isInitialized = false;
double stepflow::tests::performances::iTestTimer::timeFactor = 0;
DWORD stepflow::tests::performances::iTestTimer::ticksPerSecond = 0;

void stepflow::tests::performances::iTestTimer::InitializeTimer(void)
{
	QueryPerformanceFrequency(&localTimer);
	ticksPerSecond = localTimer.QuadPart;
	timeFactor = (1000.f / ticksPerSecond);
}

using namespace stepflow::tests::performances;


// difinitions for calls on std::vector
template<typename T, typename E>
void StdVectorIteration(T* list, typename void(*action)(E, E&), E* dest)
{
	int c = 0;
	for(auto it = (*list).begin(); it != (*list).end(); it++)
		action(*it, dest[c++]);
}

template<typename Proband>
void FillStdVector(Proband* test, void* loade)
{
	Proband::CONTAINER* p = &test->getProband();
	Proband::DATA_TYPE* elements = (Proband::DATA_TYPE*)loade;
	int count = test->getElementCount();
	for(int i = 0; i < count; i++)
		p->push_back(*elements++);
}


template<typename Proband>
void ClearAllDataOnStdVector(Proband* test, void* loade)
{
	test->getProband().clear();
}



// Definitions for calls on stepflow::ListTypes

template<typename T, typename E>
void StepflowListIteration(T* list, typename void(*action)(E, E&), E* dest)
{
	int c = 0;
	for(unsigned id = (*list).First(); id != (*list).Last(); id = (*list).Next(id))
		action((*list)[id], dest[c++]);
}

template<typename Proband>
void FillStepflowList(Proband* test, void* loade)
{
	Proband::CONTAINER* p = &test->getProband();
	Proband::DATA_TYPE* elements = (Proband::DATA_TYPE*)loade;
	int count = test->getElementCount();
	for(int i = 0; i < count; i++)
		p->Add(*elements++);
}

template<typename Proband>
void ForwardCycle(Proband* test, void* dataBlock)
{
	Proband::CONTAINER* p = &test->getProband();
	Proband::DATA_TYPE* dest = (Proband::DATA_TYPE*)dataBlock;
	unsigned c =-1;
	for(unsigned id = p->StartCycle(p->Count() / 2); id == p->Cycle(); id = p->Next(id))
		(*p)[id] = dest[++c];
}

template<typename Proband>
void BackwardCycle(Proband* test, void* dataBlock)
{
	Proband::CONTAINER* p = &test->getProband();
	Proband::DATA_TYPE* dest = (Proband::DATA_TYPE*)dataBlock;
	unsigned c = -1;
	for(unsigned id = p->StartCycle(p->Count() / 2); id == p->Cycle(); id = p->Prev(id))
		(*p)[id] = dest[++c];
}

template<typename Proband>
void iterate(Proband* test,void* testData)
{
	test->performforeach(&test->getProband(), testData);
}

template<typename Proband>
void ClearAllDataOnStepLists(Proband* test, void* loade)
{
	test->getProband().Clear(false);
}



template<class ProbandType, typename DataType, typename CtorP1, const int NUMBER_OF_ACTIONS>
TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>* CreateTestCaseFunction()
{
	typedef TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS> Test;
	Test* testCase = new TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>();
	DataType* tData = testCase->SetCheckDataBuffer(new DataType[stepflow::tests::performances::SIZE], stepflow::tests::performances::SIZE);
	testCase->SetContainerSize(stepflow::tests::performances::SIZE);

	int c = 0;
	for(int i = stepflow::tests::performances::SIZE - 1; i >= 0; i--)
		tData[c++] = i;

	return testCase;
}

#define CreateTestCase(probtype,datatype,ctortype,COUNT) typedef TestCase<probtype,datatype,COUNT> ADD_SUFFIX(probtype,TestCase); \
														 ADD_SUFFIX(probtype,TestCase) *ADD_SUFFIX(probtype,Test) = CreateTestCaseFunction<probtype,datatype,ctortype,COUNT>()

template<class ProbandType, typename DataType, const int NUMBER_OF_ACTIONS, typename CtorP1>
bool Evaluiere(TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>* testCase,CtorP1 ctorP1)
{
	typedef TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS> TEST;

	testCase->SetConstructionParameter(&ctorP1);
	DWORD totalTime = testCase->ConstructAndRunTests<CtorP1>();

	printf("\n\n\nResults for: %s\n", typeid(ProbandType).name());
	printf( "total amount: %i ticks = %f ms\n\n",
			totalTime, testCase->ToMilliSeconds(totalTime) );

	for( TEST::PHASEN phase = TEST::PHASEN(0);
		 phase < TEST::PHASEN::anzahl;
		 phase = TEST::PHASEN(phase + 1) )
		testCase->getResult(phase);

	delete testCase;
	return true;
}

#ifdef min
#define min_win min
#undef min
#endif

int main(int argc, char** argv)
{
	iTestTimer::InitializeTimer();
	printf( "Timer Initialized: %i ticks per second.",
		    iTestTimer::getTicksPerSecond() );

	typedef stepflow::List<int, 8192> StepList;
	CreateTestCase(StepList, int, int, 5);
	StepListTest->SetIterationFunction(&StepflowListIteration);
	StepListTest->AddTestFunction(&FillStepflowList, "Fill", 1);
	StepListTest->AddTestFunction(&iterate, "Iteration", 2);
	StepListTest->AddTestFunction(&ForwardCycle, "ForwardCycle", 3);
	StepListTest->AddTestFunction(&BackwardCycle, "BackwardCycle", 4);
	StepListTest->AddTestFunction(&ClearAllDataOnStepLists, "Clear", 5);
	Evaluiere(StepListTest, std::numeric_limits<int>::min());// -8192);

	typedef stepflow::DynamicList<int> DynamicList;
	CreateTestCase(DynamicList, int, int, 5);
	DynamicListTest->SetIterationFunction(&StepflowListIteration);
	DynamicListTest->AddTestFunction(&FillStepflowList, "Fill", 1);
	DynamicListTest->AddTestFunction(&iterate, "Iteration", 2);
	DynamicListTest->AddTestFunction(&ForwardCycle, "ForwardCycle", 3);
	DynamicListTest->AddTestFunction(&BackwardCycle, "BackwardCycle", 4);
	DynamicListTest->AddTestFunction(&ClearAllDataOnStepLists, "Clear", 5);
	Evaluiere(DynamicListTest, std::numeric_limits<int>::min());// -8192);

	typedef std::vector<int> StdVector;
	CreateTestCase(StdVector, int, int, 3);
	StdVectorTest->SetIterationFunction(&StdVectorIteration);
	StdVectorTest->AddTestFunction(&FillStdVector, "Fill", 1);
	StdVectorTest->AddTestFunction(&iterate, "Iteration", 2);
	StdVectorTest->AddTestFunction(&ClearAllDataOnStdVector, "Clear", 3);
	Evaluiere(StdVectorTest, 0);

	printf("\n\npress any key for exit...");
	std::getchar();

	return 0;
}


