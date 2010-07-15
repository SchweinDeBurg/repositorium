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

// PreferencesUITasklistPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesUITasklistPage.h"

#include "../../../CodeProject/Source/ColorDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistPage property page

CPreferencesUITasklistPage::CPreferencesUITasklistPage() : 
	CPreferencesPageBase(CPreferencesUITasklistPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesUITasklistPage)
	m_bHideDoneTimeField = FALSE;
	//}}AFX_DATA_INIT

}

CPreferencesUITasklistPage::~CPreferencesUITasklistPage()
{
}

void CPreferencesUITasklistPage::DoDataExchange(CDataExchange* pDX)
{ 
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUITasklistPage)
	DDX_Check(pDX, IDC_USEISODATEFORMAT, m_bUseISOForDates);
	DDX_Check(pDX, IDC_SHOWWEEKDAYINDATES, m_bShowWeekdayInDates);
	DDX_Check(pDX, IDC_SHOWPARENTSASFOLDERS, m_bShowParentsAsFolders);
	DDX_Check(pDX, IDC_DISPLAYFIRSTCOMMENTLINE, m_bDisplayFirstCommentLine);
	DDX_Check(pDX, IDC_LIMITINFOTIPCOMMENTS, m_bLimitInfoTipCommentsLength);
	DDX_Check(pDX, IDC_AUTOFOCUSTASKLIST, m_bAutoFocusTasklist);
	DDX_Check(pDX, IDC_SHOWSUBTASKCOMPLETION, m_bShowSubtaskCompletion);
	DDX_Check(pDX, IDC_RIGHTSIDECOLUMNS, m_bShowColumnsOnRight);
	DDX_Check(pDX, IDC_HIDEDUETIME, m_bHideDueTimeField);
	DDX_Check(pDX, IDC_HIDESTARTTIME, m_bHideStartTimeField);
	DDX_Check(pDX, IDC_LIMITCOLWIDTHS, m_bLimitColumnWidths);
	DDX_Text(pDX, IDC_COLWIDTHS, m_nMaxColumnWidth);
	DDX_Check(pDX, IDC_HIDEDONETIME, m_bHideDoneTimeField);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DISPLAYPATHINHEADER, m_bShowPathInHeader);
	DDX_Check(pDX, IDC_STRIKETHROUGHDONE, m_bStrikethroughDone);
	DDX_Check(pDX, IDC_FULLROWSELECTION, m_bFullRowSelection);
	DDX_Check(pDX, IDC_TREECHECKBOXES, m_bTreeCheckboxes);
	DDX_Control(pDX, IDC_COLUMNVISIBILITY, m_lbColumnVisibility);
	DDX_Check(pDX, IDC_SHOWINFOTIPS, m_bShowInfoTips);
	DDX_Check(pDX, IDC_SHOWCOMMENTS, m_bShowComments);
	DDX_LBIndex(pDX, IDC_COLUMNVISIBILITY, m_nSelColumnVisibility);
	DDX_Text(pDX, IDC_INFOTIPCOMMENTSMAX, m_nMaxInfoTipCommentsLength);
	DDX_Check(pDX, IDC_HIDEUNDEFINEDTIMEST, m_bHideZeroTimeCost);
	DDX_Check(pDX, IDC_HIDESTARTDUEFORDONETASKS, m_bHideStartDueForDoneTasks);
	DDX_Check(pDX, IDC_ROUNDTIMEFRACTIONS, m_bRoundTimeFractions);
	DDX_Check(pDX, IDC_SHOWNONFILEREFSASTEXT, m_bShowNonFilesAsText);
	DDX_Check(pDX, IDC_USEHMSTIMEFORMAT, m_bUseHMSTimeFormat);
	DDX_Check(pDX, IDC_SHOWPERCENTPROGRESSBAR, m_bShowPercentAsProgressbar);
	DDX_Check(pDX, IDC_HIDEPERCENTFORDONETASKS, m_bHidePercentForDoneTasks);
}


BEGIN_MESSAGE_MAP(CPreferencesUITasklistPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesUITasklistPage)
	ON_BN_CLICKED(IDC_SHOWCOMMENTS, OnShowcomments)
	ON_BN_CLICKED(IDC_SHOWINFOTIPS, OnShowinfotips)
	ON_BN_CLICKED(IDC_LIMITINFOTIPCOMMENTS, OnLimitinfotipcomments)
	ON_BN_CLICKED(IDC_LIMITCOLWIDTHS, OnLimitcolwidths)
	ON_BN_CLICKED(IDC_TREECHECKBOXES, OnTreecheckboxes)
	ON_BN_CLICKED(IDC_SHOWPARENTSASFOLDERS, OnShowparentsasfolders)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistPage message handlers

BOOL CPreferencesUITasklistPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_COLUMNGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_DATETIMEGROUP, *this);

	GetDlgItem(IDC_DISPLAYFIRSTCOMMENTLINE)->EnableWindow(m_bShowComments);
	GetDlgItem(IDC_LIMITINFOTIPCOMMENTS)->EnableWindow(m_bShowInfoTips);
	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);
	GetDlgItem(IDC_COLWIDTHS)->EnableWindow(m_bLimitColumnWidths);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CPreferencesUITasklistPage::GetMaxInfoTipCommentsLength() const
{
	if (m_bShowInfoTips)
		return m_bLimitInfoTipCommentsLength ? max(0, m_nMaxInfoTipCommentsLength) : -1;

	return -1;
}

int CPreferencesUITasklistPage::GetMaxColumnWidth() const
{
	return m_bLimitColumnWidths ? max(0, m_nMaxColumnWidth) : -1;
}

void CPreferencesUITasklistPage::OnShowcomments() 
{
	UpdateData();

	GetDlgItem(IDC_DISPLAYFIRSTCOMMENTLINE)->EnableWindow(m_bShowComments);
}

void CPreferencesUITasklistPage::OnShowinfotips() 
{
	UpdateData();

	GetDlgItem(IDC_LIMITINFOTIPCOMMENTS)->EnableWindow(m_bShowInfoTips);
	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);
}

void CPreferencesUITasklistPage::OnLimitinfotipcomments() 
{
	UpdateData();

	GetDlgItem(IDC_INFOTIPCOMMENTSMAX)->EnableWindow(m_bShowInfoTips && m_bLimitInfoTipCommentsLength);
}

int CPreferencesUITasklistPage::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	return m_lbColumnVisibility.GetVisibleColumns(aColumns);
}

void CPreferencesUITasklistPage::SetVisibleColumns(const CTDCColumnArray& aColumns) 
{
	m_lbColumnVisibility.SetVisibleColumns(aColumns);
	SaveColumns();
}

void CPreferencesUITasklistPage::SaveColumns() const
{
	CPreferences prefs;
	CTDCColumnArray aCols;

	int nCol = m_lbColumnVisibility.GetVisibleColumns(aCols);
	prefs.WriteProfileInt("Preferences\\ColumnVisibility", "Count", nCol);

	while (nCol--)
	{
		CString sKey;
		sKey.Format("Col%d", nCol);

		prefs.WriteProfileInt("Preferences\\ColumnVisibility", sKey, aCols[nCol]);
	}
}

void CPreferencesUITasklistPage::LoadPreferences(const CPreferences& prefs)
{
	// column visibility
	CTDCColumnArray aCols;
	int nCol = prefs.GetProfileInt("Preferences\\ColumnVisibility", "Count", -1);

	if (nCol == -1) // backward compat required
	{
		for (int nCol = 0; ; nCol++)
		{
			CString sKey;
			sKey.Format("Col%d", nCol);
			
			BOOL bVisible = prefs.GetProfileInt("Preferences\\ColumnVisibility", sKey, -1);
			
			if (bVisible == -1)
				break; // all done
			
			if (bVisible)
			{
				TDC_COLUMN col = m_lbColumnVisibility.MapOldColumnIndex(nCol);
				aCols.Add(col);
			}
		}
	}
	else
	{
		while (nCol--)
		{
			CString sKey;
			sKey.Format("Col%d", nCol);

			TDC_COLUMN col = (TDC_COLUMN)prefs.GetProfileInt("Preferences\\ColumnVisibility", sKey, -1);

			if (col != (TDC_COLUMN)-1)
				aCols.Add(col);
		}
	}

	m_lbColumnVisibility.SetVisibleColumns(aCols);

	// prefs
	m_bShowInfoTips = prefs.GetProfileInt("Preferences", "ShowInfoTips", FALSE);
	m_bShowComments = prefs.GetProfileInt("Preferences", "ShowComments", TRUE);
	m_bDisplayFirstCommentLine = prefs.GetProfileInt("Preferences", "DisplayFirstCommentLine", FALSE);
	m_bStrikethroughDone = prefs.GetProfileInt("Preferences", "StrikethroughDone", TRUE);
	m_bShowPathInHeader = prefs.GetProfileInt("Preferences", "ShowPathInHeader", TRUE);
	m_bFullRowSelection = prefs.GetProfileInt("Preferences", "FullRowSelection", FALSE);
	m_bTreeCheckboxes = prefs.GetProfileInt("Preferences", "TreeCheckboxes", TRUE);
	m_bUseISOForDates = prefs.GetProfileInt("Preferences", "DisplayDatesInISO", FALSE);
	m_bShowWeekdayInDates = prefs.GetProfileInt("Preferences", "ShowWeekdayInDates", FALSE);
	m_bShowParentsAsFolders = prefs.GetProfileInt("Preferences", "ShowParentsAsFolders", TRUE);
	m_nMaxInfoTipCommentsLength = prefs.GetProfileInt("Preferences", "MaxInfoTipCommentsLength", 100);
	m_bLimitInfoTipCommentsLength = prefs.GetProfileInt("Preferences", "LimitInfoTipCommentsLength", TRUE);
	m_bHidePercentForDoneTasks = prefs.GetProfileInt("Preferences", "HidePercentForDoneTasks", TRUE);
	m_bHideStartDueForDoneTasks = prefs.GetProfileInt("Preferences", "HideStartDueForDoneTasks", TRUE);
	m_bHideZeroTimeCost = prefs.GetProfileInt("Preferences", "HideZeroTimeEst", TRUE);
	m_bShowPercentAsProgressbar = prefs.GetProfileInt("Preferences", "ShowPercentAsProgressbar", FALSE);
	m_bRoundTimeFractions = prefs.GetProfileInt("Preferences", "RoundTimeFractions", FALSE);
	m_bShowNonFilesAsText = prefs.GetProfileInt("Preferences", "ShowNonFilesAsText", FALSE);
	m_bUseHMSTimeFormat = prefs.GetProfileInt("Preferences", "UseHMSTimeFormat", FALSE);
	m_bAutoFocusTasklist = prefs.GetProfileInt("Preferences", "AutoFocusTasklist", FALSE);
	m_bShowSubtaskCompletion = prefs.GetProfileInt("Preferences", "ShowSubtaskCompletion", FALSE);
	m_bShowColumnsOnRight = prefs.GetProfileInt("Preferences", "ShowColumnsOnRight", FALSE);
	m_bHideDueTimeField = prefs.GetProfileInt("Preferences", "HideDueTimeField", FALSE);
	m_bHideStartTimeField = prefs.GetProfileInt("Preferences", "HideStartTimeField", FALSE);
	m_bHideDoneTimeField = prefs.GetProfileInt("Preferences", "HideDoneTimeField", FALSE);
	m_bLimitColumnWidths = prefs.GetProfileInt("Preferences", "LimitColumnWidths", FALSE);
	m_nMaxColumnWidth = prefs.GetProfileInt("Preferences", "MaxColumnWidth", 60);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesUITasklistPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	// column visibility
	CTDCColumnArray aCols;
	int nCol = m_lbColumnVisibility.GetVisibleColumns(aCols);

	prefs.WriteProfileInt("Preferences\\ColumnVisibility", "Count", nCol);

	while (nCol--)
	{
		CString sKey;
		sKey.Format("Col%d", nCol);

		prefs.WriteProfileInt("Preferences\\ColumnVisibility", sKey, aCols[nCol]);
	}

	// save settings
	prefs.WriteProfileInt("Preferences", "ShowInfoTips", m_bShowInfoTips);
	prefs.WriteProfileInt("Preferences", "ShowComments", m_bShowComments);
	prefs.WriteProfileInt("Preferences", "DisplayFirstCommentLine", m_bDisplayFirstCommentLine);
	prefs.WriteProfileInt("Preferences", "ShowPercentColumn", m_bShowPercentColumn);
	prefs.WriteProfileInt("Preferences", "ShowPriorityColumn", m_bShowPriorityColumn);
	prefs.WriteProfileInt("Preferences", "StrikethroughDone", m_bStrikethroughDone);
	prefs.WriteProfileInt("Preferences", "ShowPathInHeader", m_bShowPathInHeader);
	prefs.WriteProfileInt("Preferences", "FullRowSelection", m_bFullRowSelection);
	prefs.WriteProfileInt("Preferences", "TreeCheckboxes", m_bTreeCheckboxes);
	prefs.WriteProfileInt("Preferences", "DisplayDatesInISO", m_bUseISOForDates);
	prefs.WriteProfileInt("Preferences", "ShowWeekdayInDates", m_bShowWeekdayInDates);
	prefs.WriteProfileInt("Preferences", "ShowParentsAsFolders", m_bShowParentsAsFolders);
	prefs.WriteProfileInt("Preferences", "MaxInfoTipCommentsLength", max(m_nMaxInfoTipCommentsLength, 0));
	prefs.WriteProfileInt("Preferences", "LimitInfoTipCommentsLength", m_bLimitInfoTipCommentsLength);
	prefs.WriteProfileInt("Preferences", "HidePercentForDoneTasks", m_bHidePercentForDoneTasks);
	prefs.WriteProfileInt("Preferences", "HideStartDueForDoneTasks", m_bHideStartDueForDoneTasks);
	prefs.WriteProfileInt("Preferences", "HideZeroTimeEst", m_bHideZeroTimeCost);
	prefs.WriteProfileInt("Preferences", "ShowPercentAsProgressbar", m_bShowPercentAsProgressbar);
	prefs.WriteProfileInt("Preferences", "RoundTimeFractions", m_bRoundTimeFractions);
	prefs.WriteProfileInt("Preferences", "ShowNonFilesAsText", m_bShowNonFilesAsText);
	prefs.WriteProfileInt("Preferences", "UseHMSTimeFormat", m_bUseHMSTimeFormat);
	prefs.WriteProfileInt("Preferences", "AutoFocusTasklist", m_bAutoFocusTasklist);
	prefs.WriteProfileInt("Preferences", "ShowSubtaskCompletion", m_bShowSubtaskCompletion);
	prefs.WriteProfileInt("Preferences", "ShowColumnsOnRight", m_bShowColumnsOnRight);
	prefs.WriteProfileInt("Preferences", "HideDueTimeField", m_bHideDueTimeField);
	prefs.WriteProfileInt("Preferences", "HideStartTimeField", m_bHideStartTimeField);
	prefs.WriteProfileInt("Preferences", "HideDoneTimeField", m_bHideDoneTimeField);
	prefs.WriteProfileInt("Preferences", "LimitColumnWidths", m_bLimitColumnWidths);
	prefs.WriteProfileInt("Preferences", "MaxColumnWidth", m_nMaxColumnWidth);
//	prefs.WriteProfileInt("Preferences", "", m_b);
}



void CPreferencesUITasklistPage::OnLimitcolwidths() 
{
	UpdateData();

	GetDlgItem(IDC_COLWIDTHS)->EnableWindow(m_bLimitColumnWidths);
}

void CPreferencesUITasklistPage::OnTreecheckboxes() 
{
	// if the user is turning this preference on then make sure that
	// the completion column is hidden else the checkbox won't turn up
	UpdateData();

	if (m_bTreeCheckboxes)
		m_lbColumnVisibility.SetColumnVisible(TDCC_DONE, FALSE);
}

void CPreferencesUITasklistPage::OnShowparentsasfolders() 
{
	UpdateData();

	if (m_bShowParentsAsFolders)
		m_lbColumnVisibility.SetColumnVisible(TDCC_ICON);
}
