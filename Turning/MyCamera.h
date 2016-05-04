/*********************************************************

MyCamera.h

used for camera control

*********************************************************/
#pragma once
#include <osgGA/CameraManipulator>
#include <osg/Array>
#include "JoystickData.h"

class CMyCamera : public osgGA::CameraManipulator
{
public:
	CMyCamera(void);
	~CMyCamera(void);

	void  changePosition(const osg::Vec3d& delta);	// translate the position by delta
	double getSpeed();								// get camera moving speed
	void  setSpeed(double speed);					// set camera moving speed
	void  setPosition(const osg::Vec3d& position);  // set camera position
	void  setPosition(double*);		// set camera position, overloaded func, receiving double array data

	void  setRotatefZ(double rotatefZ);            // set absolute rotation angle, by getting the attached object z rotation, and adding it to view left & right rotation angle
	
	// Get x, y, z component of vector m_vRotation
	double getRotatefX();
	double getRotatefY();
	double getRotatefZ();

	// Set the Z value of m_vRotation (= the object rotation.Z + left & right view rotation)
	void resetRotateZ();

	osg::Vec3d GetPosition();						// get camera position



	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us); // event handling
	
	// Description: Position and attitude matrix of the camera manipulator
	// this matrix is composed of two matrices: a rotation matrix (represented by m_vRotation), and a translation matrix (using m_vPosition)
	virtual osg::Matrixd getMatrix(void) const;
	virtual osg::Matrixd getInverseMatrix(void) const;	// View matrix of the camera: the most important matrix, which will be called in every frame

	// Used by KeySwitchMatrixManipulator 
	virtual void setByMatrix(const osg::Matrixd& matrix);
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);

private:
	double						m_fMoveSpeed;	    // camera moving speed
	double						m_fAngle;			// Delta angle (in degrees) of left & right view rotation
	osg::Vec3d				m_vPosition;        // position of camera
	double						m_fCarRotateZ;       // rotation angle (around Z axis) of the attached object (car)

	osg::Vec3d				m_vRotation;	    // camera rotation angle around X, Y, Z axis	
	double                      m_fMyRotateZ;        // camera left & right view rotation angle
};
