// TurningDoc.h : interface of the CTurningDoc class
//

#pragma once
#include "Road.h"
#include "Vehicle.h"

class CTurningDoc : public CDocument
{
protected: // create from serialization only
	CTurningDoc();
	DECLARE_DYNCREATE(CTurningDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
//	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CString GetSceneName() const { return m_csFileName; }
//#ifdef SHARED_HANDLERS
//	virtual void InitializeSearchContent();
//	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
//#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CTurningDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_csFileName;
	CRoad c_Road;
	CVehicle c_Vehicle;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

//#ifdef SHARED_HANDLERS
// Helper function that sets search content for a Search Handler
//	void SetSearchContent(const CString& value);
//#endif // SHARED_HANDLERS
public:
//	afx_msg void OnVehicleLoad();
};
