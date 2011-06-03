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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ServerDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ServerDlg.h"
#include "../../CodeProject/Source/AfxRegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

CMap<UINT, UINT, CString, CString&> CServerDlg::s_mapText;

const CString INTERNETSETTINGS(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"));

CServerDlg::CServerDlg(LPCTSTR szServer, LPCTSTR szUsername, LPCTSTR szPassword, AL_TYPE nAnonymousLogin):
m_sServer(szServer),
m_sUsername(szUsername),
m_sPassword(szPassword),
m_nAnonLogin(nAnonymousLogin)
{
	BOOL bShowAnonLogin = (m_nAnonLogin != ANONLOGIN_HIDE);
	int LOGINOFFSET = 0;

	AddRCControl(_T("LTEXT"), _T(""), _T("Server:"), 0, 0, 7, 10, 24, 8, IDC_SD_SERVERLABEL);
	AddRCControl(_T("COMBOBOX"), _T(""), _T(""), CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP | CBS_AUTOHSCROLL, 0, 49, 7, 134, 100, IDC_SD_SERVER);

	if (bShowAnonLogin)
	{
		AddRCControl(_T("CONTROL"), _T("Button"), _T("Anonymous Login"), BS_AUTOCHECKBOX | WS_TABSTOP, 0, 49, 29, 134, 10, IDC_SD_ANONLOGIN);
		LOGINOFFSET = 20;
	}

	AddRCControl(_T("LTEXT"), _T(""), _T("Username:"), 0, 0, 7, 29 + LOGINOFFSET, 35, 8, IDC_SD_USERNAMELABEL);
	AddRCControl(_T("COMBOBOX"), _T(""), _T(""), CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP, 0, 49, 27 + LOGINOFFSET, 134, 100, IDC_SD_USERNAME);
	AddRCControl(_T("LTEXT"), _T(""), _T("Password:"), 0, 0, 7, 48 + LOGINOFFSET, 34, 8, IDC_SD_PASSWORDLABEL);
	AddRCControl(_T("EDITTEXT"), _T(""), _T(""), ES_PASSWORD | ES_AUTOHSCROLL | WS_TABSTOP, 0, 49, 45 + LOGINOFFSET, 134, 14, IDC_SD_PASSWORD);

	AddRCControl(_T("LTEXT"), _T(""), _T("Proxy:"), 0, 0, 7, 67 + LOGINOFFSET, 34, 8, IDC_SD_PROXYLABEL);
	AddRCControl(_T("EDITTEXT"), _T(""), _T(""), ES_AUTOHSCROLL | WS_TABSTOP, 0, 49, 65 + LOGINOFFSET, 80, 14, IDC_SD_PROXY);
	AddRCControl(_T("LTEXT"), _T(""), _T("Port:"), 0, 0, 133, 67 + LOGINOFFSET, 20, 8, IDC_SD_PROXYPORTLABEL);
	AddRCControl(_T("EDITTEXT"), _T(""), _T(""), ES_NUMBER | ES_AUTOHSCROLL | WS_TABSTOP, 0, 153, 65 + LOGINOFFSET, 30, 14, IDC_SD_PROXYPORT);

	AddRCControl(_T("CONTROL"), _T("Static"), _T(""), SS_ETCHEDHORZ, 0, 7, 85 + LOGINOFFSET, 176, 1, (UINT)IDC_STATIC);
	AddRCControl(_T("DEFPUSHBUTTON"), _T(""), _T("OK"), WS_TABSTOP, 0, 77, 94 + LOGINOFFSET, 50, 14, IDOK);
	AddRCControl(_T("PUSHBUTTON"), _T(""), _T("Cancel"), WS_TABSTOP, 0, 133, 94 + LOGINOFFSET, 50, 14, IDCANCEL);

	if (m_sServer.IsEmpty())
	{
		m_sServer = AfxGetApp()->GetProfileString(_T("RemoteSettings"), _T("LastServer"));
	}

	if (m_sUsername.IsEmpty())
	{
		m_sUsername = AfxGetApp()->GetProfileString(_T("RemoteSettings"), _T("LastUsername"));
	}

	if (m_nAnonLogin == ANONLOGIN_AUTO)
	{
		m_nAnonLogin = AfxGetApp()->GetProfileInt(_T("RemoteSettings"), _T("LastAnonLogin"), ANONLOGIN_NO) ? ANONLOGIN_YES : ANONLOGIN_NO;
	}

	m_sProxy = AfxGetApp()->GetProfileString(_T("RemoteSettings"), _T("Proxy"));
	m_nProxyPort = AfxGetApp()->GetProfileInt(_T("RemoteSettings"), _T("ProxyPort"), 80);

	// if the proxy settings are blank, try to get them from the registry
	if (m_sProxy.IsEmpty())
	{
		CAfxRegKey reg;

		if (reg.Open(HKEY_CURRENT_USER, INTERNETSETTINGS) == ERROR_SUCCESS)
		{
			// is proxy enabled?
			DWORD dwProxyEnabled = FALSE;

			if (reg.Read(_T("ProxyEnabled"), dwProxyEnabled) == ERROR_SUCCESS && dwProxyEnabled)
			{
				CString sProxy;

				if (reg.Read(_T("ProxyServer"), sProxy) == ERROR_SUCCESS && !sProxy.IsEmpty())
				{
					int nColon = sProxy.Find(_T(':'), 0);

					if (nColon != -1)
					{
						m_sProxy = sProxy.Left(nColon);
						m_nProxyPort = _ttoi(sProxy.Mid(nColon + 1));
					}
					else
					{
						m_sProxy = sProxy;
						m_nProxyPort = 80;
					}
				}
			}
		}
	}
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDlg)
	DDX_CBString(pDX, IDC_SD_SERVER, m_sServer);
	DDX_CBString(pDX, IDC_SD_USERNAME, m_sUsername);
	DDX_Text(pDX, IDC_SD_PASSWORD, m_sPassword);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_SD_PROXY, m_sProxy);
	DDX_Text(pDX, IDC_SD_PROXYPORT, m_nProxyPort);
	DDX_Control(pDX, IDC_SD_SERVER, m_cbServers);
	DDX_Control(pDX, IDC_SD_USERNAME, m_cbUsernames);

	if (m_nAnonLogin >= ANONLOGIN_NO)
	{
		DDX_Check(pDX, IDC_SD_ANONLOGIN, (int&)m_nAnonLogin);
	}

	if (pDX->m_bSaveAndValidate)
	{
		m_sServer.TrimLeft();
		m_sServer.TrimRight();
		m_sUsername.TrimLeft();
		m_sUsername.TrimRight();
		m_sPassword.TrimLeft();
		m_sPassword.TrimRight();
	}
}

BEGIN_MESSAGE_MAP(CServerDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CServerDlg)
	ON_CBN_EDITCHANGE(IDC_SD_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SD_ANONLOGIN, OnAnonLogin)
	ON_EN_CHANGE(IDC_SD_PROXY, OnChangeProxy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg message handlers

void CServerDlg::OnChangeServer()
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(!m_sServer.IsEmpty());
}

void CServerDlg::OnChangeProxy()
{
	UpdateData();

	GetDlgItem(IDC_SD_PROXYPORTLABEL)->EnableWindow(!m_sProxy.IsEmpty());
	GetDlgItem(IDC_SD_PROXYPORT)->EnableWindow(!m_sProxy.IsEmpty());
}

BOOL CServerDlg::OnInitDialog()
{
	CRuntimeDlg::OnInitDialog();

	// popuplate comboboxes from registry
	int nServer = AfxGetApp()->GetProfileInt(_T("RemoteSettings"), _T("ServerCount"), 0);

	while (nServer--)
	{
		CString sServer, sItem;

		sItem.Format(_T("Server%d"), nServer);
		sServer = AfxGetApp()->GetProfileString(_T("RemoteSettings"), sItem);

		if (!sServer.IsEmpty() && m_cbServers.FindString(-1, sServer) == CB_ERR)
		{
			m_cbServers.InsertString(0, sServer);
		}
	}

	// add m_sServer as appropriate and select
	if (!m_sServer.IsEmpty() && m_cbServers.FindString(-1, m_sServer) == CB_ERR)
	{
		m_cbServers.InsertString(0, m_sServer);
	}

	m_cbServers.SelectString(-1, m_sServer);

	int nName = AfxGetApp()->GetProfileInt(_T("RemoteSettings"), _T("UsernameCount"), 0);

	while (nName--)
	{
		CString sName, sItem;

		sItem.Format(_T("Username%d"), nName);
		sName = AfxGetApp()->GetProfileString(_T("RemoteSettings"), sItem);

		if (!sName.IsEmpty() && m_cbUsernames.FindString(-1, sName) == CB_ERR)
		{
			m_cbUsernames.InsertString(0, sName);
		}
	}

	// add m_sUsername as appropriate and select
	if (!m_sUsername.IsEmpty() && m_cbUsernames.FindString(-1, m_sUsername) == CB_ERR)
	{
		m_cbUsernames.InsertString(0, m_sUsername);
	}

	m_cbUsernames.SelectString(-1, m_sUsername);

	OnChangeProxy();
	OnChangeServer();
	OnAnonLogin();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CServerDlg::OnOK()
{
	CRuntimeDlg::OnOK();

	// save server list to registry and last selected item
	int nServer = m_cbServers.GetCount();

	AfxGetApp()->WriteProfileInt(_T("RemoteSettings"), _T("ServerCount"), nServer);

	while (nServer--)
	{
		CString sServer, sItem;

		sItem.Format(_T("Server%d"), nServer);
		m_cbServers.GetLBText(nServer, sServer);

		AfxGetApp()->WriteProfileString(_T("RemoteSettings"), sItem, sServer);
	}

	AfxGetApp()->WriteProfileString(_T("RemoteSettings"), _T("LastServer"), m_sServer);

	// save username list to registry and last selected item
	int nName = m_cbUsernames.GetCount();

	AfxGetApp()->WriteProfileInt(_T("RemoteSettings"), _T("UsernameCount"), nName);

	while (nName--)
	{
		CString sName, sItem;

		sItem.Format(_T("Username%d"), nName);
		m_cbUsernames.GetLBText(nName, sName);

		if (!sName.IsEmpty())
		{
			AfxGetApp()->WriteProfileString(_T("RemoteSettings"), sItem, sName);
		}
	}

	AfxGetApp()->WriteProfileString(_T("RemoteSettings"), _T("LastUsername"), m_sUsername);
	AfxGetApp()->WriteProfileInt(_T("RemoteSettings"), _T("LastAnonLogin"), m_nAnonLogin);
}

void CServerDlg::OnAnonLogin()
{
	UpdateData();

	if (m_nAnonLogin != ANONLOGIN_HIDE)
	{
		GetDlgItem(IDC_SD_USERNAME)->EnableWindow(m_nAnonLogin == ANONLOGIN_NO);
		GetDlgItem(IDC_SD_PASSWORD)->EnableWindow(m_nAnonLogin == ANONLOGIN_NO);
	}
}

CString CServerDlg::GetItemText(UINT nIDItem, LPCTSTR szDefault)
{
	CString sText;
	s_mapText.Lookup(nIDItem, sText);

	if (sText.IsEmpty() && szDefault)
	{
		return szDefault;
	}

	return sText;
}

void CServerDlg::SetItemText(UINT nIDItem, UINT nIDText)
{
	CString sText;
	sText.LoadString(nIDText);
	s_mapText[nIDItem] = sText;
}
