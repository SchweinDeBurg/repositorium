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

#if !defined(AFX_MASKEDIT_H__03CD6C84_2D70_4E44_9E7E_5673B6F6F97E__INCLUDED_)
#define AFX_MASKEDIT_H__03CD6C84_2D70_4E44_9E7E_5673B6F6F97E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MaskEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit window

enum
{
	ME_EXCLUDE			= 0x01,
	ME_LOCALIZEDECIMAL	= 0x02,
};

class CMaskEdit : public CEdit
{
// Construction
public:
	CMaskEdit(LPCTSTR szMask = NULL, DWORD dwFlags = 0);

	void SetMask(LPCTSTR szMask, DWORD dwFlags = 0);

protected:
	CString m_sMask;
	DWORD m_dwFlags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaskEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMaskEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMaskEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MASKEDIT_H__03CD6C84_2D70_4E44_9E7E_5673B6F6F97E__INCLUDED_)
