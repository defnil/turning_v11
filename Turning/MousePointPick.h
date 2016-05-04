#pragma once
#include <osg/io_utils>
#include <osg/Geode>
#include <osg/Group>
#include <osgViewer/Viewer>
#include <osg/Point>
#include <osg/ShapeDrawable>


enum e_pick{not_Pick, is_Pick, finish_Pick};		// enum type must be put in front of functions using it.
//const osg::Vec4 selectedColor(1.0f, 0.0f, 0.0f, 1.0f);

class MousePointPick : public osgGA::GUIEventHandler
{
public:
	
	MousePointPick(osg::Group* rt);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void init();


	void pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& aa);
	osg::Node* DrawPickedPoint(osg::Vec3f position);
	osg::Geode* DrawPickedPoint_dot(osg::Vec3f position);
	

	void setResetPick(bool pick);
	bool getResetPick();

	void setPickStatus(enum e_pick viewpick) {status_Picking = viewpick;}
	enum e_pick getPickStatus() {return status_Picking;}

	osg::Vec3Array* getPickedPoints();
	osg::Vec3Array* setPickedPoints();


protected:
	enum e_pick status_Picking;
	bool m_bPick;
	osg::ref_ptr<osg::Vec3Array> PickedPoints;
	osgViewer::Viewer *mv;
	osg::ref_ptr<osg::Switch> pointsGroup;
	osg::ref_ptr<osg::Group> root;
	//osg::ref_ptr<osg::Geode> pickedPoints;
};

