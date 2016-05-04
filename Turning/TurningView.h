// TurningView.h : interface of the CTurningView class
#pragma once

#include "OptionsDialog.h"
#include "MFC_OSG.h"
class CTurningView : public CView
{
protected: // create from serialization only
	CTurningView();
	DECLARE_DYNCREATE(CTurningView)

// Attributes
public:
	CTurningDoc* GetDocument() const;
	cOSG* GetOSG() { return global_OSG; }

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////
	/* 3D mouse Variables&Functions */
	/* Members */
	//SiHdl       m_DevHdl;       /* Handle to 3D mouse Device */

	/* 3D Mouse Functions --See CPP File for Additional Details */
	//int   SbInit();
	//void  SbMotionEvent(SiSpwEvent *pEvent);
	//void  SbZeroEvent();
	//void  SbButtonPressEvent(int buttonnumber);
	//void  SbButtonReleaseEvent(int buttonnumber);
	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////

// Implementation
public:
	virtual ~CTurningView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CRenderingThread* mThreadHandle;

// Generated message map functions
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////
	//afx_msg LRESULT On3DMouse( WPARAM wParam, LPARAM lParam );
	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////

	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg LRESULT OnGoodbye(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnVehicleLoad();
	afx_msg void OnRoadEndpicking();
	afx_msg void OnRoadReset();
	afx_msg void OnRoadPickboundary1();
	afx_msg void OnRoadPickboundary2();
//	afx_msg void OnRoadCenterline();
//	afx_msg void OnVehicleDefaultpath();
	afx_msg void OnConsoleInput();
	afx_msg void OnConsoleOutput();
	afx_msg void OnConsoleClose();
//	afx_msg void OnVehicleAnalysis();
//	afx_msg void OnGoStart();
//	afx_msg void OnTestAnimationtest();
//	afx_msg void OnScene3dconnexion();
	afx_msg void OnSceneSavedata();
	afx_msg void OnSceneLoaddata();
	afx_msg void OnSceneHideplant();
//	afx_msg void OnSceneCreateboundaries();
//	afx_msg void OnSceneAdd3dboundaries();
//	afx_msg void OnVehicleBoundingbox32805();
//	afx_msg void OnVehicleAabb();
	afx_msg void OnVehicleObb();
	afx_msg void OnConsoletestTest();
	afx_msg void OnToolsSimulate();
	afx_msg void OnRoadAnalyse();
	afx_msg void OnSceneEnablephysx();
	afx_msg void OnRoadLoadbound();
private:
	COptionsDialog* m_pDlg;
public:

	//friend class COptionsDialog;
	afx_msg void OnToolsOptionspanel();
//	afx_msg void OnCbnSelchangeDatasource();
//	afx_msg void OnCbnSelchangeDatasource();
	afx_msg void OnVehicleDriving();
	afx_msg void OnToolsJoystick();
	afx_msg void OnSceneFog();
	afx_msg void OnSceneSnow();
	afx_msg void OnSceneRain();
	afx_msg void OnMenu();
};

#ifndef _DEBUG  // debug version in TurningView.cpp
inline CTurningDoc* CTurningView::GetDocument() const
   { return reinterpret_cast<CTurningDoc*>(m_pDocument); }
#endif
