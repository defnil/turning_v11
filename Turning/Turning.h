// Turning.h : main header file for the Turning application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CTurningApp:
// See Turning.cpp for the implementation of this class
//

class CTurningApp : public CWinAppEx
{
public:
	CTurningApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

//	virtual void PreLoadState();
//	virtual void LoadCustomState();
//	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTurningApp theApp;
