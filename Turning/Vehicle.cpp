#include "StdAfx.h"
#include "Vehicle.h"

CVehicle::CVehicle(void) :
		drivingMode(0),
		//wheelbase(1500.0f),		// AC700
		wheeldis(2.57f),
		//wheeldis(250.0f),
		FrontOffset (osg::Vec3(6.60f, 0.0f, 0.0f)),
		//FrontOffset(osg::Vec3(1015.0f, 0.0f, 0.0f)),
		tireRadius(0.77f),
		//tireRadius(68.0f)
		maxTurningAngle(0.500)
{
	v_wheelBase = new osg::DoubleArray;
	//v_maxTurningAngle = new osg::DoubleArray;

	v_wheelBase->push_back(5.400);
	v_wheelBase->push_back(4.606);
	v_wheelBase->push_back(8.390);

	//v_maxTurningAngle->push_back(0.506);
	//v_maxTurningAngle->push_back(0.501);

	vec_frontpath = new osg::Vec3Array;
	vec_rearpath = new osg::Vec3Array;

	vec_turningpartFront = new osg::Vec3Array;
	vec_turningpartRear = new osg::Vec3Array;

	vec_fronttraj = new osg::Vec3Array;
	vec_reartraj = new osg::Vec3Array;

	critical_rearpath = new osg::Vec3Array;
	critical_frontpath = new osg::Vec3Array;

	final_rearpath = new osg::Vec3Array;
	final_frontpath = new osg::Vec3Array;
}

CVehicle::~CVehicle(void)
{
}

std::string CVehicle::s_vehiclename;  // static member variable serves as a global variable