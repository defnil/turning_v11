#include "StdAfx.h"
#include <fstream>
#include "mfcEventHandler.h"
#include "PxTkStream.h"
#include "VehicleOperating.h"
#include <osgUtil/Optimizer>

CRoad* 	global_Road;
CVehicle*	global_Vehicle;
//cOSG*		glo
/********************************physx*****************************************/

void mfcEventHandler::drawGround(osg::Box* boxShape)	// create the ground (plane) using a box size = "100 x 100 x 1"£¬·ÖÁ½²¿·Ö£ºÒ»²¿·ÖÊÇosg»æÖÆ£¬ÁíÒ»²¿·ÖÊÇphysxÄ£Äâ
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> basicGround = new osg::ShapeDrawable(boxShape);	// ÍùÏÂÃæ»­Ò»µã²»ºÍ Mianma µØÃæÖØµþµ¼ÖÂäÖÈ¾ÎÊÌâ, ºÐ×Ó³¤¿í¼´ÊÇMianmaµØÍ¼µÄ³¤¿íÖµ
	basicGround->setColor(osg::Vec4(0.3f, 0.4f, 0.6f, 1.0f));
	geode->addDrawable(basicGround.get() );

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->addChild( geode.get() );
	root->addChild( mt.get() );

	//PhysXInterface::instance()->createWorld( osg::Plane(0.0f, 0.0f, 1.0f, 0.0f), gravity );		// ÔÚphysx¿Õ¼äÀï´´½¨ground.
}

//void mfcEventHandler::addWall(osg::Box* shape, osg::Vec3& pos, const osg::Quat& rot)
//{
//	int id = _staticPhysicsNodes.size();
//	PhysXInterface::instance()->createWalls(id);
//	addStaticPhysicsData(id, shape, pos, rot);
//}

void mfcEventHandler::addDynamicBox( osg::Box* shape, const osg::Vec3& pos, const PxVec3* vel, double density, bool isKinematic )		// ¸Ãº¯ÊýÔÚphysx¿Õ¼äÀï´´½¨Ò»¸öboxÎïÌå
{
	if(shape)
	{
		int id;
		if(isKinematic) id = _kinematicPhysicsNodes.size()-1;		// _kinematicPhysicsNodes ÒÑÔÚosg»æÖÆ²¿·Ö×÷ÁËÌî³ä£¬¹ÊÏÖÔÚÒª¶ÔphysxÏàÓ¦¿Õ¼ä×ö¶ÔÓ¦£¬Ðë¼õÒ»²Å¿ÉÒÔ
		else id = _physicsNodes.size();

		osg::Vec3 dim = shape->getHalfLengths();
		const PxVec3 ph_pos = PxVec3(pos[0], pos[1], pos[2]);
		const PxVec3 ph_dim = PxVec3(dim[0], dim[1], dim[2]);
		mPhysicsModule->createBox( id, ph_pos, ph_dim, vel, density, isKinematic );		// Ô­ÐÍ createShape(PxBoxGeometry(a/2, b/2, c/2), aMaterial); ËùÒÔÕâÀïÓÃgetHalfLengths

		// ÒÔÏÂÕâ²¿·ÖÊÇ½«physx¿Õ¼äÐÅÏ¢ÏòosgÍ¬²½£¬¶ÔÓÚkinematicÀàÐÍµÄ¶øÑÔ£¬ÊÇÏÈÔÚosgÖÐ»­ºÃ²ÅÏòphysxÍ¬²½µÄ£¬¹Ê²»ÐèÒªÔÚÕâÀï´¦Àí£¬ÐèÒªÔÚ»æÖÆ²¿·Ö×ÔÐÐÍê³É
		if (isKinematic) {/* _kinematicPhysicsNodes[id] = mt; */} // is the vehicle specifically
		else
		{
				osg::ref_ptr<osg::Geode> geode = new osg::Geode;
				geode->addDrawable( new osg::ShapeDrawable(shape) );
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
				mt->addChild( geode.get() );
				root->addChild( mt.get() );
				_physicsNodes[id] = mt;
		}
	}
	else return;
}

//void mfcEventHandler::addStaticBox( osg::Box* shape, osg::Vec3& pos, const osg::Quat& rot)
//{
//	int id = _staticPhysicsNodes.size();
//	mPhysicsModule->createStaticBox(id, shape->getHalfLengths());
//
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	geode->addDrawable( new osg::ShapeDrawable(shape) );
//	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
//	mt->addChild( geode.get() );
//	root->addChild( mt.get() );
//	_staticPhysicsNodes[id] = mt;
//}

//void mfcEventHandler::addStaticPhysicsData( int id, osg::Shape* shape, const osg::Vec3& pos, const osg::Quat& rot)
//{
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	geode->addDrawable( new osg::ShapeDrawable(shape) );
//
//	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
//	mt->addChild( geode.get() );
//	root->addChild( mt.get() );		// ÔÚ osg ¿Õ¼äÀï´´½¨Õâ¸öosg::Shape
//
//	PhysXInterface::instance()->setMatrix( id, osg::Matrix::translate(pos) * osg::Matrix::rotate(rot) );		// ÔÚphysx ¿Õ¼äÀïÓÃosg::Matrix¶¨Òåposition
//	_staticPhysicsNodes[id] = mt;		// osg ¿Õ¼äÀïÉè¶¨ÎïÌå±ä»»Î»ÖÃµÄ id
//}
/*****************************************************************************/

mfcEventHandler::mfcEventHandler(osg::Group* rt, osgViewer::CompositeViewer* compositeViewer, osg::Switch* switch_Plant) :
root(rt),
mViews(compositeViewer),
mswitch_plantinfo(switch_Plant),
pickChannel(true),
b_dataload(false),
b_hideplant(true),
b_physxEnabled(false),
b_fogEnabled(false),
b_rainEnabled(false),
b_snowEnabled(false),
b_OBBEnabled(false),
b_3dsBoundaryModel_loaded(false),
b_exBoundaryModel_loaded(false),
//b_newState(false),
b_drivingMode(false),
status_Picking(not_Pick),
mPhysicsModule(new PhysXInterface)
{
	global_Road = new CRoad;
	global_Vehicle = new CVehicle;

	pointsGroup = new osg::Switch;
	pathGroup = new osg::MatrixTransform;
	mswitch_vehicleinfo = new osg::Switch;
	mWalls = new osg::Group;

	pathGroup->setMatrix(osg::Matrix::translate(0.0f, 0.0f, 0.0f));

	root->addChild(pointsGroup.get());
	root->addChild(pathGroup.get());
	root->addChild(mswitch_vehicleinfo.get());
	root->addChild(mWalls.get());

	m_apcb_Driving = new osg::AnimationPathCallback;

	// precipitation
	m_precipitation = osgCookBook::initPrecipitationEffect();
	mswitch_plantinfo->addChild(m_precipitation.get());
}

void mfcEventHandler::resetScene()
{
	pickChannel = true; // default: pick 1st boundary
	status_Picking = not_Pick;
	global_Road->vec_Boundary1->clear();
	global_Road->vec_Boundary2->clear();
	global_Road->vec_CenterLine->clear();
	global_Vehicle->vec_frontpath->clear();
	global_Vehicle->vec_fronttraj->clear();
	global_Vehicle->vec_rearpath->clear();
	global_Vehicle->vec_reartraj->clear();
	global_Vehicle->vec_turningpartFront->clear();
	global_Vehicle->vec_turningpartRear->clear();
	global_Vehicle->critical_frontpath->clear();
	global_Vehicle->critical_rearpath->clear();
	global_Vehicle->final_frontpath->clear();
	global_Vehicle->final_rearpath->clear();

	if(pointsGroup->getNumChildren()) pointsGroup->removeChildren(0, pointsGroup->getNumChildren());	// delete all the children of the pointsGroup
	if(pathGroup->getNumChildren()) pathGroup->removeChildren(0, pathGroup->getNumChildren());
}

bool mfcEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	if ( status_Picking==is_Pick && b_dataload==false &&	ea.getEventType()==osgGA::GUIEventAdapter::RELEASE &&
		ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
	{
		//osgViewer::View* mv = dynamic_cast<osgViewer::View*>(&aa);
		//if(mv) pick(mv, ea);
		pick(mViews->getView(0), ea);
	}

	/***********************************************Physx************************************************/
	if(ea.getEventType()==osgGA::GUIEventAdapter::FRAME	&& b_physxEnabled==true)	 syncPhysXOSG();	// Every frame sync the data between PhysX and OSG
	/****************************************************************************************************/

	bool b_newState;
	switch ( ea.getEventType() )
	{
		case GUIEventAdapter::KEYDOWN:			b_newState = true;	break;
		case GUIEventAdapter::KEYUP:				b_newState = false;	break;
		default: break;
	}

	if(b_drivingMode)
	{
		switch( ea.getKey() )
		{
		case GUIEventAdapter::KEY_Left:			mVehicleOneData->_bturnLeft = b_newState;					break;
		case GUIEventAdapter::KEY_Right:		mVehicleOneData->_bturnRight = b_newState;					break;
		case GUIEventAdapter::KEY_Up:			mVehicleOneData->_bmoveForward = b_newState;			break;
		case GUIEventAdapter::KEY_Down:		mVehicleOneData->_bmoveBackward = b_newState;		break;
		case GUIEventAdapter::KEY_P:			mVehicleOneData->_bPause = b_newState;						break;

		case 'z':			mVehicleOneData->e_DrivingMode = ROADDRIVING; break;
		case 'x':			mVehicleOneData->e_DrivingMode = ALLSTEERING; break;
		case 'c':			mVehicleOneData->e_DrivingMode = REDUCEDSWINGOUT; break;
		case 'v':			mVehicleOneData->e_DrivingMode = CRAB; break;
		default: break;
		}
	}

	switch ( ea.getEventType() )
	{
		case osgGA::GUIEventAdapter::USER:
				{
						const OwnDefineEventAdpater* adpater =
							dynamic_cast<const OwnDefineEventAdpater *>(ea.getUserData());

						switch(adpater->m_eventType)
						{
									case LOADVEHICLE:
										{
											//root->removeChild(0,1);
											mCrane = osgDB::readNodeFile(CVehicle::s_vehiclename);
											osgUtil::Optimizer optimizer;
											optimizer.optimize(mCrane);
											break;
										}

									case RESETSCENE:
										{
											resetScene();
											break;
										}

									case ADDFOG:
										{
											if(b_fogEnabled)
											{
												root->getOrCreateStateSet()->setMode(GL_FOG, osg::StateAttribute::OFF);
												b_fogEnabled = false;
											}
											else
											{
												root->setStateSet(osgCookBook::setFogState(osg::Vec4(0.8f, 0.8f, 0.8f, 0.0f), 0.05f, 0.0f, 300.0f));
												b_fogEnabled = true;
											}

											break;
										}

									case ADDRAIN:
										{
											if(b_rainEnabled)
											{
												m_precipitation->rain(0.0f);
												b_rainEnabled = false;
											}
											else
											{
												m_precipitation->rain(0.4f);
												b_rainEnabled = true;
											}
											
											break;
										}

									case ADDSNOW:
										{
											if(b_snowEnabled)
											{
												m_precipitation->snow(0.0f);
												b_snowEnabled = false;
											}
											else
											{
												m_precipitation->snow(0.4f);
												b_snowEnabled = true;
											}

											break;
										}

									case PRECIPITATONENABLED:
										{
											if(mswitch_plantinfo->containsNode(m_precipitation.get()))
												mswitch_plantinfo->removeChild(m_precipitation.get());
											else mswitch_plantinfo->addChild(m_precipitation.get());
											break;
										}

									case ENDPICKING:	// after finishing drawing, the points should be ereased
										{
											pointsGroup->addChild(drawBoundary(global_Road->vec_Boundary1));
											pointsGroup->addChild(drawBoundary(global_Road->vec_Boundary2));

											// output these boundary data to external files

											if(b_datasave == true)
											{
												std::ofstream data_boundary1, data_boundary2;
												data_boundary1.open("inner_boundary1.txt");
												data_boundary2.open("outer_boundary2.txt");

												saveArrayData(global_Road->vec_Boundary1, data_boundary1);
												saveArrayData(global_Road->vec_Boundary2, data_boundary2);
											}

											break;
										}

									case ANALYSE:
										{
											global_Road->vec_CenterLine = global_Road->getCenterLine(global_Road->vec_Boundary1, global_Road->vec_Boundary2);
											pointsGroup->addChild(drawCenterLine(global_Road->vec_CenterLine));

											global_Road->v_TCenter = global_Road->getturningCenter(global_Road->vec_CenterLine);
											pointsGroup->addChild(drawTurningCircle(global_Road->v_TCenter, global_Road->Rmin, 100));

											// save the centerline data into file
											if(b_datasave == true)
											{
												std::ofstream data_centerline;
												data_centerline.open("initial_centerline.txt");

												saveArrayData(global_Road->vec_CenterLine, data_centerline);
											}

											// cal the front and rear traj (the midpoint between two wheels) which follows the exact centerline. This can be used to find the trajectories of each wheel.
											calDefaultPaths(global_Road->vec_CenterLine, global_Road->v_TCenter, global_Vehicle->v_wheelBase->at(0),
												global_Vehicle->vec_frontpath, global_Vehicle->vec_rearpath,
												global_Vehicle->vec_turningpartFront, global_Vehicle->vec_turningpartRear);

											// two trajectories which affect the swept shape most
											// The outer traj is relevant to front critical point array, the inner traj is relevant to rear critical array
											// the critical path is generated from arc part data.
											calCriticalPoints(global_Vehicle->vec_turningpartRear, global_Vehicle->vec_turningpartFront, global_Vehicle->wheeldis,
												global_Vehicle->critical_rearpath, global_Vehicle->critical_frontpath);

											//pointsGroup->addChild(drawLines(global_Vehicle->vec_frontpath, 2.0f, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f),
											//	1, 0x00AA));
											//pointsGroup->addChild(drawLines(global_Vehicle->vec_rearpath, 2.0f, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
											//	1, 0x00AA));

											pathGroup->addChild(drawLines(global_Vehicle->vec_turningpartFront, 3.0f, osg::Vec4(0.627f, 0.125f, 0.941f, 1.0f),
												0, 0));
											pathGroup->addChild(drawLines(global_Vehicle->vec_turningpartRear, 3.0f, osg::Vec4(1.0f, 0.490f, 0.251f, 1.0f),
												0, 0));

											pathGroup->addChild(drawLines(global_Vehicle->critical_frontpath, 2.0f, osg::Vec4(0.545f, 0.271f, 0.075f, 1.0f),
												0, 0));
											pathGroup->addChild(drawLines(global_Vehicle->critical_rearpath, 2.0f, osg::Vec4(0.0f, 0.392f, 0.0f, 1.0f),
												0, 0));

											// export data to files
											if(b_datasave == true)
											{
												std::ofstream data_arcFront, data_arcRear, data_outerfrontwheel, data_innerrearwheel;

												data_arcFront.open("arc_frontpath.txt");
												data_arcRear.open("arc_rearpath.txt");
												data_outerfrontwheel.open("wheelpath_outerfront.txt");
												data_innerrearwheel.open("wheelpath_innerrear.txt");

												saveArrayData(global_Vehicle->vec_turningpartFront, data_arcFront);
												saveArrayData(global_Vehicle->vec_turningpartRear, data_arcRear);
												saveArrayData(global_Vehicle->critical_frontpath, data_outerfrontwheel);
												saveArrayData(global_Vehicle->critical_rearpath, data_innerrearwheel);
											}

											// calculate the intersections between external boundary and outer critical path (outer front wheel)
											osg::ref_ptr<osg::Vec3Array> inter_OuterPoints = new osg::Vec3Array;
											osg::ref_ptr<osg::FloatArray> centerouterDis = new osg::FloatArray;
											inter_F_B2(global_Road->v_TCenter, global_Vehicle->critical_frontpath, global_Road->vec_Boundary2,
												inter_OuterPoints, centerouterDis);

											// calculate the intersections between internal boundary and inner critical path (inner rear wheel)
											osg::ref_ptr<osg::Vec3Array> inter_InnerPoints = new osg::Vec3Array;
											osg::ref_ptr<osg::FloatArray> centerInnerDis = new osg::FloatArray;
											inter_R_B1(global_Road->v_TCenter, global_Vehicle->critical_rearpath, global_Road->vec_Boundary1,
												inter_InnerPoints, centerInnerDis);

											float minvalue = centerouterDis->at(0);
											int smallestitr = 0;
											for (int i=0; i != centerouterDis->getNumElements(); ++i)	// get the smallest element
											{
												if(centerouterDis->at(i) < minvalue)
												{
													minvalue = centerouterDis->at(i);
													smallestitr = i;
												}
											}
											osg::Vec3f p1 = (*inter_OuterPoints)[smallestitr];

											float maxvalue = centerInnerDis->at(0);
											int largestitr = 0;
											for (int i=0; i != centerInnerDis->getNumElements(); ++i)	// get the largest element
											{
												if(centerInnerDis->at(i) > maxvalue)
												{
													maxvalue = centerInnerDis->at(i);
													largestitr = i;
												}
											}
											osg::Vec3f p2 = (*inter_InnerPoints)[largestitr];

											osg::Vec3f direction(0.0f, 0.0f, 0.0f);
											bool pass_OK;
											analysisPath(maxvalue, minvalue, p1, p2,
												global_Road->v_TCenter, pass_OK, direction);

											if(pass_OK == false) AfxMessageBox("Impassable!");
											else if(direction == osg::Vec3f(0.0f, 0.0f, 0.0f))
											{
												global_Vehicle->vec_fronttraj = global_Vehicle->vec_turningpartFront;
												global_Vehicle->vec_reartraj = global_Vehicle->vec_turningpartRear;
												global_Vehicle->final_frontpath = global_Vehicle->critical_frontpath;
												global_Vehicle->final_rearpath = global_Vehicle->critical_rearpath;
											}
											else
											{
												for(osg::Vec3Array::iterator it=global_Vehicle->vec_turningpartFront->begin(); it != global_Vehicle->vec_turningpartFront->end(); ++it)
													global_Vehicle->vec_fronttraj->push_back(*it + direction);

												for(osg::Vec3Array::iterator it=global_Vehicle->vec_turningpartRear->begin(); it != global_Vehicle->vec_turningpartRear->end(); ++it)
													global_Vehicle->vec_reartraj->push_back(*it + direction);

												for(osg::Vec3Array::iterator it=global_Vehicle->critical_frontpath->begin(); it != global_Vehicle->critical_frontpath->end(); ++it)
													global_Vehicle->final_frontpath->push_back(*it + direction);

												for(osg::Vec3Array::iterator it=global_Vehicle->critical_rearpath->begin(); it != global_Vehicle->critical_rearpath->end(); ++it)
													global_Vehicle->final_rearpath->push_back(*it + direction);
											}
											//pointsGroup->addChild(drawLines(inter_OuterPoints, 30.0f, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
											//	0, 0));
											//pointsGroup->addChild(drawLines(inter_InnerPoints, 30.0f, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
											//	0, 0));
											//osg::ref_ptr<osg::Switch> switchgroup = new osg::Switch;
											pathGroup->addChild(drawLines(global_Vehicle->final_frontpath, 30.0f, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
												0, 0));
											pathGroup->addChild(drawLines(global_Vehicle->final_rearpath, 30.0f, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f),
												0, 0));
											//root->addChild(switchgroup.get());

											break;
										}

									case LOADDATA: // load picked boundary points
										{
											b_dataload = true;
											std::ifstream data_boundary1, data_boundary2;

											data_boundary1.open("inner_boundary1.txt");
											data_boundary2.open("outer_boundary2.txt");

											loadArrayData(global_Road->vec_Boundary1, data_boundary1);
											loadArrayData(global_Road->vec_Boundary2, data_boundary2);

											break;
										}

									case LOADBOUND: // load the exdata of boundary (from LIQING or 3ds Max model)
										{
											if(!b_physxEnabled) AfxMessageBox("PhysX Not Enabled!");
											else
											{
												if(b_exBoundaryModel_loaded)
												{
													// the extra data has been loaded before, clear all the data to prepare for loading 3ds or external model
													//if(global_Road->vec_exdataOfBound->getNumElements()) global_Road->vec_exdataOfBound->clear();
													std::map<int, physx::PxRigidStatic*>::iterator itr = mPhysicsModule->_staticActors.end();
													--itr;
													PxRigidStatic* wall_actor = itr->second;
													mPhysicsModule->getScene()->removeActor(*wall_actor);
													mPhysicsModule->_staticActors.erase(itr);
													_staticPhysicsNodes.erase(std::prev( _staticPhysicsNodes.end()));
													if(mWalls->getNumChildren())mWalls->removeChildren(0, mWalls->getNumChildren());
													//root->removeChild(mWalls.get());  // ÍòÍò²»¿ÉÉ¾³ýÕâÐ©½áµã£¬ÒòÎªËüÃÇºóÃæÒªÖØ¸´Ê¹ÓÃ£¬ÇÒÖ»ÔÚmfceventhandlerÀï³õÊ¼»¯ÁËÒ»´Î¡£

													b_exBoundaryModel_loaded = false;
												}
												if(b_3dsBoundaryModel_loaded)
												{
													// clear data
													std::map<int, physx::PxRigidStatic*>::iterator itr = mPhysicsModule->_staticActors.end();
													for(int k=1; k!=6; k++)
													{
														assert(mPhysicsModule->_staticActors.size() >= (size_t)k);
														std::advance(itr, -1);
														PxRigidStatic* wall_actor = itr->second;
														mPhysicsModule->getScene()->removeActor(*wall_actor);
													}

													//std::map<int, physx::PxRigidStatic*>::iterator itr1 = mPhysicsModule->_staticActors.end();
													//std::advance(itr1, -5); ÉÏÃæµÄitrÑ­»·ºóÒÑ¾­ÊÇµ½Ä©5Î»ÁË¡£Ö±½ÓÊ¹ÓÃ¼´¿É¡£
													mPhysicsModule->_staticActors.erase(itr, mPhysicsModule->_staticActors.end());

													std::map<int, osg::observer_ptr<osg::MatrixTransform> >::iterator itr2 = _staticPhysicsNodes.end();
													std::advance(itr2, -5);
													_staticPhysicsNodes.erase(itr2, _staticPhysicsNodes.end());

													if(mWalls->getNumChildren())mWalls->removeChildren(0, mWalls->getNumChildren());
													//root->removeChild(mWalls.get());

													b_3dsBoundaryModel_loaded = false;
												}

												if(b_datasource==false)
												{
													if(global_Road->vec_exdataOfBound->getNumElements()==0)	// read it once only
													{
														std::ifstream exdata_bound;
														exdata_bound.open("exdata_bound.txt");
														if (exdata_bound.is_open())
														{
															loadArrayData(global_Road->vec_exdataOfBound, exdata_bound);
														}
														else AfxMessageBox("Open Boundary Data Failed!");
													}

													osg::ref_ptr<osg::Geode> WallMesh = new osg::Geode;
													WallMesh->addDrawable(global_Road->drawBoundaryMesh(global_Road->vec_exdataOfBound, global_Road->ThicknessOfWall, global_Road->HeightOfWall));
													cookBoundaryMesh(WallMesh.get());
													b_exBoundaryModel_loaded = true;
													//root->addChild(WallMesh.get());
												}

												else
												{
													osg::ref_ptr<osg::Node> BoundaryMesh1 = osgDB::readNodeFile("D:/exdata/Wall01.osgb");	// xx.osgb.100.scale
													cookBoundaryMesh(BoundaryMesh1.get());
													osg::ref_ptr<osg::Node> BoundaryMesh2 = osgDB::readNodeFile("D:/exdata/Wall02.osgb");
													cookBoundaryMesh(BoundaryMesh2.get());
													osg::ref_ptr<osg::Node> BoundaryMesh3 = osgDB::readNodeFile("D:/exdata/Wall03.osgb");
													cookBoundaryMesh(BoundaryMesh3.get());
													osg::ref_ptr<osg::Node> BoundaryMesh4 = osgDB::readNodeFile("D:/exdata/Wall04.osgb");
													cookBoundaryMesh(BoundaryMesh4.get());
													osg::ref_ptr<osg::Node> BoundaryMesh5 = osgDB::readNodeFile("D:/exdata/Wall05.osgb");
													cookBoundaryMesh(BoundaryMesh5.get());

													b_3dsBoundaryModel_loaded = true;
												}
											}

											break;
										}

									case RELOADBOUND:
										if(!b_physxEnabled) /*AfxMessageBox("PhysX not enabled.")*/;
										else
										{
											if(b_datasource==false)
											{
												if(global_Road->vec_exdataOfBound->getNumElements()==0)	// read it once only
												{
													std::ifstream exdata_bound;
													exdata_bound.open("exdata_bound.txt");
													if (exdata_bound.is_open())
													{
														loadArrayData(global_Road->vec_exdataOfBound, exdata_bound);
													}
													else AfxMessageBox("Open Boundary Data Failed!");
												}

												if(b_exBoundaryModel_loaded)
												{
													// the extra data has been loaded before, clear all the data to prepare for loading 3ds or external model
													//if(global_Road->vec_exdataOfBound->getNumElements()) global_Road->vec_exdataOfBound->clear();
													std::map<int, physx::PxRigidStatic*>::iterator itr = mPhysicsModule->_staticActors.end();
													--itr;
													PxRigidStatic* wall_actor = itr->second;
													mPhysicsModule->getScene()->removeActor(*wall_actor);
													mPhysicsModule->_staticActors.erase(itr);
													_staticPhysicsNodes.erase(std::prev( _staticPhysicsNodes.end()));
													if(mWalls->getNumChildren())mWalls->removeChildren(0, mWalls->getNumChildren());
													//root->removeChild(mWalls.get());  // ÍòÍò²»¿ÉÉ¾³ýÕâÐ©½áµã£¬ÒòÎªËüÃÇºóÃæÒªÖØ¸´Ê¹ÓÃ£¬ÇÒÖ»ÔÚmfceventhandlerÀï³õÊ¼»¯ÁËÒ»´Î¡£

													b_exBoundaryModel_loaded = false;
												}

												osg::ref_ptr<osg::Geode> WallMesh = new osg::Geode;
												WallMesh->addDrawable(global_Road->drawBoundaryMesh(global_Road->vec_exdataOfBound, global_Road->ThicknessOfWall, global_Road->HeightOfWall));
												cookBoundaryMesh(WallMesh.get());
												b_exBoundaryModel_loaded = true;
											}
										}
										break;

									case HIDEPLANT:
										{
											if(b_hideplant)
											{
												mswitch_plantinfo->setAllChildrenOff();
											}
											else mswitch_plantinfo->setAllChildrenOn();

											b_hideplant = ! b_hideplant;  // toggle the boolean value
											break;
										}

									case ENABLEPHYSX:
										{
											if(!b_physxEnabled) mPhysicsModule->onInit();
											b_physxEnabled = true;

											drawGround( new osg::Box(osg::Vec3(0.5*136.67f, 0.5*107.02f,-0.008f), 136.67f, 107.02f, 0.01f) );

											//addStaticPhysicsBox(new osg::Box(osg::Vec3(), 100.0f), osg::Vec3(5000.0f, 5000.0f, 200.0f), osg::Quat()); //Î»ÖÃÎ´ÄÜÔÚosgÖÐÕýÈ··´Ó¦ÊÇÒòÎªstaticµÄactorÃ»ÓÐ±»»æÖÆ
											//addWall(new osg::Box(osg::Vec3(), 100.0f), osg::Vec3(5000.0f, 5000.0f, 200.0f), osg::Quat()); // ´Ë·¨²»³É¹¦£¬ÔÚphysxÖÐÏÔÊ¾ÎªÒ»¸ö·Ç³£´óµÄÆ½Ãæ

											break;
										}

									case DRIVING:
										{
											b_drivingMode = true;

											if(mswitch_vehicleinfo->getNumChildren()) mswitch_vehicleinfo->removeChildren(0, mswitch_vehicleinfo->getNumChildren());
											//mCrane = osgDB::readNodeFile("D:/exdata/LTM1200.osgb");

											if(mCrane)
											{
												mOBBGeode = new osg::Geode;
												osg::ref_ptr<osg::Box> obbShape = getInitBoundingBox(mCrane);
												mOBBGeode->addDrawable(new osg::ShapeDrawable(obbShape.get()));
												mOBBGeode->getOrCreateStateSet()->setAttributeAndModes(
													new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE) );
												mOBBGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

												// the following obbCenter is not the real obb center, we want the obbcenter.y() == 0, thus the left and right wheels are symmetric.
												osg::Vec3 obbCenter_Actual = obbShape->getCenter();
												osg::Vec3 obbCenter = osg::Vec3(obbCenter_Actual.x(), 0.0f, obbCenter_Actual.z());

												osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
												pat->setPivotPoint(obbCenter);	// Remember obbCenter is not the geometric center of the box. This needs to be adjusted in PhysX
												pat->addChild(mCrane);
												pat->addChild(mOBBGeode);

												mOBBGeode->setNodeMask(0x0);

												osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
												mt->setMatrix(/*osg::Matrix::rotate(osg::Quat(osg::PI/4, osg::Z_AXIS)) **/ osg::Matrix::translate(osg::Vec3(0.0f, 0.0f, obbCenter.z())));
												mt->addChild(pat.get());

												//osg::AnimationPath* path = createPath(global_Vehicle->vec_fronttraj, global_Vehicle->vec_reartraj, obbCenter, global_Vehicle->FrontOffset);
												//path->setLoopMode( osg::AnimationPath::LOOP );
												//osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback(path);
												//m_apcb_Driving->setAnimationPath(path);
												/*--------------------------------User Data Node Callback ---------------------------------------*/
												float xOffsetFront_BBCenter = abs(global_Vehicle->FrontOffset.x() - obbCenter.x());
												mVehicleOneData = new vehicleDataType(mCrane, mt, xOffsetFront_BBCenter, obbCenter.z(), global_Vehicle);
												mt->setUserData(mVehicleOneData);
												mt->setUpdateCallback( new vehicleNodeCallback(mViews->getView(0), root.get()) );

												/*---------------------------------------------------------------------------------------------------*/

												mswitch_vehicleinfo->addChild(mt.get());

												if(b_physxEnabled)
												{
													int i = _kinematicPhysicsNodes.size();
													_kinematicPhysicsNodes[i] = mt;

													addDynamicBox( obbShape, osg::Vec3(), NULL, 1.0f, true);
												}
											}

											//mNodeTracker = new osgGA::NodeTrackerManipulator;
											//mNodeTracker->setHomePosition(osg::Vec3(-870.0f, -77.0f, 500.0f), osg::Vec3(), osg::Z_AXIS);
											//mNodeTracker->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
											//mNodeTracker->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
											//mNodeTracker->setTrackNode(mCrane);

											//osgGA::CameraManipulator* cameraManip = mViews->getView(0)->getCameraManipulator();
											//osgGA::KeySwitchMatrixManipulator* keySwitch = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(cameraManip);
											//keySwitch->addMatrixManipulator('2', "NodeTracker", mNodeTracker.get());

											break;
										}

									case SIMULATE:
										{
											//osg::Vec3 box_dir(0.0f, 0.0f, 0.0f);
											//osg::Vec3 box_center(0.0f, 0.0f, 0.0f);

											if(mswitch_vehicleinfo->getNumChildren()) mswitch_vehicleinfo->removeChildren(0, mswitch_vehicleinfo->getNumChildren());

											//mswitch_vehicleinfo->addChild(createOBB(mswitch_vehicleinfo->getChild(0), box_center));  // just calculate the initial center point of the bounding box

											mCrane = osgDB::readNodeFile("D:/exdata/LTM1200.osgb");
											if ( mCrane )
											{
												//osg::Vec3 PositionOfFront = osg::Vec3(660.0f, 0.0f, 0.0f); // set the front wheel as the pivot position

												// draw the OBB shape
												mOBBGeode = new osg::Geode;
												osg::ref_ptr<osg::Box> obbShape = getInitBoundingBox(mCrane);
												mOBBGeode->addDrawable(new osg::ShapeDrawable(obbShape.get()));
												mOBBGeode->getOrCreateStateSet()->setAttributeAndModes(
												new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE) );
												mOBBGeode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

												osg::Vec3 obbCenter = obbShape->getCenter();

												osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
												pat->setPivotPoint(obbCenter);	// ÈÃËùÓÐ±ä»»µÄÖÐÐÄ¶¼·ÅÔÚ°üÎ§ºÐµÄ¼¸ºÎÖÐÐÄ£¬·½±ãÓëphysxÍ¬²½
												pat->addChild(mCrane);
												pat->addChild(mOBBGeode);

												mOBBGeode->setNodeMask(0x0);  // by default you cannot see the OBB

												// ÔÚ¼ÆËãÍêobbºóÔÙ×öÆ«ÒÆ£¬ÈÃcraneºÍËüµÄobb´¦ÓÚÍ¬Ò»±ä»»½áµãpatÖ®ÏÂ¡£PAT transform: ½«Ä£ÐÍµÄÔ­µã·Åµ½Ç°ÂÖÀ´£¬ºÃ×öanimationpath

												osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
												mt->addChild(pat.get());

												osg::AnimationPath* path = createPath(global_Vehicle->vec_fronttraj, global_Vehicle->vec_reartraj, obbCenter, global_Vehicle->FrontOffset);
												path->setLoopMode( osg::AnimationPath::LOOP );
												//osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback(path);
												m_apcb_Driving->setAnimationPath(path);
												mt->setUpdateCallback( m_apcb_Driving.get() );

												mswitch_vehicleinfo->addChild(mt.get());

												if(b_physxEnabled)
												{
													int i = _kinematicPhysicsNodes.size();
													_kinematicPhysicsNodes[i] = mt;

													addDynamicBox( obbShape, osg::Vec3(), NULL, 1.0f, true);
												}
											}

											break;
										}


									case SPACEMOUSE:
										{
											root->addChild( DisplayInfo() );
											break;
										}

									case OBB:
										{
											if(mOBBGeode)
											{
												mOBBGeode->setNodeMask(b_OBBEnabled ? 0x0 : 0xffffffff);
												b_OBBEnabled = !b_OBBEnabled;
											}
											else AfxMessageBox("Please load the crane model first.");

											break;
										}

									case TEST:
										{
											if(b_physxEnabled)
											{
												if(0) // if need test, set it to 1
												{
													PxVec3* vel = new PxVec3(0.0f, 0.0f, -10.0f);
													for ( unsigned int i=0; i<10; ++i )
													{
														for ( unsigned int j=0; j<10; ++j )
														{
															addDynamicBox( new osg::Box(osg::Vec3(), 100.0f), osg::Vec3((float)i * 100.0f +4000.0f, 2000.0f + 10.0f * (float)j, (float)j * 100.0f+4200.0f), vel, 1.0f, false);
														}
													}
												}
											}

											break;
										}

									default: break;		// switch m->eventType
							}
				} break;		// case osg::GUIEventAdapter::USER:

		default:	break;  // switch ea.getEventType()
	}

	return false;
}
			//case  TRANSSCENE:
			//	{
			//		osg::Node *node=root->getChild(0);

			//		osg::StateSet* ss = node->getOrCreateStateSet();
			//		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
			//		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			//		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
			//		ss->setRenderBinDetails(11, "RenderBin");
			//		break;
			//	}
//bool mfcEventHandler::setRoad(CRoad* pRoad)
//{
//	global_Road = pRoad;
//	return true;
//}
//
//bool mfcEventHandler::setVehicle(CVehicle* pVehicle)
//{
//	global_Vehicle = pVehicle;
//	return true;
//}

void mfcEventHandler::syncPhysXOSG()
{
	mPhysicsModule->simulate( 0.0167f );	// 1/60
	if(mPhysicsModule->mbStopDriving) m_apcb_Driving->setPause(true);
	for ( NodeMap::iterator itr=_physicsNodes.begin(); itr!=_physicsNodes.end(); ++itr)		// Ö»ÓÐ»î¶¯µÄÆÕÍ¨actor²ÅÐèÒªÃ¿Ö¡ÖØ»æ£¬¶østaticµÄactors Í¨³£Ö»ÐèÒª»æÖÆÒ»´Î£¬¹Ê²»ÐèÒªÔÚFRAMEÊÂ¼þÖÐÍê³É¡£
	{
		osg::Matrix matrix = mPhysicsModule->getMatrix(itr->first);	// first is the key in Map.
		itr->second->setMatrix( matrix );		// second is the value in Map.
	}

	for( NodeMap::iterator itr=_kinematicPhysicsNodes.begin(); itr!=_kinematicPhysicsNodes.end(); ++itr)
	{
		//osg::Node* node = dynamic_cast<osg::Node*>(itr->second);
		//osg::Matrix localToWorld = osg::computeLocalToWorld( node->getParent(0)->getParentalNodePaths()[0] );
		osg::Matrix matrix;
		matrix=itr->second->getMatrix();
		mPhysicsModule->setMatrix_KinematicActor(itr->first, matrix);
	}
}

void mfcEventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	/* old version-------------------------------------------------------------------------
	//osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
	//	new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
	//osgUtil::IntersectionVisitor iv(intersector.get());
	////iv.setTraversalMask( ~0x1 );
	//view->getCamera()->accept( iv );

	//if( intersector->containsIntersections() )
	//{
	//	const osgUtil::LineSegmentIntersector::Intersection& result =
	//		*(intersector->getIntersections().begin());
	//	osg::Vec3 point = result.getWorldIntersectPoint();

	//	if(pickChannel == true) global_Road->vec_Boundary1->push_back(osg::Vec3f(point.x(), point.y(), 0));
	//	else global_Road->vec_Boundary2->push_back(osg::Vec3f(point.x(), point.y(), 0));

	//	pointsGroup->addChild(DrawPickedPoint(osg::Vec3f(point.x(), point.y(), 0)));
	//}
		---------------------------------------------------------------------------------------------*/

	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (view->computeIntersections(ea.getX(),ea.getY(),intersections))
	{
		osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		osg::Vec3f temp(hitr->getWorldIntersectPoint());

		if(pickChannel == true) global_Road->vec_Boundary1->push_back(osg::Vec3f(temp.x(), temp.y(), 0));
		else global_Road->vec_Boundary2->push_back(osg::Vec3f(temp.x(), temp.y(), 0));

		pointsGroup->addChild(DrawPickedPoint(osg::Vec3f(temp.x(), temp.y(), 0)));
	}
}

//osg::ref_ptr<osg::Geode> mfcEventHandler::createBoudingShpere(osg::Node * node)
//{
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//
//	//¼ÆËãÄ£ÐÍµÄ°üÎ§ºÐ
//	const osg::BoundingSphere bs = node->getBound();
//
//	//µÃµ½ÇòÌå°ë¾¶
//	float radius =bs.radius();
//
//	//ÉèÖÃ¾«Ï¸¶È
//	osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
//	hints->setDetailRatio(0.5f);
//
//	osg::Vec3f v = bs._center;
//	osg::ShapeDrawable *shapeBall=new osg::ShapeDrawable(new osg::Sphere(v,radius),hints.get());
//
//	shapeBall->setColor(osg::Vec4(1.0f,1.0f,0.5f,0.3f));
//	geode->addDrawable(shapeBall);
//
//	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
//	//¹Ø±Õ¹âÕÕ
//	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
//	stateset->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
//	//ÉèÖÃÍ¸Ã÷äÖÈ¾
//	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//
//	return geode.release();
//}

//osg::ref_ptr<osg::MatrixTransform> mfcEventHandler::createOBB(osg::Node* _node, osg::Vec3& boxcenter)
//{
//	//osg::Switch* _node; // as an example ³õÊ¼Î»ÖÃ¼ÆËã
//
//	osg::BoundingBox bb;
//	osg::ComputeBoundsVisitor cbbv;
//	_node->accept( cbbv );	// _node»òÕßÆächild
//	osg::BoundingBox localBB = cbbv.getBoundingBox();
//	osg::Matrix localToWorld = osg::computeLocalToWorld( _node->getParent(0)->getParentalNodePaths()[0] );
//	for ( unsigned int i=0; i<8; ++i ) bb.expandBy( localBB.corner(i) * localToWorld );	// ÕâÖ»ÊÇ³õÊ¼µÄÊÀ½ç×ø±êÎ»ÖÃ
//
//	boxcenter = bb.center();
//
//	osg::ref_ptr<osg::Geode> bbGeode = new osg::Geode;
//	bbGeode->addDrawable(new osg::ShapeDrawable(new osg::Box));
//
//	osg::ref_ptr<osg::MatrixTransform> boundingboxNode = new osg::MatrixTransform;
//	boundingboxNode->addChild(bbGeode.get());
//
//	boundingboxNode->setMatrix(osg::Matrix::scale(bb.xMax()-bb.xMin(), bb.yMax()-bb.yMin(), bb.zMax()-bb.zMin()) *
//		osg::Matrix::translate(bb.center()));
//	boundingboxNode->getOrCreateStateSet()->setAttributeAndModes(
//		new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE) );
//	boundingboxNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//
//	return boundingboxNode.release();
//}

osg::ref_ptr<osg::MatrixTransform> mfcEventHandler::createBoudingBox(osg::Node* node)
{
	osg::ref_ptr<osg::MatrixTransform> bbtran = static_cast<osg::MatrixTransform*>(node);

	osg::ref_ptr<BoundingBoxCallback> bbcb = new BoundingBoxCallback;
	bbcb->_nodesToCompute.push_back( bbtran.get() );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;	// to draw the box shape
	geode->addDrawable( new osg::ShapeDrawable(new osg::Box) );

	osg::ref_ptr<osg::MatrixTransform> boundingBoxNode = new osg::MatrixTransform;  // to hold the box shape
	boundingBoxNode->addChild( geode.get() );
	boundingBoxNode->addUpdateCallback( bbcb.get() );
	boundingBoxNode->getOrCreateStateSet()->setAttributeAndModes(
		new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE) );
	boundingBoxNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	return boundingBoxNode.release();
}

osg::Geode* mfcEventHandler::DrawPickedPoint(osg::Vec3f position)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> sphereshape = new osg::ShapeDrawable;
	sphereshape->setShape(new osg::Sphere(position, 0.20f));
	sphereshape->setColor(osg::Vec4(0.7f, 0.1f, 0.8f, 1.0f));

	geode->addDrawable(sphereshape.get());
	return geode.release();
}

osg::Geode* mfcEventHandler::drawBoundary(osg::Vec3Array* vec3data)
{
	osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode;
	osg::Geometry* linesGeom = new osg::Geometry();

	linesGeom->setVertexArray(vec3data);

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(30.0f);
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f,0.8f,0.0f,1.0f));
	linesGeom->setColorArray(colors);
	linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3f(0.0f,-1.0f,0.0f));
	linesGeom->setNormalArray(normals);
	linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vec3data->getNumElements()));

	lineGeode->addDrawable(linesGeom);

	return lineGeode.release();
}

osg::Geode* mfcEventHandler::drawCenterLine(osg::Vec3Array* vec3data)
{
	osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode;
	osg::Geometry* linesGeom = new osg::Geometry();

	linesGeom->setVertexArray(vec3data);

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(0.20f);
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	osg::LineStipple* stipple= new osg::LineStipple(3, 0x0C0F); // draw dashed line
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(stipple, osg::StateAttribute::ON);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f,1.0f,0.2f,1.0f));
	linesGeom->setColorArray(colors);
	linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3f(0.0f,0.0f,-1.0f));
	linesGeom->setNormalArray(normals);
	linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vec3data->getNumElements()));

	lineGeode->addDrawable(linesGeom);

	return lineGeode.release();
}

osg::Geode* mfcEventHandler::drawTurningCircle(osg::Vec3f turningcenter, float turningradius, int numpoints)
{
	osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode;
	osg::Geometry* linesGeom = new osg::Geometry();

	osg::Vec3Array* vec3data = new osg::Vec3Array;
	for(int i=0; i<numpoints; i++)
	{
		float angle = float(i) * (float(2 * osg::PI)) / float(numpoints);
		float x = turningradius * cos(angle) + turningcenter.x();
		float y = turningradius * sin(angle) + turningcenter.y();
		vec3data->push_back(osg::Vec3f(x, y, 0.0f));
	}

	linesGeom->setVertexArray(vec3data);

	/*osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(20.0f);
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON); */

	osg::LineStipple* stipple= new osg::LineStipple(2, 0xAAAA); // draw dashed line
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(stipple, osg::StateAttribute::ON);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f,0.0f,0.9f,1.0f));
	linesGeom->setColorArray(colors);
	linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3f(0.0f,0.0f,1.0f));
	linesGeom->setNormalArray(normals);
	linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,vec3data->getNumElements()));

	lineGeode->addDrawable(linesGeom);

	return lineGeode.release();
}

void mfcEventHandler::calDefaultPaths(osg::Vec3Array* centerline, osg::Vec3f turningcenter, float wheel_base,
	osg::Vec3Array* frontpath, osg::Vec3Array* rearpath,
	osg::Vec3Array* arcfront, osg::Vec3Array* arcrear)
{
	// determine the start point and end point of the arc
	osg::Vec3f vec1 = (*centerline)[2] - (*centerline)[0];
	osg::Vec3f vec2 = (*centerline)[4] - (*centerline)[2];

	vec1.normalize();
	vec2.normalize();

	osg::Vec3f turning_startp = (*centerline)[0] + vec1* ((turningcenter - (*centerline)[0]) * vec1);  // p denotes point, differ from vector
	osg::Vec3f turning_endp = (*centerline)[2] + vec2* ((turningcenter - (*centerline)[2]) * vec2);

	osg::Vec3f startv = turning_startp - turningcenter;
	osg::Vec3f endv = turning_endp - turningcenter;

	// Determine the sign of the rotate_angle
	//float epsilon = 0.1;
	//osg::Quat rotateQ(rotate_angle, osg::Z_AXIS);
	//osg::Vec3f testendv = rotateQ * startv;
	//float dis = (testendv - endv).length();
	//float ro = acos(vec1 * vec2);
	//if(fabs(dis)< epsilon) rotate_angle = ro;
	//else rotate_angle = -ro;
	float rotate_angle;
	float test_angle = acos(vec1 * vec2);
	//  rotate_angle is a signed angle, its direction is consistent with angle between vec1 and vec2
	osg::Vec3f crossproduct = vec1 ^ vec2;
	crossproduct.z() >= 0 ? rotate_angle=test_angle : rotate_angle=-test_angle;

	int n = 10; // Number of segments to represent the arc
	float delta_angle = float(rotate_angle / n);
	float delta_arc = fabs((global_Road->Rmin) * delta_angle);  // used as the interval length

	frontpath->push_back((*centerline)[0] + vec1 * (global_Vehicle->v_wheelBase->at(0)));

	// make points uniformly distributed along the centerline (have the same interval length with the arc part)
	while((frontpath->back() - turning_startp).length() > delta_arc)
	{
		frontpath->push_back(frontpath->back() + vec1 * delta_arc);
	}

//#ifdef _DEBUG
//	osg::Vec3f vTemp = (*centerline)[0] + vec1 * (global_Vehicle->wheelbase);
//	std::cout<< vTemp[0] <<" " << vTemp[1] << " " << vTemp[2] << std::endl;
//#endif

	for(int i = 0; i <= n; i++)		// arc part of frontpath: to divide the arc part into small pieces with the same length. Because i starts from 0, the 1st point of the arc is included
	{
		osg::Vec3f temppoint = (osg::Quat(i * delta_angle, osg::Z_AXIS) * startv) + turningcenter;	 // the Equidistant Points on the arc part
		arcfront->push_back(temppoint);   // the 1st element of arcfront is on the arc, not (*centerline)[0] + vec1 * (global_Vehicle->wheelbase)
		frontpath->push_back(temppoint);

//#ifdef _DEBUG
//		osg::Vec3f vTemp = (osg::Quat(i * delta_angle, osg::Z_AXIS) * startv) + turningcenter;
//		std::cout<< vTemp[0] <<" " << vTemp[1] << " " << vTemp[2] << std::endl;
//#endif
	}

	// make points uniformly distributed along the curve (have same interval length)
	while((frontpath->back() - (*centerline)[4]).length() > delta_arc)
	{
		osg::Vec3f temp1 = frontpath->back() + vec2 * delta_arc;
		arcfront->push_back(temp1);
		frontpath->push_back(temp1);
	}

	frontpath->push_back((*centerline)[4]);
	arcfront->push_back((*centerline)[4]);

	global_Road->rearwheelpath(frontpath, rearpath, (*centerline)[0], wheel_base);  // calculate the rear path: the rear start point is (*centerline)[0]
	osg::Vec3f arcrear_start = arcfront->front() - vec1 * wheel_base;
	global_Road->rearwheelpath(arcfront, arcrear,arcrear_start, wheel_base);

	float standard1 = cos (0.0174);	// about 1 degree, almost aligned with vec2
	while((arcfront->back() - arcrear->back()) * vec2 / wheel_base > standard1)  // guarantee that  the arcfront or arcfront only contains the curve data, not including straight line data
	{
		arcrear->pop_back();
		arcfront->pop_back(); // delete those aligned elements
	}
	// arcrear and arcfront are the same size
}

osg::Geode* mfcEventHandler::drawLines(osg::Vec3Array* vec3data, float width_f,  osg::Vec4 linecolor, GLint stipplefactor, GLushort stipplepattern)
{
	osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode;
	osg::Geometry* linesGeom = new osg::Geometry();

	linesGeom->setVertexArray(vec3data);

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(width_f);
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	if(stipplefactor !=0 && stipplepattern !=0)
	{
	osg::LineStipple* stipple= new osg::LineStipple(stipplefactor, stipplepattern); // draw dashed line
	linesGeom->getOrCreateStateSet()->setAttributeAndModes(stipple, osg::StateAttribute::ON);
	}

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(linecolor);
	linesGeom->setColorArray(colors);
	linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3f(0.0f,0.0f,1.0f));
	linesGeom->setNormalArray(normals);
	linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vec3data->getNumElements()));

	lineGeode->addDrawable(linesGeom);

	return lineGeode.release();
}

void mfcEventHandler::calCriticalPoints (osg::Vec3Array* arcrear, osg::Vec3Array* arcfront, float dis,
	osg::Vec3Array* rearCritical, osg::Vec3Array* frontCritical)
{
	// because arcrear and arcfront have the same size
	for(int i=0; i != arcfront->getNumElements()-1; ++i)
	{
		osg::Vec3f vec = (*arcrear)[i] - (*arcfront)[i];
		vec.normalize();

		// calculate the rear wheel (inside)
		float angle;
		osg::Vec3f crossp = ((*arcfront)[i+1] - (*arcfront)[i]) ^ vec; // the angle between velocity and the vehicle body determines the shape of the arc
		crossp.z() >= 0 ? angle=osg::PI_2 : angle=-osg::PI_2;

		osg::Quat qr = osg::Quat(angle, osg::Z_AXIS);
		osg::Vec3f rearCriticalPoint = qr * (-vec) * (dis / 2) + (*arcrear)[i];
		rearCritical->push_back(rearCriticalPoint);

		// calculate the front wheel (outside), the angle is pi/2, equal to "angle"
		osg::Vec3f frontCriticalPoint = qr * vec * (dis / 2) + (*arcfront)[i];
		frontCritical->push_back(frontCriticalPoint);
	}

	// for the last position
	int i = arcfront->getNumElements() - 1;
	if(i-1 >=0)
	{
	osg::Vec3f vec = (*arcfront)[i] - (*arcrear)[i];
	vec.normalize();

	float angle;
	osg::Vec3f crossp = ((*arcfront)[i] - (*arcfront)[i-1]) ^ ((*arcrear)[i-1] - (*arcfront)[i-1]);
	crossp.z() >= 0 ? angle=osg::PI_2 : angle=-osg::PI_2;

	osg::Quat qr = osg::Quat(angle, osg::Z_AXIS);
	osg::Vec3f rearCriticalPoint = qr * vec * (dis / 2) + (*arcrear)[i];
	rearCritical->push_back(rearCriticalPoint);

	osg::Vec3f frontCriticalPoint = qr * (-vec) * (dis / 2) + (*arcfront)[i];
	frontCritical->push_back(frontCriticalPoint);
	}
}

//void mfcEventHandler::calInterPoints()

void mfcEventHandler::line_intersection(osg::Vec3f p1, osg::Vec3f p2, osg::Vec3f p3, osg::Vec3f p4, osg::Vec3f& point, float& dis)
{
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1.x(), x2 = p2.x(), x3 = p3.x(), x4 = p4.x();
	float y1 = p1.y(), y2 = p2.y(), y3 = p3. y(), y4 = p4.y();

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	osg::Vec3f temp;

	if (d != 0)
	{
		float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
		float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

		if ( !(x < min(x3, x4) || x > max(x3, x4) || y < min(y3, y4) || y > max(y3, y4)))	// (x, y) should be on the segment of p3 p4
		{
			 temp.set(x, y, 0.0f);
			 osg::Vec3f vec = osg::Vec3f(x-x2, y-y2, 0.0f);

			 if((x - x1)*(x2 - x1) > 0 || (y - y1)*(y2 - y1) > 0)  // make sure that this point is not at the left side of p1
			 {
				 point = temp;
				 if((x - x1)*(x - x2) > 0 || (y - y1)*(y - y2) > 0)	dis = -vec.length();  // (x, y) is at right side of point p2
				 else	dis = vec.length();
			 }
		}
	}
}

void mfcEventHandler::inter_F_B2 (osg::Vec3 p_center, osg::Vec3Array* l_F, osg::Vec3Array* l_B2,
	osg::Vec3Array* intersection_FB, osg::FloatArray* distance_FB)
{
	osg::Vec3f pointxy; // initialize the z value to 1, which is an impossible value
	float distance = 0.0f;

	for (int i=1; i != l_B2->getNumElements()-1; ++i) // start from the 2nd point of the boundary
	{
		for(int k=0; k!=l_F->getNumElements()-1; ++k)
		{
			pointxy.set(0.0f, 0.0f, 1.0f);  // every loop it must be set to zero: if no intersection, the pointxy value remains (0, 0, 1)
			line_intersection(p_center, (*l_B2)[i], (*l_F)[k], (*l_F)[k+1], pointxy, distance);
			if(pointxy.z() == 0)		// intersection happens, the z value of pointxy has been changed
			{
				intersection_FB->push_back(pointxy);
				distance_FB->push_back(distance);
				break; // just calculate one point on a segment.
			}
		}
	}
}

void mfcEventHandler::inter_R_B1 (osg::Vec3 p_center, osg::Vec3Array* l_R, osg::Vec3Array* l_B1,
	osg::Vec3Array* intersection_RB, osg::FloatArray* distance_RB)
{
	osg::Vec3f pointxy; // initialize the z value to 1, which is an impossible value
	float distance = 0.0f;

	for (int i=1; i != l_B1->getNumElements()-1; ++i) // start from the 2nd point of the boundary
	{
		for(int k=0; k!=l_R->getNumElements()-1; ++k)
		{
			pointxy.set(0.0f, 0.0f, 1.0f);  // every loop it must be set to zero
			line_intersection(p_center, (*l_B1)[i], (*l_R)[k], (*l_R)[k+1], pointxy, distance);
			if(pointxy.z() == 0)
			{
				intersection_RB->push_back(pointxy);
				distance_RB->push_back(distance);
				break; // just calculate one point on a segment.
			}
		}
	}
}

void mfcEventHandler::analysisPath(float& max_value, float& min_value, osg::Vec3f& pointOnF, osg::Vec3f& pointOnR,
	osg::Vec3f& center_point, bool& able_pass, osg::Vec3f& dir)
{
	int flag;
	if(max_value < 0 && min_value > 0) flag = 1;
	else if(max_value > 0 && min_value < 0) flag = 2;
	else if(max_value > 0 && min_value > 0) flag = 3;
	else flag = 4;

	osg::Vec3f OI = pointOnR - center_point;
	osg::Vec3f OO = pointOnF - center_point;

	OI.normalize();
	OO.normalize();

	switch(flag)
	{
	case 1:
		{
			able_pass = true;
			break;
		}

	case 2:
		{
			able_pass = false;
			break;
		}

	case 3:
		{
			if((OI * OO * max_value - min_value) > 0)	able_pass = false;
			else	dir = OI * max_value;
			break;
		}

	case 4:
		{
			if(OO * OI * (-min_value) - (-max_value) < 0)	able_pass = false;
			else	dir = OO * min_value;  // negative direction, min_value < 0
			break;
		}
	}
}

osg::AnimationPath* mfcEventHandler::createPath(osg::Vec3Array* vec_F, osg::Vec3Array* vec_R, const osg::Vec3f& bb_center, const osg::Vec3f& position_front)
{
	typedef osg::AnimationPath::ControlPoint ControlPoint;
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

	float distanceOf_front_center = osg::Vec3( bb_center[0] - position_front[0], bb_center[1] - position_front[1], 0.0f ).length();

	osg::Vec3f vec_source(1.0f, 0.0f, 0.0f);	// the initial position direction vector of the vehicle model
	osg::Vec3f vec_dir(0.0f, 0.0f, 0.0f);		// initialized value
	osg::Quat q;

	for(int i=0; i != vec_F->getNumElements(); ++i)
	{
		vec_dir = (*vec_F)[i] - (*vec_R)[i];
		vec_dir.normalize(); // Quat::makerotate requires the vec1 and vec2 normalized.
		q.makeRotate(vec_source, vec_dir);

		osg::Vec3 control_center = (*vec_F)[i] -  vec_dir * distanceOf_front_center;
		path->insert(3.0 * i, ControlPoint( control_center+osg::Vec3(0.0f, 0.0f, bb_center[2]), q));	// ±¾À´×÷±ä»»ÊÇÒÔÄ£ÐÍÔ­µãµÄ£¬ÏÖÔÚÒÔ°üÎ§ºÐÖÐÐÄ½øÐÐ±ä»»£¨·½±ãÔÚphysxÖÐµÄÍ¬²½£©, Òò´ËÄ£ÐÍ¼ÓÔØºóÒª´¦ÀíÒ»ÏÂ£¬ÈÃËü°ÑÄ£ÐÍÔ­µã×ªÒÆµ½¼¸ºÎÖÐÐÄÉÏÀ´¡£
	}

	return path.release();
}

osg::AnimationPath* mfcEventHandler::testAnimate()
{
	typedef osg::AnimationPath::ControlPoint ControlPoint;
	osg::Vec3 yoz(1.0, 0.0, 0.0), xoz(0.0, 1.0, 0.0), xoy(0.0, 0.0, 1.0);
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

	path->insert( 0.0,
		ControlPoint(osg::Vec3(0.0,0.0,0.0), osg::Quat(    0.0,yoz,0.0,xoz,    0.0,xoy)) );
	path->insert( 20.0,
		ControlPoint(osg::Vec3(5.0,0.0,0.0), osg::Quat(osg::PI,yoz,0.0,xoz,    0.0,xoy)) );
	path->insert( 40.0,
		ControlPoint(osg::Vec3(5.0,0.0,5.0), osg::Quat(osg::PI,yoz,0.0,xoz,osg::PI,xoy)) );
	path->insert( 60.0,
		ControlPoint(osg::Vec3(0.0,0.0,5.0), osg::Quat(    0.0,yoz,0.0,xoz,osg::PI,xoy)) );
	path->insert( 80.0,
		ControlPoint(osg::Vec3(0.0,0.0,0.0), osg::Quat(    0.0,yoz,0.0,xoz,    0.0,xoy)) );
	return path.release();
}

osg::Camera* mfcEventHandler::DisplayInfo()
{
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable(
		osgCookBook::createText(osg::Vec3(150.0f, 500.0f, 0.0f), "The Cessna monoplane", 20.0f) );
	textGeode->addDrawable(
		osgCookBook::createText(osg::Vec3(150.0f, 450.0f, 0.0f), "Six-seat, low-wing and twin-engined", 15.0f) );

	osg::Camera* camera = osgCookBook::createHUDCamera(0, 1024, 0, 768);
	camera->addChild( textGeode.get() );
	camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	return camera;
}

void mfcEventHandler::saveArrayData(osg::Vec3Array* array, std::ofstream& datafile)
{
	for(osg::Vec3Array::iterator itr=array->begin(); itr!=array->end()-1; ++itr)
	{
		datafile << (*itr).x() << "\t" << (*itr).y() << "\t" << (*itr).z() << std::endl;
	}
	osg::Vec3Array::iterator itr = array->end()-1;	// for the last line, don't output \n character (because we don't want the file to generate an empty line, which will bring trouble for data reading)
	datafile << (*itr).x() << "\t" << (*itr).y() << "\t" << (*itr).z();

	datafile.close();
}

void mfcEventHandler::loadArrayData(osg::Vec3Array* array, std::ifstream& datafile)
{
	while(!datafile.eof())
	{
		float i, j, k;
		datafile >> i >> j >> k;
		array->push_back(osg::Vec3(i, j, k));
	}
	datafile.close();
}

// http://sourceforge.net/apps/mediawiki/delta3d/index.php?title=Accessing_Underlying_OpenSceneGraph_Geometry
osg::Vec3Array* mfcEventHandler::GetTriangleVertices(osg::Node* node)
{
	//unsigned int numVerts = 0;
	//unsigned int numTris = 0;

	osg::ref_ptr<osg::Geode> wallGeode;
	osg::ref_ptr<osg::Geometry> wallGeometry;
	osg::ref_ptr<osg::Vec3Array> wallVertexVector;
	//osg::ref_ptr<osg::UShortArray> wallIndexVector;
	//osg::Vec3 nextVertex;
	//unsigned short nextIndex;

	osgCookBook::GeodeFinder myGeodeFinder;
	node->accept (myGeodeFinder);	// find the Geode of a loaded model

	wallGeode = myGeodeFinder.getFirst();

	if (wallGeode != NULL)
	{
#ifdef _DEBUG
		//this tells us there is only one osg::Drawable attached to the Geode
		//PR( wallGeode->getNumDrawables() );

		////this tells us what kind of osg::Drawable we've got
		//PR( wallGeode->getDrawable(0)->className() );
#endif

		//the kind of Drawable we have is a Geometry, so lets get a reference to it:
		wallGeometry = (osg::Geometry *) wallGeode->getDrawable(0);

		//do this check if you're paranoid about the type of VertexArray the Geometry
		//has, but vertices are typically Vec3Arrays
		if (! strcmp (wallGeometry->getVertexArray()->className(), "Vec3Array") /*&& !strcmp (wallGeometry->getVertexIndices()->className(), "Array")*/) // strcmp(): A zero value indicates that both strings are equal.
		{
			wallVertexVector = (osg::Vec3Array *) wallGeometry->getVertexArray();
			//numVerts = wallVertexVector->getNumElements();

			//wallIndexVector = (osg::UShortArray*) wallGeometry->getVertexIndices();	// ´ó²¿·ÖÎÄ¼þÊÇ²»Ö§³ÖvertexindicesµÄ£¬ÒòÎªÉÏÃæµÄ¶¥µãÊý¾ÝÒÑ°´Èý½ÇÐÎmeshÅÅºÃ£¬¹ÊÖ»Ðè°´0£¬1£¬2...µÄË³ÐòÉèÖÃvertexindices
			//numTris = ( wallIndexVector->getNumElements() )/ 3;
			//PR(wallGeometry->getVertexIndices()->className());	//ÓÉ´Ë¾ä·¢ÏÖ indexarray ÊÇ UShortArrayÀàÐÍµÄ
			//PR(numVerts);
			//PR(numTris);
		}
		else
		{
			//handle the error however you want -- here all I do is print a warning and bail out.
			OSG_WARN << "Unexpected VertexArray className.\n" << std::endl;
			PRS("Unexpected VertexArray className");
			return NULL;
		}

//#ifdef _DEBUG
		//for (unsigned int i = 0; i < wallVertexVector->size(); i++)
		//{
		//	nextVertex = wallVertexVector->at(i);

		//	std::cout << "Vertex #" << i << ": (" << nextVertex[0] << ", " << nextVertex[1] << ", " << nextVertex[2] << ")\n";
		//}
		//for (unsigned int i = 0; i < wallIndexVector->size(); i++)
		//{
		//	nextIndex = wallIndexVector->at(i);

		//	std::cout << "Index #" << i << "\t" << nextIndex <<"\n";
		//}
//
//#endif
	}

	// ÒòÎªPhysXÐèÒªµÄÊÇ¶¥µãÊý×éºÍË÷ÒýÊý×éµÄÄ£Ê½£¬¶ø·ÇÈý½ÇÐÎÃ¿¸öÃæµÄÊý¾Ý£¬¹Ê²»Ê¹ÓÃfunctorÁË¡£
	// 8:10 PM 7/16/2013 Tuesday: ÆäÊµ»¹ÊÇ¿ÉÒÔÓÃµÄ£¬ÉÏÃæµÄÀí½âÓÐÆ«²î£¬Ö»ÊÇ²»ÄÜÏñtrangle_stripÄÇÑù¹²Ïí¶¥µã¶øÒÑ£¬ÉÏÃæµÄ·½·¨Ò²ÊÇÍ¬Àí£¬Ã»ÓÐ¹²Ïí¶¥µã¡£
	//osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode*> (node);
	//if(geode)
	//{
	//	for(unsigned int i=0; i<geode->getNumDrawables(); ++i)
	//	{
	//		osg::Drawable* drawable = geode->getDrawable(i);
	//		//osg::TriangleFunctor<VertsCollector> functor;
	//		//functor._vertexList = new osg::Vec3Array;
	//		//drawable->accept( functor );

	//		numVerts += functor._vertexList->getNumElements();
	//		/*osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);*/
	//
	//	}
	//}
	//else
	//{
	//	OSG_WARN << "Unknown node or it cannot be converted to osg::Geode*" << std::endl;
	//}
	return wallVertexVector;
}

void mfcEventHandler::cookBoundaryMesh(osg::Node* node)
{
	osg::ref_ptr<osg::Vec3Array> wallVertices =  GetTriangleVertices(node);
	const PxU32 numVerts = wallVertices->getNumElements();
	const PxU32 numTris = numVerts / 3;

	std::vector<PxVec3> mVerts;
	std::vector<PxU32> mTris;

	for(osg::Vec3Array::iterator itr=wallVertices->begin(); itr!=wallVertices->end(); ++itr)
	{
		mVerts.push_back(PxVec3( (*itr)[0], (*itr)[1], (*itr)[2] ) );
	}

	for(unsigned int i=0; i!=numVerts; ++i)
	{
		mTris.push_back(i);
	}

	PxTriangleMesh* triMesh = PxToolkit::createTriangleMesh32(mPhysicsModule->getPhysics(), mPhysicsModule->getCooking(), &mVerts[0], numVerts, &mTris[0], numTris);
	if(!triMesh)
	{
		OSG_WARN<<"Cooking the triangle mesh failed."<<std::endl;
		PRS("Cooking the triangle mesh failed.");
	}

	mVerts.clear();
	mTris.clear();

	// create actor
	PxRigidStatic* wallActor = mPhysicsModule->getPhysics()->createRigidStatic(PxTransform::createIdentity());
	if(!wallActor)
		OSG_WARN<<"creating wall actor failed"<<std::endl;

	PxTriangleMeshGeometry geom(triMesh);
	PxShape* wallShape = wallActor->createShape(geom, *(mPhysicsModule->getDefaultMaterial()));
	if(!wallShape)
		OSG_WARN<<"creating wall shape failed"<<std::endl;

	//setCCDActive(*wallShape);  // Ä¿Ç°ÒòÎª³µËÙ²»¸ß£¬ÔÝ²»¿ªÆôÒÔ½ÚÊ¡ÏµÍ³×ÊÔ´
	mPhysicsModule->setDrivingStoppable(*wallShape);

	mPhysicsModule->getScene()->addActor(*wallActor);

	int id = _staticPhysicsNodes.size();
	mPhysicsModule->_staticActors[id] = wallActor;

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;

	mt->addChild( node );
	mWalls->addChild(mt.get());
	_staticPhysicsNodes[id] = mt;
}

osg::ref_ptr<osg::Box> mfcEventHandler::getInitBoundingBox(osg::Node* node)
{
	osg::BoundingBox bb;
	osg::ComputeBoundsVisitor cbbv;
	node->accept( cbbv );
	osg::BoundingBox localBB = cbbv.getBoundingBox();
	//osg::Matrix localToWorld = osg::computeLocalToWorld( node->getParent(0)->getParentalNodePaths()[0] );	// the loaded model node has no parent
	for ( unsigned int i=0; i<8; ++i ) bb.expandBy( localBB.corner(i) /** localToWorld*/ );	// ÕâÖ»ÊÇ³õÊ¼µÄÊÀ½ç×ø±êÎ»ÖÃ

	//osg::MatrixTransform* trans = static_cast<osg::MatrixTransform*>(node);	// °Ñ´«µÝ½øÀ´µÄ Node ×ª±äÎªMatrixTransformÀàÐÍºó£¬Í¨¹ýÐÞ¸ÄMatrixÖµ¸Ä±äÆä(boundingbox)Î»ÖÃ¡£
	//trans->setMatrix(
	//	osg::Matrix::scale(bb.xMax()-bb.xMin(), bb.yMax()-bb.yMin(), bb.zMax()-bb.zMin()) *
	//	osg::Matrix::translate(bb.center()) );

	osg::ref_ptr<osg::Box> boxShape = new osg::Box(bb.center(), bb.xMax()-bb.xMin(), bb.yMax()-bb.yMin(), bb.zMax()-bb.zMin());
	return boxShape;
}