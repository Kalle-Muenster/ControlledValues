/* A Vector of 3 Controlled<float>s */

#ifndef _CONTROLLED_VECTOR3_
#define _CONTROLLED_VECTOR3_

#ifndef __STEPFLOW_TYPES__
typedef unsigned char byte;
struct Vector3{ float x, y, z; };
union data32 { byte byte[4]; int i32; unsigned u32; float f32; };
#endif

namespace stepflow{
	class ControlledVector3
	{
	public:
		Controlled<float> x;
		Controlled<float> y;
		Controlled<float> z;

		ControlledVector3(void)
		{
			x.SetUp(0, 255, 128, 1.0f / 128, x.Clamp);
			x.ControllerActive = true;
			y.SetUp(0, 255, 128, 1.0f / 128, y.Clamp);
			y.ControllerActive = true;
			z.SetUp(0, 255, 128, 1.0f / 128, z.Clamp);
			z.ControllerActive = true;
		}

		void SetMode(Controlled<float>::ControllMode mode)
		{
			x.Mode(mode);
			y.Mode(mode);
			z.Mode(mode);
		}
		void SetMIN(Vector3 min)
		{
			x.MIN = min.x;
			y.MIN = min.y;
			z.MIN = min.z;
		}
		void SetMAX(Vector3 max)
		{
			x.MAX = max.x;
			y.MAX = max.y;
			z.MAX = max.z;
		}
		void SetMOVE(Vector3 move)
		{
			x.MOVE = move.x;
			y.MOVE = move.y;
			z.MOVE = move.z;
		}
		void ControllersActive(bool setActive)
		{
			x.ControllerActive = y.ControllerActive = z.ControllerActive = setActive;
		}
		Vector3 Check(void)
		{
			Vector3 t = { x.Check(), y.Check(), z.Check() };
			return t;
		}
		template<typename usermode> void SetUserMode(void)
		{
			x.SetUserMode<usermode>(x);
			y.SetUserMode<usermode>(y);
			z.SetUserMode<usermode>(z);
		}
		virtual operator Vector3(void)
		{
			Vector3 t = { x, y, z };
			return t;
		}
		virtual Vector3 operator =(Vector3 setter)
		{
			x = setter.x;
			y = setter.y;
			z = setter.z;
			return this->operator Vector3();
		}
	};
}

#endif

