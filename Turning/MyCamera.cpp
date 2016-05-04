/*********************************************************

MyCamera.cpp

*********************************************************/
#include "stdafx.h"
#include "MyCamera.h"
#include "mfcEventType.h"

CMyCamera::CMyCamera(void)
{
	m_fMoveSpeed		= 3.0;
	m_fAngle				= 2.5;
	m_vPosition			= osg::Vec3d();
	m_vRotation			= osg::Vec3d(osg::PI_2-0.10, 0.0, 0.0);		// osg::PI_2 is used to rotate the up direction to make the viewport looks natural.
	m_fCarRotateZ      = 0.0;
	m_fMyRotateZ       = 0.0;
}

CMyCamera::~CMyCamera(void)
{
}

double CMyCamera::getRotatefX()
{
	return m_vRotation._v[0];
}

double CMyCamera::getRotatefY()
{
	return m_vRotation._v[1];
}

double CMyCamera::getRotatefZ()
{
	return m_vRotation._v[2];
}

void CMyCamera::setByMatrix(const osg::Matrixd& matrix)
{
}

void CMyCamera::setByInverseMatrix(const osg::Matrixd& matrix)
{
}

osg::Matrixd CMyCamera::getMatrix(void) const
{
	osg::Matrixd mat;
	mat.makeRotate(m_vRotation._v[0], osg::Vec3d(1.0, 0.0, 0.0),
			   m_vRotation._v[1], osg::Vec3d(0.0, 1.0, 0.0),
			   m_vRotation._v[2], osg::Vec3d(0.0, 0.0, 1.0));
	//return mat * osg::Matrixd::translate(m_vPosition);

	mat.postMultTranslate(m_vPosition);
	return mat;	
}

osg::Matrixd CMyCamera::getInverseMatrix(void) const
{
	return osg::Matrixd::inverse(getMatrix()); //先得到当前的视图矩阵，然后返回其逆矩阵
	//osg::Matrixd matrix;
	//matrix.makeRotate(-m_vRotation._v[0], osg::Vec3d(1.0, 0.0, 0.0),
	//	-m_vRotation._v[1], osg::Vec3d(0.0, 1.0, 0.0),
	//	-m_vRotation._v[2], osg::Vec3d(0.0, 0.0, 1.0));

	//matrix.preMultTranslate(-m_vPosition);
	//return matrix;
}

void  CMyCamera::setRotatefZ(double rotatefZ)
{
	m_fCarRotateZ = rotatefZ;
	resetRotateZ();
}



//******************************************************************
//function description: 事件控制器
//
bool CMyCamera::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
	if(global_bUseJoystick2)
	{
		if(abs(global_dataSet2.lX)>100.0)
			m_fMyRotateZ = osg::PI_2*(double(global_dataSet2.lX))/1000.0;
		else m_fMyRotateZ = 0.0;

		if(abs(global_dataSet2.lY)>150.0)
			m_vRotation._v[0] = osg::PI_2 + osg::PI/3*double(global_dataSet2.lY)/1000.0;
		else m_vRotation._v[0] = osg::PI_2;
	}
	else
	{
		switch (ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::KEYDOWN): //如果是键盘按下事件
			{
				if (ea.getKey() == 'h') // left
				{
					m_fMyRotateZ += osg::DegreesToRadians(m_fAngle);
					resetRotateZ();
				}

				if (ea.getKey() == 'k')  // right
				{
					m_fMyRotateZ -= osg::DegreesToRadians(m_fAngle);
					resetRotateZ();
				}

				if (ea.getKey() == 'u')  // up
				{
					m_vRotation._v[0] += 0.015;
				}

				if (ea.getKey() == 'j') // down
				{
					m_vRotation._v[0] -= 0.015;
				}

				break;
			}

		default: break;
		}  // switch (ea.getEventType())
	}	

	return false;
}

void CMyCamera::resetRotateZ()
{
	m_vRotation._v[2] = m_fMyRotateZ + m_fCarRotateZ;
}

//******************************************************************
//function description: 更新视口位置
//
void CMyCamera::changePosition(const osg::Vec3d &delta)
{
	m_vPosition += delta;
}

void CMyCamera::setSpeed(double speed)
{
	m_fMoveSpeed = speed;
}

//******************************************************************
//function description:设置视口的位置
//
void CMyCamera::setPosition(const osg::Vec3d &position)
{
	m_vPosition = position;
}

void CMyCamera::setPosition(double *position)
{
	m_vPosition._v[0] = position[0];
	m_vPosition._v[1] = position[1];
	m_vPosition._v[2] = position[2];
}

//******************************************************************
//function description:得到视口所在的位置
//
osg::Vec3d CMyCamera::GetPosition()
{
	return m_vPosition;
}