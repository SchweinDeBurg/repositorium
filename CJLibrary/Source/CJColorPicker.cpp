// CJColorPicker.cpp : implementation file
//
// CJColorPicker is a drop-in Color picker control. Check out the 
// header file or the accompanying HTML doc file for details.
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// Updated 16 May 1998
//         31 May 1998 - added Default text (CJM)
//          9 Jan 1999 - minor vis update
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 13/12/02 23:04 $
 * $Revision: 2 $
 * $Archive: /Projects/Libraries/CJLibrary/CJLibrary/CJColorPicker.cpp $
 *
 * $History: CJColorPicker.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Anna         Date: 13/12/02   Time: 23:04
 * Updated in $/Projects/Libraries/CJLibrary/CJLibrary
 * CCJColorPicker is now theme aware
 * 
 *
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:49p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...), OnPaint() and made modifications to
 * DrawItem(...) for flicker free drawing.
 * 
 * Modified resource include for static builds.
 * 
 * Added method CheckTextColor(...) which tests if the intensity of the
 * color is greater as 128. If the intensity < 128 => color is dark, so
 * the text must be light. Stephane Routelous
 * [routelous@cad-cam-concept.de]
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Removed un-necessary calls to GetParent(), a call is made only once at
 * initialization, to ensure we are working with a valid handle.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:22p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tmschema.h>

#include "CJColorPopup.h"
#include "CJColorPicker.h"
#include "CJResource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CCJColorPicker* pColorPicker = (CCJColorPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        crColor = pColorPicker->GetColor();
    }
    else // initializing
    {
        pColorPicker->SetColor(crColor);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker

CCJColorPicker::CCJColorPicker()
{
    SetBkColor(GetSysColor(COLOR_3DFACE));
    SetTextColor(GetSysColor(COLOR_BTNTEXT));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

// 1999-06-11 begin mods KStowell
    m_strDefaultText.LoadString( IDS_COLOR_AUTO );
    m_strCustomText.LoadString( IDS_COLOR_CUST );
// 1999-06-11 end mods KStowell

// 10-14-1999 - Kirk Stowell
	m_pParentWnd = NULL;

	// AJM 5.12.2002
	m_pTheme		= NULL;
	m_bMouseOver	= FALSE;
}


CCJColorPicker::~CCJColorPicker()
{
}

IMPLEMENT_DYNCREATE(CCJColorPicker, CButton)

BEGIN_MESSAGE_MAP(CCJColorPicker, CButton)
    //{{AFX_MSG_MAP(CCJColorPicker)
    ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
    ON_CONTROL_REFLECT_EX(	BN_CLICKED,			OnClicked)
	//}}AFX_MSG_MAP
    ON_MESSAGE(				CPN_SELENDOK,		OnSelEndOK)
    ON_MESSAGE(				CPN_SELENDCANCEL,	OnSelEndCancel)
    ON_MESSAGE(				CPN_SELCHANGE,		OnSelChange)
	ON_MESSAGE(				WM_MOUSELEAVE,		OnMsgMouseLeave)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker message handlers

LONG CCJColorPicker::OnSelEndOK(UINT lParam, LONG /*wParam*/)
{
    COLORREF crNewColor = (COLORREF) lParam;
    m_bActive = FALSE;
    SetColor(crNewColor);
	//BEGIN SRO
	CheckTextColor(crNewColor);
	//END SRO
	
	// 10-14-1999 - Kirk Stowell - Removed redundant calls to GetParent().
    if (m_pParentWnd) {
        m_pParentWnd->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        m_pParentWnd->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (crNewColor != GetColor()) {
        if (m_pParentWnd) {
			m_pParentWnd->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());
		}
	}

    return TRUE;
}

LONG CCJColorPicker::OnSelEndCancel(UINT lParam, LONG /*wParam*/)
{
    m_bActive = FALSE;
    SetColor((COLORREF) lParam);
	//BEGIN SRO
	CheckTextColor((COLORREF) lParam);
	//END SRO
	
	// 10-14-1999 - Kirk Stowell - Removed redundant calls to GetParent().
    if (m_pParentWnd) {
        m_pParentWnd->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        m_pParentWnd->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CCJColorPicker::OnSelChange(UINT lParam, LONG /*wParam*/)
{
    if (m_bTrackSelection) {
		SetColor((COLORREF) lParam);
		//BEGIN SRO
		CheckTextColor((COLORREF) lParam);
		//END SRO
	}
	
	// 10-14-1999 - Kirk Stowell - Removed redundant calls to GetParent().
    if (m_pParentWnd) {
		m_pParentWnd->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());
	}

    return TRUE;
}

int CCJColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CJColorPopup window for Color selection
BOOL CCJColorPicker::OnClicked()
{
    m_bActive = TRUE;

    CRect rect;
    GetWindowRect(rect);
    new CCJColorPopup(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColor(),                       // Selected Color
                     this,                              // parent
                     m_strDefaultText,                  // "Default" text area
                     m_strCustomText);                  // Custom Text

	// 10-14-1999 - Kirk Stowell - Removed redundant calls to GetParent().
    if (m_pParentWnd) {
        m_pParentWnd->SendMessage(CPN_DROPDOWN, (LPARAM)GetColor(), (WPARAM) GetDlgCtrlID());
	}

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

// 1999-06-11 begin mods KStowell
void CCJColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);

	// define some temporary variables.
	CDC*	pDC			= CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect	rcItem		= lpDrawItemStruct->rcItem;
    int 	nState		= lpDrawItemStruct->itemState;
	DWORD	dwState		= EDGE_RAISED;
	DWORD	dwArrow		= DFCS_SCROLLDOWN;

	
	BOOL bIsThemed = ( (NULL != m_pTheme) && m_pTheme->IsAppThemed() );

	HTHEME	hTheme		= NULL;
	HTHEME	hArrowTheme	= NULL;

	if (bIsThemed)
	{
		hTheme			= m_pTheme->OpenThemeData(GetSafeHwnd(), L"BUTTON");
		hArrowTheme		= m_pTheme->OpenThemeData(GetSafeHwnd(), L"COMBOBOX");
	}


	if (NULL != hTheme)
	{
		int	iStateId = PBS_NORMAL;
		
		if ( (nState & ODS_DISABLED) != 0)
		{
			iStateId = PBS_DISABLED;
		}
		else
		{
			if ( (nState & ODS_HOTLIGHT) != 0 || m_bMouseOver)
			{
				iStateId = PBS_HOT;
			}
			if ( (nState & ODS_SELECTED) != 0 || m_bActive)
			{
				iStateId |= PBS_PRESSED;
			}
			if (nState & ODS_FOCUS)
			{
				iStateId |= PBS_DEFAULTED;
			}
			if ( (nState & ODS_DEFAULT) != 0)
			{
				iStateId |= PBS_DEFAULTED;
			}
		}
		// Draw the background image defined by the visual style
		m_pTheme->DrawThemeBackground(	hTheme,
										m_hWnd,
										pDC->GetSafeHdc(),
										BP_PUSHBUTTON,
										iStateId,
										&rcItem,
										NULL);

		// Get the size of the content area for the background defined by the visual style.
		m_pTheme->GetThemeBackgroundContentRect(hTheme,
												pDC->GetSafeHdc(),
												BP_PUSHBUTTON, 
												iStateId,
												&rcItem,
												&rcItem);

	}
	else
	{
		// Paint the background.
		pDC->FillSolidRect(rcItem, ::GetSysColor(COLOR_3DFACE));

	}
	// set display flags based on state.
	if((nState & ODS_SELECTED) || (m_bActive == TRUE)) {
		dwState = EDGE_SUNKEN;
		dwArrow = DFCS_SCROLLDOWN|DFCS_PUSHED;
	}
	if(nState & ODS_DISABLED) {
		dwArrow = DFCS_SCROLLDOWN|DFCS_INACTIVE;
	}
	
	// Draw the drop arrow and the colour box
	if (NULL != hArrowTheme)
	{
		CRect rcArrow( rcItem );

		int nArrowWidth = 16 + ::GetSystemMetrics(SM_CXEDGE) / 2;

		rcArrow.left = rcArrow.right - nArrowWidth;
		
		rcArrow.DeflateRect(2,1);
		rcArrow.OffsetRect(1,0);

		int	iStateId = CBXS_NORMAL;

		if ( (nState & ODS_DISABLED) != 0)
		{
			iStateId = CBXS_DISABLED;
		}
		else
		{
			if ( (nState & ODS_SELECTED) != 0 || m_bActive)
			{
				iStateId |= CBXS_PRESSED;
			}
			if ( (nState & ODS_HOTLIGHT) != 0 || m_bMouseOver)
			{
				iStateId = CBXS_HOT;
			}
		}

		// Draw the drop arrow
		// We use the drop arrow component from a combobox
		m_pTheme->DrawThemeBackground(	hArrowTheme,
										m_hWnd,
										pDC->GetSafeHdc(),
										CP_DROPDOWNBUTTON,
										iStateId,
										&rcArrow,
										NULL);

		// draw the separator line.
		CPen penShadow( PS_SOLID, 1, ::GetSysColor( COLOR_3DSHADOW  ));
		CPen* pOldPen = pDC->SelectObject( &penShadow );

		CPen penHilite( PS_SOLID, 1, ::GetSysColor( COLOR_3DHILIGHT ));
		pDC->SelectObject( &penHilite );
	
		// Draw the colour box.
		rcItem.DeflateRect( 1, 1 );
		rcItem.right = rcArrow.left-1;
		
		CBrush brush( ((nState & ODS_DISABLED) || m_crColorBk == CLR_DEFAULT)? 
			::GetSysColor(COLOR_3DFACE) : m_crColorBk);
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject( &brush );
		pDC->SelectStockObject(( nState & ODS_DISABLED )?WHITE_PEN:BLACK_PEN );
		pDC->Rectangle( rcItem );
		
		// Draw the window text (if any)
		CString strText;
		GetWindowText(strText);
		if (strText.GetLength())
		{
			pDC->SetBkMode(TRANSPARENT);
			if (nState & ODS_DISABLED)
			{
				rcItem.OffsetRect(1,1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				rcItem.OffsetRect(-1,-1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
			else
			{
				pDC->SetTextColor((m_crColorText == CLR_DEFAULT)? 0 : m_crColorText);
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
		}
		// fix potential resource leak - KStowell - 10-21-99
		pDC->SelectObject( pOldPen );
		pDC->SelectObject( pOldBrush );
		brush.DeleteObject();
		penHilite.DeleteObject();
		penHilite.DeleteObject();
	}
	else
	{
		CRect rcArrow( rcItem );
		rcArrow.left = rcArrow.Width()-::GetSystemMetrics( SM_CXHTHUMB );
		pDC->DrawFrameControl( &rcArrow, DFC_SCROLL, dwArrow );
		pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
		rcArrow.DeflateRect( 1,1 );
		pDC->Draw3dRect( rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
		
		if((nState & ODS_SELECTED) || (m_bActive == TRUE))
			rcArrow.OffsetRect( 1,1 );
		
		// draw the seperator line.
		CPen penShadow( PS_SOLID, 1, ::GetSysColor( COLOR_3DSHADOW  ));
		CPen* pOldPen = pDC->SelectObject( &penShadow );
		pDC->MoveTo( rcArrow.left-1, rcArrow.top+2 );
		pDC->LineTo( rcArrow.left-1, rcArrow.bottom-2 );

		CPen penHilite( PS_SOLID, 1, ::GetSysColor( COLOR_3DHILIGHT ));
		pDC->SelectObject( &penHilite );
		pDC->MoveTo( rcArrow.left, rcArrow.top+2 );
		pDC->LineTo( rcArrow.left, rcArrow.bottom-2 );
		
		// draw the control border.
		pDC->DrawEdge( rcItem, dwState, BF_RECT );
		
		// draw the focus rect.
		rcItem.DeflateRect( 2, 2 );
		pDC->Draw3dRect( rcItem, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ) );
		if( nState & ODS_FOCUS ) {
			pDC->DrawFocusRect( rcItem );
		}
		
		// draw the color box.
		if((nState & ODS_SELECTED) || (m_bActive == TRUE))
			rcItem.OffsetRect( 1,1 );
		
		rcItem.DeflateRect( 2, 2 );
		rcItem.right = rcArrow.left-4;
		
		CBrush brush( ((nState & ODS_DISABLED) || m_crColorBk == CLR_DEFAULT)? 
			::GetSysColor(COLOR_3DFACE) : m_crColorBk);
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject( &brush );
		pDC->SelectStockObject(( nState & ODS_DISABLED )?WHITE_PEN:BLACK_PEN );
		pDC->Rectangle( rcItem );
		
		// Draw the window text (if any)
		CString strText;
		GetWindowText(strText);
		if (strText.GetLength())
		{
			pDC->SetBkMode(TRANSPARENT);
			if (nState & ODS_DISABLED)
			{
				rcItem.OffsetRect(1,1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				rcItem.OffsetRect(-1,-1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
			else
			{
				pDC->SetTextColor((m_crColorText == CLR_DEFAULT)? 0 : m_crColorText);
				pDC->DrawText(strText, rcItem, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
		}

		// fix potential resource leak - KStowell - 10-21-99
		pDC->SelectObject( pOldPen );
		pDC->SelectObject( pOldBrush );
		brush.DeleteObject();
		penHilite.DeleteObject();
		penHilite.DeleteObject();
	}
}
// 1999-06-11 end mods KStowell

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker overrides

void CCJColorPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker operations

void CCJColorPicker::SetThemeHelper(CCJThemeHelper* pTheme)
{
	m_pTheme = pTheme;
}


COLORREF CCJColorPicker::GetColor()
{ 
    return (m_nSelectionMode == CP_MODE_TEXT)? 
        GetTextColor(): GetBkColor(); 
}

void CCJColorPicker::SetColor(COLORREF crColor)
{ 
    (m_nSelectionMode == CP_MODE_TEXT)? 
        SetTextColor(crColor): SetBkColor(crColor); 
}

void CCJColorPicker::SetBkColor(COLORREF crColorBk)
{
    m_crColorBk = crColorBk;
	
	//BEGIN SRO
	CheckTextColor(crColorBk);
	//END SRO
	
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetTextColor(COLORREF crColorText)
{
    m_crColorText = crColorText;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CCJColorPicker::SetDefaultText(LPCTSTR szDefaultText)
{
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
}

void CCJColorPicker::SetCustomText(LPCTSTR szCustomText)
{
    m_strCustomText = (szCustomText)? szCustomText : _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker implementation

void CCJColorPicker::SetWindowSize()
{
	// 10-14-1999 - Kirk Stowell - Added to help reduce flicker, we only need to make
	// a call to GetParent() once at initialization.
	m_pParentWnd = GetParent();

	BOOL bIsThemed = ( (NULL != m_pTheme) && m_pTheme->IsAppThemed() );

	if (!bIsThemed)
	{
		// Get size dimensions of edges
		CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

		// Get size of dropdown arrow
		int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
		int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
		CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));

		// Get window size
		CRect rect;
		GetWindowRect(rect);

		if (m_pParentWnd) {
			m_pParentWnd->ScreenToClient(rect);
		}

		// Set window size at least as wide as 2 arrows, and as high as arrow + margins
		int nWidth = max(rect.Width(), 2*ArrowSize.cx + 2*MarginSize.cx);
		MoveWindow(rect.left, rect.top, nWidth, ArrowSize.cy+2*MarginSize.cy, TRUE);

		// Get the new coords of this window
		GetWindowRect(rect);
		ScreenToClient(rect);

		// Get the rect where the arrow goes, and convert to client coords.
		m_ArrowRect.SetRect(rect.right - ArrowSize.cx - MarginSize.cx, 
							rect.top + MarginSize.cy, rect.right - MarginSize.cx,
							rect.bottom - MarginSize.cy);
	}
}


void CCJColorPicker::CheckTextColor(COLORREF crColor)
{
	int R = GetRValue(crColor);
	int G = GetGValue(crColor);
	int B = GetBValue(crColor);
	
	double Intensity = .299 * R + .587 * G + .114 * B ;
	
	if ( Intensity >= 127 ) {
		SetTextColor(RGB(0,0,0));
	} else {
		SetTextColor(RGB(255,255,255));
	}
}

BOOL CCJColorPicker::OnEraseBkgnd(CDC* pDC) 
{
	// overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJColorPicker::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// KStowell - Get the client rect.
	CRect rcClient, rcClip;
	dc.GetClipBox( &rcClip );
	GetClientRect( &rcClient );

	// KStowell - Create a memory device-context. This is done to help reduce
	// screen flicker, since we will paint the entire control to the
	// off screen device context first.
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// KStowell - Repaint the background.
	memDC.FillSolidRect( rcClient, ::GetSysColor(COLOR_WINDOW) );

	// let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}



// AJM 6.12.2002 added to support "hot" events
void CCJColorPicker::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseOver)
	{
		m_bMouseOver = TRUE;

		TRACKMOUSEEVENT tme;
		tme .cbSize = sizeof (tme);
		tme .dwFlags = TME_LEAVE;
		tme .hwndTrack = m_hWnd;
		::_TrackMouseEvent (&tme);

		InvalidateRect (NULL);
	}
	CButton::OnMouseMove(nFlags, point);
}


LRESULT CCJColorPicker::OnMsgMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bMouseOver)
	{
		m_bMouseOver = FALSE;

		InvalidateRect (NULL);
	}
	return Default();
}

