#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/NodeCallback>
#include <osg/Timer>
#include <osg/Notify>
#include <iostream>
#include "car.h"

// Derive a class from NodeCallback to manipulate a
// MatrixTransform object's matrix.
class PositionCar: public osg::NodeCallback
{
	private:
		Car * car;

	public:
    	PositionCar(Car * car) {
			this->car = car;
		}

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
        	osg::MatrixTransform * mt =
                dynamic_cast<osg::MatrixTransform*>(node);

			osg::Matrix trans;
			trans.makeTranslate(car->center);

			mt->setMatrix(trans);

        	traverse(node, nv);
		}
};

/* Constructor  */
Car::Car(osg::Vec3d center)
{
	this->center = center;
}

/* Render the car object at correct location. */
osg::ref_ptr<osg::MatrixTransform> Car::draw(void)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Cylinder> cylinder;

	/* Create car shape of suitable size, at its center. */
   	cylinder = new osg::Cylinder(osg::Vec3d(), 2.0, 2.0);

	osg::ref_ptr<osg::ShapeDrawable> sd;
	sd = new osg::ShapeDrawable(cylinder.get());
	sd->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0)); // red

	geode->addDrawable(sd.get());

	/* Keep the car at its center. */
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
    mt->setDataVariance(osg::Object::DYNAMIC);

	osg::Matrix m;
	m.makeTranslate(this->center);
	mt->setMatrix(m);
    mt->setUpdateCallback(new PositionCar(this));

	mt->addChild(geode.get());

	return mt.get();
}