// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and licenese information
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

#if !defined(AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_)
#define AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnEdit.h : header file
//

#include "MaskEdit.h"
#include "HotTracker.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CEnEdit window

const int DEF_BTNWIDTH = ::GetSystemMetrics(SM_CXHTHUMB);
const int CALC_BTNWIDTH = -1;

const UINT WM_EE_BTNCLICK = ::RegisterWindowMessage(_T("WM_EE_BTNCLICK")); // wParam = CtrlID, lParam = BtnID

class CEnEdit : public CMaskEdit
{
// Construction
public:
	CEnEdit(BOOL bComboStyle = TRUE, LPCTSTR szMask = NULL, DWORD dwFlags = 0);

	BOOL AddButton(UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH, LPCTSTR szFont = NULL);
	BOOL AddButton(UINT nID, UINT nChar, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH, LPCTSTR szFont = NULL);
	BOOL InsertButton(int nPos, UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH, LPCTSTR szFont = NULL);
	BOOL InsertButton(int nPos, UINT nID, UINT nChar, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH, LPCTSTR szFont = NULL);

	BOOL EnableButton(UINT nID, BOOL bEnable = TRUE);
	BOOL CheckButton(UINT nID, BOOL bChecked = TRUE);
	BOOL SetButtonTip(UINT nID, LPCTSTR szTip);
	BOOL SetButtonCaption(UINT nID, LPCTSTR szCaption);
	CRect GetButtonRect(UINT nID);
	BOOL SetButtonWidth(UINT nID, int nWidth);
	BOOL SetButtonWidthDLU(UINT nID, int nDLU);
	BOOL SetDropMenuButton(UINT nID, BOOL bDropMenu = TRUE);

	void SetBorders(int nTop, int nBottom);

// Attributes
protected:
	struct EDITBTN
	{
		UINT nID;
		CString sCaption;
		CString sTip;
		int nWidth;
		BOOL bEnabled;
		HFONT hFont;
		BOOL bChecked;
		BOOL bDropMenu;
	};

	CArray<EDITBTN, EDITBTN&> m_aButtons;
	BOOL m_bFirstShow;
	BOOL m_nButtonDown;
	CToolTipCtrl m_tooltip;
	BOOL m_bComboStyle;
	CHotTracker m_hotTrack;
	int m_nTopBorder, m_nBottomBorder;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void OnSetReadOnly(BOOL /*bReadOnly*/) {}

// Implementation
public:
	virtual ~CEnEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnEdit)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
#if _MSC_VER >= 1400
	afx_msg LRESULT OnNcHitTest(CPoint point);
#else
	afx_msg UINT OnNcHitTest(CPoint point);
#endif
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg LRESULT OnSetReadOnly(WPARAM /*wp*/, LPARAM /*lp*/);
	afx_msg LRESULT OnHotChange(WPARAM /*wp*/, LPARAM /*lp*/);
	virtual void OnBtnClick(UINT /*nID*/) {}
	virtual void NcPaint(CDC* pDC, const CRect& rWindow);
	virtual void RecalcBtnRects();
	DECLARE_MESSAGE_MAP()

	CRect GetButtonRectByIndex(int nBtn) const; // screen coords
	virtual void DrawButton(CDC* pDC, const CRect& rWindow, int nBtn, const CPoint& ptCursor) const;
	int ButtonHitTest(CPoint ptScreen) const;
	int ButtonHitTest(UINT nID) const;
	int GetButtonWidth(int nBtn) const;
	BOOL InsertButton(int nPos, UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth, 
						LPCTSTR szFont, BOOL bSymbolFont);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_)
