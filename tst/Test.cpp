//#include <glm\glm.hpp>
#include <vector>

#include "../inc/ControlledValues.h"
#include "../inc/StepLists.h"
#include "Test.h"
#include "WaveFormat.hpp"
#include "WaveFileReader.hpp"
#include "WaveFileWriter.hpp"
#include "FloatAudioCompressor.hpp"
#include "ControlerTestContainer.hpp"


namespace stepflow{ namespace Tests{
const int SIZE = 8192;
}}

LARGE_INTEGER localTimer = LARGE_INTEGER();
LARGE_INTEGER* getTimer(void)
{ return &localTimer; }

bool stepflow::Tests::iTestTimer::isInitialized = false;
double stepflow::Tests::iTestTimer::timeFactor = 0;
DWORD stepflow::Tests::iTestTimer::ticksPerSecond = 0;

void stepflow::Tests::iTestTimer::InitializeTimer(void)
{
    QueryPerformanceFrequency(&localTimer);
    ticksPerSecond = localTimer.QuadPart;
    timeFactor = (1000.f / ticksPerSecond);
}



using namespace stepflow;
using namespace stepflow::Tests;

void initTestTimer(void)
{
    iTestTimer::InitializeTimer();
    printf("Timer Initialized: %i ticks per second.",
        iTestTimer::getTicksPerSecond());
}

template<class ProbandType, typename CtorP1, typename DataType, const int DATASIZE, const int NUMBER_OF_ACTIONS>
TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>* CreateTestCaseFunction(bool atOnce, DataType* dataContainer=NULL)
{
    typedef TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS> Test;
    Test* testCase = new TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>(atOnce);

    if(dataContainer)
        testCase->SetCheckDataBuffer(dataContainer, DATASIZE);
    else
        testCase->SetCheckDataBuffer(DATASIZE);
//  testCase->Construct(atOnce);
    return testCase;
}

#define CreateTestCase(probtype,ctortype,TESTCOUNT,datatype,DATASIZE,buffer,createAtOnce) typedef TestCase<probtype,datatype,TESTCOUNT> ADD_SUFFIX(probtype,TestCase); \
        ADD_SUFFIX(probtype,TestCase) *ADD_SUFFIX(probtype,Test) = CreateTestCaseFunction<probtype,ctortype,datatype,DATASIZE,TESTCOUNT>(createAtOnce,buffer)

template<class ProbandType, typename DataType, const int NUMBER_OF_ACTIONS>
DataType* RunTestCase(TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>* testCase)
{
    typedef TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS> TEST;
    DWORD totalTime = 0;
    if(!testCase->dontConstruct()) {
        totalTime = testCase->RunStandardConstructor();
    } else
        totalTime = testCase->RunConstructorless();

    printf("\n\n\nResults for: %s\n", typeid(ProbandType).name());
    printf("total amount: %i ticks = %f ms\n\n",
        totalTime, testCase->ToMilliSeconds(totalTime));

    for(TEST::PHASEN phase = TEST::PHASEN(0);
        phase < TEST::PHASEN::anzahl;
        phase = TEST::PHASEN(phase + 1))
        testCase->getResult(phase);

    DataType* testData = testCase->detachData();
    delete testCase;
    return testData;
}

template<class ProbandType, typename DataType, const int NUMBER_OF_ACTIONS, typename CtorP1>
DataType* RunTestCase(TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS>* testCase, CtorP1 ctorP1)
{
    typedef TestCase<ProbandType, DataType, NUMBER_OF_ACTIONS> TEST;
    DWORD totalTime = 0;
    if(!testCase->dontConstruct()) {
        testCase->SetConstructionParameter(&ctorP1);
        totalTime = testCase->ConstructAndRunTests<CtorP1>();
    } else
        totalTime = testCase->RunConstructorless();

    printf("\n\n\nResults for: %s\n", typeid(ProbandType).name());
    printf( "total amount: %i ticks = %f ms\n\n",
            totalTime, testCase->ToMilliSeconds(totalTime) );

    for( TEST::PHASEN phase = TEST::PHASEN(0);
         phase < TEST::PHASEN::anzahl;
         phase = TEST::PHASEN(phase + 1) )
        testCase->getResult(phase);

    DataType* testData = testCase->detachData();
    delete testCase;
    return testData;
}







int testLists(void);
int testValues(void);

int main(int argc, char** argv)
{
    initTestTimer();

    char keypress = 0;
    printf("\n\nRun tests for Lists or Values ?\n");

    while(keypress != 'x')
    {
        switch(keypress)
        {
        case 'l':
            keypress = testLists();
            break;
        case 'v':
            keypress = testValues();
            break;
        default:
            printf("[press key: l or v]: ");
            keypress = getchar();
            break;
        }
    }

    printf("\n\npress any key for exit...");
    if(keypress != getchar())
       keypress = getchar();

    return 0;
}


///////////////////////////////////////////////////////
// functions for List-Tests:
///////////////////////////////////////////////////////

// definitions for calls on std::vector
template<typename T, typename E>
void StdVectorIteration(T* list, typename void(*action)(E, E&), E* dest)
{
    int c = 0;
    for(auto it = (*list).begin(); it != (*list).end(); it++)
        action(*it, dest[c++]);
}

template<typename Proband>
void FillStdVector(Proband* test, void** loade)
{
    Proband::CONTAINER* p = &test->getProband();
    Proband::DATA_TYPE* elements = (Proband::DATA_TYPE*)*loade;
    int count = test->getElementCount();
    for(int i = 0; i < count; i++)
        p->push_back(*elements++);
}


template<typename Proband>
void ClearAllDataOnStdVector(Proband* test, void** loade)
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
void FillStepflowList(Proband* test, void** loade)
{
    Proband::CONTAINER* p = &test->getProband();
    Proband::DATA_TYPE* elements = (Proband::DATA_TYPE*)*loade;
    int count = test->getElementCount();
    for(int i = 0; i < count; i++)
        p->Add(*elements++);
}

template<typename Proband>
void ForwardCycle(Proband* test, void** dataBlock)
{
    Proband::CONTAINER* p = &test->getProband();
    Proband::DATA_TYPE* dest = (Proband::DATA_TYPE*)*dataBlock;
    unsigned c = -1;
    for(unsigned id = p->StartCycle(p->Count() / 2); id == p->Cycle(); id = p->Next(id))
        (*p)[id] = dest[++c];
}

template<typename Proband>
void BackwardCycle(Proband* test, void** dataBlock)
{
    Proband::CONTAINER* p = &test->getProband();
    Proband::DATA_TYPE* dest = (Proband::DATA_TYPE*)*dataBlock;
    unsigned c = -1;
    for(unsigned id = p->StartCycle(p->Count() / 2); id == p->Cycle(); id = p->Prev(id))
        (*p)[id] = dest[++c];
}

template<typename Proband>
void iterate(Proband* test, void** testData)
{
    test->performforeach(&test->getProband(), *testData);
}

template<typename Proband>
void ClearAllDataOnStepLists(Proband* test, void** loade)
{
    test->getProband().Clear(false);
}

#ifdef min
#define min_win min
#define max_win max
#undef min
#undef max
#endif

int testLists(void)
{
    int* tData = new int[Tests::SIZE];
    int c = 0;
    for(int i = Tests::SIZE - 1; i >= 0; i--)
        tData[c++] = i;

    typedef List<int, 8192> StepList;
    CreateTestCase(StepList, int, 5, int, Tests::SIZE, tData, false);
    StepListTest->SetIterationFunction(&StepflowListIteration);
    StepListTest->AddTestFunction(&FillStepflowList, "Fill", 1);
    StepListTest->AddTestFunction(&iterate, "Iteration", 2);
    StepListTest->AddTestFunction(&ForwardCycle, "ForwardCycle", 3);
    StepListTest->AddTestFunction(&BackwardCycle, "BackwardCycle", 4);
    StepListTest->AddTestFunction(&ClearAllDataOnStepLists, "Clear", 5);
    RunTestCase(StepListTest, std::numeric_limits<int>::min());// -8192);

    c = 0;
    for(int i = Tests::SIZE - 1; i >= 0; i--)
        tData[c++] = i;
    typedef stepflow::DynamicList<int> DynamicList;

    CreateTestCase(DynamicList, int, 5, int, Tests::SIZE, tData, false);
    DynamicListTest->SetIterationFunction(&StepflowListIteration);
    DynamicListTest->AddTestFunction(&FillStepflowList, "Fill", 1);
    DynamicListTest->AddTestFunction(&iterate, "Iteration", 2);
    DynamicListTest->AddTestFunction(&ForwardCycle, "ForwardCycle", 3);
    DynamicListTest->AddTestFunction(&BackwardCycle, "BackwardCycle", 4);
    DynamicListTest->AddTestFunction(&ClearAllDataOnStepLists, "Clear", 5);
    RunTestCase(DynamicListTest, std::numeric_limits<int>::min());// -8192);

    c = 0;
    for(int i = Tests::SIZE - 1; i >= 0; i--)
        tData[c++] = i;
    typedef std::vector<int> StdVector;

    CreateTestCase(StdVector, int, 3, int, Tests::SIZE, tData, false);
    StdVectorTest->SetIterationFunction(&StdVectorIteration);
    StdVectorTest->AddTestFunction(&FillStdVector, "Fill", 1);
    StdVectorTest->AddTestFunction(&iterate, "Iteration", 2);
    StdVectorTest->AddTestFunction(&ClearAllDataOnStdVector, "Clear", 3);
    delete RunTestCase(StdVectorTest, 0);

    return 'x';
}

///////////////////////////////////////////////////////
// functions for Value-Tests:
///////////////////////////////////////////////////////

template<typename TypeOfTestCase>
void rayCastThroughBuffer(TypeOfTestCase* test,void** testData)
{
	TypeOfTestCase::CONTAINER::CONTROLLER* ray = &test->getProband().value();
	typedef TypeOfTestCase::CONTAINER::CONTROLLER::ctrlType t;
	t* buffer = (t*)*testData;
	ray.SetPin(0,&buffer[0])
	ray.SetPin(1,&buffer[44099]);

	t hit = *ray;
	
	printf("todo\n");
}

template<typename T>
void SwitchMode(T* test,void** testData)
{
    testData[0] = test->getProband().PrepareFor(test->getCurrentPhase()).testData;
}


void SaveToFile(void* data,int dataTypeSize, int samplesCount, const char* filename)
{
    WaveFileWriter waveFile = WaveFileWriter(&filename[0], 44100, dataTypeSize*8, 2, samplesCount*dataTypeSize*2);
    if(dataTypeSize == 4)
    {
        f32* buffer = (f32*)data;
        while(waveFile.WriteFrame(*(buffer++)));
    }
    else if(dataTypeSize == 2)
    {
        s16* buffer = (s16*)data;
        while(waveFile.WriteFrame(*(buffer++)));
    }
}

template<typename T>
void Oscillator(T* test,void** testData)
{
    typedef T::CONTAINER::CONTROLLER::TYPE cT;
    T::CONTAINER::CONTROLLER* val = &test->getProband().Value();
    cT* buffer = (cT*)testData[0];
    const cT* end = buffer + 44100;

    for(cT* dst = &buffer[0]; dst < end; dst++)
        *dst = *val;

    String fileName = ((T::CONTAINER::DataSet*)test->getProband().getCurrentDataSet())->name;
    fileName.append(".wav");

    SaveToFile(&buffer[0], sizeof(cT), 44100, fileName);
}

char EffectTestNumber = 'A';

template<typename T>
void Effect(T* test, void** testData)
{
    typedef T::CONTAINER::CONTROLLER::TYPE cT;
    T::CONTAINER::CONTROLLER* val = &test->getProband().Value();

    String extension("_");
    extension.append(&EffectTestNumber);
    extension.append(".wav");

    String fileName = String("AudioSampleFile")+extension;
    Audio audio = WaveFileReader(fileName).Read();

    s16* src = audio.buffer<s16>();

    if(sizeof(cT) > 2)
    {
        s16 amp = std::numeric_limits<s16>::max();
        for(unsigned i = 0; i < audio.frameCount; i++)
        {
            *val = (cT)src[i] / amp;
            src[i] = (s16)(*val * amp);
        }
    } else {
        for(unsigned i = 0; i < audio.frameCount; i++)
        {
            *val = (cT)src[i];
            src[i] = (s16)*val;
        }
    }


    fileName.clear();
    fileName.assign(((T::CONTAINER::DataSet*)test->getProband().getCurrentDataSet())->name);
    fileName.append(extension);

    WaveFileWriter(audio,fileName).Save();

    EffectTestNumber++;
}

template<typename T>
void PendelRegulator(T* testRay,void** testData)
{
    //todo...
}

int testValues(void)
{
    float DataBuffer[44100];
    const unsigned NumberOfModes = 4;
    int n = 0;


    s16 max = SHRT_MAX-67;
    s16 min = -max;
    long val = -min;
    s16 move = (s16)((max-min) / (44100 / 441));
    s16* S16Buffer = (s16*)&DataBuffer[0];

    // copy audio data which will be used for Compression tests
    // into the test executable's folder.
    for(char number = 'A'; number <= 'C'; number++)
    {
        String fileName("../tst/testData/AudioSample_");
        fileName = fileName + number;
        fileName.append(".wav");

        Audio audio = WaveFileReader(fileName).Read();

        fileName.assign("AudioSampleFile_");
        fileName = fileName + number;
        fileName.append(".wav");

        WaveFileWriter(audio,fileName).Save();
    }

    // Test case: Controlled<signed short> value in control modes: Cycle, PingPong, (UserMode)Sinus, Compress
    typedef ControlerTestContainer<s16, 44100, NumberOfModes, stepflow::SineControlled<s16>> Signed16bit;
    CreateTestCase(Signed16bit, s16, NumberOfModes * 2 + 2, s16, 44100, nullptr, true);

    Signed16bitTest->getProband().AddModeSwith("Saw16bit", min, max, move, 0, CtrlMode::Cycle);
    Signed16bitTest->getProband().setTestData("Saw16bit", S16Buffer);
    Signed16bitTest->AddTestFunction(&SwitchMode, "Switch Saw16bit", ++n);
    Signed16bitTest->AddTestFunction(&Oscillator, "fill 16bit Saw", ++n);

    Signed16bitTest->getProband().AddModeSwith("Tri16bit", min, max, move, 0, CtrlMode::PingPong);
    Signed16bitTest->getProband().setTestData("Tri16bit", S16Buffer);
    Signed16bitTest->AddTestFunction(&SwitchMode, "Switch Tri16bit", ++n);
    Signed16bitTest->AddTestFunction(&Oscillator, "fill 16bit Triangle", ++n);

    Signed16bitTest->getProband().AddModeSwith<SineControlled<s16>>("Sin16bit", min, max, move, 0);
    Signed16bitTest->getProband().setTestData("Sin16bit", S16Buffer);
    Signed16bitTest->AddTestFunction(&SwitchMode, "Switch Sin16bit", ++n);
    Signed16bitTest->AddTestFunction(&Oscillator, "fill 16bit Sinus", ++n);

    String name16bit("Compress16bit");
    Signed16bitTest->getProband().AddModeSwith(name16bit, (s16)(max*0.75), 100, 75, 0, CtrlMode::Compress);
    Signed16bitTest->getProband().setTestData(name16bit, S16Buffer);
    Signed16bitTest->AddTestFunction(&SwitchMode, String("Switch " + name16bit), ++n);
    Signed16bitTest->AddTestFunction(&Effect, String("Effect " + name16bit + " data-A"), ++n);
    Signed16bitTest->AddTestFunction(&Effect, String("Effect " + name16bit + " data-B"), ++n);
    Signed16bitTest->AddTestFunction(&Effect, String("Effect " + name16bit + " data-C"), ++n);

    RunTestCase(Signed16bitTest);


    // Test case: Controlled<float> value in control modes: Cycle, PingPong, (UserMode)Sinus, (UserMode)FloatCompressor
    typedef ControlerTestContainer<f32, 44100, NumberOfModes,SineControlled<f32>,FloatAudioCompressor> tCfloat;
    CreateTestCase(tCfloat, f32, 2 * NumberOfModes+2, f32, 44100, nullptr, true);

    n = 0;
    f32 maxF =  1;
    f32 minF = -1;
    f32 Mover = (((maxF-minF) / 44100) * 441);

    tCfloatTest->getProband().AddModeSwith("Triangle", minF, maxF, Mover, 0, CtrlMode::PingPong);
    tCfloatTest->getProband().setTestData("Triangle", &DataBuffer[0]);
    tCfloatTest->AddTestFunction(&SwitchMode, "Switch Triangle", ++n);
    tCfloatTest->AddTestFunction(&Oscillator, "Fill Triangle", ++n);

    tCfloatTest->getProband().AddModeSwith("Sawstack", minF, maxF, Mover, 0, CtrlMode::Cycle);
    tCfloatTest->getProband().setTestData("Sawstack", &DataBuffer[0]);
    tCfloatTest->AddTestFunction(&SwitchMode, "Switch Sawstack", ++n);
    tCfloatTest->AddTestFunction(&Oscillator, "Fill Sawstack", ++n);

    tCfloatTest->getProband().AddModeSwith<SineControlled<f32>>("Sinus", minF, maxF, Mover, 0);
    tCfloatTest->getProband().setTestData("Sinus", &DataBuffer[0]);
    tCfloatTest->AddTestFunction(&SwitchMode, "Switch Sinus", ++n);
    tCfloatTest->AddTestFunction(&Oscillator, "Fill Sinus", ++n);


    // For this test I wrote some ControllerUserMode (FloatAudioCompressor.h) to show some kind of compression effect
    // which not would be type independent anymore, but which should be usable for compressing audio without
    // adding such big amount of distortion to the signal as the type-independent Modes will do indeed.
    // It has ability to "fade" in/out compression over distant attack/release time. (here 100 samples is set)
    String name("CompressorWithAttackAndRelease");
    EffectTestNumber = 'A';
    tCfloatTest->getProband().AddModeSwith<FloatAudioCompressor>(name, 0.7, 0.25, 0.75, 0, CtrlMode::USERMODE, false);
    tCfloatTest->getProband().setTestData(name, &DataBuffer[0]);
    tCfloatTest->AddTestFunction(&SwitchMode, String("Switch ")+name, ++n);
    tCfloatTest->AddTestFunction(&Effect, String("Effect "+name+" data-A"), ++n);
    tCfloatTest->AddTestFunction(&Effect, String("Effect "+name+" data-B"), ++n);
    tCfloatTest->AddTestFunction(&Effect, String("Effect "+name+" data-C"), ++n);
    RunTestCase(tCfloatTest, nullptr);

    return 'x';
}

#ifdef min_win
#undef min
#undef max
#define min min_win
#define max max_win
#undef min_win
#undef max_win
#endif
