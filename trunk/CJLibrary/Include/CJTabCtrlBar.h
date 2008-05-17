// CJTabCtrlBar.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 29/08/04 22:41 $
 * $Revision: 2 $
 * $Archive: /Projects/Libraries/CJLibrary/Include/CJTabCtrlBar.h $
 *
 * $History: CJTabCtrlBar.h $
 * 
 * *****************  Version 2  *****************
 * User: Anna         Date: 29/08/04   Time: 22:41
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * 1.  Added CCJTabCtrlBar::AddView(lpszLabel, pView, nImage, nIndex)
 * 2.  Added CCJTabCtrlBar::SetViewName()
 * 3.  Added CCJTabCtrlBar::GetViewIndex()
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:29p
 * Updated in $/CodeJock/Include
 * Enabled OnInitialUpdate() to be called for tab views added to
 * CCJTabCtrlBar whenever the control bar is floating.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Fixed problems with AddView(...) setting the wrong view index to
 * active.
 * 
 * Added  EnableToolTips method which enables and disables the tooltips.
 * Thanks to Phil Baxter for help with this.
 * 
 * Fixed problem with dangling pointer in DeleteView. Thanks to Gero Kuehn
 * [gero.kuehn@isc-soft.de] for help with this.
 * 
 * Fixed potential bug with CCJTabControlBar returning image index, fixed
 * bug with GetTabFromPoint(...) returning invalid index.
 * 
 * Made class methods virtual for inhertance purposes.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:11p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:43p
 * Updated in $/CodeJockey/Include
 * Added _DYNAMIC macros for access to the CObject::IsKindOf to determine
 * the class object at runtime. Added method GetTabFromPoint which returns
 * the tab index based upon cursor position. Thanks to Sid Price
 * [sidprice@softtools.com] for suggesting this.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 9:07p
 * Updated in $/CodeJockey/Include
 * Paul [RowingWolf@aol.com] - Added the following support methods:
 * void SetActiveView(CView* pUserView)
 * void AssociateImageIndexWithRuntimeClass(int nIndex, CRuntimeClass*
 * pRuntimeClass)
 * void AssociateImageIndexWithRuntimeClass(int nIndex, LPCTSTR
 * lpszRuntimeClassName)
 * short GetImageIndexFromRuntimeClass(CRuntimeClass* pRuntimeClass)
 * BOOL RemoveAllTabs().
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 5/11/99    Time: 10:02p
 * Updated in $/CodeJockey/Include
 * After much frustration, a class re-write. Fixed many of the issues with
 * painting, invalid pointers and memory leaks. Added document support for
 * views associated with the tab control.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 11:53p
 * Updated in $/CodeJockey/Include
 * Added context popupMenu for CJTabCtrl to use Brent Corkum's BCMenu.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 11:52p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code, modified AddView(...) to allow a
 * CDocument to be associated with the view added to the tab control.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 3/14/99    Time: 11:51p
 * Updated in $/CodeJockey/Include
 * Gonzalo Pereyra [ persys@adinet.com.uy ] - I wanted to trap NM_RETURN
 * notifications in my CTreeView Class. I couldn't... It turns out that
 * CControlbar::PreTranslateMessage() will eat all dialog messages...It
 * will never get dispatched to my window. So I Overrided it in :
 * CCJTabCtrlBar::PreTranslateMessage(). this will allow for message
 * reflection and we will get NM_RETURN. I don't know if this a good
 * solution but for now , it works.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 11:49p
 * Updated in $/CodeJockey/Include
 * Phil Baxter [pgb@mrjinx.demon.co.uk] - Fixed the following problems
 * when tabs are added after creation:
 * 1. The tab was created, but the view that was showing on the tab was
 * not visible.
 * 2. Get an assertion when the AddView function attempts to perform the
 * AddTool function.
 * 3. When a new view is added, although you set it to be the active view,
 * it isn't raised to the top of the stack in the display.
 * 4. The Tab name as stored in the TCB_ITEM structure is now a CString,
 * this removes the 32 byte limit that it previously had.
 * 5. New methods for obtaining the name as stored in the TCB_ITEM
 * structure.
 * 6. New Methods for deleting a view from the control, this removes the
 * view entirely including its 	associated tab.
 * 7. Some minor fixes around assertions when trying to manipulate the
 * control when it has not had any tabs created.
 * 8. Added two new member functions:
 * void SetViewToolTip(int nView, LPCTSTR lpszLabel);
 * void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 11:48p
 * Updated in $/CodeJockey/Include
 * Commented out 2 calls to SetActiveView(). This somewhat fixes the
 * problem with CTRL+TAB in activatation of MDI child windows, however if
 * you click inside the view, this breaks CTRL+TAB again, will look at
 * this when I get some free time. Thanks to Pierre Guimont
 * (Pierre_Guimont@mitel.com) for pointing this out.
 * 
 * Added m_TabCtrl.GetRowCount() as per Rashid Thadha
 * rashidt@consort.co.uk) request.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 1/17/99    Time: 11:46p
 * Updated in $/CodeJockey/Include
 * Added helper class CCJTabCtrl to eliminate re-painting problems such as
 * when the app is minimized then restored.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/03/99    Time: 11:46p
 * Updated in $/CodeJockey/Include
 * Application freezing bug fixed by
 * LiangYiBin.Donald(mailto:lybd@yahoo.com)
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 11:45p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTABCTRLBAR_H__
#define __CJTABCTRLBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CJControlBar.h"
#include "CJTabCtrl.h"

// helper struct used by CCJTabCtrlBar
typedef struct tagTCB_ITEM
{
	CWnd    *pWnd;			// this points to the window associated with a tab
	CString szTabLabel;		// label for the tab
	CString szToolTipLabel;	// tooltip text for the tab
    UINT    uiToolTipId;	// resource id for tooltip
} TCB_ITEM;

// class forwards
class CCJTabCtrl;

// template list used to contain tab information
typedef CList <TCB_ITEM *,TCB_ITEM *> CTabViews;

// CCJTabCtrlBar is a CCJControl bar derived class which implements a
// tabbed window simular to VisualStudio's workspace window.
class _CJX_EXT_CLASS CCJTabCtrlBar : public CCJControlBar
{
	DECLARE_DYNAMIC(CCJTabCtrlBar)

public:

	// Default constructor
	//
	CCJTabCtrlBar();

	// Virtual destructor
	//
	virtual ~CCJTabCtrlBar();

protected:

	int				m_nPointTab;				// tab index set from GetTabFromPoint(...)
	int				m_nActiveTab;				// currently selected tab
    int             m_nNextToolTipId;			// tooltip resource id
	CView*			m_pActiveView;				// this points to the active view
	CTabViews		m_viewList;					// template list containing tab information
	CCJTabCtrl		m_TabCtrl;					// tab control
	CMapStringToPtr	m_ClassNameToImageIndexMap;	// Map class name to the image index
	BOOL			m_bToolTipsEnabled;			// TRUE if tooltips are enabled.

public:

	// this member function is called to add a view to the tab control
	//
	virtual BOOL AddView(
		// tab text
		LPCTSTR lpszLabel,
		// CView object associated with tab
		CRuntimeClass *pViewClass,
		// CDocument associated with view
		CDocument* pDoc = NULL,
		// index of tab to add
		UINT nIndex=-1, 
		// create context for view
		CCreateContext *pContext = NULL);

	// this member function is called to add an existing view to the tab control
	//
	virtual BOOL AddView(	LPCTSTR lpszLabel,	// tab text
							CView* pView,		// CView object associated with tab
							int nImage = -1,	// Image list index
							UINT nIndex =-1);	// index of tab to add

	// this member function will remove a view based on its index
	//
	virtual void RemoveView(int nView);

	// this member function will set a view active
	// based on index
	//
	virtual void SetActiveView(int nView);

	// this member function will set a view active
	// based on a CView object
	//
	virtual void SetActiveView(CRuntimeClass *pViewClass);

	// this member function returns a pointer to the
	// active view in the tab control
	//
	virtual CView* GetActiveView();

	// this member function returns a pointer to a view
	// based on its index
	//
	virtual CView* GetView(int nView);

	// this member function returns a pointer to a view
	// based on a CView object
	//
	virtual CView* GetView(CRuntimeClass *pViewClass);

	// this member function initializes the image list
	// for the tab control
	//
	virtual CImageList* SetTabImageList(CImageList *pImageList);

	// this member function will modify the style for the
	// tab control associated with this control.
	//
	virtual BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);

	// this member function will return the name for a view
	// based on index
    virtual LPCTSTR GetViewName(int nView);

	// this member function will return the name for a view
	// based upon a CView object
	//
    virtual LPCTSTR GetViewName(CRuntimeClass *pViewClass);

	// this member function allows the name of a view
	// within the control bar to be changed
	virtual	BOOL SetViewName(int nView, LPCTSTR pszViewName);

	// this member function will return the index for a view
	// given a pointer to it
	virtual	int GetViewIndex(CView* pView) const;

	// this member function will delete a view based upon
	// its index
    virtual void DeleteView(int nView);

	// this member function will delete a view based upon
	// a CView object
    virtual void DeleteView(CRuntimeClass *pViewClass);

	// this member function will set the tooltip for a peticular view
	// in the tab control based on its index
	//
    virtual void SetViewToolTip(int nView, LPCTSTR lpszLabel);

	// this member function will set the tooltip for a peticular view
	//  in the tab control based on a CView object
	//
    virtual void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);

	// this member function is called by the control bar, and
	// can be overloaded in derived classes to return the rect
	// for the child window associated with the control bar.
	//
	virtual void GetChildRect(
		// reference to a CRect object which contains the size of
		// the child window associated with the control bar.
		CRect &rect);

	// this member function is used to set the resource id for the popup menu used
	// by the tab control.
	//
	virtual void SetTabMenuID(
		// id for the tab control popup menu
		UINT popupMenuID,
		// id for toolbar resource associated with popup
		UINT popToolbarID=0,
		// index position in menu resource
		int nPos=0);

	// Use this member function to set the active view to the specified view -- this 
	// provides more user control than the other SetActiveView()'s
	//
	virtual void SetActiveView(CView* pUserView);

	// Use this member function to map the image index to the class name,
	// and allows the image to be set based on the view's runtime class type
	//
	virtual void AssociateImageIndexWithRuntimeClass(int nIndex, CRuntimeClass* pRuntimeClass);
	
	// Use this member function to map the image index to the class name,
	// and allows the image to be set based on the name of the view's runtime class name
	//
	void AssociateImageIndexWithRuntimeClass(int nIndex, LPCTSTR lpszRuntimeClassName);

	// Use this member function to clear out all the tabs, but the user MUST 
	// take care of the views themselves
	//
	virtual BOOL RemoveAllTabs();

	// call this member function to retrieve the tab index from
	// current cursor position.
	virtual int GetTabFromPoint(
		// Pointer to a CPoint object that contains the cursor
		// screen coordinates. Use default for current cursor position.
		CPoint point = CPoint(0,0));

protected:

	// This member function returns the image associated with a runtime class, and
	// indicates which image will be displayed with the associated view class
	//
	virtual short GetImageIndexFromRuntimeClass(CRuntimeClass* pRuntimeClass);
	
	// This member function is used by the tab control bar to remove an item from the
	// view list.
	//
	virtual void RemoveListItem(POSITION pos);

	// This member function is used by the tab control bar to simply to perform
	// debug assertion validation.
	//
	virtual void OverkillAssert(CRuntimeClass *pViewClass);

	// This member function is called by the tab control bar to resize the
	// tab control window.
	//
	virtual void SizeTabView(CWnd* pWnd);

	// This member function is used by the tab control bar to return a pointer
	// to the tooltip control associated with this control.
	//
	virtual CToolTipCtrl* GetToolTip();

	// This member function is used by the tab control bar to add a
	// tooltip to a tab.
	//
	virtual void AddToolTip(TCB_ITEM* pMember, LPCTSTR lpszLabel);

	// This member function is used by the tab control bar to reset the tooltip control.
	//
	virtual void ResetToolTips(CToolTipCtrl* pToolTip);

	// This member function is used by the control bar to create a tab item.
	//
	virtual TCB_ITEM* CreateTabListItem(LPCTSTR lpszLabel);

	// This member function is used by the control bar to create a view to
	// be associated with a tab control item.
	//
	virtual BOOL CreateTabView(CRuntimeClass *pViewClass, CDocument *pDocument, CCreateContext *pContext);

	// This member function enables / disables tooltips
	//
	virtual BOOL EnableToolTips(BOOL bEnable);

// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJTabCtrlBar::SetTabMenuID(UINT popupMenuID, UINT popToolbarID, int nPos)
	{ m_TabCtrl.SetMenuID(popupMenuID, popToolbarID, nPos); }

_CJXLIB_INLINE CView* CCJTabCtrlBar::GetActiveView()
	{ return m_pActiveView; }

_CJXLIB_INLINE CImageList* CCJTabCtrlBar::SetTabImageList(CImageList *pImageList)
	{ return m_TabCtrl.SetImageList (pImageList); }

_CJXLIB_INLINE BOOL CCJTabCtrlBar::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
	{ return m_TabCtrl.ModifyStyle(dwRemove, dwAdd, nFlags); }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJTABCTRLBAR_H__
