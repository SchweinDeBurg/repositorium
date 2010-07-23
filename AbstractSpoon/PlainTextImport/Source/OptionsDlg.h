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

#if !defined(AFX_OPTIONSDLG_H__D9ADE42C_81F7_476C_B8D9_7B3D5FA2C91E__INCLUDED_)
#define AFX_OPTIONSDLG_H__D9ADE42C_81F7_476C_B8D9_7B3D5FA2C91E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(BOOL bImport, CWnd* pParent = NULL);   // standard constructor

	CString GetIndent() const
	{
		return m_sIndent;
	}
	BOOL GetWantProject() const
	{
		return m_bIncludeProject;
	}

protected:
// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	CComboBox	m_cbIndent;
	CString	m_sLabel;
	int		m_nIndent;
	BOOL	m_bIncludeProject;
	//}}AFX_DATA
	BOOL m_bImport;
	CString		m_sIndent, m_sTitle, m_sProjectLabel;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__D9ADE42C_81F7_476C_B8D9_7B3D5FA2C91E__INCLUDED_)
