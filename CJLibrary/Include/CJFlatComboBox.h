// CJFlatComboBox.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 6:01p $
 * $Revision: 17 $
 * $Archive: /CodeJock/Include/CJFlatComboBox.h $
 *
 * $History: CJFlatComboBox.h $
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 6:01p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) and re-wrote OnPaint() to handle flicker
 * free drawing.
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:24p
 * Updated in $/CodeJock/Include
 * Added COLORREF members to class to eliminate continuous calls to
 * GetSysColor(...). Overrode OnSysColorChange() to re-initialize COLORREF
 * variables. 
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:39p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 9:24p
 * Updated in $/CodeJockey/Include
 * Gilberto Botaro [seafox@techno.com.br] - Added code for auto
 * completion.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 5/03/99    Time: 10:44p
 * Updated in $/CodeJockey/Include
 * Fixed code ommisions.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 5/03/99    Time: 8:42p
 * Updated in $/CodeJockey/Include
 * Danilo Lax [Danilo_Lax@www2.alpeng.com] - The control is painted flat
 * when the mouse leaves the box, even if the control has the focus, now
 * checking for the focus in OnTimer(.
 * 
 * Removed, unhandled msgs are processed by
 * CComboBox::PreTranslateMessage(pMsg). Used reflected msgs
 * BN_GETFOCUS/CBN_KILLFOCUS instead of WM_GETFOCUS/WM_KILLFOCUS.
 * 
 * ::GetFocus() doesn't return CFlatComboBox::m_hWnd, even if the control
 * has the focus (don't know why, maybe ::GetFocus() only returns the
 * handle to the EditField part of the ComboBox), used flag m_bHasFocus
 * instead.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:26p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 1/22/99    Time: 4:26p
 * Updated in $/CodeJockey/Include
 * Fixed accelerator problem reported by YOSH YAMASHITA
 * (yosh@muj.biglobe.ne.jp) with Matt Weagle's solution of overloading
 * PreTranslateMessage().
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 1/02/99    Time: 4:25p
 * Updated in $/CodeJockey/Include
 * Renamed class from CJComboBox to CCJFlatComboBox, changed DRAWSTATE to
 * enum, removed method Offset(), and replaced with a direct call to
 * ::GetSystemMetrics().
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 11/02/98   Time: 4:24p
 * Updated in $/CodeJockey/Include
 * Added OnSetFocus(), OnKillFocus() and modified OnPaint() so the control
 * remains beveled until it actually looses focus - (Vachik
 * Hovhannissian).
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 10/05/98   Time: 4:24p
 * Updated in $/CodeJockey/Include
 * Fixed offset bug to use GetSystemMetrics(), instead of hard coded
 * values -
 * Todd Brannam (tbrannam@msn.com).
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/02/98   Time: 4:23p
 * Created in $/CodeJockey/Include
 * First release version.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFLATCOMBOBOX_H__
#define __CJFLATCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CCJFlatComboBox is a CComboBox derived class which is simular to
// the flat combobox as seen in the Microsoft office products.
class _CJX_EXT_CLASS CCJFlatComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCJFlatComboBox)

public:
	
	// Default constructor
	//
	CCJFlatComboBox();

	// Virtual destructor
	//
	virtual ~CCJFlatComboBox();

protected:

	// enum used to determine the state the combo box should be
	//
	enum STATE { NORMAL = 1, RAISED = 2, PRESSED = 3 };

	BOOL		m_bLBtnDown;		// TRUE if left mouse button is pressed
	BOOL		m_bPainted;			// used during paint operations
	BOOL		m_bHasFocus;		// TRUE if control has focus
	BOOL		m_bAutoComp;		// Used by Autocompleting.
	COLORREF	m_clrHilite;		// set to ::GetSysColor(COLOR_BTNHIGHLIGHT)
	COLORREF	m_clrShadow;		// set to ::GetSysColor(COLOR_BTNSHADOW)
	COLORREF	m_clrNormal;		// set to ::GetSysColor(COLOR_BTNFACE)

public:
	BOOL IsWin95();

	// Call this member function to enable auto completion.
	//
	virtual void EnableAutoCompletion(BOOL bEnable=TRUE);

	// This member function determines if the mouse is within
	// the controls rect.
	//
	virtual BOOL PointInRect();

protected:

	// this member function is called by the combo box whenever a paint
	// operation should occur.
	//
	virtual void DrawCombo(STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight, CDC* pDC=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatComboBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus();
	afx_msg void OnKillFocus();
	afx_msg void OnEditUpdate();
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJFlatComboBox::EnableAutoCompletion(BOOL bEnable/*=TRUE*/)
	{ ASSERT(::IsWindow(m_hWnd)); m_bAutoComp = bEnable; }

_CJXLIB_INLINE BOOL CCJFlatComboBox::PointInRect()
	{ ASSERT(::IsWindow(m_hWnd)); CPoint pt; GetCursorPos(&pt); CRect rcItem; GetWindowRect(&rcItem); return rcItem.PtInRect(pt); }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJCOMBOBOX_H__
