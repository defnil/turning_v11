#pragma once
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventAdapter>
#include <osg/NodeCallback>
#include <osgGA/EventVisitor>
#include <osgViewer/view>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osg/Vec3d>
#include <dinput.h>

#include "Vehicle.h"
#include "JoystickData.h"
#include "MyCamera.h"
#include "CommonFunctions.h"

using namespace osg;
using namespace osgGA;

enum DRIVING_MODE
{
	ROADDRIVING,
	ALLSTEERING,
	REDUCEDSWINGOUT,
	CRAB,
	INDEPENDENT,
};

class vehicleDataType : public osg::Referenced
{
public:
	vehicleDataType( osg::Node* vehicleNode, osg::MatrixTransform* mt, float xOffsetCenterFront, float BBCenterZ, CVehicle* vehicle);

	void turnFrontLeft();
	void turnFrontRight();
	void noFrontTurning();
	void moveFrontForward();
	void moveFrontBackward();
	void noFrontAcceleration();
	void movePause();

	void initializeArray_tires();
	//void preProcess();
	void postProcess();
	osg::Vec3d getBBCenterTwo(int drivingMode);
	void updateData();

public:

	// used in Key press handlers
	bool						_bturnLeft;				// if keyboard left key is pressed
	bool						_bturnRight;			// if keyboard right key is pressed
	bool						_bmoveForward;	// if keyboard up key is pressed
	bool						_bmoveBackward;	// if keyboard down key is pressed
	bool						_bPause;		// if pause key (p) is pressed (maybe other key that treated as such function)

	DRIVING_MODE		e_DrivingMode;

	// store rear point info
	//osg::Vec3d				_pRearOne;		// position of rear point in step 1
	//osg::Vec3d				_pRearTwo;		// position of rear point in step 2

	// store front point info
	osg::Vec3d				_pFrontOne;		// position of front point in step 1
	osg::Vec3d				_pFrontTwo;		// position of front point in step 2
	double						_frontThetaOne;			// angle (vehicle_ body ------  X-axis) in stage 1, [-Pi, Pi]
	double						_frontThetaTwo;		// angle (vehicle_ body ------  X-axis) in stage 2, [-Pi, Pi]
	
	//osg::Vec3d				_vFrontSpeedOne;  // front speed vector stage 1, the default direction is same with ReartoFrontIni
	osg::Vec3d				_vFrontSpeedTwo;		// front speed vector stage 2

	double						_distanceCenterFront;		// distance (in XOY plane) between bounding box center and front point 
		
	double						_speedFront;			// front speed/velocity value
	double						_speedAngle;			// the angle between the direction of the vehicle body and the current speed, Controlled by the keyboard or steering wheel
	//double						_speedAngleTwo;			// angle (vehicle body ------ speed direction), in stage 2
	//double						_speedAngleFromXOne;	// the angle between the current speed vector and x axis. stage 1
	double						_speedAngleFromXTwo; // the angle between the current speed vector and x axis. stage 2
	double						_BBCenterZ;		// z value of the bounding box center

	//bool						_bLeftLock;
	//bool						_bRightLock;

	// The following two parameters can determine the direction of (rear to front vector).
	osg::Vec3d				_pBBCenterIni;	// the initial bounding box center in the model coordinate system
	osg::Vec3d				_vRearToFrontIni;  // vector : rear->front (initial position)

	//osg::Vec3d				_vRearToFrontOne;	// vector: rear->front in step/status 1
	osg::Vec3d				_vRearToFrontTwo;		// vector: rear->front in step/status 2
	//osg::Matrixd				_matrix; // store the initial matrix info
	osg::Vec3d				_iniTranslation;		// initial translation info in the matrixtransform node
	osg::Quat					_iniRotation;		// initial rotation info in the matrixtransform node
	osg::Vec3d				_pBBCenterOne;		// position of bounding box center in status 1
	osg::Vec3d				_pBBCenterTwo;	// position of bounding box center in status 2

	CVehicle*					_vehicleData;
	osg::Node*				_vehicleNode;
	osg::ref_ptr<osg::MatrixTransform>		_mt;

	// tires handling
	osg::ref_ptr<osg::MatrixTransform> _pat_tires[10];		// array for holding all the tire nodes
	osg::ref_ptr<osg::Vec3dArray> _array_tireCenterInWorld;		// position of tires in the previous step
	osg::ref_ptr<osg::Vec3dArray> _array_tireCenter;		// array for holding position of the tires
	osg::ref_ptr<osg::DoubleArray> _array_tireTravelTheta;	// array for holding traveling distance of each tire
	double _Sfrontcenterpoint; // the distance traveled by the front center point
};

class vehicleNodeCallback : public osg::NodeCallback
{
public:

	vehicleNodeCallback(osgViewer::View* view, osg::Group* scene)
	{
		_mRoot = scene;
		_mCameraManip = new CMyCamera;
		osgGA::KeySwitchMatrixManipulator* keySwitch = new osgGA::KeySwitchMatrixManipulator;
		keySwitch->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator());
		keySwitch->addMatrixManipulator('2', "NodeTracker", _mCameraManip);
			// Init the switcher to the first manipulator (in this case the only manipulator)
			//keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
		view->setCameraManipulator(keySwitch);
	}

	virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
	{
		_vehicleData = dynamic_cast<vehicleDataType*> (node->getUserData());
		//osg::ref_ptr<osg::PositionAttitudeTransform> pat = static_cast<osg::PositionAttitudeTransform*>(node);
		if(_vehicleData)
		{
			_vehicleData->updateData();
			osg::Vec3d cameraPosition = _vehicleData->_iniTranslation + _vehicleData->_pBBCenterTwo + osg::Vec3d(5.0, 0.0, 0.10) * osg::Matrix(osg::Quat(_vehicleData->_frontThetaTwo, osg::Z_AXIS));  // AC700: osg::Vec3d(810.0, -95.0, -150.0)
			_mCameraManip ->setPosition(cameraPosition);
			_mCameraManip->setRotatefZ(_vehicleData->_frontThetaTwo - osg::PI_2);
		}

		traverse(node, nv);
	}

protected:
	vehicleDataType* _vehicleData;
	osg::Group*	_mRoot;
	CMyCamera* _mCameraManip;
};
