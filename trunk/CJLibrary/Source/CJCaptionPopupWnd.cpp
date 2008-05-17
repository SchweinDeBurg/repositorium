// CJCaptionPopupWnd.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:43p $
 * $Revision: 5 $
 * $Archive: /CodeJock/CJLibrary/CJCaptionPopupWnd.cpp $
 *
 * $History: CJCaptionPopupWnd.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:43p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:46p
 * Created in $/CodeJockey/CJLibrary
 * Initial creation. Added popup class as a helper class for the
 * CCJCaption class. This class is used to display a popup window which
 * contains a child CWnd derived object.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CJResource.h"
#include "CJCaptionPopupWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJCaptionPopupWnd

CCJCaptionPopupWnd::CCJCaptionPopupWnd()
{
	m_pChildWnd		= NULL;
	m_pChildParent	= NULL;
	m_pParentWnd	= NULL;
	m_hIconOn		= NULL;
	m_hIconOff		= NULL;
}

CCJCaptionPopupWnd::~CCJCaptionPopupWnd()
{
	// fix potential resource leak - KStowell - 10-22-99.
	_destroyIcon(m_hIconOn);
	_destroyIcon(m_hIconOff);
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CCJCaptionPopupWnd, CWnd)
	//{{AFX_MSG_MAP(CCJCaptionPopupWnd)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CAPT_BUTTON, OnCaptButton)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJCaptionPopupWnd message handlers

BOOL CCJCaptionPopupWnd::Create(const CRect& rect, CWnd* pParentWnd, CWnd* pChildWnd) 
{
	ASSERT(pParentWnd != NULL); // must be valid.
	m_pParentWnd = pParentWnd;

	ASSERT(pChildWnd  != NULL); // must be valid.
	m_pChildWnd = pChildWnd;

	// register the wnd class.
	CString wndClass = ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		0, ::GetSysColorBrush(COLOR_BTNFACE));

	// define the styles for this window.
	DWORD dwStyleEx = WS_EX_TOOLWINDOW;
	DWORD dwStyle   = WS_VISIBLE|WS_POPUP|WS_DLGFRAME;

	// call the base class for creation.
    if( !CWnd::CreateEx( dwStyleEx, wndClass, _T(""), dwStyle, rect.left, rect.top,
		rect.Width(), rect.Height(), pParentWnd->GetSafeHwnd(), 0, NULL ))
	{
		TRACE0( "Failed to create caption popdown window.\n" );
		return FALSE;
	}

	// set the child windows parent to this popup window, save
	// the previous parent, since we need to reset this when the
	// window is destroyed.
	m_pChildParent = m_pChildWnd->SetParent(this);
	ASSERT(m_pChildParent != NULL);

	// define the caption and client rect.
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.DeflateRect(2,2);
	CRect rcCapt( rcClient );

	// move the window to fit the client area of this window.
	rcClient.top += 19;
	m_pChildWnd->MoveWindow(rcClient);

	// define the size for the push pin caption.
	rcCapt.bottom = rcCapt.top + 19;

	// Create the caption.
	if( !m_Caption.Create( this, _T( "Click push pin to keep open" ), WS_EX_FLATEDGE,
		WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE, rcCapt ))
	{
		TRACE0( "Unable to create caption.\n" );
		return FALSE;
	}

	// define the size for the caption button.
	CRect rcButton( rcCapt );
	rcButton.DeflateRect(2,2);
	rcButton.left = rcButton.right - 16;

	// Create the caption button.
	if( !m_CaptionButton.Create( NULL, WS_VISIBLE|BS_ICON|BS_OWNERDRAW|BS_CENTER|BS_VCENTER,
		rcButton, this, IDC_CAPT_BUTTON ))
	{
		TRACE0( "Unable to create caption button.\n" );
		return FALSE;
	}

	CImageList imageList;
	imageList.Create(IDB_PUSHPIN, 16, 1, RGB(255,0,255));
	
	// Associate the button icon
	m_hIconOff = imageList.ExtractIcon(0);
	m_hIconOn  = imageList.ExtractIcon(1);
	m_CaptionButton.SetIcon(m_hIconOff, m_hIconOn, CSize(16,15));
	imageList.DeleteImageList();

	// start the timer after everything has been initialized.
	SetTimer(1, 10, NULL);

	return TRUE;
}

void CCJCaptionPopupWnd::OnDestroy() 
{
	// set the child window's parent back it's original parent.
	m_pChildWnd->SetParent(m_pChildParent);
	CWnd::OnDestroy();
}

void CCJCaptionPopupWnd::OnCaptButton()
{
	// send message to parent control that push pin button was pressed.
	m_pParentWnd->SendMessage(CM_ONPUSHPINBUTTON, 0, 0);
}

void CCJCaptionPopupWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(1,1);

	// draw a border for the client area.
	dc.Draw3dRect( rc, ::GetSysColor(COLOR_BTNSHADOW),
		::GetSysColor(COLOR_BTNHILIGHT));
}

void CCJCaptionPopupWnd::OnTimer(UINT nIDEvent) 
{
	UNUSED_ALWAYS( nIDEvent );

	// get a handle to the window which currently
	// has the input focus...
	HWND hWnd_HasFocus		= GetFocus()->GetSafeHwnd();

	// get a handle to the caption, caption button, child window
	// and popup window...
	HWND hWnd_Caption		= m_Caption.GetSafeHwnd();
	HWND hWnd_CaptionButton = m_CaptionButton.GetSafeHwnd();
	HWND hWnd_Popup			= GetSafeHwnd();
	HWND hWnd_PopupChild	= m_pChildWnd->GetSafeHwnd();

	// check to see if the window that has input focus is part
	// of the caption popup window, if not, send a message
	// to the parent control.
	if(( hWnd_HasFocus != hWnd_Caption ) &&
	   ( hWnd_HasFocus != hWnd_CaptionButton ) &&
	   ( hWnd_HasFocus != hWnd_Popup ) &&
	   ( hWnd_HasFocus != hWnd_PopupChild ))
	{
		// send message to parent control that our popup window
		// has lost input focus.
		m_pParentWnd->SendMessage(CM_ONPUSHPINCANCEL, 0, 0);
	}
}
