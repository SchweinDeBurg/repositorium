// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#include "StdAfx.h"
#include "CalendarExtResource.h"
#include "../../Common/CalendarDefines.h"
#include "DlgCompletedTasks.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"
#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TASK_FONT_NAME          _T("MS Shell Dlg 2")
#define TASK_FONT_SIZE          7

/////////////////////////////////////////////////////////////////////////////
// CDlgCompletedTasks dialog


CDlgCompletedTasks::CDlgCompletedTasks(DWORD _dwFlags, CWnd* _pParent):
CDialog(CDlgCompletedTasks::IDD, _pParent),
m_dwFlags(_dwFlags)
{
	//{{AFX_DATA_INIT(CDlgCompletedTasks)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgCompletedTasks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCompletedTasks)
	DDX_Control(pDX, IDC_STATIC_SAMPLE_TEXT, m_staticSampleText);
	DDX_Control(pDX, IDC_CHK_COMPLETED_TASKS_HIDDEN, m_chkHidden);
	DDX_Control(pDX, IDC_CHK_COMPLETED_TASKS_GREY, m_chkGrey);
	DDX_Control(pDX, IDC_CHK_COMPLETED_TASKS_STRIKETHRU, m_chkStrikethru);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCompletedTasks, CDialog)
	//{{AFX_MSG_MAP(CDlgCompletedTasks)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHK_COMPLETED_TASKS_HIDDEN, OnChangeCheckbox)
	ON_BN_CLICKED(IDC_CHK_COMPLETED_TASKS_GREY, OnChangeCheckbox)
	ON_BN_CLICKED(IDC_CHK_COMPLETED_TASKS_STRIKETHRU, OnChangeCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


DWORD CDlgCompletedTasks::GetFlags() const
{
	return m_dwFlags;
}

void CDlgCompletedTasks::UpdateFlags()
{
	m_dwFlags = 0;

	BOOL bHidden = m_chkHidden.GetCheck();
	BOOL bGrey = m_chkGrey.GetCheck();
	BOOL bStrikethru = m_chkStrikethru.GetCheck();

	if (bHidden)
	{
		m_dwFlags = COMPLETEDTASKS_HIDDEN;
	}
	else
	{
		if (bGrey)
		{
			m_dwFlags |= COMPLETEDTASKS_GREY;
		}

		if (bStrikethru)
		{
			m_dwFlags |= COMPLETEDTASKS_STRIKETHRU;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCompletedTasks message handlers

BOOL CDlgCompletedTasks::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_dwFlags & COMPLETEDTASKS_HIDDEN)
	{
		m_chkHidden.SetCheck(1);
	}

	if (m_dwFlags & COMPLETEDTASKS_GREY)
	{
		m_chkGrey.SetCheck(1);
	}

	if (m_dwFlags & COMPLETEDTASKS_STRIKETHRU)
	{
		m_chkStrikethru.SetCheck(1);
	}

	OnChangeCheckbox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//this overridden to custom-draw the "completed task" preview pane
void CDlgCompletedTasks::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COLORREF crText = RGB(0, 0, 0);
	if (m_dwFlags & COMPLETEDTASKS_GREY)
	{
		//grey
		crText = RGB(200, 200, 200);
	}

	DWORD dwFontFlags = MFS_BOLD;
	if (m_dwFlags & COMPLETEDTASKS_STRIKETHRU)
	{
		//strikethru
		dwFontFlags |= MFS_STRIKETHRU;
	}

	//select GDI objects
	CFont font;
	GraphicsMisc::CreateFont(font, TASK_FONT_NAME, TASK_FONT_SIZE, dwFontFlags);
	CFont* pOldFont = dc.SelectObject(&font);

	COLORREF crOldFG = dc.SetTextColor(crText);
	int nOldBkMode = dc.SetBkMode(TRANSPARENT);

	//get rect of box
	CRect rcPreviewPane;
	m_staticSampleText.GetWindowRect(rcPreviewPane);
	ScreenToClient(rcPreviewPane);

	//fill box in white
	CBrush br;
	br.CreateSolidBrush(RGB(255, 255, 255));
	dc.FillRect(&rcPreviewPane, &br);

	BOOL bHidden = (m_dwFlags & COMPLETEDTASKS_HIDDEN);
	if (!bHidden)
	{
		//draw sample text in center of box
		dc.DrawText(_T("completed task"), rcPreviewPane, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	//restore previous GDI values
	dc.SelectObject(pOldFont);
	dc.SetTextColor(crOldFG);
	dc.SetBkMode(nOldBkMode);
}

void CDlgCompletedTasks::OnChangeCheckbox()
{
	UpdateFlags();

	//enable/disable checkboxes due to the hidden flag
	BOOL bHidden = (m_dwFlags & COMPLETEDTASKS_HIDDEN);
	m_chkGrey.EnableWindow(!bHidden);
	m_chkStrikethru.EnableWindow(!bHidden);

	Invalidate();
}

void CDlgCompletedTasks::OnOK()
{
	UpdateFlags();

	CDialog::OnOK();
}
