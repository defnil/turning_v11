#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>

#include <osg/Array>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/ComputeBoundsVisitor>
#include <osg/ShapeDrawable>
#include <osg/AnimationPath>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/TriangleFunctor>
#include <osg/PositionAttitudeTransform>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIActionAdapter>

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/GUIEventHandler>
#include <osgGA/StateSetManipulator>

#include "mfcEventType.h"
#include "Vehicle.h"
#include "Road.h"
#include "CommonFunctions.h"
#include "PhysXInterface.h"
#include "VehicleOperating.h"

extern CRoad*			global_Road;
extern CVehicle*		global_Vehicle;
enum e_pick{not_Pick, is_Pick, finish_Pick};		// enum type must be put in front of functions using it.

using namespace physx;

class mfcEventHandler : public osgGA::GUIEventHandler
{
public:
	/*-------------------------NPS tutorial keyboard handler-------------------------------*/
	//typedef void (*functionType) ();

	//enum keyStatusType
	//{
	//	KEY_UP, KEY_DOWN
	//};

	//struct functionStatusType
	//{
	//	functionStatusType() {keyState = KEY_UP; keyFunction = NULL;}
	//	functionType keyFunction;
	//	keyStatusType keyState;
	//};

	//bool addFunction(int whatKey, functionType newFunction);
	//bool addFunction(int whatKey, keyStatusType keyPressStatus, functionType newFunction);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	/********************************physx*****************************************/
	void drawGround( osg::Box* boxShape);
	//void addWall(osg::Box* shape, osg::Vec3& pos, const osg::Quat& rot);
	void addDynamicBox( osg::Box* shape=NULL, const osg::Vec3& pos=osg::Vec3(0.0f, 0.0f, 0.0f), const PxVec3* vel=NULL, double density=1.0f, bool isKinematic=false );
	//void addStaticPhysicsBox( osg::Box* shape, osg::Vec3& pos, const osg::Quat& rot);
	//void addPhysicsSphere( osg::Sphere* shape, const osg::Vec3& pos, const osg::Vec3& vel, double mass );

	osg::ref_ptr<osg::MatrixTransform> getInitBoundingBox(osg::Node* node, osg::Vec3& boxcenter);

	void syncPhysXOSG();
	/*****************************************************************************/

	mfcEventHandler(osg::Group* rt, osgViewer::CompositeViewer* compositeViewer, osg::Switch* switch_Plant);
	//mfcEventHandler(osg::Group* rt);
	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	virtual void accept(osgGA::GUIEventHandlerVisitor& v)   { v.visit(*this); };

	void resetScene(); // reset the scene

	//osg::ref_ptr<osg::Geode> createBoudingShpere(osg::Node * node);
	osg::ref_ptr<osg::MatrixTransform> createBoudingBox(osg::Node* node);
	osg::Camera* DisplayInfo();

	void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& aa);
	osg::Geode* DrawPickedPoint(osg::Vec3f position);	// To be used by pick()

	void setPickStatus(enum e_pick viewpick) { status_Picking = viewpick; }
	void setDataLoad(bool data_load) { b_dataload = data_load; }
	void setPickChannel(bool pPick) { pickChannel = pPick; }
	void setDataSave(bool data_save) { b_datasave = data_save; }
	void setPhysxState(bool physx_enabled) { b_physxEnabled = physx_enabled; }
	void EnableOBB(bool obb_enabled) { b_OBBEnabled = obb_enabled; }
	void setDataSource(bool data_source) { b_datasource = data_source; }

	enum e_pick getPickStatus() { return status_Picking; }
	bool getPickChannel() { return pickChannel; }
	bool getDataLoad() { return b_dataload; }
	bool getDataSave() { return b_datasave; }
	bool getPhysxState() { return b_physxEnabled; }
	bool getOBBState() { return b_OBBEnabled; }
	bool getDataSource() { return b_datasource; }

	void calDefaultPaths(osg::Vec3Array* centerline, osg::Vec3f turningcenter, float wheel_base,
		osg::Vec3Array* frontpath, osg::Vec3Array* rearpath,
		osg::Vec3Array* arcfront, osg::Vec3Array* arcrear);

	void calCriticalPoints (osg::Vec3Array* arcrear, osg::Vec3Array* arcfront, float dis,
		osg::Vec3Array* rearCritical, osg::Vec3Array* frontCritical);

	void line_intersection(osg::Vec3f p1, osg::Vec3f p2, osg::Vec3f p3, osg::Vec3f p4, osg::Vec3f& point, float& dis);

	void inter_F_B2 (osg::Vec3 p_center, osg::Vec3Array* l_F, osg::Vec3Array* l_B2,
		osg::Vec3Array* intersection_FB, osg::FloatArray* distance_FB);

	void inter_R_B1 (osg::Vec3 p_center, osg::Vec3Array* l_R, osg::Vec3Array* l_B1,
		osg::Vec3Array* intersection_RB, osg::FloatArray* distance_RB);

	void analysisPath(float& max_value, float& min_value, osg::Vec3f& pointOnF, osg::Vec3f& pointOnR,
		osg::Vec3f& center_point, bool& able_pass, osg::Vec3f& dir);

	osg::Geode* drawBoundary(osg::Vec3Array* vec3data);
	osg::Geode* drawCenterLine(osg::Vec3Array* vec3data);
	osg::Geode* drawTurningCircle(osg::Vec3f turningcenter, float turningradius, int numpoints);
	osg::Geode* drawDefaultPath(osg::Vec3Array* centerline, osg::Vec3f turningcenter, float wheel_base);

	// draw lines with Vec3dArray data, if stipple factor and pattern are set to 0, it will create continuous line
	osg::Geode* drawLines(osg::Vec3Array* vec3data, float width_f,  osg::Vec4 linecolor, GLint stipplefactor, GLushort stipplepattern);

	osg::AnimationPath* createPath(osg::Vec3Array* vec_F, osg::Vec3Array* vec_R, const osg::Vec3f& bb_center, const osg::Vec3f& position_front);

	osg::AnimationPath* testAnimate();

	void saveArrayData(osg::Vec3Array* array, std::ofstream& datafile);
	void loadArrayData(osg::Vec3Array* array, std::ifstream& datafile);

	osg::Vec3Array* GetTriangleVertices(osg::Node* node);

	void cookBoundaryMesh(osg::Node* node);

	//CRoad* getRoadPointer() { return global_Road.get(); }
	//CVehicle* getVehiclePointer() { return global_Vehicle.get(); }

	osg::ref_ptr<osg::Box>  getInitBoundingBox(osg::Node* node);

protected:
	/***********************************************PhysX***************************/
	void addPhysicsData( int id, osg::Shape* shape, const osg::Vec3& pos,
		const osg::Vec3& vel, double mass );		// 该函数为 actor（即这里的 shape）设定位置，速度参数
	void addStaticPhysicsData( int id, osg::Shape* shape, const osg::Vec3& pos, const osg::Quat& rot);
	/*******************************************************************************/

public:
		osg::ref_ptr<osgGA::NodeTrackerManipulator> mNodeTracker;

protected:
	/*-------------------------------------------------------NPS keyboard-------------------------------------------*/
	//typedef std::map<int, functionStatusType > keyFunctionMap;
	//keyFunctionMap keyFuncMap;
	//keyFunctionMap keyUPFuncMap;

	/***********************************************PhysX***************************/
	typedef std::map<int, osg::observer_ptr<osg::MatrixTransform> > NodeMap;
	typedef std::map<int, osg::Node*> KNodeMap;
	NodeMap _physicsNodes;	// 与physx空间中的_actors一一对应，这里是osg空间里的活动actors集合
	NodeMap _staticPhysicsNodes;		// 与physx空间中的_staticActors一一对应，代表osg空间里的静态actors集合
	NodeMap _kinematicPhysicsNodes;	// 与physx空间中_kinematicActors对应
	/******************************************************************************/

	//std::unique_ptr<CRoad> global_Road;
	//std::unique_ptr<CVehicle> global_Vehicle;

	enum e_pick status_Picking;
	bool pickChannel; // if true, the picked boundary is 1, if false, the picked boundary is 2
	//osg::ref_ptr<osg::Vec3Array> PickedPoints; // set as a member of Road?
	bool b_dataload; // true indicates the data is loaded from external files, false indicates the data is generated by mouse selection
	bool b_datasave; // true: all the data will be saved this time, false: the mouse selection data will not be saved to the file
	bool b_hideplant; // true: show the plant (default), false: hide the plant model
	bool b_physxEnabled;	// true: enable physx engine
	bool b_OBBEnabled;  // true: enable OBB (oriented bounding box)
	bool b_datasource; // true: use 3ds max model, false: use external data
	bool b_3dsBoundaryModel_loaded; // true: the 3ds Max model has been loaded
	bool b_exBoundaryModel_loaded; // true: the data from external source has been loaded
	bool b_drivingMode; // true: driving mode enabled.
	bool b_fogEnabled;
	bool b_rainEnabled;
	bool b_snowEnabled;

	osg::ref_ptr<osg::Switch> pointsGroup;
	osg::ref_ptr<osg::MatrixTransform> pathGroup;

	//osgViewer::View *mv;	// 活动的 view，用于pick操作
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<osgViewer::CompositeViewer> mViews;
	osg::ref_ptr<osg::Switch> mswitch_plantinfo;	// stores all Plant related information
	osg::ref_ptr<osg::Switch> mswitch_vehicleinfo;	// stores all Vehicle related information
	osg::ref_ptr<osg::Node> mCrane;
	osg::ref_ptr<osg::Group> mWalls;
	osg::ref_ptr<osg::Geode> mOBBGeode; // store the OBB of the vehicle
	osg::ref_ptr<osg::MatrixTransform> mAABB; //store the AABB of the vehicle
	PhysXInterface* mPhysicsModule;

	// Simulation:
	osg::ref_ptr<osg::AnimationPathCallback> m_apcb_Driving;

	osg::ref_ptr<osgParticle::PrecipitationEffect> m_precipitation;

	// Interactive Operating:
	vehicleDataType* mVehicleOneData;
};

class BoundingBoxCallback : public osg::NodeCallback
{
public:
	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )	// here parameter node is the boundingbox node
	{
		osg::BoundingBox bb;
		for ( unsigned int i=0; i<_nodesToCompute.size(); ++i )		// here just one node, so size == 1;
		{
			osg::Node* node = _nodesToCompute[i];
			osg::ComputeBoundsVisitor cbbv;
			node->accept( cbbv );

			osg::BoundingBox localBB = cbbv.getBoundingBox();
			osg::Matrix localToWorld = osg::computeLocalToWorld( node->getParent(0)->getParentalNodePaths()[0] );
			for ( unsigned int i=0; i<8; ++i )
				bb.expandBy( localBB.corner(i) * localToWorld );
		}

		osg::MatrixTransform* trans = static_cast<osg::MatrixTransform*>(node);	// 把传递进来的 Node 转变为MatrixTransform类型后，通过修改Matrix值改变其(boundingbox)位置。
		trans->setMatrix(
			osg::Matrix::scale(bb.xMax()-bb.xMin(), bb.yMax()-bb.yMin(), bb.zMax()-bb.zMin()) *
			osg::Matrix::translate(bb.center()) );
	}

	osg::NodePath _nodesToCompute;
};
