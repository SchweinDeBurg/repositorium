// CJCaption.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/05/02 15:55 $
 * $Revision: 2 $
 * $Archive: /Projects/Libraries/CJLibrary/Include/CJCaption.h $
 *
 * $History: CJCaption.h $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 10/05/02   Time: 15:55
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * Changes for compatibility with VC 7.0
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:41p
 * Updated in $/CodeJock/Include
 * Updated OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * Fixed bug when popup window is closed, the mainframe resizes and then
 * the popup is reopened and pushpin is pressed, the child will not be
 * correctly sized and positioned. Added changes to handle flicker free
 * drawing of the control window. 
 * 
 * Modified resource include for static builds.
 * 
 * Fixed the bug where the icon on the right hand side of the bar doesn't
 * clip when you resize the bar to almost nothing. Added method
 * SetTextStyle(...) to set the CDC:: DrawText(...) text styles to be user
 * defined. Thanks to Adrian for help with this.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Fixed small bug with UNICODE display placing unwanted character at the
 * end of the string.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/24/99    Time: 9:32p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 8:13p
 * Updated in $/CodeJockey/Include
 * Fixed painting bug with border, cleaned up inline methods.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 12:36p
 * Updated in $/Code Jockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 12:27p
 * Updated in $/CodeJockey/Include
 * Added further functionality to class so that you can define text size,
 * text color, border size, border and background color. Basically to
 * create a look similar to Outlook's Captions.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 2/01/99    Time: 12:28p
 * Updated in $/CodeJockey/Include
 * Made some cosmetic fixes. 
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 12:27p
 * Created in $/CodeJockey/Include
 * Initial first release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCAPTION_H__
#define __CJCAPTION_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJFlatButton.h"

// CCJCaptionButton is a CCJFlatButton derived class which is used by
// the CCJCaption class to activate a CCJCaptionPopupWnd window.
class _CJX_EXT_CLASS CCJCaptionButton : public CCJFlatButton
{
public:
	
	virtual BOOL SetButtonStyle(CFont* pFont, COLORREF clrBackground, COLORREF clrFont);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJCaptionButton)
	virtual void DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJCaptionButton)
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CCJCaptionPopupWnd;

#define WS_EX_FLATEDGE	0x01

// CCJCaption is a CStatic derived class which is simular to the
// caption or info bars that are seen in Microsoft Outlook
class _CJX_EXT_CLASS CCJCaption : public CStatic
{
public:
	
	// Default constructor.
	//
	CCJCaption();

	// Virtual destructor
	//
	virtual ~CCJCaption();

protected:

	COLORREF			m_clrBtnHilite;	// set to system color COLOR_BTNHILIGHT
	COLORREF			m_clrBtnShadow;	// set to system color COLOR_BTNSHADOW
	COLORREF			m_clrBtnFace;	// set to system color COLOR_BTNFACE
	COLORREF			m_clrBorder;	// user defined border color
	COLORREF			m_clrBackgnd;	// user defined background color
	COLORREF			m_clrFont;		// user defined font color
	int					m_nBorder;		// size in pixels for the caption border
	CFont*				m_pFont;		// user defined font pointer, default value NULL
	CFont				m_Font;			// default font created by control if m_pFont is NULL
	HICON				m_hIcon;		// user defined icon handle, default value NULL
	CString				m_strWindText;	// text that will be displayed in caption.
	int					m_cx;			// icon width.
	int					m_cy;			// icon height.
	int					m_nOffset;
	
	CWnd*				m_pChildWnd;
	CWnd*				m_pNotifyWnd;
	CCJCaptionButton	m_CaptionButton;
	CCJCaptionPopupWnd* m_pWndPopDown;
	CRect				m_rcChild;
	DWORD				m_dwExStyle;
	CRect				m_rcClient;
	DWORD				m_dwTextStyle;

public:
	
	// Call this member function to set the caption bar border, background and
	// font colors.
	//
	virtual void SetCaptionColors(
		// A COLORREF value for the new border color.
		COLORREF clrBorder,
		// A COLORREF value for the new background color.
		COLORREF clrBackground,
		// A COLORREF value for the new font color.
		COLORREF clrFont);
	
	// Call this function to modify the caption style. You can use this member function
	// to set the border size that is drawn around the caption banner, the font that the
	// caption will use, the caption text and icon to be displayed.
	//
	virtual void ModifyCaptionStyle(
		// Specifies size in pixels of the banner border.
		int nBorderSize,
		// Specifies the new caption font.
		CFont* pFont=NULL,
		// Address of a null-terminated string specifying the new caption text.
		LPCTSTR lpszWindText=NULL,
		// Handle of the icon to be drawn in the caption.
		HICON hIcon=NULL);

	virtual void SetChildWindow(CWnd* pChild, CWnd* pNotifyWnd);
	virtual void KillChildWindow();
	
	virtual void DrawCaptionBack(CDC* pDC, CRect& rcItem);
	virtual void DrawCaptionText(CDC* pDC, CRect& rcItem);
	virtual void DrawCaptionIcon(CDC* pDC, CRect& rcItem);

public:
	virtual void UpdateFont();
	virtual void UpdateCaption(LPCTSTR lpszWindowText, HICON hIcon);
	virtual void UpdateCaptionBack(CDC* pDC, CRect& rcItem);
	virtual void UpdateCaptionText(CDC* pDC, CRect& rcItem);
	virtual void SetTextStyle(DWORD dwStyle); 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJCaption)
	public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszWindowName, DWORD dwExStyle=WS_EX_FLATEDGE, DWORD dwStyle=WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE, const CRect& rect=CRect(0,0,0,0), UINT nID = 0xffff);
	//}}AFX_VIRTUAL

	// Generated message map functions

protected:
	//{{AFX_MSG(CCJCaption)
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnCaptButton();
	afx_msg LRESULT OnPushPinButton(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPushPinCancel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJCaption::SetTextStyle(DWORD dwStyle)
	{ m_dwTextStyle = dwStyle; }

_CJXLIB_INLINE void CCJCaption::SetCaptionColors(COLORREF clrBorder, COLORREF clrBackground, COLORREF clrFont)
	{ ASSERT(::IsWindow(m_hWnd)); m_clrBorder = clrBorder; m_clrBackgnd = clrBackground; m_clrFont = clrFont; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJCAPTION_H__

