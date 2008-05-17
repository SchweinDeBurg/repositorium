// CJControlBar.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:21p $
 * $Revision: 19 $
 * $Archive: /CodeJock/Include/CJControlBar.h $
 *
 * $History: CJControlBar.h $
 * 
 * *****************  Version 19  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:21p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) and OnPaint() to handle flicker free
 * drawing. Made additional modifications to load and save bar state
 * methods.
 * 
 * Modified resource include for static builds.
 * 
 * Enabled OnInitialUpdate() to be called for view classes added to a
 * CCJTabCtrlBar while the control bar is floating.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Overloaded EnableFlatLook() method so that the gripper, frame buttons
 * and borders can be set flat individually.  Added inline function
 * EnableSaveState() which can be used to enable or disable the restore
 * bar state.
 * 
 * Fixed bug with save / restore bar state, and with saved bar size
 * changing whenever the application starts.
 * 
 * Fixed small initialization bug.
 * 
 * Made class methods virtual for inheritence purposes.
 * 
 * *****************  Version 18  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:18p
 * Updated in $/CodeJockey/Include
 * Added derived button class to handle control bar caption button
 * updates. Cleaned up inline functions, and import/export macro so that
 * class will be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 8:24p
 * Updated in $/CodeJockey/Include
 * Slight modification made to popup menu code.
 * 
 * *****************  Version 13  *****************
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
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 5/03/99    Time: 9:52p
 * Updated in $/CodeJockey/Include
 * Changed gripper and button macros to enums. Renamed popup member
 * variables to begin with popup, this will be consistent throughout all
 * classes. Added m_nPos member variable which can be set to the index of
 * the popup menu in the menu resource. Removed windows message handler
 * ON_WM_CONTEXTMENU(), and added handlers ON_WM_RBUTTONDOWN(),
 * ON_WM_MEASUREITEM(), ON_WM_MENUCHAR() and ON_WM_INITMENUPOPUP() to
 * support the CCJMenu popup. Added WS_CLIPSIBLINGS to control bar style.
 * Changed OnEraseBkgnd(CDC* pDC) to only draw client area when a child is
 * not present.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 12:54p
 * Updated in $/CodeJockey/Include
 * Modified context menu to use Brent Corkum's BCMenu (CCJMenu).
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 12:52p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 3/13/99    Time: 12:54p
 * Updated in $/CodeJockey/Include
 * Patrick Bergeron [patb@softimage.com] fixed the following bugs:
 * 
 * I had a dialog nested in a CJControlBar. Unfortunately when I tried to
 * dock it to the left or right, the bottom part of my dialog would be
 * clipped off. 
 * 
 * Fix: When in docked mode, ::CalcDynamicLayout needed to return the
 * desired size, while taking into account the size of the gripper bar (if
 * any). Simply added a simple "+GetMinExt()" Now my dialog's bottom
 * doesn't get clipped. Woo-ha.
 * 
 * When nesting a CDialog based window inside a CJControlBar, the 3D rect
 * drawn by ::OnEraseBkgnd would be overwritten by the top and left dialog
 * borders. The problem was caused by the fact that in the
 * ::OnWindowPosChanged() method, the rectangle which is used to move the
 * window contains the border. A simple call to rc.DeflateRect(1,1) solved
 * this problem.
 * 
 * Added a call to UpdateWindow() in ::OnEraseBkgnd(). This helps with the
 * flickering a bit, but I Am not sure this is the correct way to go.
 * Incorporate at your own risks.
 * 
 * Added 2 ASSERT()s in ::OnMouseMove(). This was not to fix a bug, but at
 * one point I was seeing strange things that lead me to add these
 * asserts. They don't do anything bad, so why remove them?
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 3/10/99    Time: 12:50p
 * Updated in $/CodeJockey/Include
 * Fixed memory leak with button tooltips.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 3/10/99    Time: 12:48p
 * Updated in $/CodeJockey/Include
 * Added AfxRegisterWndClass() to create method to handle double clicks.
 * Thanks to Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp] for some
 * thoughts on this.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 2/28/99    Time: 12:47p
 * Updated in $/CodeJockey/Include
 * Calls default wnd proc if no context menu is defined Gonzalo Pereyra
 * [persys@adinet.com.uy]
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 12:48p
 * Updated in $/CodeJockey/Include
 * Removed extra line (typo) from OnLButtonUp(). Thanks to Ioannis
 * Stamatopoulos (ystamat@mail.datamedia.gr) for pointing this out.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 12:46p
 * Updated in $/CodeJockey/Include
 * Total class re-write, added multiple/side-by-side controlbar docking.
 * No longer uses CSizingControlBar base class.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 11/22/98   Time: 12:47p
 * Updated in $/CodeJockey/Include
 * Modified set cursor to display normal size cursor when static linked.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 10/20/98   Time: 12:45p
 * Updated in $/CodeJockey/Include
 * Fixed problem with gripper and buttons disappearing when docking
 * toggled. Overloaded IsFloating() method from base class.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 12:44p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCONTROLBAR_H__
#define __CJCONTROLBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CJFlatButton.h"
#include "CJMenu.h"        // CCJMenu class declaration

// CCJCBarButton is a CCJFlatButton derived class which is used by
// the CCJControlBar class.
class _CJX_EXT_CLASS CCJCBarButton : public CCJFlatButton
{
protected:
	virtual void DrawButtonIcon(CDC* pDC, UINT nState, CRect& rcItem);
};

typedef enum { HIDE_CONTRLBAR, 
	MAXI_H_DISABLE, MAXI_H_ENABLED, MAXI_V_DISABLE, MAXI_V_ENABLED,
	NORM_H_DISABLE, NORM_H_ENABLED, NORM_V_DISABLE, NORM_V_ENABLED
} BTN_IMG_INDEX;

typedef enum { NONE, OUTLINE, SUNKEN, RAISED } CHILD_BORDER;

// CCJControlBar is a CControlBar derived class which is simular to
// the sizing / docking windows that are seen in Developer Studio.
class _CJX_EXT_CLASS CCJControlBar : public CControlBar
{
	DECLARE_DYNAMIC(CCJControlBar)

public:
	
	// Default constructor
	//
	CCJControlBar();

	// Virtual destructor
	//
	virtual ~CCJControlBar();

public:
	
	BOOL			m_bUnique;				// used by CCJControlBar and CCJSizeDockBar,
											// set to true when the controlbar is the 
											// only one docked on any given side.
	
	BOOL			m_bToFit;				// used by CCJControlBar and CCJSizeDockBar,
											// to determine border layouts depending on bar
											// state.
	
	CHILD_BORDER	m_eChildBorder;			// determines if a border should be drawn around the child area.

protected:			

	int				m_cxOffset;				// used to offset the horizontal client area
											// to allow room for the caption when docked.
	
	int				m_cyOffset;				// used to offset the vertical client area to 
											// allow room for the caption when docked.
	
	BTN_IMG_INDEX	m_eAuxIndex;			// used to determine which button icon to 
											// display when the control bar is minimized
											// or maximized depending on which side docked.

	UINT			m_popToolbarID;			// popup menu toolbar resource id, associates 
											// toolbar icons with the popup menu.

	UINT			m_popupMenuID;			// popup menu resource id
	CCJMenu			m_popupMenu;			// popup menu
	int				m_nPos;					// index of the pop-up menu contained in the menu

	BOOL			m_bOkToDrag;			// used to determine if a drag operation
											// should occur.

	BOOL			m_bMaximized;			// used to determine if the control bar has
											// been maximized.

	BOOL			m_bDragging;			// set to TRUE when a drag operation is in
											// process.


	BOOL			m_bButtons;				// user defined flag, set to TRUE if the
											// control bar should display buttons.

	CRect			m_rcTrack;				// used to determine rect of the tracker window.
	CSize			m_sizeDesired;			// the desired size of the control bar.
	CSize			m_sizeNormal;			// normal size of the control bar.
    CSize           m_sizeVert;				// vertical size
    CSize           m_sizeHorz;				// horizontal size
    CSize           m_sizeFloat;			// float size
	CPoint			m_ptCurDrag;			// cursor location during a drag operation.
	CPoint			m_ptStartDrag;			// cursor starting point during a drag operation.
	
	CWnd*			m_pChildWnd;			// CWnd pointer which is contains child window
											// that is displayed in the control bar, see
											// SetChild(CWnd* pWnd).

	CImageList		m_ImageList;			// contains the caption control buttons images.
	CCJCBarButton	m_btnClose;				// close button used in caption area.
	CCJCBarButton	m_btnMinim;				// minimize button used in the caption area.
	CToolTipCtrl	m_ToolTip;				// tooltip used for caption buttons.

	BOOL			m_bGripper;				// user defined flag, set to TRUE if a 
											// gripper should be drawn in the control 
											// bar, see ShowFrameControls(...).
    BOOL            m_bFlatGripper;			// user defined flag, set to TRUE if for a flat
											// Gripper look
    BOOL            m_bFlatBorder;			// user defined flag, set to TRUE if for a flat
											// Border look 
	CString			m_strSection;			// used by the control bar for registry entries.
	bool			m_bAutoSave;			// true to restore size and position true by default.
	HICON			m_hIcon;				// handle to close button icon.
	HICON			m_hAuxIcon;				// handle to auxiliary button icon.
	int				m_nID;					// holds the resource id for this control.
	BOOL			m_bFloating;			// true if the control bar was floating when last destroyed.

public:

	// this member function is called to enable/disable size and
	// position at startup.
	//
	virtual void EnableSaveState(bool bAutoSave=true);

	// this member function is used to create a sizing / docking control bar.
	//
	virtual BOOL Create(
		// the parent window.
		CWnd* pParentWnd,
		// The ID of the control bar.
		UINT nID,
		// Points to a null-terminated character string that contains the 
		// control bar title.
		LPCTSTR lpszWindowName = NULL,
		// The default size the control bar is to be.
		CSize sizeDefault = CSize(200,100),
		// Specifies the window style attributes.
		DWORD dwStyle = CBRS_LEFT,
		// TRUE for flat style control bars.
		BOOL bFlatBorders = FALSE);

	// this member function is called by the control bar, and
	// can be overloaded in derived classes to return the rect
	// for the child window associated with the control bar.
	//
	virtual void GetChildRect(
		// reference to a CRect object which contains the size of
		// the child window associated with the control bar.
		CRect &rect);

	// this member function is called by the control bar to
	// determine the minimum size of the control bar should be 
	// when miminized or maximized.
	//
	virtual int GetMinExt();

	// this member function returns TRUE if the control 
	// bar is docked on the left.
	//
	virtual BOOL IsLeftDocked() const;

	// this member function returns TRUE if the control 
	// bar is docked on the right.
	//
	virtual BOOL IsRightDocked() const;

	// this member function returns TRUE if the control 
	// bar is docked on the top.
	//
	virtual BOOL IsTopDocked() const;

	// this member function returns TRUE if the control 
	// bar is docked on the bottom.
	//
	virtual BOOL IsBottomDocked() const;

	// this member function returns TRUE if the control 
	// bar is docked horizontal.
	//
	virtual BOOL IsHorzDocked() const;

	// this member function returns TRUE if the control 
	// bar is docked vertical.
	//
	virtual BOOL IsVertDocked() const;

	// this member function returns TRUE if the control 
	// bar is currently floating.
	//
	virtual BOOL IsFloating() const;

	// this member function is called by the control bar 
	// to set the normal size the control bar should be.
	//
	virtual void SetNormalSize(const CSize &cs);

	// this member function is called by the control bar 
	// to return the control bar to its normal size.
	//
	virtual void Normalize();

	// this member function is called by the control bar 
	// to minimize the control bar.
	//
	virtual void Minimize();
	
	// this member function is called by the control bar 
	// to maximize the control bar.
	//
	virtual void Maximize(int size);

	// this member function is to be called after creation to enable 
	// docking on the control bar.
	//
	virtual void EnableDockingOnSizeBar(
		// Specifies which sides of the frame window can serve as docking sites 
		// for control bars. It can be one or more of the following: 
		//
		// CBRS_ALIGN_TOP   Allows docking at the top of the client area.
		// CBRS_ALIGN_BOTTOM   Allows docking at the bottom of the client area.
		// CBRS_ALIGN_LEFT   Allows docking on the left side of the client area.
		// CBRS_ALIGN_RIGHT   Allows docking on the right side of the client area.
		// CBRS_ALIGN_ANY   Allows docking on any side of the client area. 
		DWORD dwDockStyle);

	// this member function is used to show / hide the frame controls 
	// for the control bar.
	//
	virtual void ShowFrameControls(
		// set to FALSE to hide the gripper.
		BOOL bGripper = TRUE,
		// set to FALSE to hide the frame buttons.
		BOOL bButtons = TRUE);

	// this member function is used to set the resource id for the popup menu used.
	//
	virtual void SetMenuID(UINT popupMenuID, UINT popToolbarID=0, int nPos=0);

	// this member function returns the resource id of the context menu used.
	//
	virtual UINT GetMenuID();

	virtual void EnableFlatLook(BOOL bFlat=TRUE);

	virtual void EnableFlatLook(BOOL bGripper, BOOL bBorder, BOOL bButtons);

	// this member function is used to determine the width when docked vertical.
	//
	virtual int GetVertSize();

	// this member function is used to determine the height when docked horizontal
	//
	virtual int GetHorzSize();

protected:

	// this member function is called by the control bar to caluclate
	// the size of the gripper in the caption area.
	//
	virtual CRect GetGripperRect();

	// this member function is called by the control bar to calculate the 
	// size of the buttons used in the caption area.
	//
	virtual CRect GetButtonRect();

	// this member function is called by the control bar to draw
	// the gripper in the caption area.
	//
	virtual void DrawGripper(CDC* pDC);

	// this member function is used to associate a child window 
	// that is to be displayed in the control bar.
	//
	virtual void SetChild(CWnd* pWnd);

	// this member function is called by the control bar
	// during resizing of splitters.
	//
	virtual void OnInvertTracker(const CRect& rect);

	// this member function is called by the control bar
	// to determine cursor position
	// for drag operations.
	//
	virtual void HitTest(CPoint &point);

	// this member function is called by the control bar
	// to draw its borders.
	//
	virtual void DrawBorders(CDC *pDC, CRect &rect);

	// this member function is called by the control bar
	// to perform paint operations.
	//
	virtual void EraseNonClient();

	// this member function is used to draw the new flat style control bar.
	//
    virtual void DrawGriperCaption( CDC * pDC, LPRECT pRect );

	virtual void LoadBarState(CSize& size, int nID);

	virtual void SaveBarState(CSize& size);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJControlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
 	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJControlBar)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
#if (_MFC_VER < 0x0900)
	afx_msg UINT OnNcHitTest(CPoint point);
#else
	afx_msg LRESULT OnNcHitTest(CPoint point);
#endif   // _MFC_VER
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg void OnButtonMinimize();
	afx_msg void OnUpdateButtonMinimize(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define AFX_IDW_SIZEBAR_LEFT	AFX_IDW_DOCKBAR_LEFT	+ 4
#define AFX_IDW_SIZEBAR_RIGHT	AFX_IDW_DOCKBAR_RIGHT	+ 5
#define AFX_IDW_SIZEBAR_TOP		AFX_IDW_DOCKBAR_TOP		+ 6
#define AFX_IDW_SIZEBAR_BOTTOM	AFX_IDW_DOCKBAR_BOTTOM	+ 7

const DWORD dwSizeBarMap[4][2] =
{
	{ AFX_IDW_SIZEBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_SIZEBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_SIZEBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_SIZEBAR_RIGHT,    CBRS_RIGHT  },
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJControlBar::EnableSaveState(bool bAutoSave)
	{ ASSERT(::IsWindow(m_hWnd)); m_bAutoSave = bAutoSave; }

_CJXLIB_INLINE int CCJControlBar::GetVertSize()
	{ ASSERT(::IsWindow(m_hWnd)); return m_sizeDesired.cx; }

_CJXLIB_INLINE int CCJControlBar::GetHorzSize()
	{ ASSERT(::IsWindow(m_hWnd)); return m_sizeDesired.cy; }

_CJXLIB_INLINE UINT CCJControlBar::GetMenuID()
	{ ASSERT(::IsWindow(m_hWnd)); return m_popupMenuID; }

_CJXLIB_INLINE void CCJControlBar::SetMenuID(UINT popupMenuID, UINT popToolbarID, int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); m_popupMenuID=popupMenuID; m_popToolbarID=popToolbarID; m_nPos=nPos; }

_CJXLIB_INLINE void CCJControlBar::ShowFrameControls(BOOL bGripper, BOOL bButtons)
	{ ASSERT(::IsWindow(m_hWnd)); m_bGripper=bGripper; m_bButtons=bButtons; }

_CJXLIB_INLINE void CCJControlBar::SetNormalSize(const CSize &cs)
	{ ASSERT(::IsWindow(m_hWnd)); m_sizeDesired=m_sizeNormal=cs; }

_CJXLIB_INLINE BOOL CCJControlBar::IsVertDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (IsLeftDocked() || IsRightDocked()); }

_CJXLIB_INLINE BOOL CCJControlBar::IsHorzDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (IsTopDocked() || IsBottomDocked()); }

_CJXLIB_INLINE BOOL CCJControlBar::IsBottomDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (GetParent()->GetDlgCtrlID()==AFX_IDW_SIZEBAR_BOTTOM); }

_CJXLIB_INLINE BOOL CCJControlBar::IsTopDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (GetParent()->GetDlgCtrlID()==AFX_IDW_SIZEBAR_TOP); }

_CJXLIB_INLINE BOOL CCJControlBar::IsRightDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (GetParent()->GetDlgCtrlID()==AFX_IDW_SIZEBAR_RIGHT); }

_CJXLIB_INLINE BOOL CCJControlBar::IsLeftDocked() const
	{ ASSERT(::IsWindow(m_hWnd)); return (GetParent()->GetDlgCtrlID()==AFX_IDW_SIZEBAR_LEFT); }

_CJXLIB_INLINE BOOL CCJControlBar::IsFloating() const
	{ ASSERT(::IsWindow(m_hWnd)); return (!IsHorzDocked() && !IsVertDocked()); }

_CJXLIB_INLINE void CCJControlBar::SetChild(CWnd *pWnd)
	{ ASSERT(::IsWindow(m_hWnd)); m_pChildWnd=pWnd; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJCONTROLBAR_H__

