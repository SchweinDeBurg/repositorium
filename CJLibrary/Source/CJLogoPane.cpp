// CJLogoPane.cpp : implementation file
//
// Copyright © 1999 Pierre MELLINAND
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 11:08p $
 * $Revision: 1 $
 * $Archive: /CodeJock/CJLibrary/CJLogoPane.cpp $
 *
 * $History: CJLogoPane.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 11:08p
 * Created in $/CodeJock/CJLibrary
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJLogoPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJLogoPane

CCJLogoPane::CCJLogoPane()
{
	SetLogoFont(_T("Arial"));
}

CCJLogoPane::~CCJLogoPane()
{
	m_Font.DeleteObject();
}

IMPLEMENT_DYNCREATE(CCJLogoPane, CWnd)

BEGIN_MESSAGE_MAP(CCJLogoPane, CWnd)
	//{{AFX_MSG_MAP(CCJLogoPane)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJLogoPane message handlers

void CCJLogoPane::SetLogoFont(LPCTSTR lpszFontName, int nHeight/*=24*/,
	int nWeight/*=FW_BOLD*/, BOOL bItalic/*=TRUE*/, BOOL bUnderline/*=FALSE*/)
{
	if(m_Font.m_hObject) {
		m_Font.DeleteObject();
	}

	m_Font.CreateFont(nHeight, 0, 0, 0, nWeight, (BYTE)bItalic, (BYTE)bUnderline, 0,0,0,0,0,0, lpszFontName);
}

void CCJLogoPane::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetWindowRect(rect);

	dc.SetBkMode(TRANSPARENT);

	CFont * OldFont = dc.SelectObject(&m_Font);

	// draw text in DC
	COLORREF OldColor = dc.SetTextColor(::GetSysColor( COLOR_3DHILIGHT));

	rect.right  = rect.Width();
	rect.bottom = rect.Height();
	rect.left   = rect.top = 0;
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_3DFACE));

	dc.DrawText( m_strLogoText, rect + CPoint(1,1), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW));
	dc.DrawText( m_strLogoText, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	// restore old text color
	dc.SetTextColor( OldColor);
	// restore old font
	dc.SelectObject(OldFont);
}

BOOL CCJLogoPane::Create(LPCTSTR lpszWindowName, DWORD dwStyle, CWnd* pParentWnd, UINT nID) 
{
	CString wndClass = ::AfxRegisterWndClass(CS_BYTEALIGNCLIENT);

	if(!CWnd::Create(wndClass, lpszWindowName, dwStyle, CRect(0,0,0,0), pParentWnd, nID)) {
		return FALSE;
	}

	m_strLogoText = lpszWindowName;
	return TRUE;
}
