// TurningView.cpp : implementation of the CTurningView class

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Turning.h"
#endif

#include "TurningDoc.h"
#include "TurningView.h"
#include "ConsolePrinter.h"
#include "mfcEventType.h"

////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////
//#include "spwmacro.h"  /* Common macros used by SpaceWare functions. */
//#include "si.h"        /* Required for any SpaceWare support within an app.*/
//extern "C"
//{
//#include "siapp.h"     /* Required for siapp.lib symbols */
//}
////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////

#include "Road.h"
#include "Vehicle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTurningView
ConsolePrinter *g_pCslPrt;

IMPLEMENT_DYNCREATE(CTurningView, CView)

//int WM_3DMOUSE = RegisterWindowMessage (_T("SpaceWareMessage00"));

BEGIN_MESSAGE_MAP(CTurningView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_GOODBYE, OnGoodbye)

	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////
	//ON_REGISTERED_MESSAGE( WM_3DMOUSE, On3DMouse )
	////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////

ON_COMMAND(ID_VEHICLE_LOAD, &CTurningView::OnVehicleLoad)
ON_COMMAND(ID_ROAD_ENDPICKING, &CTurningView::OnRoadEndpicking)
ON_COMMAND(ID_ROAD_RESET, &CTurningView::OnRoadReset)
ON_COMMAND(ID_ROAD_PICKBOUNDARY1, &CTurningView::OnRoadPickboundary1)
ON_COMMAND(ID_ROAD_PICKBOUNDARY2, &CTurningView::OnRoadPickboundary2)
//ON_COMMAND(ID_ROAD_CENTERLINE, &CTurningView::OnRoadCenterline)
//ON_COMMAND(ID_VEHICLE_DEFAULTPATH, &CTurningView::OnVehicleDefaultpath)
ON_COMMAND(ID_CONSOLE_INPUT, &CTurningView::OnConsoleInput)
ON_COMMAND(ID_CONSOLE_OUTPUT, &CTurningView::OnConsoleOutput)
ON_COMMAND(ID_CONSOLE_CLOSE, &CTurningView::OnConsoleClose)
//ON_COMMAND(ID_VEHICLE_ANALYSIS, &CTurningView::OnVehicleAnalysis)
//ON_COMMAND(ID_GO_START, &CTurningView::OnGoStart)
//ON_COMMAND(ID_TEST_ANIMATIONTEST, &CTurningView::OnTestAnimationtest)
//ON_COMMAND(ID_SCENE_3DCONNEXION, &CTurningView::OnScene3dconnexion)
ON_COMMAND(ID_SCENE_SAVEDATA, &CTurningView::OnSceneSavedata)
ON_COMMAND(ID_SCENE_LOADDATA, &CTurningView::OnSceneLoaddata)
ON_COMMAND(ID_SCENE_HIDEPLANT, &CTurningView::OnSceneHideplant)
//ON_COMMAND(ID_SCENE_CREATEBOUNDARIES, &CTurningView::OnSceneCreateboundaries)
//ON_COMMAND(ID_SCENE_ADD3DBOUNDARIES, &CTurningView::OnSceneAdd3dboundaries)
//ON_COMMAND(ID_VEHICLE_BOUNDINGBOX32805, &CTurningView::OnVehicleBoundingbox32805)
//ON_COMMAND(ID_VEHICLE_AABB, &CTurningView::OnVehicleAabb)
ON_COMMAND(ID_VEHICLE_OBB, &CTurningView::OnVehicleObb)
ON_COMMAND(ID_CONSOLETEST_TEST, &CTurningView::OnConsoletestTest)
ON_COMMAND(ID_TOOLS_SIMULATE, &CTurningView::OnToolsSimulate)
ON_COMMAND(ID_ROAD_ANALYSE, &CTurningView::OnRoadAnalyse)
ON_COMMAND(ID_SCENE_ENABLEPHYSX, &CTurningView::OnSceneEnablephysx)
ON_COMMAND(ID_ROAD_LOADBOU, &CTurningView::OnRoadLoadbound)
ON_COMMAND(ID_TOOLS_OPTIONSPANEL, &CTurningView::OnToolsOptionspanel)
ON_COMMAND(ID_VEHICLE_DRIVING, &CTurningView::OnVehicleDriving)
ON_COMMAND(ID_TOOLS_JOYSTICK, &CTurningView::OnToolsJoystick)
ON_COMMAND(ID_SCENE_FOG, &CTurningView::OnSceneFog)
ON_COMMAND(ID_SCENE_SNOW, &CTurningView::OnSceneSnow)
ON_COMMAND(ID_SCENE_RAIN, &CTurningView::OnSceneRain)
ON_COMMAND(ID_Menu, &CTurningView::OnMenu)
END_MESSAGE_MAP()

// CTurningView construction/destruction

CTurningView::CTurningView()
{
	global_OSG = 0L;
	m_pDlg = new COptionsDialog(this);
}

CTurningView::~CTurningView()
{
	delete m_pDlg;
}

BOOL CTurningView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTurningView drawing

void CTurningView::OnDraw(CDC* /*pDC*/)
{
	CTurningDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

// CTurningView diagnostics

#ifdef _DEBUG
void CTurningView::AssertValid() const
{
	CView::AssertValid();
}

void CTurningView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTurningDoc* CTurningView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTurningDoc)));
	return (CTurningDoc*)m_pDocument;
}
#endif //_DEBUG

////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSbtestmfcView 3D Mouse Functions

/*--------------------------------------------------------------------------
 * Function: CSbtestmfcView::SbInit()
 *
 * Description:
 *    This function initializes the 3D mouse and opens ball for use.
 *
 *
 * Args: None
 *
 *
 * Return Value:
 *    int  res         result of SiOpen, =0 if Fail =1 if it Works
 *
 *--------------------------------------------------------------------------*/

//int	CSbtestmfcView::SbInit()
//{
//	int res;                                 //result of SiOpen, to be returned
//	SiOpenData oData;                        //OS Independent data to open ball
//
//	if (SiInitialize() == SPW_DLL_LOAD_ERROR)   //init the SpaceWare input library
//	{
//		MessageBox(_T("Error: Could not load SiAppDll dll files"),
//			NULL, MB_ICONEXCLAMATION);
//	}
//	SiOpenWinInit (&oData, m_hWnd);          //init Win. platform specific data
//	SiSetUiMode(&m_DevHdl, SI_UI_ALL_CONTROLS); //Config SoftButton Win Display
//
//	//open data, which will check for device type and return the device handle
//	// to be used by this function
//	if ( (m_DevHdl = SiOpen ("TestSP", SI_ANY_DEVICE, SI_NO_MASK,
//		SI_EVENT, &oData)) == NULL )
//	{
//		SiTerminate();  //called to shut down the SpaceWare input library
//		res = 0;        //could not open device
//		return res;
//	}
//	else
//	{
//		res = 1;        //opened device succesfully
//		return res;
//	}
//}
////////////////////////////////////////////* 3D Mouse *////////////////////////////////////////////////////////////////////

// CTurningView message handlers
int CTurningView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Let MFC create the window before OSG
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Now that the window is created setup OSG
	global_OSG = new cOSG(m_hWnd);
	return 1;
}

void CTurningView::OnDestroy()
{
	delete mThreadHandle;
	if(global_OSG != 0) delete global_OSG;

	CView::OnDestroy();
}

void CTurningView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// Get Filename from DocumentOpen Dialog
	CString csFileName = GetDocument()->GetSceneName();

	// Init the osg class
	global_OSG->InitOSG(csFileName.GetString()); //CString 2 string

	// Start the thread to do OSG Rendering
	mThreadHandle = new CRenderingThread(global_OSG);
	mThreadHandle->start();
}

void CTurningView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Pass Key Presses into OSG
	//global_OSG->getViewer()->getEventQueue()->keyPress(nChar);

	// Close Window on Escape Key
	if(nChar == VK_ESCAPE)
	{
		GetParent()->SendMessage(WM_CLOSE);
	}
}

BOOL CTurningView::OnEraseBkgnd(CDC* pDC)
{
	/* Do nothing, to avoid flashing on MSW */
	return true;
}

void CTurningView::OnVehicleLoad()
{
	CFileDialog dlgFile(TRUE);
	//CString fileName;

	if(dlgFile.DoModal() == IDOK) CVehicle::s_vehiclename = dlgFile.GetPathName().GetBuffer(0);  // CString2string

	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(LOADVEHICLE);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnRoadEndpicking()
{
	if(global_OSG->getMFChandler()->getPickChannel() == true && global_OSG->getMFChandler()->getDataLoad() == false) // the 2nd boundary has not been picked
		AfxMessageBox("Please Pick the 2nd Boundary");
	else
	{
	global_OSG->getMFChandler()->setPickStatus(finish_Pick); // then the boundaries should be drawn automatically, so call the user event.

	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ENDPICKING);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
	}
}

void CTurningView::OnRoadReset()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(RESETSCENE);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnRoadPickboundary1()
{
	global_OSG->getMFChandler()->setPickChannel(true);
	global_OSG->getMFChandler()->setPickStatus(is_Pick);
}

void CTurningView::OnRoadPickboundary2()
{
	global_OSG->getMFChandler()->setPickChannel(false);
	global_OSG->getMFChandler()->setPickStatus(is_Pick);
}

//void CTurningView::OnRoadCenterline()
//{
//	if(global_OSG->getMFChandler()->getPickStatus()!=finish_Pick)
//		AfxMessageBox("Please Finish Picking First");
//	else
//	{
//		OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//		ownType->setEventType(CENTERLINE);
//		global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//	}
//}

//void CTurningView::OnVehicleDefaultpath()
//{
//	if(global_OSG->getMFChandler()->getPickStatus()!=finish_Pick)
//		AfxMessageBox("Please Finish Picking First");
//	else
//	{
//		OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//		ownType->setEventType(DEFAULTPATH);
//		global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//	}
//}

void CTurningView::OnConsoleInput()
{
	// TODO: Add your command handler code here
	g_pCslPrt->Destroy();
	g_pCslPrt->Instance(true);
}

void CTurningView::OnConsoleOutput()
{
	// TODO: Add your command handler code here
	g_pCslPrt->Destroy();
	g_pCslPrt->Instance(false);
}

void CTurningView::OnConsoleClose()
{
	// TODO: Add your command handler code here
	g_pCslPrt->Destroy();
}

//void CTurningView::OnVehicleAnalysis()
//{
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(ANALYSIS);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

//void CTurningView::OnGoStart()
//{
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(PLAY);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

//void CTurningView::OnTestAnimationtest()
//{
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(TEST_ANIMATION);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

//void CTurningView::OnScene3dconnexion()
//{
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(SPACEMOUSE);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

void CTurningView::OnSceneSavedata()
{
	global_OSG->getMFChandler()->setDataSave(true);
}

void CTurningView::OnSceneLoaddata()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(LOADDATA);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnSceneHideplant()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(HIDEPLANT);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

//void CTurningView::OnSceneAdd3dboundaries()
//{
//	//global_OSG->getMFChandler()->setPhysxState(true); // 这个放在osg的handler里处理最好。
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(INIT_PHYSX_SCENE);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

//void CTurningView::OnVehicleAabb()
//{
//	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
//	ownType->setEventType(AABB);
//	global_OSG->getViewer()->getEventQueue()->userEvent(ownType);
//}

void CTurningView::OnVehicleObb()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(OBB);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnConsoletestTest()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(TEST);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnToolsSimulate()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(SIMULATE);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnRoadAnalyse()
{
	if(global_OSG->getMFChandler()->getPickStatus()!=finish_Pick)
		AfxMessageBox("Please Finish Picking First");

	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ANALYSE);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnSceneEnablephysx()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ENABLEPHYSX);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnRoadLoadbound()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(LOADBOUND);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnToolsOptionspanel()
{
	if(m_pDlg->GetSafeHwnd() == 0)
	{
			 // set the default data source
			m_pDlg->Create();
	}
	else m_pDlg->DestroyWindow();
}

// destroy the dialog
LRESULT CTurningView::OnGoodbye(WPARAM wParam, LPARAM lParam)
{
	TRACE("CTurningView::OnGoodbye %x, %lx\n", wParam, lParam);
	//TRACE("Dialog edit1 contents = %s\n", (const char*)m_pDlg->m_strEdit1);
	m_pDlg->DestroyWindow();
	return 0L;
}

void CTurningView::OnVehicleDriving()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(DRIVING);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}

void CTurningView::OnToolsJoystick()
{
}

void CTurningView::OnSceneFog()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ADDFOG);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}


void CTurningView::OnSceneSnow()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ADDSNOW);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}


void CTurningView::OnSceneRain()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(ADDRAIN);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}


void CTurningView::OnMenu()
{
	OwnDefineEventAdpater *ownType=new OwnDefineEventAdpater();
	ownType->setEventType(PRECIPITATONENABLED);
	global_OSG->getMainView()->getEventQueue()->userEvent(ownType);
}
