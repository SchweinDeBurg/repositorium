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

#if !defined(AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_)
#define AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnBrowserCtrl.h : header file
//

#include "WebBrowserCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl window

class CEnBrowserCtrl : public CWebBrowserCtrl
{
	// Construction
public:
	CEnBrowserCtrl();
	virtual ~CEnBrowserCtrl();

	// Operations
public:
	void Print(LPCTSTR szFile = NULL);
	void PrintPreview(LPCTSTR szFile = NULL);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnBrowserCtrl)
	//}}AFX_VIRTUAL

protected:
	int m_nAction;

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnBrowserCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// event sink notification handlers
	afx_msg void OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL);
	DECLARE_EVENTSINK_MAP()

protected:
	BOOL SafeExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENBROWSERCTRL_H__73203E8C_CCFC_40B0_BE3E_3BC06DD234EB__INCLUDED_)
