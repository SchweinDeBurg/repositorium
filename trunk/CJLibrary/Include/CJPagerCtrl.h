// CJPagerCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:51 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/Include/CJPagerCtrl.h $
 *
 * $History: CJPagerCtrl.h $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:51
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:45p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/02/99    Time: 10:44p
 * Updated in $/CodeJockey/Include
 * Cleaned up class source, wrapped all pager messages.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/02/98   Time: 10:44p
 * Created in $/CodeJockey/Include
 * First release version.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJPAGERCTRL_H__
#define __CJPAGERCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJPagerCtrl is a CWnd derived class which wraps the windows
// Pager common control.
class _CJX_EXT_CLASS CCJPagerCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCJPagerCtrl)
public:

	// Default constructor
	//
	CCJPagerCtrl();

	// Virtual destructor
	//
	virtual ~CCJPagerCtrl();

protected:

	int m_nWidth;	// user defined scroll width see SetScrollArea(...)
	int m_nHeight;	// user defined scroll height see SetScrollArea(...)

public:

	// this member function sets the scroll area for the
	// pager
	//
	void SetScrollArea (int nWidth, int nHeight);

	/////////////////////////////////
	// CPagerCtrl class wrappers.
	/////////////////////////////////

	// Sets the contained window for the pager control
	//
	void SetChild(
		// Handle to the window to be contained.
		HWND hwndChild);

	// Forces the pager control to recalculate the size of the 
	// contained window
	//
	void RecalcSize();

	// Enables or disables mouse forwarding for the pager control
	//
	void ForwardMouse(
		// BOOL value that determines if mouse forwarding is enabled 
		// or disabled. If this value is nonzero, mouse forwarding is 
		// enabled. If this value is zero, mouse forwarding is disabled. 		
		BOOL bForward);

	// Sets the current background color for the pager control
	//
	COLORREF SetBkColor(
		// COLORREF value that contains the new background 
		// color of the pager control
		COLORREF clr);

	// Retrieves the current background color for the pager control
	//
	COLORREF GetBkColor();

	// Sets the current border size for the pager control
	//
	int SetBorder(
		// INT value that contains the new size of the 
		// border, in pixels
		int iBorder);

	// Retrieves the current border size for the pager control
	//
	int GetBorder();

	// Sets the scroll position for the pager control
	//
	int SetPos(
		// INT value that contains the new scroll position, in pixels
		int iPos);

	// Retrieves the current scroll position of the pager control
	//
	int GetPos();

	// Sets the current button size for the pager control
	//
	int SetButtonSize(
		// INT value that contains the new button size, in pixels
		int iSize);

	// Retrieves the current button size for the pager control
	//
	int GetButtonSize();

	// Retrieves the state of the specified button in a pager control
	//
	DWORD GetButtonState(
		// Indicates which button to retrieve the state for
		int iButton);

	// Retrieves a pager control's IDropTarget interface pointer
	//
	void GetDropTarget(
		// Address of an IDropTarget pointer that receives the 
		// interface pointer
		IDropTarget **ppdt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJPagerCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJPagerCtrl)
	virtual BOOL OnPagerScroll(NMHDR* pNotifyStruct, LRESULT * pResult);
	virtual BOOL OnPagerCalcSize(NMHDR * pNotifyStruct, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJPagerCtrl::SetScrollArea (int nWidth, int nHeight)
	{ ASSERT(::IsWindow(m_hWnd)); m_nWidth  = nWidth; m_nHeight = nHeight; }

_CJXLIB_INLINE void CCJPagerCtrl::SetChild(HWND hwndChild)
	{ ASSERT(::IsWindow(m_hWnd)); Pager_SetChild(m_hWnd, hwndChild); }

_CJXLIB_INLINE void CCJPagerCtrl::RecalcSize()
	{ ASSERT(::IsWindow(m_hWnd)); Pager_RecalcSize(m_hWnd); }

_CJXLIB_INLINE void CCJPagerCtrl::ForwardMouse(BOOL bForward)
	{ ASSERT(::IsWindow(m_hWnd)); Pager_ForwardMouse(m_hWnd, bForward); }

_CJXLIB_INLINE COLORREF CCJPagerCtrl::SetBkColor(COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_SetBkColor(m_hWnd, clr); }

_CJXLIB_INLINE COLORREF CCJPagerCtrl::GetBkColor()
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_GetBkColor(m_hWnd); }

_CJXLIB_INLINE int CCJPagerCtrl::SetBorder(int iBorder)
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_SetBorder(m_hWnd, iBorder); }

_CJXLIB_INLINE int CCJPagerCtrl::GetBorder()
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_GetBorder(m_hWnd); }

_CJXLIB_INLINE int CCJPagerCtrl::SetPos(int iPos)
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_SetPos(m_hWnd, iPos); }

_CJXLIB_INLINE int CCJPagerCtrl::GetPos()
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_GetPos(m_hWnd); }

_CJXLIB_INLINE int CCJPagerCtrl::SetButtonSize(int iSize)
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_SetButtonSize(m_hWnd, iSize); }

_CJXLIB_INLINE int CCJPagerCtrl::GetButtonSize()
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_GetButtonSize(m_hWnd); }

_CJXLIB_INLINE DWORD CCJPagerCtrl::GetButtonState(int iButton)
	{ ASSERT(::IsWindow(m_hWnd)); return Pager_GetButtonState(m_hWnd, iButton); }

_CJXLIB_INLINE void CCJPagerCtrl::GetDropTarget(IDropTarget **ppdt)
	{ ASSERT(::IsWindow(m_hWnd)); Pager_GetDropTarget(m_hWnd, ppdt); }

#ifndef PGM_SETCHILD
#error CJPagerCtrl requires a newer version of the SDK than you have!
#error Please update your SDK files.
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJPAGERCTRL_H__
