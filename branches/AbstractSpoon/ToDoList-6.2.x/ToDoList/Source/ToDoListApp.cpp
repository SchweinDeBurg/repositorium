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

// ToDoListApp.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToDoListWnd.h"
#include "PreferencesGenPage.h"
#include "WelcomeWizard.h"
#include "TDCMsg.h"
#include "TDLPrefMigrationDlg.h"

#include "../../../CodeProject/Source/LimitSingleInstance.h"
#include "../../../CodeProject/Source/EnCommandLineInfo.h"
#include "../../../CodeProject/Source/DriveInfo.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/AfxRegKey.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../Common/preferences.h"

#include "../../Common/xmlnodewrapper.h"
#include "../../../CodeProject/Source/Ini.h"

#include <afxpriv.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLimitSingleInstance g_SingleInstanceObj(_T("{3A4EFC98-9BA9-473D-A3CF-6B0FE644470D}"));

BOOL CALLBACK FindOtherInstance(HWND hwnd, LPARAM lParam);

const LPCTSTR REGKEY = _T("AbstractSpoon");
const LPCTSTR APPREGKEY = _T("Software\\AbstractSpoon\\ToDoList");

const LPCTSTR ONLINEHELP = _T("http://abstractspoon.pbwiki.com/OnlineDocumentation");
const LPCTSTR CONTACTUS = _T("mailto:abstractspoon2@optusnet.com.au");
const LPCTSTR FEEDBACKANDSUPPORT = _T("http://www.codeproject.com/tools/todolist2.asp");
const LPCTSTR LICENSE = _T("http://www.opensource.org/licenses/eclipse-1.0.php");
const LPCTSTR ONLINE = _T("http://www.abstractspoon.com/tdl_resources.html");
const LPCTSTR WIKI = _T("http://abstractspoon.pbwiki.com/");
const LPCTSTR DONATE = _T("https://www.paypal.com/cgi-bin/webscr?cmd=_xclick&business=abstractspoon2%40optusnet%2ecom%2eau&item_name=Software");

const TCHAR FILESTATEKEY[] = _T("FileStates");
const UINT LENFILESTATEKEY = sizeof(FILESTATEKEY);

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp

BEGIN_MESSAGE_MAP(CToDoListApp, CWinApp)
	//{{AFX_MSG_MAP(CToDoListApp)
	ON_COMMAND(ID_HELP_CONTACTUS, OnHelpContactus)
	ON_COMMAND(ID_HELP_FEEDBACKANDSUPPORT, OnHelpFeedbackandsupport)
	ON_COMMAND(ID_HELP_LICENSE, OnHelpLicense)
	ON_COMMAND(ID_HELP_ONLINE, OnHelpOnline)
	ON_COMMAND(ID_HELP_WIKI, OnHelpWiki)
	ON_COMMAND(ID_HELP_COMMANDLINE, OnHelpCommandline)
	ON_COMMAND(ID_HELP_DONATE, OnHelpDonate)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_TOOLS_IMPORTPREFS, OnImportPrefs)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_IMPORTPREFS, OnUpdateImportPrefs)
	ON_COMMAND(ID_TOOLS_EXPORTPREFS, OnExportPrefs)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORTPREFS, OnUpdateExportPrefs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp construction

CToDoListApp::CToDoListApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CToDoListApp object

CToDoListApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CToDoListApp initialization

BOOL CToDoListApp::InitInstance()
{
	AfxOleInit(); // for initializing COM and handling drag and drop via explorer
	AfxEnableControlContainer(); // embedding IE

	// load localized resources
	CEnString sResVersion(IDS_RES_VERSION);
	HINSTANCE hResDll = LoadLibrary(_T("ToDoListLOC.dll"));

	if (hResDll)
	{
		// check resource version
		TCHAR szResVer[10];
		::LoadString(hResDll, IDS_RES_VERSION, szResVer, sizeof(szResVer) - 1);

		AfxSetResourceHandle(hResDll);

		// allow user to proceed but warn them
		if (_ttoi(sResVersion) != _ttoi(szResVer))
		{
			AfxMessageBox(IDS_MISMATCHEDRESOURCES);
		}
	}

	CEnCommandLineInfo cmdInfo;

	if (!ParseCommandLine(&cmdInfo))
	{
		return FALSE;
	}

	// see if the user just wants to see the commandline options
	if (cmdInfo.HasOption(_T("h")) || cmdInfo.HasOption(_T("help")) ||
		cmdInfo.HasOption(_T("?")))
	{
		AfxMessageBox(IDS_COMMANDLINEOPTIONS, MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	// before anything else make sure we've got MSXML3 installed
	if (!CXmlDocumentWrapper::IsVersion3orGreater())
	{
		AfxMessageBox(IDS_BADMSXML);
		return FALSE;
	}

	if (!InitPreferences(&cmdInfo))
	{
		return FALSE;
	}

	// commandline options
	TDCSTARTUP startup(cmdInfo);

	// does the user want single instance only
	BOOL bSingleInstance = !CPreferencesDlg().GetMultiInstance();

	if (bSingleInstance && g_SingleInstanceObj.IsAnotherInstanceRunning())
	{
		HWND hWnd = NULL;
		EnumWindows(FindOtherInstance, (LPARAM)&hWnd);

		// make sure its not closing
		if (hWnd && !::SendMessage(hWnd, WM_TDL_ISCLOSING, 0, 0))
		{
			// check version
			int nVer = ::SendMessage(hWnd, WM_TDL_GETVERSION, 0, 0);

			if (nVer < CToDoListWnd::GetVersion())
			{
				MessageBox(NULL, CEnString(IDS_EARLIERVERRUNNING), CEnString(IDS_COPYRIGHT), MB_OK);
			}

			::SendMessage(hWnd, WM_TDL_SHOWWINDOW, 0, 0);

			// pass on file to open
			COPYDATASTRUCT cds;

			cds.dwData = TDL_STARTUP;
			cds.cbData = sizeof(startup);
			cds.lpData = (void*)&startup;

			::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);

			::SetForegroundWindow(hWnd);

			return FALSE;
		}
	}

	CToDoListWnd* pTDL = new CToDoListWnd;

	if (pTDL && pTDL->Create(startup))
	{
		m_pMainWnd = pTDL;
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToDoListApp::ParseCommandLine(CEnCommandLineInfo* pInfo)
{
	ASSERT(pInfo);

	CWinApp::ParseCommandLine(*pInfo); // default

	// check for task link
	CString sLink;

	if (pInfo->GetOption(_T("l"), sLink) && sLink.Find(TDL_PROTOCOL) != -1)
	{
		CString sFilePath;
		DWORD dwID = 0;

		CToDoCtrl::ParseTaskLink(sLink, dwID, sFilePath);

		if (!sFilePath.IsEmpty() && dwID)
		{
			// remove possible trailing slash on file path
			sFilePath.TrimRight(_T('\\'));

			// replace possible %20 by spaces
			sFilePath.Replace(_T("%20"), _T(" "));

			// verify the file existence unless the path is relative
			if (FileMisc::FileExists(sFilePath))
			{
				pInfo->m_strFileName = sFilePath;
				pInfo->SetOption(_T("tid"), dwID);
			}
			else
			{
				pInfo->m_strFileName.Empty();
				pInfo->DeleteOption(_T("tid"));
				AfxMessageBox(CEnString(IDS_TDLLINKLOADFAILED, sFilePath));

				return FALSE;
			}
		}
	}
	// else validate file path
	else if (!FileMisc::FileExists(pInfo->m_strFileName))
	{
		pInfo->m_strFileName.Empty();
	}

	return TRUE;
}

BOOL CALLBACK FindOtherInstance(HWND hwnd, LPARAM lParam)
{
	CString sCaption;

	int nLen = ::GetWindowTextLength(hwnd);
	::GetWindowText(hwnd, sCaption.GetBuffer(nLen + 1), nLen + 1);
	sCaption.ReleaseBuffer();

	if (sCaption.Find(CEnString(IDS_COPYRIGHT)) != -1)
	{
		HWND* pWnd = (HWND*)lParam;
		*pWnd = hwnd;
		return FALSE;
	}

	return TRUE;
}

BOOL CToDoListApp::PreTranslateMessage(MSG* pMsg)
{
	// give first chance to main window for handling accelerators
	if (m_pMainWnd && m_pMainWnd->PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

void CToDoListApp::OnHelp()
{
	DoHelp();
}

void CToDoListApp::WinHelp(DWORD dwData, UINT nCmd)
{
	if (nCmd == HELP_CONTEXT)
	{
		DoHelp((LPCTSTR)dwData);
	}
}

void CToDoListApp::DoHelp(const CString& sHelpRef)
{
	CString sHelpUrl(ONLINEHELP);

	if (sHelpRef.IsEmpty())
	{
		sHelpUrl += _T("FrontPage");
	}
	else
	{
		sHelpUrl += sHelpRef;
	}

	::ShellExecute(*m_pMainWnd, NULL, sHelpUrl, NULL, NULL, SW_SHOWNORMAL);
}

BOOL CToDoListApp::InitPreferences(CEnCommandLineInfo* pInfo)
{
	BOOL bUseIni = FALSE;

	// get the app file path
	CString sTemp = FileMisc::GetModuleFileName(), sDrive, sFolder, sAppName;
	FileMisc::SplitPath(sTemp, &sDrive, &sFolder, &sAppName);

	// try command line first
	CString sIniPath;

	if (pInfo->GetOption(_T("i"), sIniPath) && !sIniPath.IsEmpty())
	{
		// prefix application path if path is relative
		if (PathIsRelative(sIniPath))
		{
			FileMisc::MakePath(sIniPath, sDrive, sFolder, sIniPath);
		}

		bUseIni = FileMisc::FileExists(sIniPath);
	}

	// else if there is a tasklist on the commandline then try for an
	// ini file of the same name
	if (!bUseIni && !pInfo->m_strFileName.IsEmpty())
	{
		sIniPath = pInfo->m_strFileName;
		FileMisc::ReplaceExtension(sIniPath, _T("ini"));

		bUseIni = FileMisc::FileExists(sIniPath);

		// if we found one then make sure it does not have single
		// instance specified
		if (bUseIni)
		{
			WriteProfileInt(_T("Preferences"), _T("MultiInstance"), TRUE);
		}
	}

	// else try for an ini file having the same name as the executable
	if (!bUseIni)
	{
		// then try current working dir followed by app folder
		FileMisc::MakePath(sIniPath, NULL, FileMisc::GetCwd(), sAppName, _T(".ini"));
		bUseIni = FileMisc::FileExists(sIniPath);

		// followed by app folder
		if (!bUseIni)
		{
			FileMisc::MakePath(sIniPath, sDrive, sFolder, sAppName, _T(".ini"));
			bUseIni = FileMisc::FileExists(sIniPath);
		}
	}

	// then try registry
	CTDLWelcomeWizard* pWizard = NULL;

	if (!bUseIni)
	{
		BOOL bFirstTime = !CAfxRegKey::KeyExists(HKEY_CURRENT_USER, _T("Software\\AbstractSpoon\\ToDoList"));

		if (bFirstTime) // first time
		{
			pWizard = new CTDLWelcomeWizard;

			if (pWizard->DoModal() != ID_WIZFINISH)
			{
				delete pWizard;
				return FALSE; // user cancelled so quit app
			}

			bUseIni = pWizard->GetUseIniFile();
		}
	}

	// finally make the choice
	if (bUseIni)
	{
		free((void*)m_pszProfileName);
		m_pszProfileName = _tcsdup(sIniPath);
	}
	else
	{
		SetRegistryKey(REGKEY);
	}

	if (pWizard)
	{
		// set up some default preferences
		CPreferences prefs;

		if (pWizard->GetShareTasklists())
		{
			// set up source control for remote tasklists
			prefs.WriteProfileInt(_T("Preferences"), _T("EnableSourceControl"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("SourceControlLanOnly"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("AutoCheckOut"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("CheckoutOnCheckin"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("CheckinOnClose"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("CheckinNoEditTime"), 1);
			prefs.WriteProfileInt(_T("Preferences"), _T("CheckinNoEdit"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("Use3rdPartySourceControl"), FALSE);
		}

		// setup default columns
		CTDCColumnArray aColumns;
		pWizard->GetVisibleColumns(aColumns);

		int nCol = aColumns.GetSize();
		prefs.WriteProfileInt(_T("Preferences\\ColumnVisibility"), _T("Count"), nCol);

		while (nCol--)
		{
			CString sKey;
			sKey.Format(_T("Col%d"), nCol);

			prefs.WriteProfileInt(_T("Preferences\\ColumnVisibility"), sKey, aColumns[nCol]);
		}

		if (pWizard->GetHideAttributes())
		{
			// hide clutter
			prefs.WriteProfileInt(_T("Preferences"), _T("ShowCtrlsAsColumns"), TRUE);
			prefs.WriteProfileInt(_T("Preferences"), _T("ShowEditMenuAsColumns"), TRUE);
		}

		// set up initial file
		CString sSample = pWizard->GetSampleFilePath();

		if (!sSample.IsEmpty())
		{
			pInfo->m_strFileName = sSample;
		}

		delete pWizard;
	}
	else
	{
		UpgradePreferences(bUseIni);
	}

	return TRUE;
}

void CToDoListApp::UpgradePreferences(BOOL /*bUseIni*/)
{
	CPreferences prefs;
	BOOL bIconColumn = prefs.GetProfileInt(_T("Preferences"), _T("IconColumn"));

	if (bIconColumn)
	{
		return;   // already fixed
	}

	prefs.WriteProfileInt(_T("Preferences"), _T("IconColumn"), TRUE);

	// add icon column to default columns
	int nCol = prefs.GetProfileInt(_T("Preferences\\ColumnVisibility"), _T("Count"), -1);

	if (nCol >= 0)
	{
		CString sKey;
		sKey.Format(_T("Col%d"), nCol);

		prefs.WriteProfileInt(_T("Preferences\\ColumnVisibility"), sKey, TDCC_ICON);

		nCol++;
		prefs.WriteProfileInt(_T("Preferences\\ColumnVisibility"), _T("Count"), nCol);
	}

	// we don't handle the registry because it's too hard (for now)
	if (m_pszRegistryKey)
	{
		return;
	}

	CIni ini(m_pszProfileName);
	CStringArray aSections;

	int nNumSection = ini.GetSectionNames(aSections);

	for (int nSection = 0; nSection < nNumSection; nSection++)
	{
		Misc::ProcessMsgLoop(); // for feedback dialog

		CString sSection = aSections[nSection];

		// does it start with "FileStates\\"
		if (sSection.Find(FILESTATEKEY) == 0)
		{
			// split the path
			CStringArray aSubSections;

			if (Misc::ParseIntoArray(sSection, aSubSections, FALSE, _T("\\")) == 3)
			{
				if (aSubSections[2] == _T("Columns"))
				{
					// add icon column
					int nCol = prefs.GetProfileInt(sSection, _T("Count"), -1);

					if (nCol >= 0)
					{
						CString sKey;
						sKey.Format(_T("Item%d"), nCol);

						prefs.WriteProfileInt(sSection, sKey, TDCC_ICON);

						nCol++;
						prefs.WriteProfileInt(sSection, _T("Count"), nCol);
					}
				}
			}
		}
	}
}

int CToDoListApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	// make sure app window is visible
	if (m_pMainWnd && !m_pMainWnd->IsWindowVisible())
	{
		m_pMainWnd->SendMessage(WM_TDL_SHOWWINDOW, 0, 0);
	}

	return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}

void CToDoListApp::OnImportPrefs()
{
	// default location is always app folder
	CString sIniPath = FileMisc::GetModuleFileName();
	sIniPath.MakeLower();
	sIniPath.Replace(_T("exe"), _T("ini"));

	CFileDialog dialog(TRUE, _T("ini"), sIniPath,
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		CEnString(IDS_INIFILEFILTER));

	if (dialog.DoModal() == IDOK)
	{
		CAfxRegKey reg;

		if (reg.Open(HKEY_CURRENT_USER, APPREGKEY) == ERROR_SUCCESS)
		{
			sIniPath = dialog.GetPathName();

			if (reg.ImportFromIni(sIniPath)) // => import ini to registry
			{
				// use them now?
				// only ask if we're not already using the registry
				BOOL bUsingIni = (m_pszRegistryKey == NULL);

				if (bUsingIni)
				{
					if (AfxMessageBox(IDS_POSTIMPORTPREFS, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						// renames existing prefs file
						CString sNewName(sIniPath);
						sNewName += _T(".bak");

						if (FileMisc::FileExists(sNewName))
						{
							DeleteFile(sNewName);
						}

						if (MoveFile(sIniPath, sNewName))
						{
							// and initialize the registry
							SetRegistryKey(REGKEY);

							// reset prefs
							m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
						}
					}
				}
				else // reset prefs
				{
					m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
				}
			}
			else // notify user
			{
				CEnString sMessage(IDS_INVALIDPREFFILE, dialog.GetFileName());
				AfxMessageBox(sMessage, MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}
}

void CToDoListApp::OnUpdateImportPrefs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CToDoListApp::OnExportPrefs()
{
	ASSERT(m_pszRegistryKey != NULL);

	CAfxRegKey reg;

	if (reg.Open(HKEY_CURRENT_USER, APPREGKEY) == ERROR_SUCCESS)
	{
		// default location is always app folder
		CString sAppPath = FileMisc::GetModuleFileName();

		CString sIniPath(sAppPath);
		sIniPath.MakeLower();
		sIniPath.Replace(_T("exe"), _T("ini"));

		CFileDialog dialog(FALSE, _T("ini"), sIniPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
			CEnString(IDS_INIFILEFILTER));

		if (dialog.DoModal() == IDOK)
		{
			BOOL bUsingReg = (m_pszRegistryKey != NULL);
			sIniPath = dialog.GetPathName();

			if (bUsingReg && reg.ExportToIni(sIniPath))
			{
				// use them now?
				CString sAppFolder, sIniFolder;

				FileMisc::SplitPath(sAppPath, NULL, &sAppFolder);
				FileMisc::SplitPath(sIniPath, NULL, &sIniFolder);

				// only if they're in the same folder as the exe
				if (sIniFolder.CompareNoCase(sAppFolder) == 0)
				{
					if (AfxMessageBox(IDS_POSTEXPORTPREFS, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						free((void*)m_pszRegistryKey);
						m_pszRegistryKey = NULL;

						free((void*)m_pszProfileName);
						m_pszProfileName = _tcsdup(sIniPath);

						// reset prefs
						m_pMainWnd->SendMessage(WM_TDL_REFRESHPREFS);
					}
				}
			}
		}
	}
}

void CToDoListApp::OnUpdateExportPrefs(CCmdUI* pCmdUI)
{
	BOOL bUsingReg = (m_pszRegistryKey != NULL);
	pCmdUI->Enable(bUsingReg);
}

void CToDoListApp::OnHelpContactus()
{
	::ShellExecute(*m_pMainWnd, NULL, CONTACTUS, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpFeedbackandsupport()
{
	::ShellExecute(*m_pMainWnd, NULL, FEEDBACKANDSUPPORT, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpLicense()
{
	::ShellExecute(*m_pMainWnd, NULL, LICENSE, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpOnline()
{
	::ShellExecute(*m_pMainWnd, NULL, ONLINE, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpWiki()
{
	::ShellExecute(*m_pMainWnd, NULL, WIKI, NULL, NULL, SW_SHOWNORMAL);
}

void CToDoListApp::OnHelpCommandline()
{
	AfxMessageBox(IDS_COMMANDLINEOPTIONS, MB_OK | MB_ICONINFORMATION);
}

void CToDoListApp::OnHelpDonate()
{
	::ShellExecute(*m_pMainWnd, NULL, DONATE, NULL, NULL, SW_SHOWNORMAL);
}

int CToDoListApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}
