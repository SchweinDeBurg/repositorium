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
// - adjusted #include's paths
//*****************************************************************************

#if !defined(AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_)
#define AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
	// Construction
public:
	CColorButton();

	COLORREF GetColor() { return m_color; }
	void SetColor(COLORREF color);

protected:
	COLORREF m_color;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnClicked();
	//}}AFX_MSG
	afx_msg LRESULT OnShowAccelerators(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__4FA393FA_20E7_4C07_A682_5AA7A8306DF4__INCLUDED_)
