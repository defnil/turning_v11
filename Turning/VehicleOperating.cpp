#include "StdAfx.h"
#include "VehicleOperating.h"

vehicleDataType::vehicleDataType( osg::Node* vehicleNode, osg::MatrixTransform* mt, float xOffsetCenterFront, float BBCenterZ, CVehicle* vehicle ):
_bturnLeft(false),
	_bturnRight(false),
	_bmoveForward(false),
	_bmoveBackward(false),
	_bPause(false),
	e_DrivingMode(ROADDRIVING)
{
	// frontWheelIniPos, bbCenterIniPos both are initial position, which will be affected if _matrix != identity matrix, i.e., mt contains some transformation info.
	// vehicle node parameter initialization. Note: z component of all the vectors need to be set to zero.

	// all the following parameters are calculated by treating the original point as (0, 0, 0). Ignoring _matrix first.
	_vehicleNode							= vehicleNode;
	_mt										=	mt;
	//_matrix									=	mt->getMatrix();
	_iniTranslation						= mt->getMatrix().getTrans();
	_iniRotation							= mt->getMatrix().getRotate();
	//osg::Quat ab = _rotate.conj();

	_BBCenterZ							= double(BBCenterZ);
	
	// Note: bbcenter--->frontwheel is treated in the direction X-axis ( may have small error actually)
	_distanceCenterFront				= double(xOffsetCenterFront);
	
	// after the setPivotPoint(bbcenter), the actual position of bbcenter is at (0,0,0). So _posFrontOne and _positionBBCenterIni should be careful
	_pFrontOne							=	_iniRotation * osg::Vec3d(xOffsetCenterFront, 0.0, 0.0);	// Initial coordinates (in static status, not moving) of front wheel (midpoint of two front wheels)
	_pBBCenterIni						= _iniRotation * osg::Vec3d(0.0, 0.0, 0.0);
	_pBBCenterOne						=	_pBBCenterIni;  // Initial coordinates of bounding box

	_vRearToFrontIni					= _iniRotation * osg::Vec3d(1.0, 0.0, 0.0);		// treated the vehicle body lies in direction osg::X_axis
	

	
	//_vFrontSpeedOne					=	osg::Vec3d(1.0, 0.0, 0.0);
	_vFrontSpeedTwo					=	_vRearToFrontIni;
	_speedAngle							= 0.0;		// _speed angle is the angle between velocity vector and vehicle body vector. This is the value controlled by the steering wheel	
	_speedFront							=	0.0;

	//_vRearToFrontOne = _vRearToFrontIni;
	//_pRearOne = _pFrontOne - _vRearToFrontOne * _wheelBase;
	//_speedAngleFromXOne = 0.0;
	_speedAngleFromXTwo = 0.0;
	_frontThetaOne = asin(_vRearToFrontIni.y());

	_pFrontTwo = _pFrontOne;
	//_pRearTwo = _pRearOne;
	_pBBCenterTwo = _pBBCenterOne;
	//_vRearToFrontTwo = _vRearToFrontOne;
	//_vFrontSpeedTwo = _vFrontSpeedOne;
	//_speedAngleFromXTwo = _speedAngleFromXOne;
	_frontThetaTwo = _frontThetaOne;

	_Sfrontcenterpoint = 0.0;
	_vehicleData = vehicle;
	_array_tireCenterInWorld = new osg::Vec3dArray;
	_array_tireCenter = new osg::Vec3dArray;
	_array_tireTravelTheta = new osg::DoubleArray;
	initializeArray_tires();		// _mt_tires[] is initialized.
}


void vehicleDataType::initializeArray_tires()
{
	// http://osghelp.com/?p=72
	std::string tires_name[] = {"tire1L", "tire1R", "tire2L", "tire2R", "tire3L", "tire3R", "tire4L", "tire4R", "tire5L", "tire5R"};
	
	for(int i=0; i<10; i++)
	{
		osg::Node* found = osgCookBook::FindNodeByName(_vehicleNode, tires_name[i]);
		_pat_tires[i] = osgCookBook::AddMatrixTransform(found);
		osg::Vec3d bscenter = _pat_tires[i]->getBound().center();
		_array_tireCenter->push_back(bscenter);


		_array_tireCenterInWorld->push_back(bscenter * osg::computeLocalToWorld(  _pat_tires[i]->getParent(0)->getParentalNodePaths()[0] ));

		_array_tireTravelTheta->push_back(0.0);
	}
}


void vehicleDataType::postProcess()
{
	// _frontThetaOne cannot be changed suddenly, so use stage 1 value
	_speedAngleFromXTwo = _frontThetaOne + _speedAngle;		// range [-Pi, Pi]
	if(_speedAngleFromXTwo > osg::PI) _speedAngleFromXTwo -= 2.0 * osg::PI;
	else if(_speedAngleFromXTwo < -osg::PI) _speedAngleFromXTwo += 2.0*osg::PI;

	_vFrontSpeedTwo = osg::Vec3d(cos(_speedAngleFromXTwo), sin(_speedAngleFromXTwo), 0.0);

	_pFrontTwo = osg::Vec3d( _pFrontOne.x()+_vFrontSpeedTwo.x()*_speedFront, _pFrontOne.y()+_vFrontSpeedTwo.y()*_speedFront, 0.0);


	
	// Calculate the center position and orientation of the box
	if(_vehicleData->drivingMode==3)		// crab mode
		_pBBCenterTwo = _pBBCenterOne + _vFrontSpeedTwo * _speedFront;
	else
		_pBBCenterTwo = getBBCenterTwo(_vehicleData->drivingMode);
	
	_vRearToFrontTwo = _pFrontTwo - _pBBCenterTwo;
	_vRearToFrontTwo.normalize();

	osg::Quat q;
	q.makeRotate(osg::Vec3d(1.0, 0.0, 0.0), _vRearToFrontTwo); // no matter what the initial angle is when the model is loaded, we set the position directly

	double temp_frontThetaTwo = osg::Vec3d(1.0, 0.0, 0.0) * _vRearToFrontTwo;
	if(temp_frontThetaTwo<=-1.0) _frontThetaTwo = osg::PI;
	else if(temp_frontThetaTwo>=1.0) _frontThetaTwo = 0.0;
	else _frontThetaTwo = acos(temp_frontThetaTwo);		// [0, Pi]
	_frontThetaTwo = ((osg::Vec3d(1.0, 0.0, 0.0) ^ _vRearToFrontTwo).z() >= 0) ?  _frontThetaTwo : -_frontThetaTwo; // [-Pi, Pi]

	osg::Matrixd matrix;
	matrix.makeRotate(q);
	matrix.postMultTranslate(_pBBCenterTwo + _iniTranslation);
	_mt->setMatrix(matrix);


	// tires handling
		osg::Vec3d tirePosInWorld;
		osg::Matrixd wheelMatrix;
		double wheelAngle, axleDistance, tireTravelAngle, equivalentWheelbase, equivalentRearXPosition;
		if(_vehicleData->drivingMode!=3)
		{
			equivalentWheelbase = _vehicleData->v_wheelBase->at(_vehicleData->drivingMode);
			equivalentRearXPosition = _array_tireCenter->at(0).x() - equivalentWheelbase;
		}

		for(int i=0; i<10; i++)
		{
			// y-axis rotation --- self rolling
			tirePosInWorld = _pat_tires[i]->getBound().center() * osg::computeLocalToWorld(  _pat_tires[i]->getParent(0)->getParentalNodePaths()[0] );
			if(_speedFront>=0.0)
				_array_tireTravelTheta->at(i) += (_array_tireCenterInWorld->at(i) - tirePosInWorld).length() / _vehicleData->tireRadius;
			else
				_array_tireTravelTheta->at(i) -= (_array_tireCenterInWorld->at(i) - tirePosInWorld).length() / _vehicleData->tireRadius;
			tireTravelAngle = _array_tireTravelTheta->at(i);
			//if(tireTravelAngle>2*osg::PI) tireTravelAngle -= 2 * osg::PI;
			//if(tireTravelAngle<0.0) tireTravelAngle += 2 * osg::PI;

			_array_tireCenterInWorld->at(i) = tirePosInWorld;


			// z-axis rotation
			if(_vehicleData->drivingMode==3)
			{
				wheelAngle = _speedAngle;
			}
			else
			{
				axleDistance = _array_tireCenter->at(i).x() - equivalentRearXPosition;
				wheelAngle = _speedAngle * axleDistance / equivalentWheelbase;
			}




			// apply matrix transformation
			wheelMatrix.makeRotate(osg::Quat(tireTravelAngle, osg::Vec3d(0.0, 1.0, 0.0)) * osg::Quat(wheelAngle, osg::Vec3d(0.0, 0.0, 1.0)));
			wheelMatrix.preMultTranslate(-_array_tireCenter->at(i));
			wheelMatrix.postMultTranslate(_array_tireCenter->at(i));
			_pat_tires[i]->setMatrix(wheelMatrix);	
		} // for i

}


void vehicleDataType::turnFrontLeft()
{
	double maxAngle = _vehicleData->maxTurningAngle;
	_speedAngle += 0.004;
	if(_speedAngle>maxAngle) _speedAngle = maxAngle;
	else if(_speedAngle<-maxAngle) _speedAngle = -maxAngle;
}

void vehicleDataType::turnFrontRight()
{
	double maxAngle = _vehicleData->maxTurningAngle;
	_speedAngle -= 0.004;
	if(_speedAngle>maxAngle) _speedAngle = maxAngle;
	else if(_speedAngle<-maxAngle) _speedAngle = -maxAngle;
}

void vehicleDataType::noFrontTurning()
{
	if(_speedAngle > 0.0) _speedAngle -= 0.002;
	else if(abs(_speedAngle)<0.003) _speedAngle = 0.0;
	else _speedAngle += 0.002;

	double maxAngle = _vehicleData->maxTurningAngle;
	if(_speedAngle>maxAngle) _speedAngle = maxAngle;
	else if(_speedAngle<-maxAngle) _speedAngle = -maxAngle;
}

void vehicleDataType::movePause()
{
	if( _speedFront>0.0 )	_speedFront -= 0.0015;
	else if( abs(_speedFront)< 0.0018 )	_speedFront = 0.0;
	else	_speedFront += 0.0015;
}

void vehicleDataType::moveFrontForward()
{
	_speedFront += 0.00020;
	if( _speedFront > 0.08 )
		_speedFront = 0.08;
}

void vehicleDataType::moveFrontBackward()
{
	_speedFront -= 0.00020;
	if( _speedFront < -0.08 )
		_speedFront = -0.08;
}

void vehicleDataType::noFrontAcceleration()		// due to friction, the speed will decrease
{
	///**************do nothing

	if( _speedFront>0.0 )	_speedFront -= 0.000150;
	else if( abs(_speedFront)< 0.00020 )	_speedFront = 0.0;
	else	_speedFront += 0.000150;
}

osg::Vec3d vehicleDataType::getBBCenterTwo(int drivingMode)
{
	// Calculate _posRearTwo with  _posFrontOne, _posFrontTwo and _posRearOne
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	
	//if ( F0F1.length() < 0.001f ) return _posRearOne;	// just set it to the previous step value
	//if(_speedFront == 0.0) return _pRearOne;
	if(abs(_speedFront)<=1.0e-7) return _pBBCenterOne;

	if(_speedFront>=0.0)
	{
		osg::Vec3d F0F1 = _pFrontTwo - _pFrontOne;	// if no movement, F0F1 == 0. Cannot be the denominator!

		double S = (F0F1).length();
		//TRACE("S= %f\n", S);

		//if(S<0.0001) return _pBBCenterOne;

		// Length of F0M0
		double L = _vehicleData->v_wheelBase->at(drivingMode);
		osg::Vec3d F0M0 = _pBBCenterOne - _pFrontOne;
		double temp_1 = F0M0 * F0F1 / (S * _distanceCenterFront);		// F0M0.length() is equal to _distanceCenterFront

		osg::Vec3d vF1F0 = -F0F1;
		vF1F0.normalize();// normalized F1F0

		// alpha is the angle C ( angle F0)
		double alpha, beta;  // beta is the angle D, alpha is angle C (angle F0)
		if(temp_1<=-1.0)
		{
			alpha = osg::PI;
			beta = 0.0;
		}
		else if(temp_1>=1.0)
		{
			alpha = 0.0;
			beta = 0.0;
		}
		else
		{
			alpha = abs(acos (temp_1));
			beta = abs(atan(sin(alpha) / (2.0 * L / S - cos(alpha))));
			if(abs(beta)<1.0e-7)
			{
				beta = 0.0;
			}
		}


		double test_AngleF1, angle_F1;
		test_AngleF1 = abs(osg::PI-alpha-2.0*beta);
		if(test_AngleF1<1.0e-7)
		{
			//angle_F1 = 0.0;
			return vF1F0 * _distanceCenterFront + _pFrontTwo;
		}
		else
		{
			osg::Vec3d crossp = F0M0 ^ F0F1;
			angle_F1 = (crossp.z() >= 0) ? test_AngleF1 : -test_AngleF1;
			osg::Quat q = osg::Quat(angle_F1, osg::Vec3d(0.0, 0.0, 1.0)); // clockwise, negative value, vice versa

			return (q * vF1F0) * _distanceCenterFront + _pFrontTwo;
		}
	}
	else		// speed < 0: backwarding
	{
		osg::Vec3d pF0	= _pFrontTwo;
		osg::Vec3d pF1	= _pFrontOne;
		osg::Vec3d pM1	= _pBBCenterOne;

		osg::Vec3d F1F0 = pF0 - pF1;
		osg::Vec3d F1M1 = pM1 - pF1;

		double S = F1F0.length();
		osg::Vec3d vF1F0 = F1F0/S;	// normalized.

		double angle_F1, D, M, N, C;

		double test_F1 = vF1F0 * F1M1/_distanceCenterFront;
		if(test_F1>=1.0)
		{
			//angle_F1 = 0.0;
			M = osg::PI;
		}
		else if(test_F1<=-1.0)
		{
			//angle_F1 = osg::PI;
			M = 0.0;
		}
		else
		{
			angle_F1 = abs(acos(test_F1));

			if (angle_F1<1.0e-7)
			{
				//angle_F1 = 0.0;
				M = osg::PI;
			}
			else
				M = osg::PI - angle_F1;
		}


		N = 2.0*_vehicleData->v_wheelBase->at(drivingMode)/S;
		double temp0 = 1.0+N*N+2.0*N*cos(M);
		ASSERT(temp0>0.0);		// because temp0 cannot be 0 or negative values, just assign a small value to it
		double temp1 = (N+cos(M))/sqrt(temp0);

		if(temp1>=1.0)
		{
			//D = 0.0;
			C = M;
		}
		else if(temp1<=-1.0)
		{
			//D = osg::PI;
			C = M - 2.0* osg::PI;
		}
		else
		{
			D = acos(temp1);
			if(D<1.0e-7)
			{
				//D = 0.0;
				C = M;
			}
			else
			{
				C = M - 2.0*D;
			}
		}

		
		C = (F1M1^F1F0).z()>0 ? C : -C;
		osg::Quat q(C, osg::Vec3d(0.0, 0.0, 1.0));

		osg::Vec3d vF0M0 = q * (-vF1F0); //Use Quat to rotate

		return pF0 + vF0M0 * _distanceCenterFront;

#ifdef SOLVECENTER
			// In the Turn.lsp model, with F1(frontOne), F0(frontTwo), B1(rearOne) known, solve for B0(rearTwo)
			//  O(Ox, Oy) is the center of the circle, see the Turn.lsp model. The equation is solved using mathematica.
		// tested in mathematica, no type error
		double Ox = -(0.5f*(B1y-F1y)*(-F0x*F0x-F0y*F0y+F1x*F1x+F1y*F1y)-(F0y-F1y)*(B1x*(-B1x+F1x)+B1y*(-B1y+F1y)))/(B1y*(F0x-F1x)+F0y*F1x-F0x*F1y+B1x*(-F0y+F1y));
		double Oy = (-2.0*B1x*B1x*(F0x-F1x)-2.0*B1y*B1y*(F0x-F1x)+2.0*B1y*(F0x-F1x)*F1y+B1x*(F0x*F0x+F0y*F0y+2.0*F0x*F1x-3.0*F1x*F1x-F1y*F1y)+F1x*(-F0x*F0x-F0y*F0y+F1x*F1x+F1y*F1y))/(2.0*(-F0y*F1x+B1y*(-F0x+F1x)+B1x*(F0y-F1y)+F0x*F1y));

		// Solve for B0, there are two possible solutions.
		double B0x1 = (F0x*F0x*F0x-2.0*F0x*F0x*Ox+LSquare*Ox+F0x*(F0y*F0y-LSquare+Ox*Ox-2.0*F0y*Oy+Oy*Oy)-sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy)))/(F0x*F0x+F0y*F0y-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy);
		double B0y1 = (F0y*F0y*F0y*F0y+F0x*F0x*F0y*(F0y-Oy)-3.0*F0y*F0y*F0y*Oy-LSquare*Oy*Oy-F0y*Oy*(-2.0*LSquare+Ox*Ox+Oy*Oy)-Ox*sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy))+F0y*F0y*(-LSquare+Ox*Ox+3.0*Oy*Oy)+F0x*(-2.0*F0y*F0y*Ox+2.0*F0y*Ox*Oy+sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy))))/((F0y-Oy)*(F0x*F0x+F0y*F0y-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy));

		double B0x2 = (F0x*F0x*F0x-2.0*F0x*F0x*Ox+LSquare*Ox+F0x*(F0y*F0y-LSquare+Ox*Ox-2.0*F0y*Oy+Oy*Oy)+sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy)))/(F0x*F0x+F0y*F0y-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy);
		double B0y2 = (F0y*F0y*F0y*F0y+F0x*F0x*F0y*(F0y-Oy)-3.0*F0y*F0y*F0y*Oy-LSquare*Oy*Oy-F0y*Oy*(-2.0*LSquare+Ox*Ox+Oy*Oy)+Ox*sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy))+F0y*F0y*(-LSquare+Ox*Ox+3.0*Oy*Oy)+F0x*(-2.0*F0y*F0y*Ox+2.0*F0y*Ox*Oy-sqrt(LSquare*(F0y-Oy)*(F0y-Oy)*(F0x*F0x+F0y*F0y-LSquare-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy))))/((F0y-Oy)*(F0x*F0x+F0y*F0y-2.0*F0x*Ox+Ox*Ox-2.0*F0y*Oy+Oy*Oy));

		osg::Vec3d F0B01(B0x1-F0x, B0y1-F0y, 0.0);
		osg::Vec3d F0B02(B0x2-F0x, B0y2-F0y, 0.0);

		osg::Vec3d F0O(Ox-F0x, Oy-F0y, 0.0);

		osg::Vec3d crossproduct0 = F1B1 ^ F1F0;

		osg::Vec3d crossproduct1 = F0O ^ F0B01;
		osg::Vec3d crossproduct2 = F0O ^ F0B02;

		/*if(crossproduct0.z() * crossproduct1.z() >0)*/ return osg::Vec3d(B0x1, B0y1, 0.0);
		/*else if(crossproduct0.z() * crossproduct2.z() >0)*/ //return osg::Vec3d(B0x2, B0y2, 0.0);
		//else return osg::Vec3d();
#endif
 }
}





void vehicleDataType::updateData()
{
	if(global_bUseJoystick1)	// Use joystick
	{
		switch(global_DrivingTypes)
		{
		case GearType::BTN_SIX: _vehicleData->drivingMode = 0; break;
		case GearType::BTN_SEVEN: _vehicleData->drivingMode = 1; break;
		case GearType::BTN_TEN: _vehicleData->drivingMode = 2; break;
		case GearType::BTN_ELEVEN: _vehicleData->drivingMode = 3; break;
		default: _vehicleData->drivingMode = 0; break;
		}


		// turning control
		if(abs(global_dataSet1.lX)>10.0)
			_speedAngle = -_vehicleData->maxTurningAngle*(double(global_dataSet1.lX))/1000.0;
		else
			noFrontTurning();
		//TRACE("                                                                  lX value = %f \n", double(global_dataSet1.lX));

		// accelerating & decelerating. zeroth gear, cannot accelerate nor decelerate
		if(global_dataSet1.lRz<980.0 && global_GearTypes !=GearType::BTN_FOUR)		// use button four for gear zero
		{

			// the steering wheel is in range [-1000, 1000]
			double deltaspeed = 0.00030 * abs(1000.0 - double(global_dataSet1.lRz))/2000.0;

				// Forward
				if(global_GearTypes !=GearType::BTN_FIVE)	// use button five for reversing function
				{
					_speedFront += deltaspeed;

					switch(global_GearTypes)
					{
					case GearType::BTN_ONE:	// actually it is gear one (use button one).
						if(_speedFront>=0.02)
							_speedFront = 0.02;
						break;

					case GearType::BTN_ZERO:	// gear two use button zero
						if(_speedFront>=0.05)
							_speedFront = 0.05;
						break;

					case GearType::BTN_TWO:		// gear three
						if(_speedFront>=0.08)
							_speedFront = 0.08;
						break;

					case GearType::BTN_THREE:	// gear four
						if(_speedFront>=0.11)
							_speedFront = 0.11;
						break;

					default:
						if(_speedFront>=0.11)
							_speedFront=0.11;
						break;
					}
				}
				else	// Reversing
				{
					_speedFront -= 0.5* deltaspeed;
					if( _speedFront <= -0.03 )
						_speedFront = -0.03;
				}
		}	// if lY is pressed && not GearZero
		else
			noFrontAcceleration();

		// Pause ... Brake
		// the lY range is [-1000,1000]
		if(global_dataSet1.lY<980.0)
		{
			double delta_speed = 0.0016 * abs(1000.0 - double(global_dataSet1.lY))/2000.0;
			if( _speedFront>0.0 )	_speedFront -= delta_speed;
			else if( abs(_speedFront)< 0.0018 )	_speedFront = 0.0;
			else	_speedFront += delta_speed;
		}

		// reversing
		if(global_dataSet1.rgbButtons[5] & 0x80)
			_speedFront = -0.010;	

	}
	else		// Use keyboard
	{
			switch(e_DrivingMode)
			{
			case ROADDRIVING:
				_vehicleData->drivingMode = 0; break;

			case ALLSTEERING:
				_vehicleData->drivingMode = 1; break;

			case REDUCEDSWINGOUT:
				_vehicleData->drivingMode = 2; break;

			case CRAB:
				_vehicleData->drivingMode = 3; break;

			default: _vehicleData->drivingMode = 0; break;
			}

			if(_bturnLeft)						turnFrontLeft();
			else if(_bturnRight)				turnFrontRight();
			else if(abs(_speedFront)>1.0e-7 && _vehicleData->drivingMode!=3) 	noFrontTurning();  // when speed == 0 or in crab mode, the turning angle deos not change.

			if(_bmoveForward)				moveFrontForward();
			else if(_bmoveBackward)	moveFrontBackward();
			else									noFrontAcceleration();

			if(_bPause)						movePause();
	}

	// Now the _speedAngle, _speedFront have been changed
	//if(_speedAngle>osg::PI/3) _speedAngle = osg::PI/3;
	//else if(_speedAngle<-osg::PI/3) _speedAngle = -osg::PI/3;
	//if(abs(_speedAngle)<0.002f) _speedAngle = 0.0;

		//if(abs(_speedFront)>1.0e-7)		
			postProcess();
		// Assign the values of stage "two" to stage "one", for the next calculation loop
		_pFrontOne = _pFrontTwo;
		//_pRearOne = _pRearTwo;
		_pBBCenterOne = _pBBCenterTwo;

		//_vRearToFrontOne = _vRearToFrontTwo;
		//_vFrontSpeedOne = _vFrontSpeedTwo;
		//_speedAngleFromXOne = _speedAngleFromXTwo;
		_frontThetaOne = _frontThetaTwo;
}