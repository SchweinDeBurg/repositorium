// CJTabCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 2:18p $
 * $Revision: 11 $
 * $Archive: /CodeJock/Include/CJTabCtrl.h $
 *
 * $History: CJTabCtrl.h $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:18p
 * Updated in $/CodeJock/Include
 * OverrodeOnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:48p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:44p
 * Updated in $/CodeJockey/Include
 * Added _DYNAMIC macros for access to the CObject::IsKindOf to determine
 * the class object at runtime. Thanks to Sid Price
 * [sidprice@softtools.com] for suggesting this.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:47p
 * Updated in $/CodeJockey/Include
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/11/99    Time: 9:33p
 * Updated in $/CodeJockey/Include
 * Renamed popup menu resource id to m_popupMenuID to be consistent with
 * other classes.  Added member variable m_popToolbarID to contain the
 * toolbar resource to be used for popup menus  and m_nPos to contain the
 * index of the submenu in the menu resource. Added
 * CCJTabCtrl::Create(...) to simplify the creation process, and to set
 * the tab font.  Cleaned up OnRButtonDown(..) message handler which
 * activates the popup menu. Modified SetMenuID(...) to accept the index
 * of the popup menu in the menu resource.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 5:25p
 * Updated in $/CodeJockey/Include
 * Removed class from CCJTabCtrlBar files into its own files. Added popup
 * menu support for CCJMenus.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 4:34p
 * Created in $/CodeJockey/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTABCTRL_H__
#define __CJTABCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJMenu.h"        // CCJMenu class declaration

// CCJTabCtrl is a CTabCtrl derived class which is used by CCJTabCtrl
// to help fix problems with repainting in the tab control
class _CJX_EXT_CLASS CCJTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CCJTabCtrl)

public:

	// Default constructor
	//
	CCJTabCtrl();

	// Virtual destructor
	//
	virtual ~CCJTabCtrl();

public:

	UINT			m_popToolbarID;	// popup menu toolbar resource id, associates 
									// toolbar icons with the popup menu.
	
	UINT			m_popupMenuID;	// popup menu resource id
	CCJMenu			m_popupMenu;	// popup menu
	int				m_nPos;			// index of the pop-up menu contained in the menu
	CFont			m_Font;			// font used by tab control.

public:
	virtual void UpdateFont();

	// this member function is called to associate a menu and toolbar
	// resource with the context menu.
	//
	virtual void SetMenuID(UINT popupMenuID, UINT popToolbarID=0, int nPos=0);

	// this member function returns the menu resource associated with
	// the tab control.
	//
	virtual UINT GetMenuID();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabCtrl)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID=AFX_IDC_TAB_CONTROL, DWORD dwStyle=WS_VISIBLE|WS_CHILD|TCS_BOTTOM|TCS_TOOLTIPS|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, const CRect& rect=CRect(0,0,0,0));
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJTabCtrl::SetMenuID(UINT popupMenuID, UINT popToolbarID, int nPos)
	{ m_popupMenuID = popupMenuID; m_popToolbarID = popToolbarID; m_nPos = nPos; }

_CJXLIB_INLINE UINT CCJTabCtrl::GetMenuID()
	{ return m_popupMenuID; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJTABCTRL_H__

