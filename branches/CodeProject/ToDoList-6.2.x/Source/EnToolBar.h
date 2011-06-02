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
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_)
#define AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar window

const COLORREF NO_COLOR = (COLORREF) - 1;

class CEnBitmapEx;

class CEnToolBar : public CToolBar
{
	// Construction
public:
	CEnToolBar();
	virtual ~CEnToolBar();

	BOOL LoadToolBar(LPCTSTR lpszResourceName, LPCTSTR szImagePath = NULL);
	BOOL LoadToolBar(UINT nIDResource, LPCTSTR szImagePath = NULL);
	BOOL LoadToolBar(UINT nIDResource, UINT nIDImage);
	BOOL SetImage(const CString& sImagePath, COLORREF crMask = NO_COLOR);
	BOOL SetImage(UINT nIDImage, COLORREF crMask = NO_COLOR);
	int GetButtonCount(BOOL bIgnoreSeparators = FALSE) const;

	void RefreshButtonStates(BOOL bImmediate = TRUE);
	void SetBackgroundColors(COLORREF crFrom, COLORREF crTo, BOOL bGradient);

	int CalcHeightRequired(int cx) const;
	int Resize(int cx, CPoint ptTopLeft);


	// Attributes
protected:
	CImageList m_ilDisabled, m_ilNormal;
	COLORREF m_crFrom, m_crTo;
	BOOL m_bGradient;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnToolBar)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnToolBar)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnRefreshButtonStates(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSizeParent(WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		return 0L;
	}
	DECLARE_MESSAGE_MAP()

	virtual LRESULT OnItemPrePaint(LPNMTBCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
	virtual LRESULT OnItemPostPaint(LPNMTBCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}

	// pseudo message handler
	BOOL OnEraseBkgnd(CDC* pDC);

	BOOL SetImage(CEnBitmapEx* pBitmap, COLORREF crMask);
	void RefreshDisabledImageList(CEnBitmapEx* pBitmap, COLORREF crMask);

	static BOOL GrayScale(CEnBitmapEx* pBitmap, COLORREF crMask = GetSysColor(COLOR_3DFACE));

	BOOL HasBkgndColor() const
	{
		return m_crFrom != NO_COLOR;
	}

	int GetRowCount() const;
	int GetRowHeight() const;

	int CalcRowsRequired(int cx) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTOOLBAR_H__9AA29CEC_1405_4BBC_BBD0_94C1BD6D3120__INCLUDED_)
