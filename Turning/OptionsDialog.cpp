// OptionsDialog.cpp : implementation file
//
#include "stdafx.h"

#include "Turning.h"

#include "afxdialogex.h"
#include "OptionsDialog.h"

#define DI_SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// Joysticks Handling
// Steering wheel
LPDIRECTINPUT8          g_pDI = NULL;
LPDIRECTINPUTDEVICE8    g_pJoystick = NULL;

// Joystick for view control
LPDIRECTINPUT8          g_pDI2 = NULL;
LPDIRECTINPUTDEVICE8    g_pJoystick2 = NULL;

const UINT ID_TIMER_SECONDS = 0x1000;		// a flag for timer

struct DI_ENUM_CONTEXT
{
	DIJOYCONFIG* pPreferredJoyCfg;
	bool bPreferredJoyCfgValid;
};

// COptionsDialog dialog

IMPLEMENT_DYNAMIC(COptionsDialog, CDialogEx)

COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(COptionsDialog::IDD, pParent)
	, m_pView(NULL)
{
	m_nDataSource = 0;
	m_nSlider1 = 0;
	m_nSlider2 = 0;

	global_bUseJoystick1 = false;
	global_bUseJoystick2 = false;
}

COptionsDialog::COptionsDialog(CView* pView)
{
	m_pView = pView;

	m_nDataSource = 0;

	m_nSlider1 = 0;

	m_nSlider2 = 0;
}

COptionsDialog::~COptionsDialog()
{
}

void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_DATASOURCE, m_nDataSource);
	DDX_Slider(pDX, IDC_SLIDER_WALLTHICKNESS, m_nSlider1);
	DDV_MinMaxInt(pDX, m_nSlider1, 1, 200);
	DDX_Slider(pDX, IDC_SLIDER_WALLHEIGHT, m_nSlider2);
	DDV_MinMaxInt(pDX, m_nSlider2, 1, 1000);

	global_Road->ThicknessOfWall = static_cast<float>(m_nSlider1);
	global_Road->HeightOfWall = static_cast<float>(m_nSlider2);
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(RELOADBOUND);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

BEGIN_MESSAGE_MAP(COptionsDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &COptionsDialog::OnOK)
	ON_BN_CLICKED(IDCANCEL, &COptionsDialog::OnCancel)
	ON_CBN_SELCHANGE(IDC_DATASOURCE, &COptionsDialog::OnSelchangeDatasource)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_MFCBUTTON_PICK, &COptionsDialog::OnClickedMfcbuttonPick)
	ON_BN_CLICKED(IDC_MFCBUTTON_OK, &COptionsDialog::OnClickedMfcbuttonOk)
	ON_BN_CLICKED(IDC_MFCBUTTON_CANCEL, &COptionsDialog::OnClickedMfcbuttonCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// COptionsDialog message handlers

BOOL COptionsDialog::Create()
{
	return CDialogEx::Create(COptionsDialog::IDD);
}

void COptionsDialog::OnOK()
{
	if(m_pView!=NULL)
	{
		UpdateData(TRUE);
		//m_pView->PostMessageA(WM_GOODBYE, IDOK); // 注释掉这句，使按ok不退出，只做数据交换
	}
	else CDialogEx::OnOK();
}

void COptionsDialog::OnCancel()
{
	if(m_pView!=NULL)
	{
		m_pView->PostMessageA(WM_GOODBYE, IDCANCEL);

		// Cleanup everything
		KillTimer(ID_TIMER_SECONDS);
		FreeDirectInput();
	}
	else CDialogEx::OnCancel();
}

BOOL COptionsDialog::OnInitDialog()
{
	HWND hDlg = GetSafeHwnd();
	// Joysticks Handling
	HRESULT hr1 = InitDirectInput();
	HRESULT hr2 = InitDirectInput2();

	global_bUseJoystick1 = SUCCEEDED(hr1) ? true : false;
	global_bUseJoystick2 = SUCCEEDED(hr2) ? true : false;

	if( FAILED(hr1) && FAILED(hr2))
	{
		GetDlgItem(IDC_DEVICE_STATUS)->SetWindowTextA(_T("No device Connected."));
	}
	else
	{
		GetDlgItem(IDC_DEVICE_STATUS)->SetWindowTextA(_T("Input Devices:"));
	}
	// Set a timer to go off 30 times a second. At every timer message
	// the input device will be read
	SetTimer( ID_TIMER_SECONDS, 1000 / 30, NULL );

	m_nSlider1 = static_cast<int>(global_Road->ThicknessOfWall);
	m_nSlider2 = static_cast<int>(global_Road->HeightOfWall);

	CString strText1;
	CSliderCtrl* pSlider1 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_WALLTHICKNESS);
	pSlider1->SetRange(1, 200);
	pSlider1->SetPos(m_nSlider1);
	pSlider1->SetTicFreq(25);
	strText1.Format("%d", pSlider1->GetPos());
	SetDlgItemTextA(IDC_SLIDER_WALLTHICKNESS, strText1);

	CString strText2;
	CSliderCtrl* pSlider2 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER_WALLHEIGHT);
	pSlider2->SetRange(1, 1000);
	pSlider2->SetPos(m_nSlider2);
	pSlider2->SetTicFreq(125);
	strText2.Format("%d", pSlider2->GetPos());
	SetDlgItemTextA(IDC_SLIDER_WALLHEIGHT, strText2);

	return CDialogEx::OnInitDialog();  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::OnSelchangeDatasource()
{
		UpdateData();

		if(m_nDataSource==0) global_OSG->getMFChandler()->setDataSource(true);
		else global_OSG->getMFChandler()->setDataSource(false);

		OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
		ownType->setEventType(LOADBOUND);
		global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void COptionsDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = (CSliderCtrl*) pScrollBar;
	CString strText;

	switch(pScrollBar->GetDlgCtrlID())
	{
		case IDC_SLIDER_WALLTHICKNESS:
			strText.Format("%d", pSlider->GetPos());
			SetDlgItemTextA(IDC_STATIC_SLIDER1, strText);
			break;

		case IDC_SLIDER_WALLHEIGHT:
			strText.Format("%d", pSlider->GetPos());
			SetDlgItemTextA(IDC_STATIC_SLIDER2, strText);
			break;
	}

	//CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COptionsDialog::OnClickedMfcbuttonPick()
{
	// TODO: Add your control notification handler code here
}

void COptionsDialog::OnClickedMfcbuttonOk()
{
	// TODO: Add your control notification handler code here
}

void COptionsDialog::OnClickedMfcbuttonCancel()
{
	// TODO: Add your control notification handler code here
}

void COptionsDialog::OnTimer(UINT_PTR nIDEvent)
{
	HRESULT hr1=NULL, hr2=NULL;
	hr1 = UpdateInputState();
	hr2 = UpdateInputState2();
	// TODO: Add your message handler code here and/or call default
	if( FAILED(hr1) && FAILED(hr2))
	{
		KillTimer( ID_TIMER_SECONDS );
		GetDlgItem(IDC_DEVICE_STATUS)->SetWindowTextA(_T("Devices Lost."));
	}

	//CDialogEx::OnTimer(nIDEvent);
}

//-----------------------------------------------------------------------------
// Name: InitDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
HRESULT COptionsDialog::InitDirectInput()
{
	HRESULT hr;

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	// Create a DInput object
	if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, ( VOID** )&g_pDI, NULL ) ))
		return hr;

	// Look for a simple joystick we can use for this sample program.
	if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, ::EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY ) ))
		return hr;

	// Make sure we got a joystick
	if( NULL == g_pJoystick)
	{
		//GetDlgItem(IDC_DEVICE_STATUS)->SetWindowTextA(_T("No Device Found"));
		return E_FAIL;
	}

	// Set the data format to "simple joystick" - a predefined data format
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
		return hr;

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	HWND hDlg = GetSafeHwnd();
	if( FAILED( hr = g_pJoystick->SetCooperativeLevel( hDlg, DISCL_EXCLUSIVE | DISCL_BACKGROUND ) ))
		return hr;

	// Enumerate the joystick objects. The callback function enabled user
	// interface elements for objects that are found, and sets the min/max
	// values property for discovered axes.
	if( FAILED( hr = g_pJoystick->EnumObjects( ::EnumObjectsCallback, NULL, DIDFT_ALL ) ) )
		return hr;

	return S_OK;
}

HRESULT COptionsDialog::InitDirectInput2()
{
	HRESULT hr;

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	// Create a DInput object
	if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, ( VOID** )&g_pDI2, NULL ) ))
		return hr;

	// Look for a simple joystick we can use for this sample program.
	if( FAILED( hr = g_pDI2->EnumDevices( DI8DEVCLASS_GAMECTRL, ::EnumJoysticksCallback2, NULL, DIEDFL_ATTACHEDONLY ) ))
		return hr;

	// Make sure we got a joystick
	if( NULL == g_pJoystick2)
	{
		//GetDlgItem(IDC_DEVICE_STATUS)->SetWindowTextA(_T("No Device Found"));
		return E_FAIL;
	}

	// Set the data format to "simple joystick" - a predefined data format
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = g_pJoystick2->SetDataFormat( &c_dfDIJoystick2 ) ) )
		return hr;

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	HWND hDlg = GetSafeHwnd();
	if( FAILED( hr = g_pJoystick2->SetCooperativeLevel( hDlg, DISCL_EXCLUSIVE | DISCL_BACKGROUND ) ))
		return hr;

	// Enumerate the joystick objects. The callback function enabled user
	// interface elements for objects that are found, and sets the min/max
	// values property for discovered axes.
	if( FAILED( hr = g_pJoystick2->EnumObjects( ::EnumObjectsCallback2, NULL, DIDFT_ALL ) ) )
		return hr;

	return S_OK;
}

HRESULT COptionsDialog::UpdateInputState()
{
	HRESULT hr;
	TCHAR strText[512] = {0}; // Device state text
	DIJOYSTATE2 js;           // DInput joystick state

	if( NULL == g_pJoystick )
		return E_FAIL;

	// Poll the device to read the current state
	hr = g_pJoystick->Poll();
	if( FAILED( hr ) )
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = g_pJoystick->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = g_pJoystick->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of
		// switching, so just try again later
		return S_OK;
	}

	// Get the input's device state
	if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof( DIJOYSTATE2 ), &js ) ) )
		return hr; // The device should have been acquired during the Poll()

	// Sync Data
	global_dataSet1 = js;
	//float temp_a = float(global_dataSet1.lX);
	//TRACE("Joystick X-axis Reading = %f                                                                \n", temp_a);
	// Display joystick state to dialog

	// Axes
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lX );
	GetDlgItem(IDC_WHEEL_X)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lY );
	GetDlgItem(IDC_WHEEL_Y)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lZ );
	GetDlgItem(IDC_WHEEL_Z)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRx );
	GetDlgItem(IDC_WHEEL_ROTX)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRy );
	GetDlgItem(IDC_WHEEL_ROTY)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRz );
	GetDlgItem(IDC_WHEEL_ROTZ)->SetWindowTextA(strText);

	// Slider controls
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rglSlider[0] );
	GetDlgItem(IDC_WHEEL_SLIDER0)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rglSlider[1] );
	GetDlgItem(IDC_WHEEL_SLIDER1)->SetWindowTextA(strText);

	// Points of view
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[0] );
	GetDlgItem(IDC_WHEEL_POV0)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[1] );
	GetDlgItem(IDC_WHEEL_POV1)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[2] );
	GetDlgItem(IDC_WHEEL_POV2)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[3] );
	GetDlgItem(IDC_WHEEL_POV3)->SetWindowTextA(strText);

	// Fill up text with which buttons are pressed
	StringCchCopy( strText, 512, TEXT( "" ) );
	for(unsigned int i = 0; i < 128; i++ )
	{
		if( js.rgbButtons[i] & 0x80 )
		{
			TCHAR sz[128];
			StringCchPrintf( sz, 128, TEXT( "%02d " ), i );
			StringCchCat( strText, 512, sz );

			// Determine button 0~5, other buttons are not used. In these 6 buttons only one can function. So use enum type.
			if(i<=5) global_GearTypes = (GearType::TGearType)i;

			// button 6 and 7, 10, 11
			else if(i==6 || i==7 || i==10 || i==11) global_DrivingTypes = (GearType::TDringType)i;
			//else
			//	global_GearTypes = GearType::BTN_INVALID;
		}
	}

	TRACE("GearType=%d\n", int(global_GearTypes));

	GetDlgItem(IDC_WHEEL_BUTTON)->SetWindowTextA(strText);

	return S_OK;
}

HRESULT COptionsDialog::UpdateInputState2()
{
	HRESULT hr;
	TCHAR strText[512] = {0}; // Device state text
	DIJOYSTATE2 js;           // DInput joystick state

	if( NULL == g_pJoystick2 )
		return E_FAIL;

	// Poll the device to read the current state
	hr = g_pJoystick2->Poll();
	if( FAILED( hr ) )
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = g_pJoystick2->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = g_pJoystick2->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of
		// switching, so just try again later
		return S_OK;
	}

	// Get the input's device state
	if( FAILED( hr = g_pJoystick2->GetDeviceState( sizeof( DIJOYSTATE2 ), &js ) ) )
		return hr; // The device should have been acquired during the Poll()

	// data sync
	global_dataSet2 = js;


	// Display joystick state to dialog

	// Axes
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lX );
	GetDlgItem(IDC_JS1_X)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lY );
	GetDlgItem(IDC_JS1_Y)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lZ );
	GetDlgItem(IDC_JS1_Z)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRx );
	GetDlgItem(IDC_JS1_ROTX)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRy );
	GetDlgItem(IDC_JS1_ROTY)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.lRz );
	GetDlgItem(IDC_JS1_ROTZ)->SetWindowTextA(strText);

	// Slider controls
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rglSlider[0] );
	GetDlgItem(IDC_JS1_SLIDER0)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rglSlider[1] );
	GetDlgItem(IDC_JS1_SLIDER1)->SetWindowTextA(strText);

	// Points of view
	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[0] );
	GetDlgItem(IDC_JS1_POV0)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[1] );
	GetDlgItem(IDC_JS1_POV1)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[2] );
	GetDlgItem(IDC_JS1_POV2)->SetWindowTextA(strText);

	StringCchPrintf( strText, 512, TEXT( "%ld" ), js.rgdwPOV[3] );
	GetDlgItem(IDC_JS1_POV3)->SetWindowTextA(strText);

	// Fill up text with which buttons are pressed
	StringCchCopy( strText, 512, TEXT( "" ) );
	for( int i = 0; i < 128; i++ )
	{
		if( js.rgbButtons[i] & 0x80 )
		{
			TCHAR sz[128];
			StringCchPrintf( sz, 128, TEXT( "%02d " ), i );
			StringCchCat( strText, 512, sz );
		}
	}

	GetDlgItem(IDC_JS1_BUTTON)->SetWindowTextA(strText);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID COptionsDialog::FreeDirectInput()
{
	// Unacquire the device one last time just in case
	// the app tried to exit while the device is still acquired.
	if( g_pJoystick )
		g_pJoystick->Unacquire();
	if( g_pJoystick2 )
		g_pJoystick2->Unacquire();

	// Release any DirectInput objects.
	DI_SAFE_RELEASE( g_pJoystick );
	DI_SAFE_RELEASE( g_pJoystick2 );
	DI_SAFE_RELEASE( g_pDI );
	DI_SAFE_RELEASE( g_pDI2 );
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
	VOID* pContext )
{
	#define STEERINGWHEEL_ID    0xb6550000  // Product ID for steering wheel
	//#define JOYSTICK2_ID    0xc2860000  // Product ID for joystick
	GUID guid = pdidInstance->guidProduct;
	// Check to see if this is a 3Dx device.  Look at the VendorID before stopping the enumeration.
	unsigned long temp_data = guid.Data1 & 0xffff0000;
	if (temp_data != STEERINGWHEEL_ID)
		return DIENUM_CONTINUE;

	HRESULT hr;
	// Obtain an interface to the enumerated joystick.
	hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );
	if(FAILED(hr))
			return DIENUM_CONTINUE;

	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_STOP;
}

BOOL CALLBACK EnumJoysticksCallback2( const DIDEVICEINSTANCE* pdidInstance,
	VOID* pContext )
{
//#define STEERINGWHEEL_ID    0x00180000  // Product ID for steering wheel
	#define JOYSTICK2_ID    0xc2140000  // Product ID for joystick
	GUID guid = pdidInstance->guidProduct;
	// Check to see if this is a 3Dx device.  Look at the VendorID before stopping the enumeration.
	unsigned long temp_data = guid.Data1 & 0xffff0000;
	if (temp_data != JOYSTICK2_ID)
		return DIENUM_CONTINUE;

	HRESULT hr;
	// Obtain an interface to the enumerated joystick.
	hr = g_pDI2->CreateDevice( pdidInstance->guidInstance, &g_pJoystick2, NULL );
	if(FAILED(hr))
		return DIENUM_CONTINUE;

	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
	VOID* pContext )
{
	//static int nSliderCount = 0;  // Number of returned slider controls
	//static int nPOVCount = 0;     // Number of returned POV controls

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof( DIPROPRANGE );
		diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = +1000;

		// Set the range for the axis
		if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ))
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

BOOL CALLBACK EnumObjectsCallback2( const DIDEVICEOBJECTINSTANCE* pdidoi,
	VOID* pContext )
{
	//static int nSliderCount = 0;  // Number of returned slider controls
	//static int nPOVCount = 0;     // Number of returned POV controls

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof( DIPROPRANGE );
		diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = +1000;

		// Joystick2 works under relative mode. Set DIPROPDWORD instead of DIPROPRANGE
		//DIPROPDWORD dipdw;
		//dipdw.diph.dwSize       = sizeof(DIPROPDWORD);    
		//dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);    
		//dipdw.diph.dwHow        = DIPH_DEVICE;    
		//dipdw.diph.dwObj        = 0; // set for whole device not an axis (really only needs to be done once)   
		//dipdw.dwData            = DIPROPAXISMODE_REL;  




		// Set the range for the axis
		if( FAILED( g_pJoystick2->SetProperty( DIPROP_RANGE, &diprg.diph ) ))
		//if(FAILED(g_pJoystick2->SetProperty(DIPROP_AXISMODE, &dipdw.diph)))

			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}