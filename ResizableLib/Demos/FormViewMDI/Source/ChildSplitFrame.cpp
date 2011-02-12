// ChildSplitFrame.cpp : implementation file
//

#include "StdAfx.h"
#include "demo.h"
#include "ChildSplitFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildSplitFrame

IMPLEMENT_DYNCREATE(CChildSplitFrame, CResizableMDIChild)

CChildSplitFrame::CChildSplitFrame()
{
}

CChildSplitFrame::~CChildSplitFrame()
{
}


BEGIN_MESSAGE_MAP(CChildSplitFrame, CResizableMDIChild)
	//{{AFX_MSG_MAP(CChildSplitFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildSplitFrame message handlers

BOOL CChildSplitFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	// CG: The following block was added by the Splitter Bar component.
	{
		if (!m_wndSplitter.Create(this,
		                          2, 2,          // TODO: adjust the number of rows, columns
		                          CSize(10, 10), // TODO: adjust the minimum pane size
		                          pContext))
		{
			TRACE0("Failed to create splitter bar ");
			return FALSE;    // failed to create
		}

		return TRUE;
	}
}

void CChildSplitFrame::ActivateFrame(int nCmdShow) 
{
	if ( EnableSaveRestore(_T("Demo MDI SplitFrame")) )
	{
		// if correctly restored, activate window and return
		MDIActivate();
		return;
	}

	CResizableMDIChild::ActivateFrame(nCmdShow);
}
