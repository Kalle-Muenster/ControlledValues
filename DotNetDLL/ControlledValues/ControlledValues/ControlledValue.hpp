/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ControlledValue.hpp                        ||
||     Author:    Kalle Muenster                             ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _ControlledValue_hpp_
#define _ControlledValue_hpp_

#include <limits>
#include <ControlledValues.h>

template<typename cT = System::Single>
class DotNetDelegateMode : public stepflow::UserModeControl<cT>
{
protected:
	typedef cT(*functionator)(cT%, cT%, cT%, cT%);
	functionator functor;
	void Init(void) {
	}
public:
	virtual cT checkVALUE(cT* value) {
		return functor(*value,
			controller->MIN,
			controller->MAX,
			controller->MOVE);
	}
	void setModeFunction(void* f) {
		functor = (functionator)f;
	}
};

using Marshal = System::Runtime::InteropServices::Marshal;

#define CreateControlledType(vTtype) ref class vTtype : public ValueController<Stepflow::vTtype> { \
									   public: vTtype():ValueController<Stepflow::vTtype>() {} \
									   vTtype(vTtype::MODE m):ValueController<Stepflow::vTtype>(m) {} \
									   vTtype(Stepflow::vTtype% refVar):ValueController<Stepflow::vTtype>(refVar) {} \
									   ~vTtype() { ValueController<Stepflow::vTtype>::storeConfiguration(this); delete controller; } \
									   static operator Stepflow::vTtype (vTtype^ cast){ return *cast->controller; } \
									   virtual System::String^ ToString() override	{ return ((Stepflow::vTtype)*controller).ToString(); } \
										}  
namespace Stepflow
{

	typedef System::Byte   Byte;
	typedef System::SByte  Char;
	typedef System::Int16  Short;
	typedef System::UInt16 Sword;
	typedef System::Int32  Int;
	typedef System::UInt32 Unt;
	typedef System::Single Float;
	typedef System::Double Double;
	typedef System::UInt64 QWORD;
	typedef System::UInt32 DWORD;
	typedef System::UInt16 SWORD;


	public ref class ControllerBase abstract
	{
	public:
		enum class MODE : byte
		{
			None = stepflow::CtrlMode::None,       // Indicates the controller not is being active - value will stay as is..
			Invert = stepflow::CtrlMode::Invert,     // INVERT: - The value will stay the same, but will be returned negated when getted.
			Clamp = stepflow::CtrlMode::Clamp,      // CLAMP: - The value will be clamped to MIN/MAX when set.
			Damp = stepflow::CtrlMode::Damp,       // DAMP: - Value behaves like a mass thats being moved through some resistant medium - MOVE controls it's maximum speed reachable, MAX stores it's delta, MIN stores last Value
			Compress = stepflow::CtrlMode::Compress,   // COMPRESS: - If the value's delta-movement becomes over the threshold MIN or lower it's inverse -MIN, it will be amplified by factor MAX (MAX from 0 to 1 will effect reduction, MAX greater 1 effect's amplification)...
			Expand = stepflow::CtrlMode::Expand,     // EXPAND: - If delta-movement is between MIN and -MIN, it will be amplified by factor MAX (supposing MAX was given greater than 1).
			Moderate = stepflow::CtrlMode::Moderate,   // MODERATE: - When delta movement is between MIN and -MIN, it will be Expanded by factor MAX. If it becomes greater then MIN or lower then -MIN, it will be Compressed by factor 1/MAX.
			Cycle = stepflow::CtrlMode::Cycle,      // CYCLE:  - Every time the value is checked, it will be moved by adding MOVE to it.. If it get's greater it's MAX, it will be set back to it's MIN...
			PingPong = stepflow::CtrlMode::PingPong,   // PINGPONG: - Every time the value is checked, it will move by MOVE. when reaches MAX or MIN, MOVE changes it's sign.      
			UseDelegate = stepflow::CtrlMode::USERMODE, // // Indicates that the value is being controlled by some other controller-implemented object.
		};

		virtual void SetCheckAtGet(bool) = 0;
		virtual void SetCheckAtSet(bool) = 0;
		virtual void SetCheckAtGet(void) = 0;
		virtual void SetCheckAtSet(void) = 0;
		virtual void SetReadOnly(bool) = 0;
		virtual System::IntPtr GetTarget(void) = 0;
		virtual void SetPointerToTarget(System::IntPtr) = 0;

	};

	template<typename cT = System::Single>
	public ref class ValueController : public ControllerBase
	{
	public:
		delegate cT UserModeDelegate(cT%,cT%,cT%,cT%);
		typedef cT TYPE;
		[System::FlagsAttribute]
		enum class StateFlags : byte {
			Active = 0x1,
			SetAtGet = 0x2,
			ReadOnly = 0x4,
			UserSettingA = 0x8,
			UserSettingB = 0x10,
			UserSettingC = 0x20,
			UserSettingD = 0x40
		};
		value class Configuration
		{
		public:
			TYPE min, max, mov;
			MODE mod;
			StateFlags flags;
			Configuration(cT Min, cT Max, cT Mov) {
				min = Min;
				max = Max;
				mov = Mov;
				mod = MODE::None;
				flags = StateFlags::Active | StateFlags::SetAtGet | StateFlags::UserSettingA;
			}
		};

//	protected:
//		byte useDelegate = 0;
		
	internal:
		static Configuration stored;
		static void storeConfiguration(ValueController^ copy)
		{
			stored.min = copy->controller->MIN;
			stored.max = copy->controller->MAX;
			stored.mov = copy->controller->MOVE;
			stored.mod = copy->Mode;
			stored.flags = StateFlags(copy->controller->getConfiguration());
		}

		stepflow::Controlled<cT>* controller;
	public:

		virtual void SetUp(Configuration settings)
		{
			this->SetUp(settings.min,
						settings.max,
						settings.mov,
						(*controller),
						settings.mod);

			this->SetCheckAtGet(((int)settings.flags / 2) % 2);
			this->SetReadOnly(((int)settings.flags / 4) % 2);
			this->active = (int)settings.flags % 2;
		}

		ValueController(void)
		{
			controller = new stepflow::Controlled<cT>();
			(*controller).SetUp( std::numeric_limits<cT>::min(),
								 std::numeric_limits<cT>::max(),
								 0, 1, (byte)MODE::None );
		}
		ValueController(Configuration^ copy)
		{
			controller = new stepflow::Controlled<cT>();
			SetUp(copy);
		}
		ValueController(MODE mode)
		{
			controller = new stepflow::Controlled<cT>();
			if (mode == MODE::UseDelegate) {
				(*controller).SetUserMode<DotNetDelegateMode<cT>>( std::numeric_limits<cT>::min(),
																   std::numeric_limits<cT>::max(),
															       0, 0 );
			}
			else
				(*controller).SetUp( std::numeric_limits<cT>::min(),
									 std::numeric_limits<cT>::max(),
									 0, 1, byte(mode) );
		}
		ValueController(ValueController^ copy)
		{
			controller = new stepflow::Controlled<cT>();
			ValueController::storeConfiguration(copy);
			SetUp(stored);
			Value = copy->Value;
		}
		ValueController(cT% variable)
		{
			controller = new stepflow::Controlled<cT>();
			(*controller).SetUp( std::numeric_limits<cT>::min(),
								 std::numeric_limits<cT>::max(),
								 0, 1, (byte)MODE::None );
			pin_ptr<cT> fixed = &variable;
			controller->SetVariable(fixed);
		}

		static operator cT(ValueController^ cast)
		{
			return (*cast->controller);
		}

		virtual Configuration GetData(void) {
			ValueController::storeConfiguration(this);
			return stored;
		}

		virtual void SetUp(cT min, cT max, cT mov, cT val, MODE mod)
		{
			controller->SetUp(min, max, val, mov, byte(mod));
		}

		property MODE Mode {
			MODE get(void)
			{
				return MODE(controller->Mode());
			}
			void set(MODE value)
			{
		/*		if (value == MODE::ControlElement && MODE(controller->Mode() + useDelegate) != MODE::ControlElement) {
					controller->SetUserMode<stepflow::Element<cT>>(controller->MIN, controller->MAX, *controller, controller->MOVE);
					useDelegate = 1;
				}
				else {
					useDelegate = 0;*/
					controller->Mode(byte(value));
			//	}
			}
		}
		property bool active {
			bool get(void)
			{
				return controller->ControllerActive;
			}
			void set(bool value)
			{
				controller->ControllerActive = value;
			}
		}

		property cT Value {
			cT get(void)
			{
				return *controller;
			}
			void set(cT value)
			{
				*controller = value;
			}
		}
		property cT MIN {
			cT get(void)
			{
				return (*controller).MIN;
			}
			void set(cT value)
			{
				(*controller).MIN = value;
			}
		}
		property cT MAX {
			cT get(void)
			{
				return (*controller).MAX;
			}
			void set(cT value)
			{
				(*controller).MAX = value;
			}
		}
		property cT MOV {
			cT get(void)
			{
				return (*controller).MOVE;
			}
			void set(cT value)
			{
				(*controller).MOVE = value;
			}
		}

		virtual void SetMIN(cT setValueToNewMIN)
		{
			controller->SetMIN(setValueToNewMIN);
		}
		virtual void SetMAX(cT setValueToNewMAX)
		{
			controller->SetMAX(setValueToNewMAX);
		}
		virtual void SetMOV(cT setMoveActive)
		{
			controller->SetMOVE(setMoveActive);
		}

		virtual void SetCheckAtGet(bool shoudCheckAtGet) override
		{
			controller->SetCheckAtGet(shoudCheckAtGet);
		}
		virtual void SetCheckAtSet(bool shoudCheckAtSet)  override
		{
			controller->SetCheckAtGet(!shoudCheckAtSet);
		}
		virtual void SetCheckAtGet() override
		{
			controller->SetCheckAtGet(true);
		}
		virtual void SetCheckAtSet() override
		{
			controller->SetCheckAtGet(false);
		}
		virtual void SetReadOnly(bool shouldBeReadOnly)  override
		{
			controller->SetReadOnly(shouldBeReadOnly);
		}

		virtual System::IntPtr GetTarget(void) override
		{
			return System::IntPtr((void*)controller->GetPointerToValue());
		}

		virtual void SetTarget(cT% targetVariable)
		{
			pin_ptr<cT> p = &targetVariable;
			controller->SetVariable(p);
		}

		virtual void UseDelegateFunction(UserModeDelegate^ mD)
		{
			controller->SetUserMode<DotNetDelegateMode<cT>>(Value)
				.setModeFunction(
					Marshal::GetFunctionPointerForDelegate(mD).ToPointer()
				);
		}
/*
		property bool Cycled {
			bool get(void) {
				if (Mode == MODE::ControlElement)
					return controller->GetUserMode<stepflow::Element<cT>>()->cycled();
				else return false;
			}
			void set(bool value) {
				if (Mode == MODE::ControlElement)
					controller->GetUserMode<stepflow::Element<cT>>()->cycled(value);
			}
		}
		property bool ForceUnsigned {
			bool get(void)
			{
				if (Mode == MODE::ControlElement)
					return controller->GetUserMode<stepflow::Element<cT>>()->uForced();
				else return false;
			}
			void set(bool value) {
				if (Mode == MODE::ControlElement)
					controller->GetUserMode<stepflow::Element<cT>>()->uForced(value);
			}
		}
		property cT peak {
			cT get(void) {
				if (Mode == MODE::ControlElement)
					return controller->GetUserMode<stepflow::Element<cT>>()->PEAK;
				else return cT();
			}
			void set(cT value) {
				if (Mode == MODE::ControlElement)
					controller->GetUserMode<stepflow::Element<cT>>()->PEAK = value;
			}
		}
*/
		virtual void SetPointerToTarget(System::IntPtr target) override
		{
			controller->SetVariable((cT*)target.ToPointer());
		}
#if unsafe
		virtual cT* GetPointerToTarget(void)
		{
			return controller->GetPointerToValue();
		}
#endif
	};
}


namespace Stepflow
{

	generic<typename gT> //where gT : System::ValueType
		ref class controlled;

		public ref class Controlled : public ControllerBase
		{
		public:

			CreateControlledType(Float);
			CreateControlledType(Double);
			CreateControlledType(Int);
			CreateControlledType(Unt);
			CreateControlledType(Short);
			CreateControlledType(Sword);
			CreateControlledType(Char);
			CreateControlledType(Byte);
			
			

			generic<typename gT> //where gT : System::ValueType
				static ControllerBase^ Create()
			{
				switch (System::Type::GetTypeCode(gT::typeid))
				{
				case System::TypeCode::Single: return gcnew Float();
				case System::TypeCode::Double: return gcnew Double();
				case System::TypeCode::Int32: return gcnew Int();
				case System::TypeCode::UInt32: return gcnew Unt();
				case System::TypeCode::Int16: return gcnew Short();
				case System::TypeCode::UInt16: return gcnew Sword();
				case System::TypeCode::SByte: return gcnew Char();
				case System::TypeCode::Byte: return gcnew Byte();
				}
			}

		protected:
			generic<typename gT> //where gT : System::ValueType
				static ControllerBase^ Create(ControllerBase^ This)
				{
					switch (System::Type::GetTypeCode(gT::typeid))
					{
					case System::TypeCode::Single: ((Float^)This)->controller = new stepflow::Controlled<float>(); return This;
					case System::TypeCode::Double: ((Double^)This)->controller = new stepflow::Controlled<double>(); return This;
					case System::TypeCode::Int32:  ((Int^)This)->controller = new stepflow::Controlled<int>(); return This;
					case System::TypeCode::UInt32: ((Unt^)This)->controller = new stepflow::Controlled<unsigned>(); return This;
					case System::TypeCode::Int16:  ((Short^)This)->controller = new stepflow::Controlled<short>(); return This;
					case System::TypeCode::UInt16: ((Sword^)This)->controller = new stepflow::Controlled<SWORD>(); return This;
					case System::TypeCode::SByte:  ((Char^)This)->controller = new stepflow::Controlled<System::SByte>(); return This;
					case System::TypeCode::Byte:   ((Byte^)This)->controller = new stepflow::Controlled<byte>(); return This;
					}
				}
		};


		generic<typename gT> //where gT:System::ValueType
			public ref class controlled : public Controlled
			{
			public:
				controlled()
				{
					Create<gT>(this);
				}

				static operator controlled ^ (ControllerBase^ cast)
				{
					switch (System::Type::GetTypeCode(gT::typeid))
					{
					case System::TypeCode::Single: return (Float^)cast;
					case System::TypeCode::Double: return (Double^)cast;
					case System::TypeCode::Int32: return  (Int^)cast;
					case System::TypeCode::UInt32: return (Unt^)cast;
					case System::TypeCode::Int16: return  (Short^)cast;
					case System::TypeCode::UInt16: return (Sword^)cast;
					case System::TypeCode::SByte: return  (Char^)cast;
					case System::TypeCode::Byte: return   (Byte^)cast;
					}
				}

				property gT Value {
					gT get(void)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single: return (gT)(System::Single) ((Float^)this)->Value;
						case System::TypeCode::Double: return (gT)(System::Double) ((Double^)this)->Value;
						case System::TypeCode::Int32: return  (gT)(System::Int32)  ((Int^)this)->Value;
						case System::TypeCode::UInt32: return (gT)(System::UInt32) ((Unt^)this)->Value;
						case System::TypeCode::Int16: return  (gT)(System::Int16)  ((Short^)this)->Value;
						case System::TypeCode::UInt16: return (gT)(System::UInt16) ((Sword^)this)->Value;
						case System::TypeCode::SByte: return  (gT)(System::SByte)  ((Char^)this)->Value;
						case System::TypeCode::Byte: return   (gT)(System::Byte)   ((Byte^)this)->Value;
						}
					}
					void set(gT value)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single: ((Float^)this)->Value = (System::Single)value;
						case System::TypeCode::Double: ((Double^)this)->Value = (System::Double)value;
						case System::TypeCode::Int32:  ((Int^)this)->Value = (System::Int32) value;
						case System::TypeCode::UInt32: ((Unt^)this)->Value = (System::UInt32)value;
						case System::TypeCode::Int16:  ((Short^)this)->Value = (System::Int16) value;
						case System::TypeCode::UInt16: ((Sword^)this)->Value = (System::UInt16)value;
						case System::TypeCode::SByte:  ((Char^)this)->Value = (System::SByte) value;
						case System::TypeCode::Byte:   ((Byte^)this)->Value = (System::Byte)  value;
						}
					}
				}

				property gT MIN {
					gT get(void)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single: return (gT)(System::Single)  ((Float^)this)->MIN;
						case System::TypeCode::Double: return (gT)(System::Double) ((Double^)this)->MIN;
						case System::TypeCode::Int32: return  (gT)(System::Int32)     ((Int^)this)->MIN;
						case System::TypeCode::UInt32: return (gT)(System::UInt32)    ((Unt^)this)->MIN;
						case System::TypeCode::Int16: return  (gT)(System::Int16)   ((Short^)this)->MIN;
						case System::TypeCode::UInt16: return (gT)(System::UInt16)  ((Sword^)this)->MIN;
						case System::TypeCode::SByte: return  (gT)(System::SByte)    ((Char^)this)->MIN;
						case System::TypeCode::Byte: return   (gT)(System::Byte)     ((Byte^)this)->MIN;
						}
					}
					void set(gT value)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single:  ((Float^)this)->MIN = (System::Single)value;
						case System::TypeCode::Double: ((Double^)this)->MIN = (System::Double)value;
						case System::TypeCode::Int32:     ((Int^)this)->MIN = (System::Int32) value;
						case System::TypeCode::UInt32:    ((Unt^)this)->MIN = (System::UInt32)value;
						case System::TypeCode::Int16:   ((Short^)this)->MIN = (System::Int16) value;
						case System::TypeCode::UInt16:  ((Sword^)this)->MIN = (System::UInt16)value;
						case System::TypeCode::SByte:    ((Char^)this)->MIN = (System::SByte) value;
						case System::TypeCode::Byte:     ((Byte^)this)->MIN = (System::Byte)  value;
						}
					}
				}

				property gT MAX {
					gT get(void)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single: return (gT)(System::Single)  ((Float^)this)->MAX;
						case System::TypeCode::Double: return (gT)(System::Double) ((Double^)this)->MAX;
						case System::TypeCode::Int32: return  (gT)(System::Int32)     ((Int^)this)->MAX;
						case System::TypeCode::UInt32: return (gT)(System::UInt32)    ((Unt^)this)->MAX;
						case System::TypeCode::Int16: return  (gT)(System::Int16)   ((Short^)this)->MAX;
						case System::TypeCode::UInt16: return (gT)(System::UInt16)  ((Sword^)this)->MAX;
						case System::TypeCode::SByte: return  (gT)(System::SByte)    ((Char^)this)->MAX;
						case System::TypeCode::Byte: return   (gT)(System::Byte)     ((Byte^)this)->MAX;
						}
					}
					void set(gT value)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single:  ((Float^)this)->MAX = (System::Single)value;
						case System::TypeCode::Double: ((Double^)this)->MAX = (System::Double)value;
						case System::TypeCode::Int32:     ((Int^)this)->MAX = (System::Int32) value;
						case System::TypeCode::UInt32:    ((Unt^)this)->MAX = (System::UInt32)value;
						case System::TypeCode::Int16:   ((Short^)this)->MAX = (System::Int16) value;
						case System::TypeCode::UInt16:  ((Sword^)this)->MAX = (System::UInt16)value;
						case System::TypeCode::SByte:    ((Char^)this)->MAX = (System::SByte) value;
						case System::TypeCode::Byte:     ((Byte^)this)->MAX = (System::Byte)  value;
						}
					}
				}
				property gT MOV {
					gT get(void)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single: return (gT)(System::Single)  ((Float^)this)->MOV;
						case System::TypeCode::Double: return (gT)(System::Double) ((Double^)this)->MOV;
						case System::TypeCode::Int32: return  (gT)(System::Int32)     ((Int^)this)->MOV;
						case System::TypeCode::UInt32: return (gT)(System::UInt32)    ((Unt^)this)->MOV;
						case System::TypeCode::Int16: return  (gT)(System::Int16)   ((Short^)this)->MOV;
						case System::TypeCode::UInt16: return (gT)(System::UInt16)  ((Sword^)this)->MOV;
						case System::TypeCode::SByte: return  (gT)(System::SByte)    ((Char^)this)->MOV;
						case System::TypeCode::Byte: return   (gT)(System::Byte)     ((Byte^)this)->MOV;
						}
					}
					void set(gT value)
					{
						switch (System::Type::GetTypeCode(gT::typeid))
						{
						case System::TypeCode::Single:  ((Float^)this)->MOV = (System::Single)value;
						case System::TypeCode::Double: ((Double^)this)->MOV = (System::Double)value;
						case System::TypeCode::Int32:     ((Int^)this)->MOV = (System::Int32) value;
						case System::TypeCode::UInt32:    ((Unt^)this)->MOV = (System::UInt32)value;
						case System::TypeCode::Int16:   ((Short^)this)->MOV = (System::Int16) value;
						case System::TypeCode::UInt16:  ((Sword^)this)->MOV = (System::UInt16)value;
						case System::TypeCode::SByte:    ((Char^)this)->MOV = (System::SByte) value;
						case System::TypeCode::Byte:     ((Byte^)this)->MOV = (System::Byte)  value;
						}
					}
				}

			};

}//end of Stepflow


#endif
