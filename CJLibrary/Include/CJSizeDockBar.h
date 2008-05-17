// CJSizeDockBar.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/29/99 12:44a $
 * $Revision: 12 $
 * $Archive: /CodeJock/Include/CJSizeDockBar.h $
 *
 * $History: CJSizeDockBar.h $
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 10/29/99   Time: 12:44a
 * Updated in $/CodeJock/Include
 * Added static member variable CCJSizeDockBar::m_bIsDockBar used for save
 * and restore bar state.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 2:55p
 * Updated in $/CodeJock/Include
 * Fixed bug with just clicking on the sizing tracker and the control bar
 * resizing one pixel to the left or right. The bar will only be sized if
 * the cursor has changed its location.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 9/16/99    Time: 2:35p
 * Updated in $/CodeJockey/Include
 * Fixed bug with save / restore bar state, and with saved bar size
 * changing whenever the application starts.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/09/99    Time: 10:25p
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
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 11:07p
 * Updated in $/CodeJockey/Include
 * Changed CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd; to
 * CFrameWnd *pFrame = GetParentFrame(); see line 432.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 11:06p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/29/99    Time: 11:06p
 * Updated in $/CodeJockey/Include
 * Made some cosmetic enhancements to more closely match DevStudio docking
 * windows.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 11:06p
 * Created in $/CodeJockey/Include
 * Initial release to add side-by-side docking support for CCJControlBar
 * class.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSIZEDOCKBAR_H__
#define __CJSIZEDOCKBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// class forwards
class CCJControlBar;

// CCJSizeDockBar is a CDockBar derived class which is used by the framework
// to handle CCJControlBar's.
class _CJX_EXT_CLASS CCJSizeDockBar : public CDockBar
{
	DECLARE_DYNAMIC(CCJSizeDockBar)

public:
	
	// Default constructor
	//
	CCJSizeDockBar();

	// Virtual destructor
	//
	virtual ~CCJSizeDockBar();

	static bool m_bIsDockBar;

protected:

	int			m_iSafeSpace;			// dockbar offset when docked

	int			m_iActualSize;			// actual size of the dockbar
	
	BOOL		m_bDragging;			// set to TRUE when a drag operation is in
										// process.

	BOOL		m_bOkToDrag;			// used to determine if a drag operation
										// should occur.

	CRect		m_rcTrack;				// used to determine rect of the tracker window.
	CPoint		m_ptCurDrag;			// cursor location during a drag operation.
	CPoint		m_ptStartDrag;			// cursor starting point during a drag operation.

	HCURSOR		m_curHSplitBar;			// handle of the cursor to be displayed when
										// the mouse passes over a horizontal splitter.

	HCURSOR		m_curVSplitBar;			// handle of the cursor to be displayed when
										// the mouse passes over a vertical splitter.

    BOOL        m_bFlatBorder;			// set to TRUE for flat border.
	CPoint		m_ptCurrent;			// holds the current cursor position.

public:

	// this member function is called by the control bar 
	// to return the control bar to its normal size.
	//
	void Normalize(CCJControlBar*);

	// this member function is called by the control bar 
	// to maximize the control bar.
	//
	void Maximize(CCJControlBar*);

	// This member function is called to recalculate the
	// CCJControlBar docked layout
	//
	void CalcSizeBarLayout();

	// This member function is called to set the size
	// of a CCJControlBar
	//
	void SetActualSize(int iSize);

	// this member function is called by the control bar
	// to perform paint operations.
	//
	void EraseNonClient();

	// this member function is called by the control bar
	// to draw its borders.
	//
	void DrawBorders(CDC *pDC,CRect& rect);

	// This member function is called to recalculate the
	// CCJControlBar docked layout
	//
	void RecalcAllExcept(CCJSizeDockBar *pBar);

	// this member function returns a pointer to a CCJControlBar
	// object based upon docked position
	//
	CCJControlBar* GetDockedSizeBar(int nPos);

    // this member function is used to initialize flat borders.
	//
    void FlatBorder( BOOL bFlat = TRUE );

protected:

	// this member function is called to determine if there
	// are any additional control bars left.
	//
	BOOL IsLastControlBar(int index);

	// this member function returns the size for a docked
	// control bar
	//
	void GetAvailableRect(CRect &rect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJSizeDockBar)
	public:
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual void DoPaint(CDC *pDC);
	virtual BOOL IsDockBar() const;
	virtual BOOL IsDockSizeBar() const;
	virtual void OnInvertTracker(const CRect&);
	virtual void HitTest(const CPoint& point);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJSizeDockBar)
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE BOOL CCJSizeDockBar::IsDockBar() const
	{ return m_bIsDockBar; }

_CJXLIB_INLINE BOOL CCJSizeDockBar::IsDockSizeBar() const
	{ return TRUE; }

_CJXLIB_INLINE void CCJSizeDockBar::SetActualSize(int iSize)
	{ m_iActualSize=iSize; }

_CJXLIB_INLINE void CCJSizeDockBar::FlatBorder( BOOL bFlat )
	{ if(m_bFlatBorder!=bFlat){m_bFlatBorder=bFlat;InvalidateRect(NULL,FALSE);UpdateWindow();}}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJSIZEDOCKBAR_H__

