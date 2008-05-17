// CJTreeCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the CTreeView articles from http://www.codeguru.com/treeview
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 2:17p $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/CJTreeCtrl.h $
 *
 * $History: CJTreeCtrl.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:17p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:49p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:09a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Created in $/CodeJockey/Include
 * Initial creation.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTREECTRL_H__
#define __CJTREECTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

// CCJTreeCtrl is a CTreeCtrl derived class which extends the base class to add
// additional functionality and enhancements.
class _CJX_EXT_CLASS CCJTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CCJTreeCtrl)

public:

	// Default constructor
	//
	CCJTreeCtrl();

	// Virtual destructor
	//
	virtual ~CCJTreeCtrl();

protected:

	HTREEITEM	m_hItemFirstSel;	// first selected item in a multi selection tree control.
	DWORD		m_dwDragStart;		// the number of milliseconds that have elapsed since the system was started.
	BOOL		m_bMultiSelect;		// TRUE for a multiselection tree control.

	struct Color_Font {	COLORREF color;	LOGFONT logfont; };
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;
	
public:

	// Call this function to retrieve the tree view item that has the specified 
	// relationship, indicated by the nCode parameter, to hItem.
	// Returns The handle of the next item if successful; otherwise NULL.
	virtual HTREEITEM GetNextItem(
		// Handle of a tree item.
		HTREEITEM hItem,
		// A flag indicating the type of relation to hItem. This flag can be one of the 
		// following values: 
		// TVGN_CARET   Retrieves the currently selected item.
		// TVGN_CHILD   Retrieves the first child item. The hItem parameter must be NULL.
		// TVGN_DROPHILITE   Retrieves the item that is the target of a drag-and-drop operation.
		// TVGN_FIRSTVISIBLE   Retrieves the first visible item.
		// TVGN_NEXT   Retrieves the next sibling item.
		// TVGN_NEXTVISIBLE   Retrieves the next visible item that follows the specified item.
		// TVGN_PARENT   Retrieves the parent of the specified item.
		// TVGN_PREVIOUS   Retrieves the previous sibling item.
		// TVGN_PREVIOUSVISIBLE   Retrieves the first visible item that precedes the specified item.
		// TVGN_ROOT   Retrieves the first child item of the root item of which the specified item is a part. 
		UINT nCode );

	// Get next item as if outline was completely expanded
	// Returns the item immediately below the reference item.
	//
	virtual HTREEITEM GetNextItem(
		// The reference item
		HTREEITEM hItem );

	// Get previous item as if outline was completely expanded.
	// Returns the item immediately above the reference item.
	//
	virtual HTREEITEM GetPrevItem(
		// The reference item
		HTREEITEM hItem );

	// Gets last item in the branch, returns the last item.
	//
	virtual HTREEITEM GetLastItem(
		// Node identifying the branch. NULL will return the last item in outine.
		HTREEITEM hItem );
	
	// Returns the previously selected tree item in a multi selection tree control.
	//
	virtual HTREEITEM GetPrevSelectedItem(
		// The reference item.
		HTREEITEM hItem );

	// Returns the next selected item in a multi selection tree control.
	virtual HTREEITEM GetNextSelectedItem(
		// The reference item.
		HTREEITEM hItem );
	
	// Returns the first selected item in a multi selection tree control.
	//
	virtual HTREEITEM GetFirstSelectedItem();

	// Returns the RGB text value for the referenced tree item.
	// Returns (COLORREF)-1 if color was not set.
	//
	virtual COLORREF GetItemColor(
		// The reference item
		HTREEITEM hItem);

	// Clears all selections from multi selection tree control.
	//
	virtual void ClearSelection();

	// sets the font for the reference tree item.
	//
	virtual void SetItemFont(
		// The reference item
		HTREEITEM hItem,
		// new font for tree item.
		LOGFONT& logfont);

	// sets the font to bold for the refernece tree item.
	//
	virtual void SetItemBold(
		// The reference item
		HTREEITEM hItem,
		// TRUE for bold font.
		BOOL bBold=TRUE);

	// Sets the text color for the tree item.
	//
	virtual void SetItemColor(
		// The reference item
		HTREEITEM hItem,
		// RGB value for the tree items text.
		COLORREF color);

	// enables multi selection tree control.
	//
	virtual void EnableMultiSelect(
		// TRUE for multi selection tree control.
		BOOL bMultiSelect=TRUE);

	// returns the current font used by the tree item.
	//
	virtual BOOL GetItemFont(
		// The reference item
		HTREEITEM hItem,
		// item font.
		LOGFONT * plogfont);

	// returns TRUE if the tree item has a bold font.
	virtual BOOL GetItemBold(
		// The reference item
		HTREEITEM hItem);

	// Selects items from hItemFrom to hItemTo in a multi selection tree control.
	// Does not select child item if parent is collapsed. Removes selection from 
	// all other items.
	//
	virtual BOOL SelectItems(
		// item to start selecting from.
		HTREEITEM hItemFrom,
		// item to end selection at.
		HTREEITEM hItemTo);

	// Finds an item that contains the search string, returns the handle
	// to the item or NULL.
	//
	virtual HTREEITEM FindItem(
		// String to search for.
		LPCTSTR lpszSearch,
		// Should the search be case sensitive.
		BOOL bCaseSensitive = FALSE,
		// Search direction - TRUE for down.
		BOOL bDownDir = TRUE,
		// True if search should match whole words.
		BOOL bWholeWord = FALSE,
		// Item to start searching from. NULL for currently selected item.
		HTREEITEM hItem = NULL);

protected:

	// Virtual function used by FindItem to allow this function to filter the 
	// result of FindItem returns TRUE if item matches the criteria.
	//
	virtual BOOL IsFindValid(
		// Handle of the item
		HTREEITEM hti);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJTreeCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE HTREEITEM CCJTreeCtrl::GetNextItem( HTREEITEM hItem, UINT nCode )
	{ return CTreeCtrl::GetNextItem( hItem, nCode ); }

_CJXLIB_INLINE void CCJTreeCtrl::EnableMultiSelect(BOOL bMultiSelect)
	{ m_bMultiSelect = bMultiSelect; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJTREECTRL_H__
