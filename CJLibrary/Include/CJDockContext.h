// CJDockContext.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 9 $
 * $Archive: /CodeJockey/Include/CJDockContext.h $
 *
 * $History: CJDockContext.h $
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
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
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
 * User: Kirk Stowell Date: 4/14/99    Time: 12:57p
 * Updated in $/CodeJockey/Include
 * Daniel Harth [danielh@mail.matco.ro] - Added support for diagonal
 * sizing. 
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 12:56p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/10/99    Time: 12:56p
 * Updated in $/CodeJockey/Include
 * Added support for toggle docking.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 12:54p
 * Updated in $/CodeJockey/Include
 * Scott Killen [s_killen@yahoo.com] - If your frame is not your main
 * window in your app, you cannot dock a floating CCJControlBar. Changed
 * (CFrameWnd*)AfxGetApp()->m_pMainWnd to (CFrameWnd*)m_pDockSite in
 * CanDockDockBar() and DockSizeBar()
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 12:53p
 * Created in $/CodeJockey/Include
 * Initial creation to add side-by-side docking support for CCJControlBar
 * class.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJDOCKCONTEXT_H__
#define __CJDOCKCONTEXT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// class forwards
class CCJSizeDockBar;
class CCJControlBar;

// CCJDockContext is a CDockContext derived class which is used by
// CCJControlBar and CCJMiniDockFrameWnd for docking / sizing control bars.
class _CJX_EXT_CLASS CCJDockContext : public CDockContext
{
public:

	// Default constructor
	//
	CCJDockContext(CControlBar* pBar);

	// Virtual destructor
	//
	virtual ~CCJDockContext();

public:

    CPoint		m_ptOrig; // Yiannhs: trying to handle some extra use

public:

	// this member function is called by CCJDockContext when a drag
	// operation completes.
	//
	void EndDragDockBar();

	// this member function is called by CCJDockContext during a
	// drag process.
	//
	void MoveDockBar(CPoint pt);

	// this member function is called by CCJControlBar and CCJMiniDockFrameWnd
	// whenever a drag operation begins.
	//
	void StartDragDockBar(CPoint pt);

	// this member function is called by CCJDockContext to determine
	// wether a control bar is dockable.
	//
	DWORD CanDockDockBar();

	// this member function is called by CCJDockContext to determine
	// wether a control bar is dockable.
	//
	DWORD CanDockDockBar(CRect rect, DWORD dwDockStyle, CDockBar** ppDockBar = NULL);

	// this member function is called by CCJDockContext
	// whenever a drag operation begins.
	//
	BOOL TrackDockBar();

	// this member function is called by CCJDockContext
	// and returns a CCJSizeDock bar pointer.
	//
	CCJSizeDockBar* GetDockBar(DWORD dwOverDockStyle);

	// this member function is used by the framework to dock a
	// control bar.
	//
	void DockSizeBar(CControlBar *,CCJSizeDockBar* =NULL,LPRECT =NULL);

	// this member function is used to toggle the docked state of
	// a control bar.
	//
	virtual void ToggleDocking();

	// this member function is called to enable diagonal sizing
	// for the control bar.
	//
	void Stretch(CPoint pt);
	virtual void StartResize(int nHitTest, CPoint pt);
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CJDOCKCONTEXT_H__
