#include <osg/ref_ptr>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Notify>
#include <iostream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>
#include <osg/Timer>
#include "scenegraph.h"
#include "car.h"

using namespace std;

/* Driving animation related. */
osg::Vec3d dir(0.0, 0.0, 0.0);
float alpha = 0.0, velocity = 0.0;

/* Handle Steering wheel events. */
class PickHandler : public osgGA::GUIEventHandler
{
public:
	PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer;
		viewer = dynamic_cast<osgViewer::Viewer*>( &aa );
		if (!viewer) return false;
		switch( ea.getEventType() )
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			{
				if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
					alpha -= 1.0;
				else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
					alpha += 1.0;
				else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
					velocity += 0.1;
				else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
					velocity += -0.1;

				break;
			}
		default:
			return false;
		}

		dir[0] = sin(alpha * M_PI/180.0);
		dir[1] = cos(alpha * M_PI/180.0);
		dir[2] = 0.0;

		return true;
	}
};

/* Program entry point. */
int main( int argc, char** argv )
{
	osgViewer::Viewer viewer;

	//viewer.setUpViewInWindow(0, 0, 512, 512);

	Car * car = new Car(osg::Vec3d(R1+(R2-R1)/2.0, 0.0, 0.0));
	viewer.setSceneData(create_scene_graph(car).get());

	viewer.getCamera()->setProjectionMatrixAsPerspective(80., 1., 1., 140. );
	viewer.getCamera()->setClearColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	osg::Vec3d eye(R1 + (R2-R1)/2.0, 0.0, 2.0);
	osg::Vec3d target(0.0, 0.0, 0.0);
	osg::Vec3d up(0.0, 0.0, 1.0);
	osg::Vec3d center;

	dir[0] = sin(alpha * M_PI/180.0);
	dir[1] = cos(alpha * M_PI/180.0);
	dir[2] = 0.0;

	viewer.addEventHandler(new PickHandler);

	osg::Timer_t start_tick = osg::Timer::instance()->tick();
	double delta_m = 1000.0;

	while (!viewer.done())
	{
		if(delta_m >= 100.0) {
			eye[0] += velocity * dir[0];
			eye[1] += velocity * dir[1];
			eye[2] += velocity * dir[2];

			center[0] = eye[0];
			center[1] = eye[1];
			center[2] = 0.0;

			car->center = center;

			target[0] = eye[0] + dir[0];
			target[1] = eye[1] + dir[1];
			target[2] = eye[2] + dir[2];

			/* Reset */
			start_tick = osg::Timer::instance()->tick();
		}

		osg::Timer_t end_tick = osg::Timer::instance()->tick();
		delta_m = osg::Timer::instance()->delta_m(start_tick, end_tick);

		viewer.getCamera()->setViewMatrixAsLookAt(eye, target, up);
		viewer.frame();
	}

	return 1;
}