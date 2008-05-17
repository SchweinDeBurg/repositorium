// CJFrameWnd.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/29/99 12:43a $
 * $Revision: 16 $
 * $Archive: /CodeJock/Include/CJFrameWnd.h $
 *
 * $History: CJFrameWnd.h $
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/29/99   Time: 12:43a
 * Updated in $/CodeJock/Include
 * Overloaded PreCreateWindow(...) for flicker free drawing.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:41p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 6:15p
 * Updated in $/CodeJock/Include
 * Fixed bug with a document becomes the active document when the user
 * clicks inside a tab in the CCJTabControlBar to perform some operation.
 * When closing the application, MFC decides in CFrameWnd::OnClose() to
 * check if the 'active' document is modified before checking all other
 * documuents. If the active document is not modified and can close its
 * frame, it does not check other documents individually and shuts down
 * without asking the important 'modified save' questions. To fix this,
 * OnClose() was overriden to add a call to "SetActiveView(NULL)" just
 * before the base class OnClose is called. This will check all documents
 * asking the save question before closing the application. Thanks to
 * Simon Clark [spc@otleda.com] for help with this.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:05a
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) to help to eliminate screen flicker.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 9/16/99    Time: 2:35p
 * Updated in $/CodeJockey/Include
 * Fixed bug with save / restore bar state, and with saved bar size
 * changing whenever the application starts.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 8:25p
 * Updated in $/CodeJockey/Include
 * Slight modification made to popup menu code.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/09/99    Time: 8:33p
 * Updated in $/CodeJockey/Include
 * Ioannhs Stamatopoulos (Yiannhs) [ystamat@mail.datamedia.gr] - Extended
 * docking windows, removed ::GetSysColor(...) calls and WM_SYSCOLORCHANGE
 * message handler, this is now automatically handled by the base class.
 * Added redraw flags to the create method and modified paint and layout
 * handlers to reduce flicker and un-necessary command handling. Dragging
 * rect now reflects actual control bar docked size. Added "flat" look to
 * the control bars, the control bars can now use the classic DevStudio
 * look, or use flat gradient gripper, borders and frame buttons. Overall,
 * docking windows now handle dragging, docking and floating much better,
 * and behave more like DevStudio.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:18a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:13a
 * Updated in $/CodeJockey/Include
 * Re-wrote CCJMenu initialization routines with a more cleaner approach.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:47p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.  Added support for Brent Corkum's 
 * BCMenu (CCJMenu), which is a replacement for the CCoolMenu class.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 4:46p
 * Updated in $/CodeJockey/Include
 * Total re-write to add side-by-side docking support for CCJControlBar
 * class.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 4:46p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFRAMEWND_H__
#define __CJFRAMEWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJMenuBar.h"
#include "CJMenu.h"

// class forwards
class CCJSizeDockBar;

// CCJFrameWnd is a CFrameWnd derived class which is used to
// reposition toolbar layout for 3D effect, in addition to docking
// methods for CCJToolBar and CCJControlBar class
class _CJX_EXT_CLASS CCJFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CCJFrameWnd)

public:

	// Default constructor
	//
	CCJFrameWnd();

	// Virtual destructor
	//
	virtual ~CCJFrameWnd();

protected:

	static DWORD	dwDockBarMap[4][2];	// array used for docking masks
	CCJMenu			m_menu; 			// SDI frame menu.
	CCJMenuBar		m_wndMenuBar;		// menu bar

    // Yiannhs to parameterize and the dock bar border
	enum DockFlatBorders {
        brdrAllRaised       = 0x0000,
        brdrFlatTop         = 0x0001,
        brdrFlatBottom      = 0x0002,
        brdrFlatLeft        = 0x0004, 
        brdrFlatRight       = 0x0008,
        brdrFlatVertical    = 0x0003,
        brdrFlatHorizontal  = 0x000C,
        brdrFlatAll         = 0x000F
    };

protected:

	// this member function is called by the framework to associate
	// toolbar resources with the frame menu
	//
	virtual HMENU NewMenu(
		// resource id for menu
		UINT nMenuID,
		// array of toolbar resource ids
		UINT* nToolbarIDs = NULL,
		// size of array.
		int nSize = 0);

	// this member function is called by the framework to associate
	// a toolbar resource with the frame menu
	//
	virtual HMENU NewMenu(
		// resource id for menu
		UINT nMenuID,
		// toolbar resource
		UINT nToolbarID = NULL);

public:

	virtual void LoadBarState(LPCTSTR lpszProfileName);
	virtual void SaveBarState(LPCTSTR lpszProfileName) const;

	// this member function is called to initialize the CCJMenu
	//
	virtual BOOL InitializeMenu(
		// resource id for menu
		UINT nMenuID,
		// array of toolbar resource ids
		UINT* nToolbarIDs = NULL,
		// size of array.
		int nSize = 0);

	// this member function is called to initialize the Menu
	//
	virtual BOOL InitializeMenu(
		// resource id for menu
		UINT nMenuID,
		// toolbar resource
		UINT nToolbarID = NULL);

	// this member function is called to calculate the size of
	// a CCJControlBar
	//
	virtual void RecalcAllExcept(CCJSizeDockBar *pBar);

	// this member function enables docking for standard 
	// toolbar and control bars
	//
	virtual void EnableDocking(DWORD dwDockStyle);

	// this member function enables docking for CCJControl bars only
	//
	virtual void EnableDockingSizeBar(
		// Specifies which sides of the frame window can serve as docking sites 
		// for control bars. It can be one or more of the following: 
		//
		// CBRS_ALIGN_TOP   Allows docking at the top of the client area.
		// CBRS_ALIGN_BOTTOM   Allows docking at the bottom of the client area.
		// CBRS_ALIGN_LEFT   Allows docking on the left side of the client area.
		// CBRS_ALIGN_RIGHT   Allows docking on the right side of the client area.
		// CBRS_ALIGN_ANY   Allows docking on any side of the client area. 
		DWORD dwDockStyle,
		// enum for flat border styles.
		DockFlatBorders FlatFlags = brdrAllRaised );

	// this member function will dock a CCJControlBar
	//
	virtual void DockSizeBar(
		// toolbar to dock
		CControlBar* pBar,
		// dockbar object usually NULL
		CCJSizeDockBar* pDockBar=NULL,
		// initial size usually NULL
		LPRECT lpRect=NULL);

	// this memeber function will dock the control bar passed in
	// as the first argument, and dock the control bar that is passed in
	// as the second argument to the left of the first one.
	//
	virtual void DockControlBarLeftOf(
		// control bar to dock to the right of
		CControlBar* Bar,
		// already docked control bar
		CControlBar* LeftOf);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFrameWnd)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs, UINT nIconID);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJFrameWnd)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJFRAMEWND_H__

