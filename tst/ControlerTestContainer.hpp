/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ControlerTestContainer.hpp                 ||
||     Author:    Kalle                                      ||
||     Generated: 23.09.2016                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _ControlerTestContainer_hpp_
#define _ControlerTestContainer_hpp_
#include "..\inc\ControlledValues.h"
#include "..\inc\StepLists.h"


	template<typename cT, const unsigned TEST_DATA_SIZE, const unsigned NUM_OF_MODES = 0,
		typename uMode1 =  void*, typename uMode2 = uMode1,
		typename uMode3 = uMode2, typename uMode4 = uMode3,
		typename uMode5 = uMode4, typename uMode6 = uMode5, 
		typename uMode7 = uMode6, typename uMode8 = uMode7>
    class ControlerTestContainer
    {
	public:
		typedef stepflow::Controlled<cT> CONTROLLER;
		struct DataSet
		{
			cT MIN;
			cT MAX;
			cT MOVE;
			cT VALUE;
			stepflow::CtrlMode MODE;
			bool CheckAtGet, Active;
			const char* name;
			cT* testData;
		};
		
		static const unsigned getNumberOfModes(void)
							  { return NUM_OF_MODES; }

		typedef void(*ModeSwitch)(CONTROLLER&,DataSet*);
		typedef uMode1 Mode1;
		typedef uMode2 Mode2;
		typedef uMode3 Mode3;
		typedef uMode4 Mode4;
		typedef uMode5 Mode5;
		typedef uMode6 Mode6;
		typedef uMode7 Mode7;
		typedef uMode8 Mode8;





	private:
		CONTROLLER value;
		DataSet dataSets[NUM_OF_MODES];
		size_t uModeTypeList[NUM_OF_MODES];
		size_t uModeType[NUM_OF_MODES];
		void(ControlerTestContainer::*UsermodeSwitch[NUM_OF_MODES])(CONTROLLER&, DataSet*);
		
		int current;
		bool ready,running;

		template<typename uMode=ClambController<cT>>
		void SwitchMode(CONTROLLER& test, DataSet* data)
		{
			if(data->MODE < CtrlMode::USERMODE)
				test.SetUp(data->MIN, data->MAX, data->VALUE, data->MOVE, data->MODE);
			else
				test.SetUserMode<uMode>(data->MIN, data->MAX, data->VALUE, data->MOVE);

			test.SetCheckAtGet(data->CheckAtGet);
			test.ControllerActive = data->Active;
		}

		bool switchNext(void)
			 {
				 if((!running)&&(!ready)) {
					 return ++current < NUM_OF_MODES;
				 } else {
					 running = ready ? !(ready = current = 0) 
									 : ++current < NUM_OF_MODES;
					 size_t uT = uModeTypeList[current];
					 
					 if(uT == uModeType[0])
						SwitchMode(value, &dataSets[current]);
					 else if(uT == uModeType[1])
						SwitchMode<uMode1>(value, &dataSets[current]);
					 else if(uT == uModeType[2])
						SwitchMode<uMode2>(value, &dataSets[current]);
				else if(uT ==uModeType[3])
						SwitchMode<uMode3>(value, &dataSets[current]);
				else if(uT ==uModeType[4])
						SwitchMode<uMode4>(value, &dataSets[current]);
				else if(uT ==uModeType[5])
						SwitchMode<uMode5>(value, &dataSets[current]);
				else if(uT ==uModeType[6])
						SwitchMode<uMode6>(value, &dataSets[current]);
				else if(uT ==uModeType[7])
						SwitchMode<uMode7>(value, &dataSets[current]);
				else if(uT ==uModeType[8])
						SwitchMode<uMode8>(value, &dataSets[current]);
				else
					(this->*UsermodeSwitch[current])(this->value, &this->dataSets[current]);
					 
					 return running;
				 }
			 }
    public:
		

		ControlerTestContainer(float* initVal = nullptr)
		{
			uModeType[0] = typeid(void).hash_code();
			uModeType[1] = typeid(uMode1).hash_code();
			uModeType[2] = typeid(uMode2).hash_code();
			uModeType[3] = typeid(uMode3).hash_code();
			uModeType[4] = typeid(uMode4).hash_code();
			uModeType[5] = typeid(uMode5).hash_code();
			uModeType[6] = typeid(uMode6).hash_code();
			uModeType[7] = typeid(uMode7).hash_code();
			uModeType[8] = typeid(uMode8).hash_code();

			current = -1;
			ready = running = false; 
			DataSet ds = { -1, 1,0.001,0 ,stepflow::CtrlMode::PingPong, true, true, "Begin" };
			SwitchMode(value,&ds);
		};

		

		virtual ~ControlerTestContainer(void)
				 {
			//		 for(int i = 0; i < NUM_OF_MODES; i++)
			//			 dataSets[i].testData = NULL;
			//			value.~Controlled<cT>();
				 };

				 template<typename uMode=void>
				 void	 AddModeSwith(const char* nam, cT min, cT max,cT mov, cT val,
					stepflow::CtrlMode mod = stepflow::CtrlMode::USERMODE,
					bool chk = true, bool act = true ) {
					if(!ready && switchNext()) {
						dataSets[current] = { min, max, mov, val, mod, chk, act, nam };
						UsermodeSwitch[current] = &ControlerTestContainer::SwitchMode;
						uModeTypeList[current] = typeid(uMode).hash_code();
						ready = current == (NUM_OF_MODES - 1);
					} else ready = true;
				 }

		DataSet& PrepareForNext(void)
				{
					if(ready || running)
					{
						switchNext();
						return dataSets[current];
					}
				}

		DataSet& PrepareFor(const char* nextTestPhase)
		{
			int search = 0;
			bool test = std::strcmp(dataSets[search].name, &nextTestPhase[7])!=0;
			while(test && search < NUM_OF_MODES)
				test = std::strcmp(dataSets[++search].name, &nextTestPhase[7]) != 0;

			if(search == NUM_OF_MODES)
			{
				DataSet r = { 0, 0, 0, 0, stepflow::CtrlMode(0), false, false, "UnknownTestPhase" };
				return r;
			}
			current = search - 1;
			switchNext();
			return dataSets[current];
		}

		CONTROLLER&	Value() { return value; }

		void* getCurrentDataSet(void)
		{
			return (void*)&dataSets[current];
		}

		cT* testData()
		{
			return dataSets[current].testData;
		}

		int indexOfName(const char* name)
		{
			for(int i = 0; i < NUM_OF_MODES; i++)
				if(!strcmp(name, dataSets[i].name))
					return i;
			return -1;
		}

		void setTestData(const char* name,cT* buffer)
			 {
				 int index = indexOfName(name);
				 if(index>=0)
					dataSets[index].testData = buffer;
			 }
	    // public:
	//		friend class stepflow::ListBase<cT>;
//			friend class stepflow::ListBase<cT>::Utility;
			
	};



#endif
