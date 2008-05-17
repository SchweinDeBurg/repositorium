// CJTabView.h: interface for the CCJTabView class.
//
// Copyright © 1999 Mike Manzo
// mailto:mmanzo@ieee.org
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 2:17p $
 * $Revision: 10 $
 * $Archive: /CodeJock/Include/CJTabView.h $
 *
 * $History: CJTabView.h $
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:17p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:48p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 3:15a
 * Updated in $/CodeJock/Include
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTABVIEW_H__
#define __CJTABVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CJTabView.h : header file
//

typedef struct tagCTV_ITEM
{
	CWnd *pWnd;
	TCHAR szLabel[32];
} CTV_ITEM;

#define TABVIEW_BORDER 3

// template list used to contain tab information
typedef CList <CTV_ITEM*, CTV_ITEM *> CListViews;

// CCJTabView is a CCtrlView derived class which can be used to
// create a view that contains a tab control
class _CJX_EXT_CLASS CCJTabView : public CCtrlView
{
	DECLARE_DYNCREATE(CCJTabView)

protected:

	// Default constructor
	//
	CCJTabView();

	// Virtual destructor
	//
	virtual ~CCJTabView();

protected:
	
	CView*			m_pActiveView;	// pointer to the active view
	int				m_nActiveTab;	// index of the active tab
	CTabCtrl*		m_pTabCtrl;		// pointer to the tab control
	CToolTipCtrl*	m_pToolTip;		// pointer to the tooltip control
	CFont			m_TabFont;		// font used by tab control
	CListViews		m_views;		// template list containing tab information

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
		// create context for view
		CCreateContext *pContext = NULL);

	// this member function will modify the style for the
	// tab control associated with this control.
	//
	virtual BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);

	// this member function will set a view active
	// based on a CView object
	//
	virtual void SetActiveView(CRuntimeClass *pViewClass);

	// this member function will set a view active
	// based on index
	//
	virtual void SetActiveView(int nNewTab);

	// this member function will remove a view based on its index
	//
	virtual void RemoveView(int nView);

	// this member function returns a pointer to a view
	// based on a CView object
	//
	virtual CView* GetView(CRuntimeClass *pViewClass);

	// this member function returns a pointer to a view
	// based on its index
	//
	virtual CView* GetView(int nView);

	// this member function returns a pointer to the
	// active view in the tab control
	//
	virtual CView* GetActiveView();

	// this member function initializes the image list
	// for the tab control
	//
	virtual CImageList* SetTabImageList(CImageList *pImageList);

	// this member function returns a pointer to the
	// tab control associated with this control
	//
	virtual CTabCtrl* GetTabCtrl();

	// this member function is called by the view, and
	// can be overloaded in derived classes to return the rect
	// for the child window associated with the control bar.
	//
	virtual void GetChildRect(
		// reference to a CRect object which contains the size of
		// the child window associated with the control bar.
		CRect &rect);

private:

	void CalcViewRect(CRect* pRect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTabView)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJTabView)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE CTabCtrl* CCJTabView::GetTabCtrl()
	{ return m_pTabCtrl; }

_CJXLIB_INLINE CView* CCJTabView::GetActiveView()
	{ return m_pActiveView; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJTABVIEW_H__
