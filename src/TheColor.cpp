#include "TheColor.h"
#include "GuiManager.h"
#include "TheController.h"

using namespace stepflow;



Controlled<data32>
TheColor::TheMixer = Controlled<data32>();

Controlled<float>
TheAlpha = Controlled<float>();



void TheColor::InitColors(void)
{
	TheMixer.SetUserMode<TheMode>(NOLL, NOLL, NOLL, NOLL);
	TheMixer.GetUserMode<TheMode>()->SetTheAlpha(&TheColor::TheAlpha);
	TheMixer.GetUserMode<TheMode>()->SetAntiPro(false);
	theRED = TheMixer.MIN.f32;
	theGREEN = TheMixer.MAX.f32;
	theBLUE = TheMixer.MOVE.f32;
}

