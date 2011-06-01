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

// TDLAddLoggedTimeDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLAddLoggedTimeDlg.h"

#include "../../../CodeProject/Source/DateHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLAddLoggedTimeDlg dialog

CTDLAddLoggedTimeDlg::CTDLAddLoggedTimeDlg(DWORD dwTaskID, LPCTSTR szTaskTitle, CWnd* pParent /*=NULL*/):
CDialog(CTDLAddLoggedTimeDlg::IDD, pParent),
m_cbTimeWhen(TCB_HALFHOURS)
{
	//{{AFX_DATA_INIT(CTDLAddLoggedTimeDlg)
	m_dLoggedTime = 0.0;
	m_dwTaskID = dwTaskID;
	m_sTaskTitle = szTaskTitle;
	//}}AFX_DATA_INIT
	m_nUnits = THU_MINS;
	m_dtWhen = COleDateTime::GetCurrentTime();
}

void CTDLAddLoggedTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLAddLoggedTimeDlg)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dateWhen);
	DDX_Control(pDX, IDC_COMBO1, m_cbTimeWhen);
	DDX_Text(pDX, IDC_LOGGEDTIME, m_dLoggedTime);
	DDX_Text(pDX, IDC_TASKID, m_dwTaskID);
	DDX_Text(pDX, IDC_TASKTITLE, m_sTaskTitle);
	DDX_Control(pDX, IDC_LOGGEDTIME, m_eLoggedTime);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		m_nUnits = m_eLoggedTime.GetUnits();

		m_dateWhen.GetTime(m_dtWhen);
		COleDateTime time = m_cbTimeWhen.GetOleTime();

		m_dtWhen = CDateHelper::GetDateOnly(m_dtWhen) + time;
	}
	else
	{
		m_eLoggedTime.SetUnits(m_nUnits);

		m_dateWhen.SetTime(m_dtWhen);
		m_cbTimeWhen.SetOleTime(CDateHelper::GetTimeOnly(m_dtWhen));
	}
}

BEGIN_MESSAGE_MAP(CTDLAddLoggedTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLAddLoggedTimeDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLAddLoggedTimeDlg message handlers

double CTDLAddLoggedTimeDlg::GetLoggedTime() const // in hours
{
	return CTimeHelper().GetTime(m_dLoggedTime, m_nUnits, THU_HOURS);
}

COleDateTime CTDLAddLoggedTimeDlg::GetWhen() const
{
	return m_dtWhen;
}
