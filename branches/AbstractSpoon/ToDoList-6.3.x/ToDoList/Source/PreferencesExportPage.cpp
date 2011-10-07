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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// PreferencesExportPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesexportPage.h"

#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT ID_RESETCHARSET = 1;
const UINT RESETBTN = 0xc4;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage property page

IMPLEMENT_DYNCREATE(CPreferencesExportPage, CPreferencesPageBase)

CPreferencesExportPage::CPreferencesExportPage():
CPreferencesPageBase(CPreferencesExportPage::IDD),
m_eTextIndent(_T("0123456789")),
m_eLineSpaces(_T("0123456789"))
{
	//{{AFX_DATA_INIT(CPreferencesExportPage)
	m_nTextIndent = 2;
	m_nLineSpaces = 8;
	//}}AFX_DATA_INIT

	// add a 'reset' button to the charset field
	m_eCharset.InsertButton(0, ID_RESETCHARSET, RESETBTN, CEnString(IDS_PEP_RESETCHARSET), DEF_BTNWIDTH + 4,
		_T("Wingdings"));
}

CPreferencesExportPage::~CPreferencesExportPage()
{
}

void CPreferencesExportPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesExportPage)
	DDX_Control(pDX, IDC_NUMLINESPACE, m_eLineSpaces);
	DDX_Control(pDX, IDC_HTMLFONTSIZE, m_cbFontSize);
	DDX_Check(pDX, IDC_EXPORTPARENTTITLECOMMENTSONLY, m_bExportParentTitleCommentsOnly);
	DDX_Check(pDX, IDC_EXPORTSPACEFORNOTES, m_bExportSpaceForNotes);
	DDX_Text(pDX, IDC_CHARSET, m_sHtmlCharSet);
	DDX_Radio(pDX, IDC_TABTEXTINDENT, m_bUseSpaceIndents);
	DDX_Text(pDX, IDC_TABWIDTHS, m_nTextIndent);
	DDX_Text(pDX, IDC_NUMLINESPACE, m_nLineSpaces);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TABWIDTHS, m_eTextIndent);
	DDX_Control(pDX, IDC_FONTLIST, m_cbFonts);
	DDX_Check(pDX, IDC_PREVIEWEXPORT, m_bPreviewExport);
	DDX_Check(pDX, IDC_EXPORTVISIBLEONLY, m_bExportVisibleOnly);
	DDX_Control(pDX, IDC_CHARSET, m_eCharset);

	m_cbFonts.DDX(pDX, m_sHtmlFont);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		CString sSize;
		m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), sSize);
		m_nHtmlFontSize = _ttoi(sSize);
	}
	else
	{
		CString sSize;
		sSize.Format(_T("%d"), m_nHtmlFontSize);

		if (CB_ERR == m_cbFontSize.SelectString(-1, sSize))
		{
			m_nHtmlFontSize = 3;
			m_cbFontSize.SelectString(-1, _T("3"));
		}

		m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), sSize);
		m_nHtmlFontSize = _ttoi(sSize);
	}
}

BEGIN_MESSAGE_MAP(CPreferencesExportPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesExportPage)
	ON_BN_CLICKED(IDC_TABTEXTINDENT, OnChangeTextIndentType)
	ON_BN_CLICKED(IDC_SPACETEXTINDENT, OnChangeTextIndentType)
	ON_BN_CLICKED(IDC_EXPORTSPACEFORNOTES, OnExportspacefornotes)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage message handlers

BOOL CPreferencesExportPage::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	m_eTextIndent.EnableWindow(m_bUseSpaceIndents);
	GetDlgItem(IDC_TEXTINDENTTRAIL)->EnableWindow(m_bUseSpaceIndents);

	m_eLineSpaces.EnableWindow(m_bExportSpaceForNotes);
	GetDlgItem(IDC_NUMLINESPACETRAIL)->EnableWindow(m_bExportSpaceForNotes);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesExportPage::OnChangeTextIndentType()
{
	UpdateData();

	m_eTextIndent.EnableWindow(m_bUseSpaceIndents);
	GetDlgItem(IDC_TEXTINDENTTRAIL)->EnableWindow(m_bUseSpaceIndents);

	CPreferencesPageBase::OnControlChange();
}

CString CPreferencesExportPage::GetHtmlCharSet() const
{
	if (m_sHtmlCharSet.IsEmpty())
	{
		return _T("Windows - ") + Misc::GetDefCharset();
	}

	// else
	return m_sHtmlCharSet;
}

void CPreferencesExportPage::LoadPreferences(const CPreferences& prefs)
{
	m_sHtmlFont = prefs.GetProfileString(_T("Preferences"), _T("HTMLFont"), _T("Verdana"));
	m_bPreviewExport = prefs.GetProfileInt(_T("Preferences"), _T("PreviewSaveAs"), TRUE);
	m_nHtmlFontSize = prefs.GetProfileInt(_T("Preferences"), _T("HtmlFontSize"), 2);
	m_nTextIndent = prefs.GetProfileInt(_T("Preferences"), _T("TextIndent"), 2);
	m_nLineSpaces = prefs.GetProfileInt(_T("Preferences"), _T("LineSpaces"), 8);
	m_bExportVisibleOnly = prefs.GetProfileInt(_T("Preferences"), _T("ExportVisibleOnly"), FALSE);
	m_bExportParentTitleCommentsOnly = prefs.GetProfileInt(_T("Preferences"), _T("ExportParentTitleCommentsOnly"), FALSE);
	m_bExportSpaceForNotes = prefs.GetProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), FALSE);
	m_bUseSpaceIndents = prefs.GetProfileInt(_T("Preferences"), _T("UseSpaceIndents"), TRUE);

	CString sDefCharset = _T("Windows-") + Misc::GetDefCharset();
	m_sHtmlCharSet = prefs.GetProfileString(_T("Preferences"), _T("HtmlCharSet"), sDefCharset);
}

void CPreferencesExportPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileString(_T("Preferences"), _T("HTMLFont"), m_sHtmlFont);
	prefs.WriteProfileInt(_T("Preferences"), _T("HtmlFontSize"), m_nHtmlFontSize);
	prefs.WriteProfileInt(_T("Preferences"), _T("PreviewSaveAs"), m_bPreviewExport);
	prefs.WriteProfileInt(_T("Preferences"), _T("TextIndent"), m_nTextIndent);
	prefs.WriteProfileInt(_T("Preferences"), _T("LineSpaces"), m_nLineSpaces);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExportVisibleOnly"), m_bExportVisibleOnly);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExportParentTitleCommentsOnly"), m_bExportParentTitleCommentsOnly);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), m_bExportSpaceForNotes);
	prefs.WriteProfileString(_T("Preferences"), _T("HtmlCharSet"), m_sHtmlCharSet);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseSpaceIndents"), m_bUseSpaceIndents);
}

LRESULT CPreferencesExportPage::OnEEBtnClick(WPARAM wp, LPARAM lp)
{
	if (wp == IDC_CHARSET && lp == ID_RESETCHARSET)
	{
		m_sHtmlCharSet = _T("Windows-") + Misc::GetDefCharset();
		UpdateData(FALSE);

		CPreferencesPageBase::OnControlChange();
	}

	return 0L;
}

void CPreferencesExportPage::OnExportspacefornotes()
{
	UpdateData();

	m_eLineSpaces.EnableWindow(m_bExportSpaceForNotes);
	GetDlgItem(IDC_NUMLINESPACETRAIL)->EnableWindow(m_bExportSpaceForNotes);

	CPreferencesPageBase::OnControlChange();
}
