#include "StdAfx.h"
#include "MousePointPick.h"


MousePointPick::MousePointPick(osg::Group* rt) : 
root(rt),
m_bPick(true), 
status_Picking(not_Pick)
{
	PickedPoints = new osg::Vec3Array;
	pointsGroup = new osg::Switch;
}

//void MousePointPick::init()
//{
//	PickedPoints = 0;
//	status_Picking = not_Pick;
//	m_bPick = true;
//	if(pointsGroup)
//	{
//		root->removeChild(pointsGroup);
//	}
//}


bool MousePointPick::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if ( status_Picking!=is_Pick ||
		ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||
		ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ||
		!(ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
		return false;

	m_bPick = false;

	mv = dynamic_cast<osgViewer::Viewer*>(&aa);
	if(mv) pick(mv, ea);

	return false;
}

void MousePointPick::pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	osg::Group* root = dynamic_cast<osg::Group*>(viewer->getSceneData());       
	if (!root) return;

	//osg::ref_ptr<osg::Plane> virtualplane = new osg::Plane;


	if (viewer->computeIntersections(ea.getX(),ea.getY(),intersections))
	{
		
		osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		osg::Vec3f temp(hitr->getWorldIntersectPoint());
		PickedPoints->push_back(osg::Vec3f(temp.x(), temp.y(), 0));
		
		 pointsGroup->addChild(DrawPickedPoint_dot(osg::Vec3f(temp.x(), temp.y(), 0)));
		//root->addChild(DrawPickedPoint(osg::Vec3f(temp.x(), temp.y(), 10)));
		root->addChild(pointsGroup);
	}
}



//osg::Geode* MousePointPick::DrawPickedPoint_dot(osg::Vec3f position)
//{
//
//
//	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
//	(*colors)[0] = selectedColor;
//
//	osg::ref_ptr<osg::Geometry> _selector = new osg::Geometry;
//	//_selector->setDataVariance( osg::Object::DYNAMIC );
//	_selector->setUseDisplayList( false );
//	_selector->setUseVertexBufferObjects( true );
//	_selector->setVertexArray( new osg::Vec3Array(1) );
//	_selector->setColorArray( colors.get() );
//	_selector->setColorBinding( osg::Geometry::BIND_OVERALL );
//	_selector->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, 1) );
//
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	geode->addDrawable( _selector.get() );
//	geode->getOrCreateStateSet()->setAttributeAndModes( new osg::Point(10000.0f) );
//	geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//	return geode.release();
//}




osg::Geode* MousePointPick::DrawPickedPoint_dot(osg::Vec3f position)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> sphereshape = new osg::ShapeDrawable;
	sphereshape->setShape(new osg::Sphere(position, 20.0f));
	sphereshape->setColor(osg::Vec4(0.7f, 0.1f, 0.8f, 1.0f));

	geode->addDrawable(sphereshape.get());
	return geode.release();
}




osg::Node* MousePointPick::DrawPickedPoint(osg::Vec3f position)
{
	osg::Group * root = new osg::Group () ;
	//设置点的大小
	osg::ref_ptr <osg::Point > pointsize = new osg::Point ;
	pointsize  ->setSize (5.0) ;   
	root->getOrCreateStateSet()->setAttributeAndModes(pointsize.get (),osg::StateAttribute::ON);

	//设置几何结点
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();

	//可画几何结点加入
	pyramidGeode->addDrawable(pyramidGeometry); 
	root->addChild(pyramidGeode);

	int size = 20;
	osg::ref_ptr <osg::Vec3Array> trangle = new osg::Vec3Array ;
	trangle ->push_back (osg::Vec3 (position.x ()-2*size, position.y () , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()-0.5*size, position.y ()-1.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()-0.5*size, position.y ()-3.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x (), position.y ()-4.0*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()+0.5*size, position.y ()-3.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()+0.5*size, position.y ()-1.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()+2.0*size, position.y () , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()+0.5*size, position.y ()+1.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()+0.5*size, position.y () +3.5*size,3)) ;
	trangle ->push_back (osg::Vec3 (position.x (), position.y ()+4.0*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()-0.5*size, position.y ()+3.5*size , 3)) ;
	trangle ->push_back (osg::Vec3 (position.x ()-0.5*size, position.y ()+1.5*size , 3)) ;

	//把点充入画区
	pyramidGeometry->setVertexArray( trangle.get () );

	//设置所画格式
	osg::DrawElementsUInt * pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON ,0);
	pyramidBase->push_back(11);
	pyramidBase->push_back(10);
	pyramidBase->push_back(9);
	pyramidBase->push_back(8);
	pyramidBase->push_back(7);
	pyramidBase->push_back(6);
	pyramidBase->push_back(5);
	pyramidBase->push_back(4); 
	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	pyramidGeometry->addPrimitiveSet(pyramidBase);

	//颜色设置
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 0 red
	//设置颜色
	pyramidGeometry->setColorArray(colors);
	pyramidGeometry->setColorBinding(osg::Geometry::BIND_OVERALL );

	return (osg::Node *) root ;
}





void MousePointPick::setResetPick(bool pick)
{
	m_bPick = pick;
}

bool MousePointPick::getResetPick()
{
	return m_bPick;
}

osg::Vec3Array* MousePointPick::getPickedPoints()
{
	return PickedPoints;
}



