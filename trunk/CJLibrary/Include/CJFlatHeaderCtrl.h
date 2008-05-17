// CJFlatHeaderCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 13/06/02 16:44 $
 * $Revision: 3 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/Include/CJFlatHeaderCtrl.h $
 *
 * $History: CJFlatHeaderCtrl.h $
 * 
 * *****************  Version 3  *****************
 * User: Andy         Date: 13/06/02   Time: 16:44
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Added CCJFlatHeaderCtrl::GetSortImageWidth() to assist in column
 * autosizing
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 28/03/01   Time: 15:44
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Bug fix by Paulo Breda Vieira 1999-11-19
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:01p
 * Updated in $/CodeJock/Include
 * Further Updates to OnEraseBkgnd(...) and OnPaint() for flicker free
 * drawing.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:23p
 * Updated in $/CodeJock/Include
 * Overrode OnPaint(...) for flicker free drawing and fixed bug where
 * column is resized and border leaves a trail.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:39p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/Include
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 8:23p
 * Updated in $/CodeJock/Include
 * Fixed size bug with header control where the end does not display
 * correctly.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:43p
 * Updated in $/CodeJockey/Include
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 8:00p
 * Updated in $/CodeJockey/Include
 * Added popup menu support and vc5 compatibility.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 4/15/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * Igor Ostriz [iostriz@usa.net] - fixed bug with drag/drop and changing
 * column order.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:26p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 4:26p
 * Updated in $/CodeJockey/Include
 * Fixed repainting problem when header is moved or window is resized. The
 * class now uses log font instead of hard coded Tahoma font.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * Fixed problem with sort arrow drawn. If hditem has an associated
 * bitmap, the arrow is not drawn.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 4:26p
 * Created in $/CodeJockey/Include
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFLATHEADERCTRL_H__
#define __CJFLATHEADERCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJMenu.h"        // CCJMenu class declaration

// CCJFlatHeaderCtrl is a CHeaderCtrl derived class which is simular
// to the listbox flat header controls seen in Visual Studio and Outlook.
class _CJX_EXT_CLASS CCJFlatHeaderCtrl : public CHeaderCtrl
{

public:
	
	// Default constructor
	//
	CCJFlatHeaderCtrl();

	// Virtual destructor
	//
	virtual ~CCJFlatHeaderCtrl();

protected:
	UINT	m_popToolbarID;	// popup menu toolbar resource id, associates 
							// toolbar icons with the popup menu.
	
	UINT	m_popupMenuID;	// popup menu resource id
	CCJMenu	m_popupMenu;	// popup menu
	int		m_nPos;			// index of the pop-up menu contained in the menu
	BOOL	m_bBoldFont;	// true for bold fonts see SetFontBold(...)
	BOOL	m_bLBtnDown;	// true if left mouse button is pressed
	BOOL	m_bSortAsc;		// used when column is pressed to draw sort arrow.
	CFont	m_Font;			// font the header will use
	int		m_nOffset;		// ammount to offset the sort arrow.
	int		m_nSortCol;		// last column pressed during sort.
	CWnd*	m_pParentWnd;	// holds a pointer to the parent window.
	CRect   m_rcOldSize;

public:

	// this member function is called to associate a menu and toolbar
	// resource with the context menu.
	//
	virtual void SetMenuID(UINT popupMenuID, UINT popToolbarID=0, int nPos=0);

	// this member function returns the menu resource associated with
	// the tab control.
	//
	virtual UINT GetMenuID();

	// this member function must be called after creation to initialize the
	// font the header will use.
	//
	virtual void InitializeHeader(bool bBoldFont);

	// this member function can be used to toggle the font from bold to
	// normal.
	//
	virtual void SetFontBold(BOOL bBoldFont = TRUE);
	
	// this member function is called by the header during sort
	// operations
	//
	virtual int SetSortImage( int nCol, BOOL bAsc );

	virtual int GetSortImageWidth(void) const
					{ return 3 * m_nOffset; }

protected:

	// this member function is called by the header during paint
	// operations.
	//
	virtual void DrawFlatBorder(CDC* pDC);
// Changes in code by Paulo Breda Vieira 1999-11-19
//	virtual void ResizeWindow();

#ifdef _VC_VERSION_5
	// this member function was added for vc5 support. It returns the number
	// of columns currently in the header.
	//
	virtual int CCJFlatHeaderCtrl::GetItemCount() const;
	virtual BOOL GetOrderArray(LPINT piArray, int iCount= -1);
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatHeaderCtrl)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

// Changes in code by Paulo Breda Vieira 1999-11-19
//	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJFlatHeaderCtrl::SetMenuID(UINT popupMenuID, UINT popToolbarID, int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); m_popupMenuID=popupMenuID; m_popToolbarID=popToolbarID; m_nPos=nPos; }

_CJXLIB_INLINE UINT CCJFlatHeaderCtrl::GetMenuID()
	{ ASSERT(::IsWindow(m_hWnd)); return m_popupMenuID; }

_CJXLIB_INLINE void CCJFlatHeaderCtrl::SetFontBold(BOOL bBoldFont)
	{ ASSERT(::IsWindow(m_hWnd)); m_bBoldFont = bBoldFont; }

#ifdef _VC_VERSION_5
_CJXLIB_INLINE int CCJFlatHeaderCtrl::GetItemCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, HDM_GETITEMCOUNT, 0, 0L); }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJFLATHEADERCTRL_H__
