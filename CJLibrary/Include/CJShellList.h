// CJShellList.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based upon the MFCENUM example. MFCENUM is a companion sample application 
// for Chapter 14 in "Programming the Windows 95 User Interface" by Nancy Cluts,
// available in the Microsoft(R) Development Library.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 10:46p $
 * $Revision: 9 $
 * $Archive: /CodeJock/Include/CJShellList.h $
 *
 * $History: CJShellList.h $
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:46p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/12/99    Time: 3:00a
 * Updated in $/CodeJockey/Include
 * Cleaned up image list, view initialization and sorting to use base
 * class virtual function SortList(...).
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 3:38p
 * Updated in $/CodeJockey/Include
 * Added version macro  _WIN32_IE, you will need to have at IE4 or greater
 * installed to use this class. This will ensure that the version of
 * SHELL32.DLL is compatible.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:05a
 * Updated in $/CodeJockey/Include
 * Added flag m_bBuildingList, set to true whenever the list is refreshed
 * or initialized.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 3/19/99    Time: 11:03p
 * Created in $/CodeJockey/Include
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSHELLLIST_H__
#define __CJSHELLLIST_H__

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CJShell.h"
#include "CJListCtrl.h"

// class forwards
class CCJShellTree;

// CCJShellList is a CCJListCtrl derived class which is used to display
// system files and folders, typically associated with a CCJShellTree object
class _CJX_EXT_CLASS CCJShellList : public CCJListCtrl
{
	DECLARE_DYNAMIC(CCJShellList)

public:

	// Default constructor
	//
	CCJShellList();

	// Virtual destructor
	//
	virtual ~CCJShellList();

public:

	CCJShell		m_shell;			// used for shell operations
	CCJShellTree*	m_pTreeCtrl;		// points to a CCJShellTree that is associated with this control
	BOOL			m_bBuildingList;
	BOOL			m_bShowHidden;

public:
	virtual void BuildDefaultColumns();

	// this member function rebuilds the current directory list.
	//
	virtual void RefreshList();

	// call this member function to associate a CCJShellTree object with
	// this control
	//
	virtual void AssociateTree(CCJShellTree *pTreeCtrl);

	// this member function is called to initialize the system image list
	// used by the list control
	//
	virtual BOOL InitSystemImageLists();

	// this member function is called to initialize the list control items
	//
	virtual BOOL InitListViewItems(
		// Pointer to TreeView item data
		LPTVITEMDATA lptvid,
		// Pointer to parent shell folder
		LPSHELLFOLDER lpsf);

	// this member function is called to actually populate the list control
	// items
	//
	virtual BOOL PopulateListView(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf);

	// this member function is called to insert a single item into the
	// list control
	//
	virtual int InsertListItem(CString strFileName, BOOL bParse=TRUE);

	// Callback routine for sorting list
	//
	static int CALLBACK ListViewCompareProc(LPARAM, LPARAM, LPARAM);

	// override this member function in your derived class to
	// peform custom sort routines.
	//
	virtual bool SortList(
		// passed in from control, index of column clicked.
		int nCol,
		// passed in from control, true if sort order should be ascending.
		bool bAscending );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJShellList)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJShellList)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg long OnShellNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJShellList::AssociateTree(CCJShellTree *pTreeCtrl)
	{ ASSERT( pTreeCtrl ); m_pTreeCtrl = pTreeCtrl; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _WIN32_IE >= 0x0400
#endif // __CJSHELLLIST_H__
