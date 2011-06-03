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
//*****************************************************************************

#if !defined(AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_)
#define AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLSendTasksDlg.h : header file
//

enum TD_SENDWHAT
{
	TDSW_ACTIVETASKLIST,
	TDSW_SELECTEDTASKS,
};

enum TD_SENDAS
{
	TDSA_TASKLIST,
	TDSA_BODYTEXT,
};

/////////////////////////////////////////////////////////////////////////////
// CTDLSendTasksDlg dialog

class CTDLSendTasksDlg : public CDialog
{
// Construction
public:
	CTDLSendTasksDlg(TD_SENDWHAT nWhat = TDSW_ACTIVETASKLIST, TD_SENDAS nSendAs = TDSA_TASKLIST, CWnd* pParent = NULL);   // standard constructor

	void GetSendOptions(TD_SENDWHAT& nWhat, TD_SENDAS& nSendAs) const
	{
		nWhat = (TD_SENDWHAT)m_nSendWhat;
		nSendAs = (m_nSendWhat == TDSW_ACTIVETASKLIST) ? TDSA_TASKLIST : (TD_SENDAS)m_nSendTasksAsOption;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CTDLSendTasksDlg)
	enum { IDD = IDD_SENDTASKS_DIALOG };
	int		m_nSendWhat;
	int		m_nSendTasksAsOption;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLSendTasksDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLSendTasksDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSendWhat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLSENDTASKSDLG_H__02DB12DD_A125_4754_AE1C_F8BB9EE45903__INCLUDED_)
