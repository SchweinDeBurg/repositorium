// OptionsDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "PlainTextImportApp.h"
#include "OptionsDlg.h"

#include "../../../CodeProject/Source/EnString.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(BOOL bImport, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OPTIONSDIALOG, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)

	//}}AFX_DATA_INIT
	m_sLabel.LoadString(bImport ? IDS_IMPORTLABEL : IDS_EXPORTLABEL);
	m_sTitle.LoadString(bImport ? IDS_IMPORTTITLE : IDS_EXPORTTITLE);
	m_sProjectLabel.LoadString(bImport ? IDS_IMPORTPROJECTLABEL : IDS_EXPORTPROJECTLABEL);

	m_bIncludeProject = AfxGetApp()->GetProfileInt("Importers\\PlainText", "IncludeProject", FALSE);

	BOOL bSpaceIndent = AfxGetApp()->GetProfileInt("Importers\\PlainText", "UseSpaceIndents", -1);
	int nSpaces = 2;

	if (bSpaceIndent == -1) // first time
	{
		bSpaceIndent = AfxGetApp()->GetProfileInt("Preferences", "UseSpaceIndents", TRUE);
		nSpaces = AfxGetApp()->GetProfileInt("Preferences", "TextIndent", 2);
	}
	else
		nSpaces = AfxGetApp()->GetProfileInt("Importers\\PlainText", "TextIndent", 2);

	if (bSpaceIndent)
		m_nIndent = (int)((log((double)nSpaces) + 0.001) / log((double)2)) + 1;
	else // tabs
		m_nIndent = 0;
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_TABWIDTHS, m_cbIndent);
	DDX_Text(pDX, IDC_CBLABEL, m_sLabel);
	DDX_CBIndex(pDX, IDC_TABWIDTHS, m_nIndent);
	DDX_Check(pDX, IDC_PROJECTINCLUDED, m_bIncludeProject);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		if (m_nIndent == 0)
			m_sIndent = "\t";
		else
		{
			CString sSpaces;
			m_cbIndent.GetLBText(m_nIndent, sSpaces);

			m_sIndent = CString(' ', atoi(sSpaces));
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_sTitle);
	GetDlgItem(IDC_PROJECTINCLUDED)->SetWindowText(m_sProjectLabel);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK()
{
	CDialog::OnOK();

	// save state
	AfxGetApp()->WriteProfileInt("Importers\\PlainText", "UseSpaceIndents", m_nIndent != 0);
	AfxGetApp()->WriteProfileInt("Importers\\PlainText", "TextIndent", m_nIndent == 0 ? 0 : m_sIndent.GetLength());
	AfxGetApp()->WriteProfileInt("Importers\\PlainText", "IncludeProject", m_bIncludeProject);
}
