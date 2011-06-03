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
// - adjusted #include's paths
//*****************************************************************************

#if !defined(AFX_ENCOLORDIALOG_H__0C8EC4ED_8B90_4F81_96FC_085F8F36229D__INCLUDED_)
#define AFX_ENCOLORDIALOG_H__0C8EC4ED_8B90_4F81_96FC_085F8F36229D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnColorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnColorDialog dialog

class CEnColorDialog : public CColorDialog
{
	DECLARE_DYNAMIC(CEnColorDialog)

public:
	CEnColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
	~CEnColorDialog();

protected:
	//{{AFX_MSG(CEnColorDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCOLORDIALOG_H__0C8EC4ED_8B90_4F81_96FC_085F8F36229D__INCLUDED_)
