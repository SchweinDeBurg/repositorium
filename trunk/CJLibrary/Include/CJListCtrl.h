// CJListCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the CListView articles from http://www.codeguru.com/listview
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 15/12/02 18:04 $
 * $Revision: 5 $
 * $Archive: /Projects/Libraries/CJLibrary/Include/CJListCtrl.h $
 *
 * $History: CJListCtrl.h $
 * 
 * *****************  Version 5  *****************
 * User: Anna         Date: 15/12/02   Time: 18:04
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * Added support for native (ComCtl v6) sort images in header controls
 * 
 * *****************  Version 4  *****************
 * User: Anna         Date: 15/12/02   Time: 18:02
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * 
 * *****************  Version 3  *****************
 * User: Andy         Date: 7/06/02    Time: 22:21
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Eliminate warnings when compiled with VC.NET
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 2/25/02    Time: 4:40p
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * 1.  CCJListCtrl::OnCustomDraw() is now an ON_NOTIFY_REFLECT_EX() hander
 * which returns FALSE - allowing the parent dialog to handle the message
 * if necessary
 * 2.  CCJListCtrl::SetRowColors() and CCJListView::SetRowColors() now
 * take
 *  default parameters of value CLR_DEFAULT to allow the text/background
 * colours to be reset to system defaults
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 7:13p
 * Updated in $/CodeJock/Include
 * Made m_bAscending protected rather than private for access in derived
 * classes.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:22p
 * Updated in $/CodeJock/Include
 * Overrode OnPaint() and  OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:42p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
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
 * User: Kirk Stowell Date: 7/18/99    Time: 10:14p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/12/99    Time: 3:15a
 * Updated in $/CodeJockey/Include
 * Cleaned up initialization, sorting and column info saving. Removed sort
 * functions, which were replaced with the virtual function SortList(...).
 * Override this function in your derived class to perform custom sorting.
 * See CCJShellList for an implementation of this virtual function.
 *
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 8:38p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 8:37p
 * Updated in $/CodeJockey/Include
 * Added InitializeFlatHeader() in order to initialize the list control
 * manually.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 8:37p
 * Created in $/CodeJockey/Include
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLISTCTRL_H__
#define __CJLISTCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "CJThemeHelper.h"		// CCJThemeHelper class declaration

class CCJFlatHeaderCtrl;


// CCJListCtrl is a CListCtrl derived class which implements a
// flat header and generic sorting routines.
class _CJX_EXT_CLASS CCJListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCJListCtrl)

public:

	// Default constructor
	//
	CCJListCtrl();

	// Virtual destructor
	//
	virtual ~CCJListCtrl();

protected:

	int					m_nMinColWidth;		// minimum width for columns
	int					m_nMaxColWidth;		// maximum width for columns
	int					m_nSortedCol;		// index of currently selected column
	CCJFlatHeaderCtrl*	m_pHeaderCtrl;		// points to subclassed flat header control.
	CString				m_strSection;		// registry section name.
	CString				m_strEntry;			// registry entry name.
	CString				m_strDefault;		// registry default value.
	CListCtrl*			m_pListCtrl;		// points to the list control associated with this view.
	COLORREF			m_clrText;			// RGB value for row text color.
	COLORREF			m_clrTextBk;		// RGB value for row text background color.
	bool				m_bNoColSizing;		// if true, column sizing is disabled.
	int					m_nMinSize;			// if set, columns will have a minimum size requirement.
	bool				m_bAscending;		// flag to determine sorting order
	CCJThemeHelper*		m_pTheme;			// WinXP theme helper (AJM 13.12.2002)

private:

	bool				m_bAutoSave;		// true if saving column widths


// Operations
public:
	// this member function will return the column width stored in the registry.
	//
	virtual int GetStoredWidth(int nCol);

	// this member function will automatically size a given column.
	//
	virtual void AutoSizeColumn(int nCol=-1);

	// this member function will set the column width from the
	// registry settings.
	//
	virtual void SetColumnWidth(int nCol);

	// this member function loads column widths from the registry.
	//
	virtual void LoadColumnWidths();

	// this member function saves column widths to the registry.
	//
	virtual void SaveColumnWidths();

	// this member function will initialize the autosave feature. If called
	// column widths will be saved to system registry when the list control
	// window is destroyed.
	//
	virtual void AutoSaveColumns(
		// registry section name, if NULL the default is "Settings".
		LPCTSTR lpszSection=NULL,
		// registry entry name, if NULL the default is "Column Info".
		LPCTSTR lpszEntry=NULL,
		// registry default value, if NULL the default id "".
		LPCTSTR lpszDefault=NULL);

	// call this member function to subclass the the flat header control.
	//
	virtual bool SubclassHeader(bool bBoldFont=false);

	// override this member function in your derived class to
	// peform custom sort routines.
	//
	virtual bool SortList(
		// passed in from control, index of column clicked.
		int nCol,
		// passed in from control, true if sort order should be ascending.
		bool bAscending );

	// this member function is called to set extended styles for the list
	// control ie: LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES, LVS_EX_HEADERDRAGDROP.
	//
	virtual void SetExtendedStyle( DWORD dwExStyle );

	// this member function is called to return the list control extended style.
	//
	virtual DWORD GetExtendedStyle();

	// call this member function to enable/disable column sizing
	//
	virtual void DisableColumnSizing(bool bDisable);

	// call this member function to set a minimum column size
	//
	virtual void SetMinimumColSize(int nSize);

	// call this member function to determine the row index and column index for a point.
	// returns the row index or -1 if point is not over a row.
	//
	virtual int HitTestEx(
		// point to be tested.
		CPoint &point,
		// to hold the column index
		int *col) const;

	// adds column after the right most column.
	//
	virtual int AddColumn(
		// the column's heading
		LPCTSTR lpszColHeading,
		// Width of the column, in pixels. If this parameter is -1, 
		// the column width is the same as previous column
		int nWidth = -1,
		// the alignment of the column. Can be LVCFMT_LEFT,
		// LVCFMT_RIGHT, or LVCFMT_CENTER.
		int nFormat=LVCFMT_LEFT);
	
	// this member function will build the columns for the list control. Returns
	// false if an error occured.
	//
	virtual bool BuildColumns(
		// number of columns the list control contains
		int nCols,
		// int array containing the column widths
		int* nWidth,
		// CString array containing the string for each column
		CString* strColString );
	
	// this member function will build the columns for the list control. Returns
	// false if an error occured.
	//
	virtual bool BuildColumns(
		// number of columns the list control contains
		int nCols,
		// int array containing the column widths
		int* nWidth,
		// int array containing the  string resource id for each column
		int* nColString );

	// sets the row text and background colors.
	virtual void SetRowColors(COLORREF clrText = CLR_DEFAULT, COLORREF clrTextBk = CLR_DEFAULT);
	
	// returns the number of columns in the list.
	virtual int GetColumnCount() const;

	// copies a row to a new location.
	//
	virtual bool CopyRow(int nFrom, int nTo);

	// moves a row to a new location.
	//
	virtual bool MoveRow(int nFrom, int nTo);

			void SetThemeHelper(CCJThemeHelper* pTheme);
	
	virtual	int SetSortImage(int nCol, bool bAsc);
			

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJListCtrl)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJListCtrl)
	afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE DWORD CCJListCtrl::GetExtendedStyle()
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); }

_CJXLIB_INLINE void CCJListCtrl::SetExtendedStyle(DWORD dwExStyle)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, GetExtendedStyle()|dwExStyle); }

_CJXLIB_INLINE void CCJListCtrl::SetRowColors(COLORREF clrText, COLORREF clrTextBk)
	{ m_clrText = clrText; m_clrTextBk = clrTextBk; }

_CJXLIB_INLINE void CCJListCtrl::SetMinimumColSize(int nSize)
	{ m_nMinSize = nSize; }

_CJXLIB_INLINE void CCJListCtrl::DisableColumnSizing(bool bDisable)
	{ m_bNoColSizing = bDisable; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJLISTCTRL_H__
