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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// ExportDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLExportDlg.h"

#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../Common/Preferences.h"

#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

enum { ACTIVETASKLIST, ALLTASKLISTS };

CTDLExportDlg::CTDLExportDlg(const CImportExportMgr& mgr, BOOL bSingleTaskList, BOOL bShowSubtaskCheckbox,
		BOOL bVisibleColumnsOnly, LPCTSTR szFilePath, LPCTSTR szFolderPath, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EXPORT_DIALOG, pParent),
	  m_mgrImportExport(mgr),
	  m_bSingleTaskList(bSingleTaskList),
	  m_sFilePath(szFilePath), m_sOrgFilePath(szFilePath),
	  m_sFolderPath(szFolderPath), m_sOrgFolderPath(szFolderPath),
	  m_taskSel(_T("Exporting"), bShowSubtaskCheckbox, bVisibleColumnsOnly),
	  m_eExportPath(FES_COMBOSTYLEBTN | FES_SAVEAS),
	  m_nFormatOption(0)
{
	CPreferences prefs;

	//{{AFX_DATA_INIT(CExportDlg)
	m_sPathLabel = _T("");
	//}}AFX_DATA_INIT
	m_bExportOneFile = FALSE;

	if (m_bSingleTaskList)
	{
		m_nExportOption = ACTIVETASKLIST;
	}
	else
	{
		m_nExportOption = prefs.GetProfileInt(_T("Exporting"), _T("ExportOption"), ACTIVETASKLIST);
	}

	m_nFormatOption = prefs.GetProfileInt(_T("Exporting"), _T("FormatOption"), 0);

	if (m_nFormatOption >= mgr.GetNumExporters())
	{
		m_nFormatOption = 0;
	}

	if (m_sFolderPath.IsEmpty())
	{
		m_sFolderPath = prefs.GetProfileString(_T("Exporting"), _T("LastFolder"));
	}

	if (m_bSingleTaskList || m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
	{
		m_sExportPath = m_sFilePath; // default

		if ((m_sFilePath.IsEmpty() || PathIsRelative(m_sFilePath)) &&
				!m_sFolderPath.IsEmpty())
		{
			CString sFName;
			FileMisc::SplitPath(m_sFilePath, NULL, NULL, &sFName);

			// handle empty filename
			if (sFName.IsEmpty())
			{
				sFName.LoadString(IDS_TDC_UNTITLEDFILE);
			}

			FileMisc::MakePath(m_sExportPath, NULL, m_sFolderPath, sFName);
		}

		ReplaceExtension(m_sExportPath, m_nFormatOption);
		m_sPathLabel.LoadString(IDS_ED_FILEPATH);
	}
	else // multiple files
	{
		if (!m_sFolderPath.IsEmpty())
		{
			m_sExportPath = m_sFolderPath;
		}

		else if (!m_sFilePath.IsEmpty())
		{
			CString sDrive, sPath;

			FileMisc::SplitPath(m_sFilePath, &sDrive, &sPath);
			FileMisc::MakePath(m_sExportPath, sDrive, sPath);
		}

		m_sPathLabel.LoadString(IDS_ED_FOLDER);
	}
}


void CTDLExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Control(pDX, IDC_FORMATOPTIONS, m_cbFormat);
	DDX_Control(pDX, IDC_EXPORTPATH, m_eExportPath);
	DDX_CBIndex(pDX, IDC_TASKLISTOPTIONS, m_nExportOption);
	DDX_Text(pDX, IDC_EXPORTPATH, m_sExportPath);
	DDX_Check(pDX, IDC_EXPORTONEFILE, m_bExportOneFile);
	DDX_Text(pDX, IDC_PATHLABEL, m_sPathLabel);
	DDX_CBIndex(pDX, IDC_FORMATOPTIONS, m_nFormatOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTDLExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_CBN_SELCHANGE(IDC_TASKLISTOPTIONS, OnSelchangeTasklistoptions)
	ON_CBN_SELCHANGE(IDC_FORMATOPTIONS, OnSelchangeFormatoptions)
	ON_BN_CLICKED(IDC_EXPORTONEFILE, OnExportonefile)
	ON_EN_CHANGE(IDC_EXPORTPATH, OnChangeExportpath)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_TASKSELDLG_CHANGE, OnChangeTaskSelOption)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers

BOOL CTDLExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	VERIFY(m_taskSel.Create(IDC_FRAME, this));

	// set initial control states
	GetDlgItem(IDC_TASKLISTOPTIONS)->EnableWindow(!m_bSingleTaskList);
	GetDlgItem(IDC_EXPORTONEFILE)->EnableWindow(!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);

	m_eExportPath.SetFilter(m_mgrImportExport.GetExporterFileFilter(m_nFormatOption));
	m_eExportPath.EnableStyle(FES_FOLDERS, (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile));

	// build the format comboxbox
	for (int nExp = 0; nExp < m_mgrImportExport.GetNumExporters(); nExp++)
	{
		CString sExporter = m_mgrImportExport.GetExporterMenuText(nExp);
		CString sFileExt = m_mgrImportExport.GetExporterFileExtension(nExp);

		CString sFormat;

		if (sFileExt.IsEmpty())
		{
			sFormat = sExporter;
		}
		else
		{
			sFormat.Format(_T("%s (*.%s)"), sExporter, sFileExt);
		}

		m_cbFormat.AddString(sFormat);
	}

	m_cbFormat.SetCurSel(m_nFormatOption);
	m_eExportPath.EnableWindow(m_mgrImportExport.ExporterHasFileExtension(m_nFormatOption));

	EnableOK();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLExportDlg::OnSelchangeTasklistoptions()
{
	UpdateData();

	// can't do all tasklists and selected item
	if (m_taskSel.GetWantSelectedTasks() && m_nExportOption == ALLTASKLISTS)
	{
		m_taskSel.SetWantWhatTasks(TSDT_ALL);
	}

	// only process this notification if something's _really_ changed
	if (!(m_nExportOption == ALLTASKLISTS && m_bExportOneFile))
	{
		BOOL bFolder = (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile);

		m_eExportPath.EnableStyle(FES_FOLDERS, bFolder);
		m_sPathLabel.LoadString(bFolder ? IDS_ED_FOLDER : IDS_ED_FILEPATH);

		// check file extension is correct
		if (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile)
		{
			m_sExportPath = m_sFolderPath;
		}
		else
		{
			m_sExportPath = m_sFilePath;
			ReplaceExtension(m_sExportPath, m_nFormatOption);
		}
	}

	GetDlgItem(IDC_EXPORTONEFILE)->EnableWindow(!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);

	UpdateData(FALSE);
}

void CTDLExportDlg::OnSelchangeFormatoptions()
{
	UpdateData();

	// check exporter has a file extension
	if (m_mgrImportExport.ExporterHasFileExtension(m_nFormatOption))
	{
		// enable path edit
		m_eExportPath.EnableWindow(TRUE);

		// check file extension is correct
		m_eExportPath.SetFilter(m_mgrImportExport.GetExporterFileFilter(m_nFormatOption));

		if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
		{
			if (m_sExportPath.IsEmpty())
			{
				m_sExportPath = m_sOrgFilePath;
			}

			ReplaceExtension(m_sExportPath, m_nFormatOption);
			UpdateData(FALSE);
		}
		else if (m_sExportPath.IsEmpty())
		{
			m_sExportPath = m_sOrgFolderPath;
		}
	}
	else // disable path edit and remove file path
	{
		m_eExportPath.SetWindowText(_T(""));
		m_eExportPath.EnableWindow(FALSE);
	}
}

void CTDLExportDlg::ReplaceExtension(CString& sPathName, int nFormat)
{
	if (!m_mgrImportExport.ExporterHasFileExtension(nFormat))
	{
		return;
	}


	CString sExt = m_mgrImportExport.GetExporterFileExtension(nFormat);

	sPathName.TrimRight();

	int nFind = sPathName.Find(sExt);

	if (nFind != sPathName.GetLength() - sExt.GetLength())
	{
		CString sDrive, sFolder, sFName;

		FileMisc::SplitPath(sPathName, &sDrive, &sFolder, &sFName);
		FileMisc::MakePath(sPathName, sDrive, sFolder, sFName, sExt);
	}
}

void CTDLExportDlg::OnOK()
{
	BOOL bExporterHasFileExt = m_mgrImportExport.ExporterHasFileExtension(m_nFormatOption);

	if (bExporterHasFileExt)
	{
		m_sExportPath.TrimLeft();
		m_sExportPath.TrimRight();

		// if the export path is relative we build a path based
		// on the exe path and check with the user
		if (::PathIsRelative(m_sExportPath))
		{
			CString sPath = FileMisc::GetAppFileName(), sDrive, sFolder;

			FileMisc::SplitPath(sPath, &sDrive, &sFolder);
			FileMisc::MakePath(sPath, sDrive, sFolder, m_sExportPath);

			CString sMessage;

			if (m_nExportOption == ALLTASKLISTS)
			{
				sMessage.Format(IDS_ED_CONFIRMEXPORTPATHMULTI, sPath);
			}
			else
			{
				sMessage.Format(IDS_ED_CONFIRMEXPORTPATH, sPath);
			}

			UINT nRet = MessageBox(sMessage, CEnString(IDS_ED_CONFIRMEXPORTPATH_TITLE), MB_YESNO);

			if (nRet == IDNO)
			{
				// re-display dialog
				m_eExportPath.SetSel(0, -1);
				m_eExportPath.SetFocus();
				return;
			}
			else
			{
				m_sExportPath = sPath;
			}
		}

		// make sure the output folder is valid
		BOOL bBadFolder = (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile) ?
				!FileMisc::CreateFolder(m_sExportPath) :
				!FileMisc::CreateFolderFromFilePath(m_sExportPath);

		if (bBadFolder)
		{
			CEnString sMessage(IDS_ED_NOMAKEEXPORTPATH, m_sExportPath);

			UINT nRet = MessageBox(sMessage, CEnString(IDS_ED_NOMAKEEXPORTPATH_TITLE), MB_OKCANCEL);

			// re-display dialog
			if (nRet == IDOK)
			{
				m_eExportPath.SetSel(0, -1);
				m_eExportPath.SetFocus();
				return;
			}
			else
			{
				EndDialog(IDCANCEL);
				return;
			}
		}
	}

	CDialog::OnOK();

	// make sure extension is right
	if (bExporterHasFileExt)
	{
		if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
		{
			ReplaceExtension(m_sExportPath, m_nFormatOption);
		}
	}

	CPreferences prefs;
	prefs.WriteProfileInt(_T("Exporting"), _T("FormatOption"), m_nFormatOption);

	if (!m_bSingleTaskList)
	{
		prefs.WriteProfileInt(_T("Exporting"), _T("ExportOption"), m_nExportOption);

		if (bExporterHasFileExt)
		{
			if (m_nExportOption == ALLTASKLISTS)
			{
				prefs.WriteProfileString(_T("Exporting"), _T("LastFolder"), m_sExportPath);
			}
			else
			{
				prefs.WriteProfileString(_T("Exporting"), _T("LastFolder"), m_sFolderPath);
			}
		}
	}
}

BOOL CTDLExportDlg::GetExportAllTasklists()
{
	return (!m_bSingleTaskList && m_nExportOption == ALLTASKLISTS);
}

void CTDLExportDlg::OnExportonefile()
{
	UpdateData();

	BOOL bFolder = (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile);

	m_eExportPath.EnableStyle(FES_FOLDERS, bFolder);
	m_sPathLabel.LoadString(bFolder ? IDS_ED_FOLDER : IDS_ED_FILEPATH);

	if (m_nExportOption == ALLTASKLISTS && !m_bExportOneFile)
	{
		m_sExportPath = m_sFolderPath;
	}
	else
	{
		m_sExportPath = m_sFilePath;
	}

	if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
	{
		ReplaceExtension(m_sExportPath, m_nFormatOption);
	}

	UpdateData(FALSE);
}

void CTDLExportDlg::EnableOK()
{
	m_sExportPath.TrimLeft();
	m_sExportPath.TrimRight();

	GetDlgItem(IDOK)->EnableWindow(!m_sExportPath.IsEmpty());
}

void CTDLExportDlg::OnChangeExportpath()
{
	UpdateData();
	EnableOK();

	if (m_nExportOption == ACTIVETASKLIST || m_bExportOneFile)
	{
		m_sFilePath = m_sExportPath;
	}
	else
	{
		m_sFolderPath = m_sExportPath;
	}
}

LRESULT CTDLExportDlg::OnChangeTaskSelOption(WPARAM /*wp*/, LPARAM /*lp*/)
{
	if (m_taskSel.GetWantSelectedTasks() && m_nExportOption == ALLTASKLISTS)
	{
		m_nExportOption = ACTIVETASKLIST;
		UpdateData(FALSE);

		OnSelchangeTasklistoptions();
	}

	return 0;
}

CString CTDLExportDlg::GetExportPath()
{
	if (m_mgrImportExport.ExporterHasFileExtension(m_nFormatOption))
	{
		return m_sExportPath;
	}

	// else
	return _T("");
}
