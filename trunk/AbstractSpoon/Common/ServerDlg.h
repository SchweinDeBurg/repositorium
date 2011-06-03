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

#if !defined(AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_)
#define AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerDlg.h : header file
//

#include "../../CodeProject/Source/RuntimeDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

enum
{
	SD_TITLE = 0,
	// don't put anything here else it will clash with IDOK, etc

	IDC_SD_SERVER = 1001,
	IDC_SD_SERVERLABEL,
	IDC_SD_USERNAME,
	IDC_SD_USERNAMELABEL,
	IDC_SD_PASSWORD,
	IDC_SD_PASSWORDLABEL,
	IDC_SD_ANONLOGIN,
	IDC_SD_PROXY,
	IDC_SD_PROXYLABEL,
	IDC_SD_PROXYPORT,
	IDC_SD_PROXYPORTLABEL,
};

enum AL_TYPE
{
	ANONLOGIN_HIDE  = -2,
	ANONLOGIN_AUTO  = -1,
	ANONLOGIN_NO    = 0,
	ANONLOGIN_YES   = 1,
};

class CServerDlg : public CRuntimeDlg
{
// Construction
public:
	CServerDlg(LPCTSTR szServer = NULL, LPCTSTR szUsername = NULL,
		LPCTSTR szPassword = NULL, AL_TYPE nAnonymousLogin = ANONLOGIN_AUTO);   // standard constructor

	CString GetServer() const
	{
		return m_sServer;
	}
	CString GetUsername() const
	{
		return m_sUsername;
	}
	CString GetPassword() const
	{
		return m_sPassword;
	}
	BOOL GetAnonynousLogin() const
	{
		return (m_nAnonLogin == ANONLOGIN_YES);
	}
	CString GetProxy(UINT& nPort) const
	{
		nPort = m_sProxy.IsEmpty() ? 0 : m_nProxyPort;
		return m_sProxy;
	}

	int DoModal()
	{
		return CRuntimeDlg::DoModal(_T("Enter Remote Server Details"));
	}

	// for overiding the default text for translating
	static void SetItemText(UINT nIDItem, UINT nIDText);

protected:
// Dialog Data
	//{{AFX_DATA(CServerDlg)
	CString m_sServer;
	CString m_sUsername;
	CString m_sPassword;
	//}}AFX_DATA
	CString m_sProxy;
	UINT m_nProxyPort;
	CComboBox m_cbServers;
	CComboBox m_cbUsernames;
	AL_TYPE m_nAnonLogin;

	static CMap<UINT, UINT, CString, CString&> s_mapText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerDlg)
	afx_msg void OnChangeServer();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnAnonLogin();
	afx_msg void OnChangeProxy();
	DECLARE_MESSAGE_MAP()

	CString GetItemText(UINT nIDItem, LPCTSTR szDefault);
	virtual CString OverrideItemText(UINT nIDItem)
	{
		return GetItemText(nIDItem, NULL);
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_)
