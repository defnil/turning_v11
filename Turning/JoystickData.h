#pragma once
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

extern DIJOYSTATE2			global_dataSet1;
extern DIJOYSTATE2			global_dataSet2;
extern bool						global_bUseJoystick1;
extern bool						global_bUseJoystick2;

// http://stackoverflow.com/questions/112433/should-i-use-define-enum-or-const
namespace GearType
{
	enum TGearType
	{
		BTN_ZERO=0,
		BTN_ONE,
		BTN_TWO,
		BTN_THREE,
		BTN_FOUR,
		BTN_FIVE,		

		BTN_INVALID = 20,
	};

	enum TDringType
	{
		BTN_SIX			= 6,
		BTN_SEVEN,
		BTN_TEN			= 10,
		BTN_ELEVEN,
	};
}

extern GearType::TGearType global_GearTypes;
extern GearType::TDringType global_DrivingTypes;