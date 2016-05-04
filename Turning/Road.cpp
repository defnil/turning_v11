#include "StdAfx.h"
#include <iostream>
#include "Road.h"
#include <cmath>
#include <osgUtil/SmoothingVisitor>

CRoad::CRoad(void):
	ThicknessOfWall(0.30f),
	HeightOfWall(3.0f),
	Rmin(10.0f)	// smallest turning radius of LTM1200
{
	vec_Boundary1 = new osg::Vec3Array;
	vec_Boundary2 = new osg::Vec3Array;
	vec_CenterLine = new osg::Vec3Array(5);	// use 5 points to draw a simple centerline
	vec_exdataOfBound = new osg::Vec3Array;
}

CRoad::~CRoad(void)
{
}

osg::Vec3Array* CRoad::getCenterLine(osg::Vec3Array* vec1, osg::Vec3Array* vec2)
{
	osg::Vec3Array* cline = new osg::Vec3Array(5);;
	if(!vec1 || !vec2) return NULL;

	unsigned int num1, num2;
	num1 = vec1->getNumElements();
	num2 = vec2->getNumElements();

	(*cline)[0].set(((*vec1)[0]+(*vec2)[0])/2);
	(*cline)[1].set(((*vec1)[1]+(*vec2)[1])/2);
	(*cline)[3].set(((*vec1)[num1-2]+(*vec2)[num2-2])/2);
	(*cline)[4].set(((*vec1)[num1-1]+(*vec2)[num2-1])/2);
	osg::Vec3f temp = lineintersection((*cline)[0], (*cline)[1], (*cline)[3], (*cline)[4]);
	if (temp.z() == 0)	(*cline)[2].set(temp);
	else (*cline)[2].set(osg::Vec3f(0.0f, 0.0f, 0.0f));	// two parallel roads, 180 degrees U turn, needs further handling.

	return cline;
}

osg::Geometry* CRoad::drawBoundaryMesh(osg::Vec3Array* vec3data, float& wall_thickness, float& wall_height)
{
	float t = 0.5 * wall_thickness;

	// calculate the left and right parallel lines. Because we don't know whether left or right is the inside part of the road, we just treat the road data as the center skeleton.
	// firstly get the left point, and then the right can be obtained.
	// 原始数据可以看出最后一组数据与第一组数据相同，这样便是首尾相接的图形。为了使最后一个交点方便的算出来，把第二个和第三个点追加到数据中。这个首尾相接的封闭mesh只由4个侧面构成。
	vec3data->push_back((*vec3data)[1]);
	vec3data->push_back((*vec3data)[2]);

	unsigned int array_capacity = vec3data->getNumElements();
	osg::ref_ptr<osg::Vec3Array> wall_LowerLeft = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> wall_LowerRight = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> wall_UpperLeft = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> wall_UpperRight = new osg::Vec3Array;

	for(unsigned i=0; i!=array_capacity-2; ++i)
	{
		osg::Vec3 dir1 = (*vec3data)[i+1] - (*vec3data)[i];
		osg::Vec3 dir2 = (*vec3data)[i+2] - (*vec3data)[i+1];
		dir1.normalize();
		dir2.normalize();

		osg::Quat quat(osg::PI_2, osg::Z_AXIS);

		osg::Vec3 p1 = quat * dir1 * t + (*vec3data)[i];
		osg::Vec3 p2 = quat * dir1 * t + (*vec3data)[i+1];
		osg::Vec3 p3 = quat * dir2 * t + (*vec3data)[i+1];
		osg::Vec3 p4 = quat * dir2 * t + (*vec3data)[i+2];

		// calculate intersection of p1p2 and p3p4
		osg::Vec3 Left_Point = lineintersection(p1, p2, p3, p4);
		if(Left_Point.z()==1.0f)
			{
				wall_LowerLeft->push_back(p2); // 此时是共线情况,p2, p3 必重合,这即是交点。
				wall_UpperLeft->push_back(p2 + osg::Vec3(0.0f, 0.0f, wall_height));
			}
		else
			{
				wall_LowerLeft->push_back(Left_Point);
				wall_UpperLeft->push_back(Left_Point + osg::Vec3(0.0f, 0.0f, wall_height));
			}

		// q点是右侧的点，与p对称
		osg::Vec3 q1 = (*vec3data)[i] * 2 - p1;
		osg::Vec3 q2 = (*vec3data)[i+1] *2 -p2;
		osg::Vec3 q3 = (*vec3data)[i+1] *2 -p3;
		osg::Vec3 q4 = (*vec3data)[i+2] *2 -p4;

		osg::Vec3 Right_Point = lineintersection(q1, q2, q3, q4);
		if(Right_Point.z()==1.0f)
			{
				wall_LowerRight->push_back(q2); // 此时是共线情况,p2, p3 必重合,这即是交点。
				wall_UpperRight->push_back(q2 + osg::Vec3(0.0f, 0.0f, wall_height));
			}
		else
			{
				wall_LowerRight->push_back(Right_Point);
				wall_UpperRight->push_back(Right_Point + osg::Vec3(0.0f, 0.0f, wall_height));
			}
	}

	osg::ref_ptr<osg::Vec3Array> vertices_Surface = new osg::Vec3Array;

	// bottomSurface
	for(unsigned int i=0; i!=wall_LowerLeft->getNumElements()-1; ++i)	// 每个面每次推入两个三角形信息,法向量尽量向外
	{
		vertices_Surface->push_back((*wall_LowerRight)[i]);
		vertices_Surface->push_back((*wall_LowerLeft)[i]);
		vertices_Surface->push_back((*wall_LowerRight)[i+1]);

		vertices_Surface->push_back((*wall_LowerRight)[i+1]);
		vertices_Surface->push_back((*wall_LowerLeft)[i]);
		vertices_Surface->push_back((*wall_LowerLeft)[i+1]);
	}

		// leftSurface
	for(unsigned int i=0; i!=wall_LowerLeft->getNumElements()-1; ++i)
	{
		vertices_Surface->push_back((*wall_LowerLeft)[i]);
		vertices_Surface->push_back((*wall_UpperLeft)[i]);
		vertices_Surface->push_back((*wall_LowerLeft)[i+1]);

		vertices_Surface->push_back((*wall_LowerLeft)[i+1]);
		vertices_Surface->push_back((*wall_UpperLeft)[i]);
		vertices_Surface->push_back((*wall_UpperLeft)[i+1]);
	}

		// rightSurface
	for(unsigned int i=0; i!=wall_LowerLeft->getNumElements()-1; ++i)
	{
		vertices_Surface->push_back((*wall_UpperRight)[i]);
		vertices_Surface->push_back((*wall_LowerRight)[i]);
		vertices_Surface->push_back((*wall_UpperRight)[i+1]);

		vertices_Surface->push_back((*wall_UpperRight)[i+1]);
		vertices_Surface->push_back((*wall_LowerRight)[i]);
		vertices_Surface->push_back((*wall_LowerRight)[i+1]);
	}

		// topSurface
	for(unsigned int i=0; i!=wall_LowerLeft->getNumElements()-1; ++i)
	{
		vertices_Surface->push_back((*wall_UpperLeft)[i]);
		vertices_Surface->push_back((*wall_UpperRight)[i]);
		vertices_Surface->push_back((*wall_UpperLeft)[i+1]);

		vertices_Surface->push_back((*wall_UpperLeft)[i+1]);
		vertices_Surface->push_back((*wall_UpperRight)[i]);
		vertices_Surface->push_back((*wall_UpperRight)[i+1]);
	}

	osg::ref_ptr<osg::Geometry> Wall_Mesh = new osg::Geometry;
	Wall_Mesh->setVertexArray(vertices_Surface.get());
	Wall_Mesh->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::TRIANGLES, 0, vertices_Surface->getNumElements()));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(0.24f, 0.57f, 0.25f, 1.0f));
	Wall_Mesh->setColorArray(colorArray);
	Wall_Mesh->setColorBinding(osg::Geometry::BIND_OVERALL);

	osgUtil::SmoothingVisitor smv;
	smv.smooth( *Wall_Mesh );
	return Wall_Mesh.release();
}

osg::Vec3f CRoad::lineintersection(osg::Vec3f& p1, osg::Vec3f& p2, osg::Vec3f& p3, osg::Vec3f& p4)
{
	float x1 = p1.x(), x2 = p2.x(), x3 = p3.x(), x4 = p4.x();
	float y1 = p1.y(), y2 = p2.y(), y3 = p3. y(), y4 = p4.y();

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if(d==0) return osg::Vec3f(0.0f, 0.0f, 1.0f); // no intersection return this special one with z != 0

	float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

	return osg::Vec3f(x, y, 0.0f); // in xy plane, so z == 0
}

osg::Vec3f CRoad::getturningCenter(osg::Vec3Array* vec3data)
{
	osg::Vec3f direction1 = (*vec3data)[2] - (*vec3data)[1];
	osg::Vec3f direction2 = (*vec3data)[3] - (*vec3data)[2];

	osg::Vec3f mirror1 = (*vec3data)[2] - (direction1/(direction1.length())) * (direction2.length()); // the mirror point of point 3
	osg::Vec3f bisectordir = (mirror1 + (*vec3data)[3])/2 - (*vec3data)[2];

	float costheta = -(direction1 * direction2) / ((direction1.length()) * (direction2.length()));
	float theta = acos (costheta) / 2;
	float modvalue = Rmin / sin(theta);

	return (bisectordir/bisectordir.length()) * modvalue + (*vec3data)[2];
}

void CRoad::rearwheelpath(osg::Vec3Array* frontdata, osg::Vec3Array* reardata, osg::Vec3f rear_start, float wheel_base)
{
	reardata->push_back(rear_start);
	/*osg::ref_ptr<osg::FloatArray> alpha = new osg::FloatArray;
	osg::ref_ptr<osg::FloatArray> qpplus = new osg::FloatArray;*/
//#ifdef _DEBUG
//	osg::Vec3f vTemp = rear_start;
//	std::cout <<  std::endl << vTemp[0] <<" " << vTemp[1] << " " << vTemp[2] << std::endl;
//#endif

	for(int i=0; i!=frontdata->getNumElements()-1; i++)
	{
		float S = ((*frontdata)[i+1] - (*frontdata)[i]).length();
		float L = wheel_base;

		osg::Vec3f F0B0 = (*reardata)[i] - (*frontdata)[i];
		osg::Vec3f F0F1 = (*frontdata)[i+1] - (*frontdata)[i];
		float temp_1 = F0B0 * F0F1 / (S * L);

		// alpha is the angle C
		float alpha;
		if(temp_1<=-1) alpha=osg::PI;
		else if(temp_1>=1) alpha=0;
			else alpha = acos (temp_1);
		float beta = atan(sin(alpha) / (2 * L / S - cos(alpha)));  // beta is the angle D

		float angle_F1;
		float gamma = osg::PI-alpha-2*beta;
		// gamma is a signed angle F0F1B1, its direction is consistent with angle B0F0F1
		osg::Vec3f crossp = F0B0 ^ F0F1;
		crossp.z() >= 0 ? angle_F1=gamma : angle_F1=-gamma;
		osg::Quat q = osg::Quat(angle_F1, osg::Z_AXIS); // clockwise, negative value, vice versa
		osg::Vec3f f1b1v = q * ((*frontdata)[i] - (*frontdata)[i+1]); //Use Quat to rotate
		reardata->push_back((*frontdata)[i+1] + (f1b1v / f1b1v.length())* L);

//#ifdef _DEBUG
//		osg::Vec3f vTemp = (*frontdata)[i+1] + (f1b1v / f1b1v.length())* L;
//		float temp_2 = sin(alpha) / (2 * L / S - cos(alpha));
//		std::cout <<  vTemp[0] <<" " << vTemp[1] << " " << vTemp[2] /*<< " " << temp_1 <<
//			" "<< alpha << " " << beta << " " << temp_2 */<< std::endl;
//#endif
	}
}