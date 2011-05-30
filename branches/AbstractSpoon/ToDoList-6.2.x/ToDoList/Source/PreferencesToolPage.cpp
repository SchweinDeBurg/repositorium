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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// PreferencesToolPage.cpp : implementation file
//

#include "StdAfx.h"
#include "Resource.h"
#include "PreferencesToolPage.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/Ini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesToolPage property page

const TCHAR* REALQUOTE = _T("\"");
const TCHAR* SAFEQUOTE = _T("{QUOTES}");

IMPLEMENT_DYNCREATE(CPreferencesToolPage, CPreferencesPageBase)

CPreferencesToolPage::CPreferencesToolPage() : CPreferencesPageBase(CPreferencesToolPage::IDD),
m_eToolPath(FES_COMBOSTYLEBTN | FES_ALLOWURL),
m_eIconPath(FES_COMBOSTYLEBTN)
{
	//{{AFX_DATA_INIT(CPreferencesToolPage)
	m_sToolPath = _T("");
	m_sCommandLine = _T("");
	m_bRunMinimized = FALSE;
	m_sIconPath = _T("");
	//}}AFX_DATA_INIT

	m_eCmdLine.AddButton(1, _T(""), CEnString(IDS_PTP_PLACEHOLDERS));
	m_eCmdLine.SetDropMenuButton(1);
}

CPreferencesToolPage::~CPreferencesToolPage()
{
}

void CPreferencesToolPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesToolPage)
	DDX_Control(pDX, IDC_CMDLINE, m_eCmdLine);
	DDX_Control(pDX, IDC_TOOLPATH, m_eToolPath);
	DDX_Control(pDX, IDC_TOOLLIST, m_lcTools);
	DDX_Text(pDX, IDC_TOOLPATH, m_sToolPath);
	DDX_Text(pDX, IDC_CMDLINE, m_sCommandLine);
	DDX_Check(pDX, IDC_RUNMINIMIZED, m_bRunMinimized);
	DDX_Text(pDX, IDC_ICONPATH, m_sIconPath);
	DDX_Control(pDX, IDC_ICONPATH, m_eIconPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesToolPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesToolPage)
	ON_BN_CLICKED(IDC_NEWTOOL, OnNewtool)
	ON_BN_CLICKED(IDC_DELETETOOL, OnDeletetool)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TOOLLIST, OnEndlabeleditToollist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TOOLLIST, OnItemchangedToollist)
	ON_EN_CHANGE(IDC_TOOLPATH, OnChangeToolpath)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TOOLLIST, OnKeydownToollist)
	ON_EN_CHANGE(IDC_CMDLINE, OnChangeCmdline)
	ON_COMMAND_RANGE(ID_TOOLS_PATHNAME, ID_TOOLS_SELTASKFILELINK, OnInsertPlaceholder)
	ON_BN_CLICKED(IDC_RUNMINIMIZED, OnRunminimized)
	ON_BN_CLICKED(IDC_TESTTOOL, OnTesttool)
	ON_EN_CHANGE(IDC_ICONPATH, OnChangeIconPath)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesToolPage message handlers

BOOL CPreferencesToolPage::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	m_ilSys.Initialize();
	m_lcTools.SetImageList(m_ilSys.GetImageList(), LVSIL_SMALL);

	CRect rList;
	m_lcTools.GetClientRect(rList);

	m_lcTools.InsertColumn(0, CEnString(IDS_PTP_TOOLNAME), LVCFMT_LEFT, 150);
	m_lcTools.InsertColumn(1, CEnString(IDS_PTP_TOOLPATH), LVCFMT_LEFT, 250);
	m_lcTools.InsertColumn(2, CEnString(IDS_PTP_ARGUMENTS), LVCFMT_LEFT, rList.Width() - 400);
	m_lcTools.InsertColumn(3, CEnString(IDS_PTP_ICONPATH), LVCFMT_LEFT, 0);

	m_lcTools.SetExtendedStyle(m_lcTools.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// add tools we loaded from the registry
	for (int nTool = 0; nTool < m_aTools.GetSize(); nTool++)
	{
		CString sToolPath = m_aTools[nTool].sToolPath;
		int nImage = m_ilSys.GetFileImageIndex(sToolPath);

		if (!m_aTools[nTool].sIconPath.IsEmpty())
		{
			nImage = m_ilSys.GetFileImageIndex(m_aTools[nTool].sIconPath);
		}

		int nIndex = m_lcTools.InsertItem(nTool, m_aTools[nTool].sToolName, nImage);

		m_lcTools.SetItemText(nIndex, 1, sToolPath);
		m_lcTools.SetItemText(nIndex, 2, m_aTools[nTool].sCmdline);
		m_lcTools.SetItemText(nIndex, 3, m_aTools[nTool].sIconPath);
		m_lcTools.SetItemData(nIndex, m_aTools[nTool].bRunMinimized);
	}

	m_lcTools.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	OnItemchangedToollist(NULL, NULL);

	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesToolPage::OnNewtool()
{
	int nIndex = m_lcTools.InsertItem(m_lcTools.GetItemCount(), CEnString(IDS_PTP_NEWTOOL), -1);
	m_lcTools.SetItemText(nIndex, 2, CmdIDToPlaceholder(ID_TOOLS_PATHNAME));

	m_lcTools.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_lcTools.SetFocus();
	m_lcTools.EditLabel(nIndex);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesToolPage::OnDeletetool()
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		m_lcTools.DeleteItem(nSel);
		m_sToolPath.Empty();
		m_sIconPath.Empty();

		EnableControls();
		UpdateData(FALSE);
	}

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesToolPage::OnEndlabeleditToollist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.pszText)
	{
		int nSel = GetCurSel();

		if (nSel >= 0)
		{
			m_lcTools.SetItemText(nSel, 0, pDispInfo->item.pszText);

			GetDlgItem(IDC_TOOLPATH)->SetFocus();
		}
	}

	*pResult = 0;

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesToolPage::OnItemchangedToollist(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	EnableControls();
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		m_sToolPath = m_lcTools.GetItemText(nSel, 1);
		m_sCommandLine = m_lcTools.GetItemText(nSel, 2);
		m_sIconPath = m_lcTools.GetItemText(nSel, 3);
		m_bRunMinimized = m_lcTools.GetItemData(nSel);
	}
	else
	{
		m_sToolPath.Empty();
		m_sCommandLine.Empty();
		m_bRunMinimized = FALSE;
		m_sIconPath.Empty();
	}

	UpdateData(FALSE);
}

void CPreferencesToolPage::EnableControls()
{
	int nSel = GetCurSel();

	GetDlgItem(IDC_NEWTOOL)->EnableWindow(m_lcTools.GetItemCount() < 16);
	GetDlgItem(IDC_DELETETOOL)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_TOOLPATH)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_ICONPATH)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_CMDLINE)->EnableWindow(nSel >= 0);
	GetDlgItem(IDC_RUNMINIMIZED)->EnableWindow(nSel >= 0);

	m_eCmdLine.EnableButton(1, nSel >= 0);
}

int CPreferencesToolPage::GetCurSel()
{
	int nSel = -1;
	POSITION pos = m_lcTools.GetFirstSelectedItemPosition();

	if (pos)
	{
		nSel = m_lcTools.GetNextSelectedItem(pos);
	}

	return nSel;
}

void CPreferencesToolPage::OnChangeToolpath()
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 1, m_sToolPath);

		// update the image
		LVITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nSel;
		if (m_sIconPath.IsEmpty())
		{
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sToolPath);
		}
		else
		{
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sIconPath);
		}

		m_lcTools.SetItem(&lvi);
	}
	else
	{
		ASSERT(0);
	}

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesToolPage::OnChangeIconPath()
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 3, m_sIconPath);

		// update the image
		LVITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = nSel;

		if (m_sIconPath.IsEmpty())
		{
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sToolPath);
		}
		else
		{
			lvi.iImage = m_ilSys.GetFileImageIndex(m_sIconPath);
		}

		m_lcTools.SetItem(&lvi);
	}
	else
	{
		ASSERT(0);
	}

	CPreferencesPageBase::OnControlChange();
}

int CPreferencesToolPage::GetUserTools(CUserToolArray& aTools) const
{
	aTools.Copy(m_aTools);

	return aTools.GetSize();
}

BOOL CPreferencesToolPage::GetUserTool(int nTool, USERTOOL& tool) const
{
	if (nTool >= 0 && nTool < m_aTools.GetSize())
	{
		tool = m_aTools[nTool];
		return TRUE;
	}

	return FALSE;
}

void CPreferencesToolPage::OnOK()
{
	CPreferencesPageBase::OnOK();

	// save tools to m_aTools
	m_aTools.RemoveAll();
	int nToolCount = m_lcTools.GetItemCount();

	for (int nTool = 0; nTool < nToolCount; nTool++)
	{
		USERTOOL ut;

		ut.sToolName = m_lcTools.GetItemText(nTool, 0);
		ut.sToolPath = m_lcTools.GetItemText(nTool, 1);
		ut.sCmdline = m_lcTools.GetItemText(nTool, 2);
		ut.sIconPath = m_lcTools.GetItemText(nTool, 3);
		ut.bRunMinimized = m_lcTools.GetItemData(nTool);

		// GetPrivateProfileString strips a leading/trailing quote pairs if
		// it finds them so we replace quotes with safe quotes
		ut.sCmdline.Replace(REALQUOTE, SAFEQUOTE);

		m_aTools.Add(ut);
	}
}

void CPreferencesToolPage::OnKeydownToollist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDown->wVKey)
	{
	case VK_DELETE:
		OnDeletetool();
		break;

	case VK_F2:
		{
			int nSel = GetCurSel();

			if (nSel >= 0)
			{
				m_lcTools.EditLabel(nSel);
			}
		}
		break;
	}

	*pResult = 0;
}

void CPreferencesToolPage::OnChangeCmdline()
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemText(nSel, 2, m_sCommandLine);

		CPreferencesPageBase::OnControlChange();
	}
}

void CPreferencesToolPage::OnInsertPlaceholder(UINT nCmdID)
{
	m_eCmdLine.ReplaceSel(CmdIDToPlaceholder(nCmdID), TRUE);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesToolPage::OnRunminimized()
{
	int nSel = GetCurSel();

	if (nSel >= 0)
	{
		UpdateData();

		m_lcTools.SetItemData(nSel, m_bRunMinimized);

		CPreferencesPageBase::OnControlChange();
	}
}

LRESULT CPreferencesToolPage::OnEEBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDC_CMDLINE:
		if (lParam == 1)
		{
			CMenu menu, *pSubMenu;

			if (menu.LoadMenu(IDR_PLACEHOLDERS))
			{
				pSubMenu = menu.GetSubMenu(0);

				if (pSubMenu)
				{
					CRect rButton = m_eCmdLine.GetButtonRect((UINT)1);

					TPMPARAMS tpmp;
					tpmp.cbSize = sizeof(TPMPARAMS);
					tpmp.rcExclude = rButton;

					::TrackPopupMenuEx(*pSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON,
						rButton.right, rButton.top, *this, &tpmp);
				}
			}
		}
	}

	return 0L;
}

void CPreferencesToolPage::OnTesttool()
{
	int nTool = GetCurSel();

	if (nTool != -1)
	{
		USERTOOL ut;

		ut.sToolName = m_lcTools.GetItemText(nTool, 0);
		ut.sToolPath = m_lcTools.GetItemText(nTool, 1);
		ut.sCmdline = m_lcTools.GetItemText(nTool, 2);
		ut.bRunMinimized = m_lcTools.GetItemData(nTool);

		GetParent()->SendMessage(WM_PTP_TESTTOOL, 0, (LPARAM)&ut);
	}
}

CString CPreferencesToolPage::CmdIDToPlaceholder(UINT nCmdID)
{
	switch (nCmdID)
	{
	case ID_TOOLS_PATHNAME:
		return _T("\"$(pathName)\"");
	case ID_TOOLS_FILETITLE:
		return _T("$(fileTitle)");
	case ID_TOOLS_FOLDER:
		return _T("$(folder)");
	case ID_TOOLS_FILENAME:
		return _T("$(fileName)");
	case ID_TOOLS_SELTASKID:
		return _T("$(selTID)");
	case ID_TOOLS_SELTASKTITLE:
		return _T("\"$(selTTitle)\"");
	case ID_TOOLS_USERDATE:
		return _T("$(userDate, var_date1, \"Date Prompt\", default_date)");
	case ID_TOOLS_USERFILEPATH:
		return _T("\"$(userFile, var_file1, \"File Prompt\", default_path)\"");
	case ID_TOOLS_USERFOLDER:
		return _T("$(userFolder, var_folder1, \"Folder Prompt\", default_folder)");
	case ID_TOOLS_USERTEXT:
		return _T("$(userText, var_text1, \"Text Prompt\", default_text)");
	case ID_TOOLS_TODAYSDATE:
		return _T("$(todaysDate)");
	case ID_TOOLS_TODOLIST:
		return _T("\"$(todolist)\"");
	case ID_TOOLS_SELTASKEXTID:
		return _T("\"$(selTExtID)\"");
	case ID_TOOLS_SELTASKCOMMENTS:
		return _T("\"$(selTComments)\"");
	case ID_TOOLS_SELTASKFILELINK:
		return _T("\"$(selTFile)\"");
	case ID_TOOLS_SELTASKALLOCBY:
		return _T("\"$(selTAllocBy)\"");
	case ID_TOOLS_SELTASKALLOCTO:
		return _T("\"$(selTAllocTo)\"");
	}

	return _T("");
}

void CPreferencesToolPage::OnImport()
{
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		CFileDialog dialog(TRUE, _T("ini"), NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
			CEnString(IDS_INIFILEFILTER));

		if (dialog.DoModal() == IDOK)
		{
			CIni ini(dialog.GetPathName());

			int nTools = ini.GetInt(_T("Tools"), _T("ToolCount"), 0);

			if (!nTools)
			{
				bContinue = (AfxMessageBox(IDS_INIHASNOTOOLS, MB_YESNO) == IDYES);
			}
			else
			{
				int nCurCount = m_lcTools.GetItemCount();

				for (int nTool = 0; nTool < nTools; nTool++)
				{
					CString sKey;
					sKey.Format(_T("Tools\\Tool%d"), nTool + 1);

					CString sName = ini.GetString(sKey, _T("Name"));
					CString sPath = ini.GetString(sKey, _T("Path"));
					CString sIconPath = ini.GetString(sKey, _T("IconPath"));
					BOOL bRunMinimized = ini.GetBool(sKey, _T("RunMinimized"), FALSE);
					CString sCmdline = ini.GetString(sKey, _T("Cmdline"));

					// replace safe quotes with real quotes
					sCmdline.Replace(SAFEQUOTE, REALQUOTE);

					// add tool to list
					int nImage = m_ilSys.GetFileImageIndex(sPath);

					if (!sIconPath.IsEmpty())
					{
						nImage = m_ilSys.GetFileImageIndex(sIconPath);
					}

					int nIndex = m_lcTools.InsertItem(nCurCount + nTool, sName, nImage);

					m_lcTools.SetItemText(nIndex, 1, sPath);
					m_lcTools.SetItemText(nIndex, 2, sCmdline);
					m_lcTools.SetItemText(nIndex, 3, sIconPath);
					m_lcTools.SetItemData(nIndex, bRunMinimized);
				}

				bContinue = FALSE;

				CPreferencesPageBase::OnControlChange();
			}
		}
		else
		{
			bContinue = FALSE;   // cancelled
		}
	}
}

void CPreferencesToolPage::LoadPreferences(const CPreferences& prefs)
{
	// load tools
	int nToolCount = prefs.GetProfileInt(_T("Tools"), _T("ToolCount"), 0);

	for (int nTool = 1; nTool <= nToolCount; nTool++)
	{
		CString sKey;
		sKey.Format(_T("Tools\\Tool%d"), nTool);

		USERTOOL ut;
		ut.sToolName = prefs.GetProfileString(sKey, _T("Name"), _T(""));
		ut.sToolPath = prefs.GetProfileString(sKey, _T("Path"), _T(""));
		ut.sCmdline = prefs.GetProfileString(sKey, _T("CmdLine"), _T(""));
		ut.bRunMinimized = prefs.GetProfileInt(sKey, _T("RunMinimized"), FALSE);
		ut.sIconPath = prefs.GetProfileString(sKey, _T("IconPath"), _T(""));

		// replace safe quotes with real quotes
		ut.sCmdline.Replace(SAFEQUOTE, REALQUOTE);

		m_aTools.Add(ut);
	}
}

void CPreferencesToolPage::SavePreferences(CPreferences& prefs)
{
	// save tools to registry and m_aTools
	int nToolCount = m_aTools.GetSize();

	for (int nTool = 0; nTool < nToolCount; nTool++)
	{
		USERTOOL ut = m_aTools[nTool];

		CString sKey;
		sKey.Format(_T("Tools\\Tool%d"), nTool + 1);

		prefs.WriteProfileString(sKey, _T("Name"), ut.sToolName);
		prefs.WriteProfileString(sKey, _T("Path"), ut.sToolPath);
		prefs.WriteProfileString(sKey, _T("IconPath"), ut.sIconPath);
		prefs.WriteProfileInt(sKey, _T("RunMinimized"), ut.bRunMinimized);

		// GetPrivateProfileString strips a leading/trailing quote pairs if
		// it finds them so we replace quotes with safe quotes
		ut.sCmdline.Replace(REALQUOTE, SAFEQUOTE);
		prefs.WriteProfileString(sKey, _T("Cmdline"), ut.sCmdline);
	}

	prefs.WriteProfileInt(_T("Tools"), _T("ToolCount"), nToolCount);
}
