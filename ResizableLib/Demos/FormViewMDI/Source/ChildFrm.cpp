// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "StdAfx.h"
#include "Demo.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CResizableMDIChild)

BEGIN_MESSAGE_MAP(CChildFrame, CResizableMDIChild)
	//{{AFX_MSG_MAP(CChildFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CResizableMDIChild::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CResizableMDIChild::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CResizableMDIChild::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if ( EnableSaveRestore(_T("Demo MDI Frame")) )
	{
		// if correctly restored, activate window and return
		MDIActivate();
		return;
	}

	CResizableMDIChild::ActivateFrame(nCmdShow);
}
