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
//*****************************************************************************

// WelcomeWizard.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "WelcomeWizard.h"

#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"
#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWelcomeWizard

IMPLEMENT_DYNAMIC(CTDLWelcomeWizard, CPropertySheet)

CTDLWelcomeWizard::CTDLWelcomeWizard() : CPropertySheet(_T(""), NULL, 0)
{
	InitSheet();
}

void CTDLWelcomeWizard::InitSheet()
{
	m_hFont = GraphicsMisc::CreateFont(_T("MS Shell Dlg 2"));

	m_page1.AttachFont(m_hFont);
	m_page2.AttachFont(m_hFont);
	m_page3.AttachFont(m_hFont);

	AddPage(&m_page1);
	AddPage(&m_page2);
	AddPage(&m_page3);
	SetWizardMode();

	m_psh.dwFlags &= ~(PSH_HASHELP);
}

CTDLWelcomeWizard::~CTDLWelcomeWizard()
{
}

BEGIN_MESSAGE_MAP(CTDLWelcomeWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CWelcomeWizard)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_WIZFINISH, OnWizFinish)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomeWizard message handlers

BOOL CTDLWelcomeWizard::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	CDialogHelper::SetFont(this, m_hFont);

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_TRAYICONXP);
	SetIcon(hIcon, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLWelcomeWizard::OnWizFinish()
{
	m_page1.UpdateData();
	m_page2.UpdateData();
	m_page3.UpdateData();

	EndDialog(ID_WIZFINISH);
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage1 property page

IMPLEMENT_DYNCREATE(CTDLWelcomePage1, CPropertyPage)

CTDLWelcomePage1::CTDLWelcomePage1() : CPropertyPage(CTDLWelcomePage1::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage1)
	m_bShareTasklists = 0;
	m_bUseIniFile = 1;
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CTDLWelcomePage1::~CTDLWelcomePage1()
{
}

void CTDLWelcomePage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage1)
	DDX_Radio(pDX, IDC_NOSHARE, m_bShareTasklists);
	DDX_Radio(pDX, IDC_REGISTRY, m_bUseIniFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLWelcomePage1, CPropertyPage)
	//{{AFX_MSG_MAP(CWelcomePage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage1 message handlers

BOOL CTDLWelcomePage1::OnInitDialog()
{
	CDialogHelper::SetFont(this, m_hFont);
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage2 property page

IMPLEMENT_DYNCREATE(CTDLWelcomePage2, CPropertyPage)

CTDLWelcomePage2::CTDLWelcomePage2() : CPropertyPage(CTDLWelcomePage2::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage2)
	//}}AFX_DATA_INIT
	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CTDLWelcomePage2::~CTDLWelcomePage2()
{
}

void CTDLWelcomePage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage2)
	DDX_Control(pDX, IDC_COLUMNLIST, m_lbColumns);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLWelcomePage2, CPropertyPage)
	//{{AFX_MSG_MAP(CWelcomePage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage2 message handlers

BOOL CTDLWelcomePage2::OnInitDialog()
{
	CDialogHelper::SetFont(this, m_hFont);
	CPropertyPage::OnInitDialog();

	// initialize the visible columns for first time users
	m_lbColumns.SetAllColumnsVisible(FALSE);
	m_lbColumns.SetColumnVisible(TDCC_PRIORITY);
	m_lbColumns.SetColumnVisible(TDCC_PERCENT);
	m_lbColumns.SetColumnVisible(TDCC_ALLOCTO);
	m_lbColumns.SetColumnVisible(TDCC_TIMEEST);
	m_lbColumns.SetColumnVisible(TDCC_TIMESPENT);
	m_lbColumns.SetColumnVisible(TDCC_STATUS);
	m_lbColumns.SetColumnVisible(TDCC_CATEGORY);
	m_lbColumns.SetColumnVisible(TDCC_TRACKTIME);
	m_lbColumns.SetColumnVisible(TDCC_DUEDATE);
	m_lbColumns.SetColumnVisible(TDCC_FILEREF);
	m_lbColumns.SetColumnVisible(TDCC_DONE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTDLWelcomePage2::OnSetActive()
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage3 property page

IMPLEMENT_DYNCREATE(CTDLWelcomePage3, CPropertyPage)

CTDLWelcomePage3::CTDLWelcomePage3() : CPropertyPage(CTDLWelcomePage3::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage3)
	m_bHideAttrib = 1;
	m_bViewSample = 1;
	//}}AFX_DATA_INIT
	CString sResFolder;
	::SHGetSpecialFolderPath(NULL, sResFolder.GetBuffer(_MAX_PATH), CSIDL_COMMON_APPDATA, TRUE);
	sResFolder.ReleaseBuffer();
	sResFolder.TrimRight(_T('\\'));
	sResFolder += _T("\\AbstractSpoon\\ToDoList");
	FileMisc::MakePath(m_sSampleTaskList, NULL, sResFolder, _T("Introduction.tdl"));

	CString sFilter;
	sFilter.LoadString(IDS_TDLFILEFILTER);
	m_eSampleTasklist.SetFilter(sFilter);

	m_psp.dwFlags &= ~(PSP_HASHELP);
}

CTDLWelcomePage3::~CTDLWelcomePage3()
{
}

void CTDLWelcomePage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage3)
	DDX_Control(pDX, IDC_SAMPLETASKLIST, m_eSampleTasklist);
	DDX_Text(pDX, IDC_SAMPLETASKLIST, m_sSampleTaskList);
	DDX_Radio(pDX, IDC_ALLOPTIONS, m_bHideAttrib);
	DDX_Radio(pDX, IDC_NOSAMPLE, m_bViewSample);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLWelcomePage3, CPropertyPage)
	//{{AFX_MSG_MAP(CWelcomePage3)
	ON_BN_CLICKED(IDC_NOSAMPLE, OnNosample)
	ON_BN_CLICKED(IDC_SAMPLE, OnSample)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage3 message handlers

BOOL CTDLWelcomePage3::OnInitDialog()
{
	CDialogHelper::SetFont(this, m_hFont);
	CPropertyPage::OnInitDialog();

	m_eSampleTasklist.SetButtonWidthDLU(1, 14);
	m_eSampleTasklist.EnableWindow(m_bViewSample);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLWelcomePage3::OnNosample()
{
	UpdateData();
	m_eSampleTasklist.EnableWindow(m_bViewSample);
}

void CTDLWelcomePage3::OnSample()
{
	UpdateData();
	m_eSampleTasklist.EnableWindow(m_bViewSample);
}

BOOL CTDLWelcomePage3::OnSetActive()
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}
