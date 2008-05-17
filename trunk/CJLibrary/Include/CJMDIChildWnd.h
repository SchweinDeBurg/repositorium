// CJMDIChildWnd.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:02p $
 * $Revision: 7 $
 * $Archive: /CodeJock/Include/CJMDIChildWnd.h $
 *
 * $History: CJMDIChildWnd.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:02p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:44p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/06/99   Time: 6:21a
 * Updated in $/CodeJockey/Include
 * Added cool menu support for MDI applications that do not have a
 * document/view architecture. Added GetInitializedMenu() to
 * CCJMDIChildWnd which will initialize the menu for a given view,  and
 * overloaded InitializeMenu() in CCJMDIFrameWnd. Thanks to Golovanov
 * Michael [Golovanov@SoftLab.RU] for help with this.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:14p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/26/99    Time: 8:30p
 * Created in $/CodeJockey/Include
 * Initial creation. Added to library to allow CCJControlBar and
 * CCJToolBar docking.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMDICHILDWND_H__
#define __CJMDICHILDWND_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// class forwards
class CCJSizeDockBar;

// The CCJMDIChildWnd class provides the functionality of a Windows multiple 
// document interface (MDI) child window, along with members for managing the 
// window. This class extends the standard CMDIChildWnd class to allow CCJControlBar
// and CCJToolBar docking.
class _CJX_EXT_CLASS CCJMDIChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCJMDIChildWnd)

// Construction / destruction
public:

	// Constructs a CCJMDIChildWnd object
	//
	CCJMDIChildWnd();

	// Destroys a CCJMDIChildWnd object
	//
	virtual ~CCJMDIChildWnd();
	
	virtual HMENU GetInitializedMenu(
		UINT nMenuID,
		UINT nToolBarID);
	
	virtual HMENU GetInitializedMenu(
		UINT nMenuID,
		UINT* nToolbarIDs,
		int nSize);

	virtual UINT GetMenuID();
	virtual UINT GetToolBarID();
	virtual UINT* GetToolBarIDs(int& nCount);
	virtual void SetResourceIDs(UINT nMenuID, UINT nToolBarID);
	virtual void SetResourceIDs(UINT nMenuID, UINT* nToolBarIDs, int nCount);

protected:

	HMENU	m_hMenu;
	UINT	m_nMenuID;
	UINT	m_nToolBarID;
	UINT*	m_nToolBarIDs;
	int		m_nCount;

	static DWORD dwDockBarMap[4][2];	// array used for docking masks

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

	// This member function is called to calculate the size of
	// a CCJControlBar
	//
	virtual void RecalcAllExcept(CCJSizeDockBar *pBar);

	// This member function enables docking for standard 
	// toolbar and control bars
	//
	virtual void EnableDocking(DWORD dwDockStyle);

	// This member function enables docking for CCJControl bars only
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

	// This member function will dock a CCJControlBar
	//
	virtual void DockSizeBar(
		// toolbar to dock
		CControlBar* pBar,
		// dockbar object usually NULL
		CCJSizeDockBar* pDockBar=NULL,
		// initial size usually NULL
		LPRECT lpRect=NULL);

	// This memeber function will dock the control bar passed in
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
	//{{AFX_VIRTUAL(CCJMDIChildWnd)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCJMDIChildWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Inline functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE UINT CCJMDIChildWnd::GetMenuID()
	{ return m_nMenuID; }

_CJXLIB_INLINE UINT CCJMDIChildWnd::GetToolBarID()
	{ return m_nToolBarID; }

_CJXLIB_INLINE UINT* CCJMDIChildWnd::GetToolBarIDs(int& nCount)
	{ nCount = m_nCount; return m_nToolBarIDs; }

_CJXLIB_INLINE void CCJMDIChildWnd::SetResourceIDs(UINT nMenuID, UINT nToolBarID)
	{ m_nMenuID = nMenuID; m_nToolBarID = nToolBarID; }

_CJXLIB_INLINE void CCJMDIChildWnd::SetResourceIDs(UINT nMenuID, UINT* nToolBarIDs, int nCount)
	{ m_nMenuID = nMenuID; m_nToolBarIDs = nToolBarIDs; m_nCount = nCount; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJMDICHILDWND_H__
