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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_TOOLSUSERINPUTDLG_H__7C10499F_E103_4106_8581_DCD5D55FAEF5__INCLUDED_)
#define AFX_TOOLSUSERINPUTDLG_H__7C10499F_E103_4106_8581_DCD5D55FAEF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ToolsUserInputDlg.h : header file
//

#include "ToolsCmdlineParser.h"
#include "../../../CodeProject/Source/RuntimeDlg.h"
#include "../../../CodeProject/Source/DialogHelper.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CToolsUserInputDlg dialog

class CToolsUserInputDlg : public CRuntimeDlg
{
// Construction
public:
	CToolsUserInputDlg(const CToolsCmdlineParser& tcp);
	virtual ~CToolsUserInputDlg();

	int DoModal(LPCTSTR szTitle)
	{
		return CRuntimeDlg::DoModal(szTitle);
	}
	CString GetResult(LPCTSTR szItemName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolsUserInputDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

	struct TUINPUTITEM
	{
		UINT nCtrlID;
		CString sLabel; // this will be a simple static text item
		CString sName;
		CString sDefValue;
		CLA_TYPE nType;
		UINT nStyle;
		CSize sizeDLU;
		CWnd* pCtrl; // ptr to dynamically allocated items
	};

	CArray<TUINPUTITEM, TUINPUTITEM&> m_aInputItems;
	CMapStringToString m_mapResults; // mapped by name

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolsUserInputDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSUSERINPUTDLG_H__7C10499F_E103_4106_8581_DCD5D55FAEF5__INCLUDED_)
