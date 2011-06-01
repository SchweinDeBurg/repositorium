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
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

#if !defined(AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_)
#define AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnStatic window

class CEnStatic : public CStatic
{
	// Construction
public:
	CEnStatic(BOOL bEnableGripper = FALSE);

	void EnableGripper(BOOL bEnable = TRUE);

	// Attributes
protected:
	BOOL m_bGripper;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnStatic)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CEnStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnStatic)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
#if _MSC_VER >= 1400
	afx_msg LRESULT OnNcHitTest(CPoint point);
#else
	afx_msg UINT OnNcHitTest(CPoint point);
#endif

	DECLARE_MESSAGE_MAP()

	BOOL IsShowingGripper();
	CRect GetGripperRect();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_)
