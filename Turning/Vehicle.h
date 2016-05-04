#pragma once
#include <string>
//#include "CommonFunctions.h"
#include <osg/NodeCallback>
#include <osg/Array>

class CVehicle : public osg::Referenced			// NodeCallback is also derieved from osg::Referenced
{
public:
	CVehicle(void);
	~CVehicle(void);

public:

	// filepathname of the vehicle
	static std::string		s_vehiclename;
	//float						wheelbase;
	float						wheeldis;
	osg::Vec3				FrontOffset;  // position of front wheels in the model coordinate system
	float						tireRadius;

	int							drivingMode; // 0: road driving, 1 all-wheel steering, 2 reduced swing-out steering. It determines v_wheelBase.
	double					maxTurningAngle;	// the max turning angle of the vehicle. It remains the same in different turning modes
	osg::ref_ptr<osg::DoubleArray> v_wheelBase;
	//osg::ref_ptr<osg::DoubleArray> v_maxTurningAngle;

	osg::ref_ptr<osg::Vec3Array> vec_frontpath;
	osg::ref_ptr<osg::Vec3Array> vec_rearpath;

	osg::ref_ptr<osg::Vec3Array> vec_turningpartFront;  // start from circular Arc, Useful for further analysis
	osg::ref_ptr<osg::Vec3Array> vec_turningpartRear;

	osg::ref_ptr<osg::Vec3Array> vec_fronttraj;
	osg::ref_ptr<osg::Vec3Array> vec_reartraj;

	osg::ref_ptr<osg::Vec3Array> critical_rearpath;
	osg::ref_ptr<osg::Vec3Array> critical_frontpath;

	osg::ref_ptr<osg::Vec3Array> final_rearpath;
	osg::ref_ptr<osg::Vec3Array> final_frontpath;
};

//class AC700 : public CVehicle
//{
//	AC700();
//	~AC700();
//};