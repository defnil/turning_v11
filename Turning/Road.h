#pragma once
#include "osg/Array"
#include "osg/Geometry"

class CRoad : public osg::Referenced
{
public:
	CRoad(void);
	~CRoad(void);

	osg::Vec3Array* getBoundaryOneData() const { return vec_Boundary1; }
	osg::Vec3Array* getBoundaryTwoData() const { return vec_Boundary2; }
	osg::Vec3Array* getCenterLine(osg::Vec3Array* vec1, osg::Vec3Array* vec2);

	void rearwheelpath(osg::Vec3Array* frontdata, osg::Vec3Array* reardata, osg::Vec3f rear_start, float wheel_base);

	osg::Geometry* drawBoundaryMesh(osg::Vec3Array* vec3data, float& wall_thickness, float& wall_height);

	osg::Vec3f lineintersection(osg::Vec3f& p1, osg::Vec3f& p2, osg::Vec3f& p3, osg::Vec3f& p4);
	osg::Vec3f getturningCenter(osg::Vec3Array* vec3data);

public:
	const float Rmin;
	float ThicknessOfWall;
	float HeightOfWall;

	osg::ref_ptr<osg::Vec3Array> vec_Boundary1;
	osg::ref_ptr<osg::Vec3Array> vec_Boundary2;
	osg::ref_ptr<osg::Vec3Array> vec_CenterLine;
	osg::ref_ptr<osg::Vec3Array> vec_exdataOfBound;
	osg::Vec3f v_TCenter;
};