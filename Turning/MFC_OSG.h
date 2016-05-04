#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <string>
#include "mfcEventHandler.h"

class cOSG : public osg::Referenced
{
public:
	cOSG(HWND hWnd);
	~cOSG();

	void InitOSG(std::string filename);
	//void InitManipulators(void);
	void InitSceneGraph(void);
	void InitCameraConfig(void);
	void SetupWindow(void);
	void SetupCamera(void);
	void PreFrameUpdate(void);
	void PostFrameUpdate(void);
	void Done(bool value) { mDone = value; }
	bool Done(void) { return mDone; }

	osg::Node* CreateLight(osg::StateSet* rootStateSet);

	osgViewer::CompositeViewer* getCompositeViewer() { return mViewers; }
	osgViewer::View* getMainView() { return mMainView; }
	osg::Group* getRoot() { return mRoot; }
	osg::Switch* getSwitchNode_Plant() { return mSwitchNode_Plant; }
	osg::Camera* getCamera() { return mCamera; }
	mfcEventHandler* getMFChandler();

	osgViewer::View* createMinorView( int x, int y, int viewWidth, int viewHeight, const osg::Vec3& lookDir,
		const osg::Vec3& up, osg::GraphicsContext* gc, osg::Node* scene );

private:
	bool mDone;
	std::string m_ModelName;
	HWND m_hWnd;

	osg::ref_ptr<osgViewer::CompositeViewer> mViewers;
	osg::ref_ptr<osgViewer::View> mMainView;
	osg::ref_ptr<osgViewer::View> mMinorView1;
	osg::ref_ptr<osgViewer::View> mMinorView2;

	osg::ref_ptr<osg::Camera> mCamera;
	osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osg::Switch> mSwitchNode_Plant;
	//osg::ref_ptr<osgGA::TrackballManipulator> trackball;
	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;
	osg::ref_ptr<osg::StateSet> mStateSet;
	mfcEventHandler* handle_mfc;
};

class CRenderingThread : public OpenThreads::Thread
{
public:
	CRenderingThread( cOSG* ptr );
	virtual ~CRenderingThread();

	virtual void run();

protected:
	cOSG* _ptr;
	bool _done;
};

extern cOSG* global_OSG;