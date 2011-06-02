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

// TDLCsvImportExportDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLCsvImportExportDlg.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLCsvImportExportDlg dialog

CTDLCsvImportExportDlg::CTDLCsvImportExportDlg(const CString& sFilePath, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CSVIMPORTEXPORT_DIALOG, pParent),
	  m_sFilePath(sFilePath), m_lcColumnSetup(TRUE), m_bImporting(TRUE), m_eFilePath(FES_NOBROWSE)
{
	m_bAlwaysExportTaskIDs = TRUE;
	m_sDelim = Misc::GetListSeparator();

	LoadMasterColumnMapping();

	// user mapping
	CTDCCsvColumnMapping aMapping;
	BuildImportColumnMapping(aMapping);
	m_lcColumnSetup.SetColumnMapping(aMapping);
}

CTDLCsvImportExportDlg::CTDLCsvImportExportDlg(const CString& sFilePath, const CTDCAttributeArray& aExportAttributes, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CSVIMPORTEXPORT_DIALOG, pParent),
	  m_sFilePath(sFilePath), m_lcColumnSetup(FALSE), m_bImporting(FALSE), m_eFilePath(FES_NOBROWSE)
{
	m_bAlwaysExportTaskIDs = TRUE;
	m_sDelim = Misc::GetListSeparator();
	m_aExportAttributes.Copy(aExportAttributes);

	LoadMasterColumnMapping();

	// user mapping
	CTDCCsvColumnMapping aMapping;
	BuildExportColumnMapping(aMapping);
	m_lcColumnSetup.SetColumnMapping(aMapping);
}


void CTDLCsvImportExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLCsvImportExportDlg)
	DDX_Text(pDX, IDC_CSVDELIMITER, m_sDelim);
	DDX_Text(pDX, IDC_CSVFILEPATH, m_sFilePath);
	DDX_Control(pDX, IDC_CSVFILEPATH, m_eFilePath);
	DDX_Control(pDX, IDC_COLUMNSETUP, m_lcColumnSetup);
	DDX_Check(pDX, IDC_EXPORTTASKIDS, m_bAlwaysExportTaskIDs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLCsvImportExportDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLCsvImportExportDlg)
	ON_EN_CHANGE(IDC_CSVDELIMITER, OnChangeCsvdelimiter)
	ON_BN_CLICKED(IDC_EXPORTTASKIDS, OnExportTaskids)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLCsvImportExportDlg message handlers

BOOL CTDLCsvImportExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(CEnString(m_bImporting ? IDS_CSV_IMPORTDLG : IDS_CSV_EXPORTDLG));

	GetDlgItem(IDC_EXPORTTASKIDS)->EnableWindow(!m_bImporting);
	GetDlgItem(IDC_EXPORTTASKIDS)->ShowWindow(m_bImporting ? SW_HIDE : SW_SHOW);

	if (!m_bImporting)
	{
		OnExportTaskids();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CTDLCsvImportExportDlg::GetColumnMapping(CTDCCsvColumnMapping& aMapping) const
{
	return m_lcColumnSetup.GetColumnMapping(aMapping);
}

void CTDLCsvImportExportDlg::OnChangeCsvdelimiter()
{
	CString sOldDelim = m_sDelim;
	UpdateData();

	if (m_bImporting && m_sDelim != sOldDelim)
	{
		CTDCCsvColumnMapping aMapping;

		if (BuildImportColumnMapping(aMapping))
		{
			m_lcColumnSetup.SetColumnMapping(aMapping);
		}
	}
}

void CTDLCsvImportExportDlg::BuildDefaultMasterColumnMapping()
{
	m_aMasterColumnMapping.RemoveAll();

	for (int nCol = 0; nCol < ATTRIB_COUNT; nCol++)
	{
		TDC_ATTRIBUTE attrib = ATTRIBUTES[nCol].attrib;
		CEnString sName(ATTRIBUTES[nCol].nAttribResID);

		m_aMasterColumnMapping.Add(CSVCOLUMNMAPPING(sName, attrib));
	}
}

int CTDLCsvImportExportDlg::BuildImportColumnMapping(CTDCCsvColumnMapping& aImportMapping) const
{
	ASSERT(m_bImporting);
	ASSERT(!m_sFilePath.IsEmpty());

	// read first few lines from file
	CStringArray aLines;

	if (!FileMisc::LoadFileLines(m_sFilePath, aLines, 5))
	{
		return FALSE;
	}

	CStringArray aColumnHeaders;

	if (!Misc::Split(aLines[0], aColumnHeaders, TRUE, m_sDelim))
	{
		return FALSE;
	}

	// build column mapping from file attributes
	for (int nCol = 0; nCol < aColumnHeaders.GetSize(); nCol++)
	{
		CString sName = aColumnHeaders[nCol];
		ASSERT(!sName.IsEmpty());

		// try to map text column names to column IDs
		aImportMapping.Add(CSVCOLUMNMAPPING(sName, GetMasterColumnAttribute(sName)));
	}

	return aImportMapping.GetSize();
}

int CTDLCsvImportExportDlg::BuildExportColumnMapping(CTDCCsvColumnMapping& aExportMapping) const
{
	ASSERT(!m_bImporting);

	// build column mapping from passed in attributes
	for (int nAttrib = 0; nAttrib < m_aExportAttributes.GetSize(); nAttrib++)
	{
		TDC_ATTRIBUTE attrib = m_aExportAttributes[nAttrib];
		ASSERT(attrib != TDCA_NONE);

		// try to map text column names to column IDs
		aExportMapping.Add(CSVCOLUMNMAPPING(GetMasterColumnName(attrib), attrib));
	}

	return aExportMapping.GetSize();
}

void CTDLCsvImportExportDlg::OnOK()
{
	CDialog::OnOK();

	// save attribute mapping
	UpdateMasterColumnMappingFromList();
	SaveMasterColumnMapping();
}

int CTDLCsvImportExportDlg::LoadMasterColumnMapping()
{
	BuildDefaultMasterColumnMapping();

	CPreferences prefs;

	m_bAlwaysExportTaskIDs = prefs.GetProfileInt(_T("CsvColumnMapping"), _T("AlwaysExportTaskIDs"), TRUE);
	int nColumns = prefs.GetProfileInt(_T("CsvColumnMapping"), _T("ColumnCount"), 0);

	// overwrite with translations unless they are empty names
	for (int nCol = 0; nCol < nColumns; nCol++)
	{
		CString sKey;

		sKey.Format(_T("ColumnAttrib%d"), nCol);
		TDC_ATTRIBUTE attrib = (TDC_ATTRIBUTE)prefs.GetProfileInt(_T("CsvColumnMapping"), sKey, TDCA_NONE);

		sKey.Format(_T("ColumnName%d"), nCol);
		CString sName = prefs.GetProfileString(_T("CsvColumnMapping"), sKey);

		if (!sName.IsEmpty())
		{
			SetMasterColumnName(attrib, sName);
		}
	}

	// load delimiter if different to default
	CString sDelim = prefs.GetProfileString(_T("CsvColumnMapping"), _T("Delimiter"));

	if (!sDelim.IsEmpty())
	{
		m_sDelim = sDelim;
		UpdateData(FALSE);
	}

	return m_aMasterColumnMapping.GetSize();
}

void CTDLCsvImportExportDlg::UpdateMasterColumnMappingFromList()
{
	// get mapping from list ctrl and update names in master mapping
	CTDCCsvColumnMapping aListMapping;
	int nListRows = m_lcColumnSetup.GetColumnMapping(aListMapping);

	for (int nRow = 0; nRow < nListRows; nRow++)
	{
		SetMasterColumnName(aListMapping[nRow].nTDCAttrib, aListMapping[nRow].sColumnName);
	}
}

void CTDLCsvImportExportDlg::SaveMasterColumnMapping() const
{
	CPreferences prefs;

	prefs.WriteProfileInt(_T("CsvColumnMapping"), _T("AlwaysExportTaskIDs"), m_bAlwaysExportTaskIDs);

	int nColumns = m_aMasterColumnMapping.GetSize();
	prefs.WriteProfileInt(_T("CsvColumnMapping"), _T("ColumnCount"), nColumns);

	for (int nCol = 0; nCol < nColumns; nCol++)
	{
		CString sKey;

		sKey.Format(_T("ColumnName%d"), nCol);
		prefs.WriteProfileString(_T("CsvColumnMapping"), sKey, m_aMasterColumnMapping[nCol].sColumnName);

		sKey.Format(_T("ColumnAttrib%d"), nCol);
		prefs.WriteProfileInt(_T("CsvColumnMapping"), sKey, m_aMasterColumnMapping[nCol].nTDCAttrib);
	}

	// save delimiter if different to default
	if (m_sDelim == Misc::GetListSeparator())
	{
		prefs.WriteProfileString(_T("CsvColumnMapping"), _T("Delimiter"), _T(""));
	}
	else
	{
		prefs.WriteProfileString(_T("CsvColumnMapping"), _T("Delimiter"), m_sDelim);
	}
}

CString CTDLCsvImportExportDlg::GetMasterColumnName(TDC_ATTRIBUTE attrib) const
{
	int nCol = FindMasterColumn(attrib);
	return (nCol == -1) ? _T("") : m_aMasterColumnMapping[nCol].sColumnName;
}

TDC_ATTRIBUTE CTDLCsvImportExportDlg::GetMasterColumnAttribute(LPCTSTR szColumn) const
{
	int nCol = FindMasterColumn(szColumn);
	return (nCol == -1) ? TDCA_NONE : m_aMasterColumnMapping[nCol].nTDCAttrib;
}

void CTDLCsvImportExportDlg::SetMasterColumnAttribute(LPCTSTR szColumn, TDC_ATTRIBUTE attrib)
{
	// check if attribute is already in use
	int nAttribCol = FindMasterColumn(attrib);
	int nNameCol = FindMasterColumn(szColumn);

	// and clear if it is
	if (nAttribCol != -1 && nAttribCol != nNameCol)
	{
		m_aMasterColumnMapping[nNameCol].nTDCAttrib = TDCA_NONE;
	}

	if (nNameCol != -1)
	{
		m_aMasterColumnMapping[nNameCol].nTDCAttrib = attrib;
	}
}

void CTDLCsvImportExportDlg::SetMasterColumnName(TDC_ATTRIBUTE attrib, LPCTSTR szColumn)
{
	// prevent setting the master mapping to an empty name
	if (szColumn && *szColumn)
	{
		// check if the column name is already in use
		int nNameCol = FindMasterColumn(szColumn);
		int nAttribCol = FindMasterColumn(attrib);

		// and clear if it is
		if (nNameCol != -1 && nNameCol != nAttribCol)
		{
			m_aMasterColumnMapping[nNameCol].sColumnName.Empty();
		}

		// and set new name
		if (nAttribCol != -1)
		{
			m_aMasterColumnMapping[nAttribCol].sColumnName = szColumn;
		}
	}
}

int CTDLCsvImportExportDlg::FindMasterColumn(TDC_ATTRIBUTE attrib) const
{
	int nColumns = m_aMasterColumnMapping.GetSize();

	for (int nCol = 0; nCol < nColumns; nCol++)
	{
		if (m_aMasterColumnMapping[nCol].nTDCAttrib == attrib)
		{
			return nCol;
		}
	}

	// else
	return -1;
}

int CTDLCsvImportExportDlg::FindMasterColumn(LPCTSTR szColumn) const
{
	int nColumns = m_aMasterColumnMapping.GetSize();

	for (int nCol = 0; nCol < nColumns; nCol++)
	{
		if (m_aMasterColumnMapping[nCol].sColumnName.CompareNoCase(szColumn) == 0)
		{
			return nCol;
		}
	}

	// else
	return -1;
}

void CTDLCsvImportExportDlg::OnExportTaskids()
{
	ASSERT(!m_bImporting);

	UpdateData();

	CTDCCsvColumnMapping aMapping;
	m_lcColumnSetup.GetColumnMapping(aMapping);

	if (!m_bAlwaysExportTaskIDs)
	{
		// Find if these attributes were originally present
		BOOL bWantTaskID = FALSE, bWantParentID = FALSE;

		for (int nAttrib = 0; nAttrib < m_aExportAttributes.GetSize(); nAttrib++)
		{
			if (!bWantTaskID)
			{
				bWantTaskID = (m_aExportAttributes[nAttrib] == TDCA_ID);
			}

			if (!bWantParentID)
			{
				bWantParentID = (m_aExportAttributes[nAttrib] == TDCA_PARENTID);
			}
		}

		// if attribute was not present in original attributes then remove
		int nCol = aMapping.GetSize();

		while (nCol--)
		{
			TDC_ATTRIBUTE attrib = aMapping[nCol].nTDCAttrib;

			if (attrib == TDCA_ID && !bWantTaskID)
			{
				aMapping.RemoveAt(nCol);
			}

			else if (attrib == TDCA_PARENTID && !bWantParentID)
			{
				aMapping.RemoveAt(nCol);
			}
		}
	}
	else // always include
	{
		// find out what's already present
		int nTaskID = -1, nParentTaskID = -1;

		for (int nCol = 0; nCol < aMapping.GetSize() && (nTaskID == -1 || nParentTaskID == -1); nCol++)
		{
			TDC_ATTRIBUTE attrib = aMapping[nCol].nTDCAttrib;

			if (attrib == TDCA_ID)
			{
				nTaskID = nCol;
			}

			else if (attrib == TDCA_PARENTID)
			{
				nParentTaskID = nCol;
			}
		}

		// Add TaskID and/or ParentTaskID if not present
		if (nTaskID == -1)
		{
			aMapping.Add(CSVCOLUMNMAPPING(GetMasterColumnName(TDCA_ID), TDCA_ID));
		}

		if (nParentTaskID == -1)
		{
			aMapping.Add(CSVCOLUMNMAPPING(GetMasterColumnName(TDCA_PARENTID), TDCA_PARENTID));
		}
	}

	m_lcColumnSetup.SetColumnMapping(aMapping);
}
