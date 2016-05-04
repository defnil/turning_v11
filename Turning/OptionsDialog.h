#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>
#include <dinputd.h>
#include <assert.h>
#include <oleauto.h>
#include <shellapi.h>
#pragma warning( disable : 4995 ) // disable deprecated warning
#include <strsafe.h>
//#pragma warning( default : 4996 )
#include "resource.h"
#include "MFC_OSG.h"
#include "JoystickData.h"

#define WM_GOODBYE	WM_USER + 5

BOOL CALLBACK    EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK    EnumObjectsCallback2( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK    EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL CALLBACK    EnumJoysticksCallback2( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

// COptionsDialog dialog
class COptionsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COptionsDialog)

public:
	COptionsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDialog();

	COptionsDialog(CView* pView);
	BOOL Create();

	// Joysticks Handling-----------------------------------------------------------------
	HRESULT InitDirectInput();
	HRESULT InitDirectInput2();
	VOID FreeDirectInput();
	HRESULT UpdateInputState();
	HRESULT UpdateInputState2(); // for the 2nd joystick
	//--------------------------------------------------------------------------------------

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
	afx_msg void OnCancel();
private:
	CView* m_pView;
	//CTurningView* m_pTurningView;
public:

	int m_nDataSource;
	virtual BOOL OnInitDialog();
//	afx_msg void OnCbnSelchangeDatasource();
//	afx_msg void OnCbnSelendokDatasource();
	afx_msg void OnSelchangeDatasource();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_nSlider1;
	int m_nSlider2;
	afx_msg void OnClickedMfcbuttonPick();
	afx_msg void OnClickedMfcbuttonOk();
	afx_msg void OnClickedMfcbuttonCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
};
