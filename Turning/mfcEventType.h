#pragma once

#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <iostream>

#include <osg/Referenced>

enum Mfc_EventType
{
	NONE,
	OBB,
	LOADVEHICLE,
	BOUNDINGSPHERE,
	RESETSCENE,
	ENDPICKING,
	ANALYSE,
	SIMULATE,
	SPACEMOUSE,
	LOADDATA,
	HIDEPLANT,
	ENABLEPHYSX,
	LOADBOUND,
	RELOADBOUND,
	DRIVING,
	TEST,
	ADDFOG,
	ADDSNOW,
	ADDRAIN,
	PRECIPITATONENABLED
};

class OwnDefineEventAdpater :public osg::Referenced
{
public:

	void setEventType(Mfc_EventType  eventype)
	{
		m_eventType=eventype;
	}

	OwnDefineEventAdpater()
	{
		m_eventType=NONE;
	}

	Mfc_EventType  m_eventType; //Event type
};
