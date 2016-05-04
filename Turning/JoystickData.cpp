#include "stdafx.h"
#include "JoystickData.h"

DIJOYSTATE2 global_dataSet1;
DIJOYSTATE2 global_dataSet2;

bool	global_bUseJoystick1 = false;
bool	global_bUseJoystick2 = false;

GearType::TGearType global_GearTypes = GearType::BTN_INVALID;
GearType::TDringType global_DrivingTypes = GearType::BTN_SIX;