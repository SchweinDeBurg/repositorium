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

#if !defined(AFX_TABCTRLEX_H__F97F9C15_5623_4A5E_ACB5_C10D4C8EB75D__INCLUDED_)
#define AFX_TABCTRLEX_H__F97F9C15_5623_4A5E_ACB5_C10D4C8EB75D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabCtrlEx.h : header file
//

#include "XPTabCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx window

// flag
enum
{
	TCE_POSTDRAW		= 0x0001,
	TCE_MBUTTONCLICK	= 0x0002,

	TCE_ALL				= 0xffff
};

#ifndef NM_MCLICK
#	define NM_MCLICK (NM_LAST+1)
#endif

typedef struct tagTCMBTNCLK
{
    NMHDR hdr;
    int iTab;
    UINT flags;
} NMTCMBTNCLK;

class CTabCtrlEx : public CXPTabCtrl
{
// Construction
public:
	CTabCtrlEx(DWORD dwFlags = TCE_ALL);

// Attributes
protected:
	DWORD m_dwFlags;
	BOOL m_bMBtnDown;
	CPoint m_ptMBtnDown;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabCtrlEx)
	afx_msg void OnPaint();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLEX_H__F97F9C15_5623_4A5E_ACB5_C10D4C8EB75D__INCLUDED_)
