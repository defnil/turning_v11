// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include <osgGA/StateSetManipulator>

#include "SkyBox.h"
#include "MFC_OSG.h"

cOSG*  global_OSG;

cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd)
{
	mStateSet = new osg::StateSet();
}

cOSG::~cOSG()
{
	mViewers->setDone(true);
	Sleep(1000);
	mViewers->stopThreading();

	//delete mViewers; // mViewers are osg::ref_ptr, not need to be deleted by hand
}

void cOSG::InitOSG(std::string modelname)
{
	m_ModelName = modelname;		// Store the name of the model to load

	// Init different parts of OSG
	//InitManipulators();
	InitSceneGraph();
	InitCameraConfig();
}

//void cOSG::InitManipulators(void)
//{
	// Create a trackball manipulator
	//trackball = new osgGA::TrackballManipulator();

	// Create a Manipulator Switcher
	//keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

	// Add our trackball manipulator to the switcher
	//keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

	// Init the switcher to the first manipulator (in this case the only manipulator)
	//keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
//}

void cOSG::InitSceneGraph(void)
{
	// Init the main Root Node/Group
	mRoot  = new osg::Group;

	// Load the Model from the model name

	osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile(m_ModelName);
	if (!modelNode) return;

	// Optimize the model
	osgUtil::Optimizer optimizer;
	optimizer.optimize(modelNode);

	mSwitchNode_Plant = new osg::Switch;
	mSwitchNode_Plant->addChild(modelNode);

	// Ground
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("ground1.jpg");
	osg::ref_ptr<osg::Geode> groundPlane = osgCookBook::createQuadwithTexture(10, 600.0, 600.0, image);
	osg::ref_ptr<osg::PositionAttitudeTransform> quadPAT = new osg::PositionAttitudeTransform;
	osg::Vec3 plantCenter = modelNode->getBound().center();
	quadPAT->setPosition(osg::Vec3(plantCenter.x(), plantCenter.y(), -0.20f));
	quadPAT->addChild(groundPlane.get());
	mSwitchNode_Plant->addChild(quadPAT.get());

	// skybox
	osg::ref_ptr<osg::Geode> skyboxGeode = new osg::Geode;
	skyboxGeode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, -10.0f), modelNode->getBound().radius())));
	skyboxGeode->setCullingActive(false);

	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes( 0, new osg::TexGen );
	skybox->setEnvironmentMap( 0,
		osgDB::readImageFile("Cubemap_snow/posx.jpg"), osgDB::readImageFile("Cubemap_snow/negx.jpg"),
		osgDB::readImageFile("Cubemap_snow/posy.jpg"), osgDB::readImageFile("Cubemap_snow/negy.jpg"),
		osgDB::readImageFile("Cubemap_snow/posz.jpg"), osgDB::readImageFile("Cubemap_snow/negz.jpg") );
	skybox->addChild( skyboxGeode.get() );
	osg::ref_ptr<osg::PositionAttitudeTransform> skyboxPAT = new osg::PositionAttitudeTransform;
	skyboxPAT->setAttitude(osg::Quat(-osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)));
	skyboxPAT->addChild(skybox.get());
	mRoot->addChild(skyboxPAT.get());





	// Add the model to the scene
	//mRoot->addChild(osgCookBook::createSkyBox());
	mRoot->addChild(mSwitchNode_Plant.get());


}

osgViewer::View* cOSG::createMinorView( int x, int y, int viewWidth, int viewHeight, const osg::Vec3& lookDir,
	const osg::Vec3& up, osg::GraphicsContext* gc, osg::Node* scene )
{
	osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
	view->getCamera()->setGraphicsContext( gc );
	view->getCamera()->setViewport( x, y, viewWidth, viewHeight );
	view->setSceneData( scene );

	osg::Vec3 center = scene->getBound().center();
	double radius = scene->getBound().radius();

	osg::Camera* mainCamera = view->getCamera();
	mainCamera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mainCamera->setClearColor(osg::Vec4f(0.2f, 0.4f, 0.2f, 1.0f));
	mainCamera->setViewMatrixAsLookAt( center - lookDir*(radius*1.6f), center, up );
	mainCamera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(viewWidth)/static_cast<double>(viewHeight), 1.0f, 1000.0f );

	//view->setCameraManipulator(new osgGA::TrackballManipulator);
	//view->addEventHandler(new osgViewer::StatsHandler);
	//view->addEventHandler(new osgViewer::WindowSizeHandler);
	//view->addEventHandler(new osgViewer::ThreadingHandler);
	//view->addEventHandler(new osgViewer::RecordCameraPathHandler);

	return view.release();
}

void cOSG::InitCameraConfig(void)	// in fact this is view config, not just camera
{
	// Local Variable to hold window size data
	RECT rect;

	// Get the current window size
	::GetWindowRect(m_hWnd, &rect);

	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;

	// Create the Graphics Context
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	//mViewer = new osgViewer::Viewer(); // Only one view
	mViewers = new osgViewer::CompositeViewer();
	// Create the viewer for this window
	//mMainView = createMinorView(0, 0, traits->width, traits->height, gc.get(), mRoot.get());

	// Add a Stats Handler to the viewer (cannot set to CompositeViewer)
	//-------------------------------Create		MainView-----------------------------------------------------
	mMainView = new osgViewer::View();
	mMainView->addEventHandler(new osgViewer::StatsHandler);
	// Init a new Camera (Master for this View)
	mCamera = new osg::Camera;

	// Assign Graphics Context to the Camera
	mCamera->setGraphicsContext(gc);

	// Set the viewport for the Camera
	mCamera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

	// Set projection matrix and camera attribtues
	mCamera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mCamera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
	mCamera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<float>(traits->width)/static_cast<float>(traits->height), 1.0, 1000.0);

	// Add the Camera to the Viewer
	//mViewer->setCamera(mCamera.get());
	mMainView->setCamera(mCamera.get());

	// Add the Camera Manipulator to the Viewer
	//mMainView->setCameraManipulator(keyswitchManipulator.get());
	mMainView->setCameraManipulator(new osgGA::TrackballManipulator());

	// Set the Scene Data
	mMainView->setSceneData(mRoot.get());
	mMainView->addEventHandler( new osgGA::StateSetManipulator(mMainView->getCamera()->getOrCreateStateSet()) );		// with this handler we can use some keys to control the state: s, w, l, etc.

	handle_mfc = new mfcEventHandler(mRoot.get(), mViewers.get(),mSwitchNode_Plant.get());
	mMainView->addEventHandler(handle_mfc);		// Add user defined event handler
	//--------------------------------------Finish Creating MainView--------------------------------------------------------------------------------

	// 状态设置,在这里是开启灯光,全局光
	mRoot->setStateSet(mStateSet.get ());
	mRoot->addChild(CreateLight(mStateSet.get ()));

	// mMinorView1: top view
	//int view1_width = traits->width/3.5, view1_height = traits->height/3;
	//mMinorView1 = createMinorView(traits->width - view1_width, 0, view1_width, view1_height, -osg::Z_AXIS, osg::Y_AXIS, gc.get (), mRoot.get ());

	//mMinorView2 = createMinorView(traits->width - view1_width, traits->height - view1_height, view1_width, view1_height, -osg::Z_AXIS, osg::Y_AXIS, gc.get (), mRoot.get ());
	//mMinorView2->setCameraManipulator(new osgGA::TrackballManipulator);

	mViewers->addView(mMainView.get ());
	//mViewers->addView(mMinorView1.get ());
	//mViewers->addView(mMinorView2.get ());

	mViewers->realize();

	// Correct aspect ratio
	/*float fovy,aspectRatio,z1,z2;
	mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
	aspectRatio=float(traits->width)/float(traits->height);
	mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::PreFrameUpdate()
{
	// Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
	// Due any postframe updates in this routine
}

CRenderingThread::CRenderingThread( cOSG* ptr )
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
}

CRenderingThread::~CRenderingThread()
{
	_done = true;
	while( isRunning() )
		OpenThreads::Thread::YieldCurrentThread();
}

void CRenderingThread::run()
{
	if ( !_ptr )
	{
		_done = true;
		return;
	}

	osgViewer::CompositeViewer* viewer = _ptr->getCompositeViewer();
	do
	{
		_ptr->PreFrameUpdate();
		viewer->frame();
		_ptr->PostFrameUpdate();
	} while ( !testCancel() && !viewer->done() && !_done );
}

osg::Node* cOSG::CreateLight(osg::StateSet* rootStateSet)
{
	//创建灯光结点
	osg::Group* lightGroup = new osg::Group;
	//创建灯光
	osg::Light* myLight = new osg::Light;
	//设置灯光ID索引
	myLight->setLightNum(0);
	myLight->setPosition(osg::Vec4(0.0f, 0.0f, 100.0f, 1.0f));
	//全局光色
	myLight->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	//混合光色
	myLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	//镜面光色
	myLight ->setSpecular(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	//添加光资源
	osg::LightSource* lightS = new osg::LightSource;
	//设置光资源中光源
	lightS->setLight(myLight);
	//打开灯光
	lightS->setLocalStateSetModes(osg::StateAttribute::ON);
	//开启模式
	lightS->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	//添加组
	lightGroup->addChild(lightS);
	return lightGroup;
}

mfcEventHandler* cOSG::getMFChandler()
{
	return handle_mfc;
}