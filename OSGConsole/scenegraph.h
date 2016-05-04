#ifndef SCENEGRAPH
#define SCENEGRAPH

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include "car.h"

#define QUAD_COUNT 256
#define R1 45.0
#define R2 50.0

osg::ref_ptr<osg::Geometry> draw_road(double r1, double r2);
osg::ref_ptr<osg::Group> create_scene_graph(Car * car);

osg::ref_ptr<osg::MatrixTransform> draw_cylinder(double rad, double theta);
osg::ref_ptr<osg::MatrixTransform> draw_trees(double rad, double theta);
void apply_road_texture(osg::Geode * geode);

#endif
