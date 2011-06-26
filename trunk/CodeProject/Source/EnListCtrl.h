// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// EnListCtrl.h : header file
//
#if !defined ( ENLISTCTRL_H )
#define ENLISTCTRL_H

#include "EnHeaderCtrl.h"
#include "EnString.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CEnListCtrl window

#define LVN_USERSELCHANGE           (LVN_FIRST-21)
#define LVN_COPY                    (LVN_FIRST-22)
#define LVN_CUT                     (LVN_FIRST-23)
#define LVN_PASTE                   (LVN_FIRST-24)
#define LVN_DELETE                  (LVN_FIRST-25)
#define LVN_USERSELCHANGEDBLCLK     (LVN_FIRST-26)
#define LVN_CHECKCHANGE             (LVN_FIRST-40)

// helper class for extending column data
class CColumnData
{
public:
	CColumnData()
	{
		crText = ::GetSysColor(COLOR_WINDOWTEXT);
		nFormat = ES_END;
	}
	COLORREF crText;
	int nFormat;
};

class CEnListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEnListCtrl)

// Construction
public:
	CEnHeaderCtrl* GetHeader();
	const CEnHeaderCtrl* GetHeader() const;
	virtual int SetCurSel(int nIndex, bool bNotifyParent = FALSE); // single selection
	virtual int GetCurSel() const;
	void SetMulSel(int nIndexStart, int nIndexEnd, BOOL bSelect = TRUE, BOOL bNotifyParent = FALSE); // multiple selection
	void SetItemFocus(int nIndex, BOOL bFocused);
	int GetFirstSel() const;
	int GetNextSel(int nPrevSel) const;
	int GetLastSel() const;
	int GetPrevSel(int nNextSel) const;
	int GetCountPerPage() const;
	void SelectAll();
	void ClearAll();
	CEnListCtrl();
	void EnableHeaderTracking(BOOL bAllow);
	void ShowGrid(BOOL bVert, BOOL bHorz);
	void IsShowingGrid(BOOL& bVert, BOOL& bHorz) const
	{
		bVert = m_bVertGrid;
		bHorz = m_bHorzGrid;
	}
	void SetView(int nView);
	int GetView() const
	{
		return m_nCurView;
	}
	int GetFocusedItem() const;
	void SetLastColumnStretchy(BOOL bStretchy);
	void SetFirstColumnStretchy(BOOL bStretchy);
	int FindItemFromData(DWORD dwItemData) const;
	int FindItemFromLabel(CString sLabel, BOOL bExact = TRUE, int nFromIndex = 0) const;
	void EnableTooltipCtrl(BOOL bEnable);
	BOOL SetTooltipCtrlText(CString sText);
	BOOL SetItemHeight(int nHeight);
	void DeleteAllColumns();
	void SetReadOnly(BOOL bReadOnly)
	{
		m_bReadOnly = bReadOnly;
	}
	BOOL IsReadOnly() const
	{
		return m_bReadOnly;
	}
	BOOL SelectDropTarget(int nItem);
	void SetItemImage(int nItem, int nImage);
	BOOL IsItemSelected(int nItem) const;
	void SetSortAscending(BOOL bAscending)
	{
		m_bSortAscending = bAscending;
	}
	BOOL GetSortAscending() const
	{
		return m_bSortAscending;
	}
	virtual void Sort();
	void EnableSorting(BOOL bEnable)
	{
		m_bSortingEnabled = bEnable;
	}
	void SetItemIndent(int nItem, int nIndent);
	int GetItemIndent(int nItem) const;
	void EnableAlternateRowColoring(BOOL bEnable = TRUE);

	// column methods
	int GetColumnCount() const;
	int GetSortColumn() const
	{
		return m_nSortColumn;
	}
	void SetSortColumn(int nColumn, BOOL bResort = TRUE);
	COLORREF GetColumnColor(int nCol) const;
	void SetColumnColor(int nCol, COLORREF color);
	void SetColumnFormat(int nCol, int nFormat);
	int GetColumnFormat(int nCol) const;
	BOOL SetColumnText(int nCol, LPCTSTR szText);

// Attributes
protected:
	CEnHeaderCtrl m_header;
	BOOL m_bVertGrid, m_bHorzGrid;
	int m_nCurView;
	NMHDR m_nmhdr; // for notification
	BOOL m_bLastColStretchy;
	BOOL m_bFirstColStretchy;
	BOOL m_bTooltipsEnabled;
	CToolTipCtrl m_ttCtrl;
	int m_nItemHeight;
	BOOL m_bReadOnly;
	int m_nItemDropHilite;
	BOOL m_bDropHiliteItemSelected;
	BOOL m_bContextPopupEnabled;
	BOOL m_bDrawCheckboxes;
	CPoint m_ptPopupPos;
	BOOL m_bUserSelChange;
	BOOL m_bSortingEnabled;
	int m_nSortColumn;
	BOOL m_bSortAscending;
	BOOL m_bInitColumns; // up to derived class to set: gets cleared in OnDestroy
	BOOL m_bAlternateRowColoring;

private:
	CMap<int, int, CColumnData*, CColumnData*> m_mapColumnData;
	CMap<DWORD, DWORD, CString, CString&> m_mapSortStrings;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnListCtrl)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CEnListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LPARAM* lParam);
	DECLARE_MESSAGE_MAP()

	// helpers
	int GetImageIndex(int nItem, int nSubItem) const;
	virtual void Copy()
	{
	} // derived class override provides logic
	virtual void Delete()
	{
	} // derived class override provides logic
	virtual void Cut()
	{
	} // derived class override provides logic
	virtual void Paste()
	{
	} // derived class override provides logic
	void NotifySelChange();
	int GetColumnAtPoint(CPoint point) const;
	void GetColumnExtents(int nCol, int& nColStart, int& nColEnd) const;
	virtual int CompareItems(DWORD dwItemData1, DWORD dwItemData2, int nSortColumn);
	virtual COLORREF GetItemTextColor(int nItem, int nSubItem, BOOL bSelected, BOOL bDropHighlighted,
		BOOL bWndFocus) const;
	virtual COLORREF GetItemBackColor(int nItem, BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus) const;
	virtual CFont* GetItemFont(int /*nItem*/, int /*nSubItem*/)
	{
		return NULL;
	}
	int GetImageStyle(BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus) const;
	virtual CString GetNoItemsText() const
	{
		return _T("");
	}

	void DeleteAllColumnData();
	virtual CColumnData* GetNewColumnData() const
	{
		return new CColumnData;
	}
	CColumnData* CreateColumnData(int nCol);
	const CColumnData* GetColumnData(int nCol) const;
	void BuildSortMap(int nCol);
	CString GetSortText(DWORD dwItemData) const;
	virtual void GetCellRect(int nRow, int nCol, CRect& rCell) const;
	virtual void GetCellEditRect(int nRow, int nCol, CRect& rCell) const;

private:
	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);
};

#endif

/////////////////////////////////////////////////////////////////////////////
