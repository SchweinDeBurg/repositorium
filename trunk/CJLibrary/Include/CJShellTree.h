// CJShellTree.h : header file
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
 * $Date: 10/30/99 2:19p $
 * $Revision: 10 $
 * $Archive: /CodeJock/Include/CJShellTree.h $
 *
 * $History: CJShellTree.h $
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:19p
 * Updated in $/CodeJock/Include
 * Changed base class from CTreeCtrl to CCJTreeCtrl for flicker free
 * drawing.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:47p
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
 * User: Kirk Stowell Date: 6/11/99    Time: 10:14p
 * Updated in $/CodeJockey/Include
 * Cleaned up system image list initialization.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 3:38p
 * Updated in $/CodeJockey/Include
 * Added version macro  _WIN32_IE, you will need to have at IE4 or greater
 * installed to use this class. This will ensure that the version of
 * SHELL32.DLL is compatible.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 3/19/99    Time: 11:05p
 * Updated in $/CodeJockey/Include
 * Initial re-write and release.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 11:05p
 * Created in $/CodeJockey/Include
 * Overloaded PopulateTree(LPCTSTR lpszPath) in the class CCJShellTree to
 * fill the tree based upon path. Takehiko Mizoguti
 * [mizoguti@m2.sys.to.casio.co.jp]
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSHELLTREE_H__
#define __CJSHELLTREE_H__

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CJShell.h"
#include "CJTreeCtrl.h"

// class forwards
class CCJShellList;

// CCJShellTree is a CCJTreeCtrl derived class which is used to display
// system files and folders, typically associated with a CCJShellList object
class _CJX_EXT_CLASS CCJShellTree : public CCJTreeCtrl
{
	DECLARE_DYNAMIC(CCJShellTree)

public:
	
	// Default constructor
	//
	CCJShellTree();

	// Virtual destructor
	//
	virtual ~CCJShellTree();

public:

	CCJShell		m_shell;		// used for shell operations
	CCJShellList*	m_pListCtrl;	// CCJShellList that is associated with this control 
	CComboBox*		m_pComboBox;	// CComboBox that is associated with this control see AssociateCombo(...)
	bool			m_bRefresh;		// true when the control is to be refreshed
	bool			m_bOutaHere;	// true when the control is destroyed
	HTREEITEM		m_htiCurrent;	// currently selected tree item

	// enumeration that holds the type of object folder or drive
	//
	enum FindAttribs { type_drive, type_folder };

public:

	// this member function obtains a handle to the system image 
	// list and attaches it to the tree control. DO NOT DELETE the imagelist
	//
	virtual BOOL InitSystemImageLists();

	// this member function fills a branch of the TreeView control. Given the
	// shell folder, enumerate the subitems of this folder,
	// and add the appropriate items to the tree.
	//
	// This function enumerates the items in the folder identifed by lpsf.
	// Note that since we are filling the left hand pane, we will only add
	// items that are folders and/or have sub-folders.  We *could* put all
	// items in here if we wanted, but that's not the intent.
	//
	virtual void FillTreeView(
		// pointer to shell folder that we want to enumerate items 
		LPSHELLFOLDER lpsf,
		// fully qualified item id list to the item that we are enumerating
		// items for.  In other words, this is the PIDL to the item
		// identified by the lpsf parameter.
		LPITEMIDLIST lpifq,
		// parent node
		HTREEITEM hParent);

	// this member function processes the File.Fill/RefreshTree command
	//
	virtual void OnFill(
		// true when refreshing tree
		bool bRefresh=false);

	// this member function populates tree based upon path. this method was
	// written by Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp]
	//
	virtual void PopulateTree(
		// path to populate
		LPCTSTR lpszPath);

	// this member function will "tunnel" the tree to find the specified path.
	// this will only work when the OnFill(...) method is used to populate the tree
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual void TunnelTree(
		// path to find
		CString szFindPath);

	// this member function is used to associate a CComboBox object with the
	// control. whenever the path changes, the combo is updated.
	//
	virtual void AssociateCombo(
		// combo box to associate
		CComboBox *pComboBox);

	// this member function is used to associate a CCJShellList object with the
	// control. whenever the path changes, the list is updated.
	//
	virtual void AssociateList(
		// list control to associate
		CCJShellList *pListCtrl);

	// this member function retrieves the path of a folder item, does not have 
	// to be selected. Pass a CString object that will hold the folder path. 
	// If the path is not in the filesystem(eg MyComputer) 
	// or none is selected it returns FALSE.
	//
	virtual BOOL GetFolderItemPath(
		// tree item to get path for
		HTREEITEM hItem,
		// reference to a CString object to contain folder path
		CString &strFolderPath);

	// this member function retrieves the path of a folder item, does not have 
	// to be selected. Pass a CString object that will hold the folder path. 
	// If the path is not in the filesystem(eg MyComputer) 
	// or none is selected it returns FALSE.
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual BOOL GetSelectedFolderPath(
		// reference to a CString object to contain folder path
		CString &strFolderPath);

	// this member function searchs the tree for a specified folder
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual BOOL SearchTree(
		// tree item to begin search from
		HTREEITEM treeNode,
		// string to search for
		CString szSearchName,
		// looking for folder or drive
		FindAttribs attr);

	// Callback routine for sorting tree
	//
	static int CALLBACK TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM lparamSort);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJShellTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJShellTree)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJShellTree::AssociateList(CCJShellList *pListCtrl)
	{ ASSERT( pListCtrl != NULL ); m_pListCtrl = pListCtrl; }

_CJXLIB_INLINE void CCJShellTree::AssociateCombo(CComboBox *pComboBox)
	{ ASSERT( pComboBox != NULL ); 	m_pComboBox = pComboBox; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _WIN32_IE >= 0x0400
#endif // __CJSHELLTREE_H__
