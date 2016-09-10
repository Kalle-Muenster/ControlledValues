#ifndef THE_COLOR_INCLUDED
#define THE_COLOR_INCLUDED


//#include "TheController.h"
#include "ControlledValues\ControlledValues.h"


namespace stepflow
{
	
	class TheColor
	{
	private:
		TheColor(void){};
		static Controlled<data32> TheMixer;
		static float TheAlpha;

	public:
		static float &theRED,&theGREEN,&theBLUE;
		static data32& Bunt;
		static data32& White;
		static data32& Wheed;
		static data32& Blue;
		static data32& Gelb;
		static data32& Green;
		static data32& Rot;
		static data32& Mint;
		static data32& Orange;
		static data32& CornflowerBlue;
		static data32& SetAlpha(float);
		static void MultiplyBy(Vector3);
		static void SetColorVariable(data32*);
		static void InitColors(void);
	};
	
}

#endif