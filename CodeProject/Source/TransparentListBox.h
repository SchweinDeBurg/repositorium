// Original file written by Ali Rafiee (http://www.codeproject.com/KB/combobox/TransListBox.aspx)
// Rewritten for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#ifndef _TRANSPARENTLISTBOX_H_
#define _TRANSPARENTLISTBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SItem
{
	DWORD dwItemData;
	BOOL bCompleted;
};

class CTransparentListBox : public CListBox
{
// Construction
public:
	CTransparentListBox();
	virtual ~CTransparentListBox();

// Operations
public:
	int  AddString(const CString& _strText, DWORD _dwItemData = 0, BOOL _bCompleted = FALSE);
	int  InsertString(int _nIndex, const CString& _strText, DWORD _dwItemData = 0, BOOL _bCompleted = FALSE);
	int  SetCurSel(int nSelect);

	int  GetTotalItemHeight() const;
	void EnsureTopItemVisible();

	void SetCompletedItemDisplayStyle(BOOL _bGrey, BOOL _bStrikethru);

// Overrides
protected:
	//{{AFX_VIRTUAL(CTransparentListBox)
	virtual void PreSubclassWindow();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem(CDC& _dc, int _nIndex, CRect& _rc, BOOL _bSelected);
	//}}AFX_VIRTUAL

// Message Map Functions
	//{{AFX_MSG(CTransparentListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnLbnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CFont       m_font;
	CFont       m_fontStrikethru;
	CBitmap     m_bmp;
	BOOL        m_bHasBackground;
	BOOL        m_bCompletedItemsGrey;
	BOOL        m_bCompletedItemsStrikethru;
	COLORREF    m_colorTextFG;
	COLORREF    m_colorSelectedTextFG;
	COLORREF    m_colorSelectedTextBG;
	COLORREF    m_colorSelectedTextOutline;
	COLORREF    m_colorTextDisabled;
	CArray <SItem*, SItem*> m_arrItems;
};

#endif//_TRANSPARENTLISTBOX_H_
