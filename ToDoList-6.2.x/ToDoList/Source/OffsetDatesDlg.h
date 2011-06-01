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

#if !defined(AFX_OFFSETDATESDLG_H__9B2E2FE0_370B_41F4_98C1_2D3BB6D1526E__INCLUDED_)
#define AFX_OFFSETDATESDLG_H__9B2E2FE0_370B_41F4_98C1_2D3BB6D1526E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OffsetDatesDlg.h : header file
//

#include "../../../CodeProject/Source/DialogHelper.h"

/////////////////////////////////////////////////////////////////////////////
// COffsetDatesDlg dialog

enum ODD_UNITS { ODDU_DAYS, ODDU_WEEKS, ODDU_MONTHS };

enum
{
	ODD_STARTDATE   = 0x1,
	ODD_DUEDATE     = 0x2,
	ODD_DONEDATE    = 0x4,
};

class COffsetDatesDlg : public CDialog, public CDialogHelper
{
// Construction
public:
	COffsetDatesDlg(CWnd* pParent = NULL);   // standard constructor

	DWORD GetOffsetWhat() const;
	int GetOffsetAmount(ODD_UNITS& nUnits) const;
	BOOL GetOffsetSubtasks() const
	{
		return m_bOffsetSubtasks;
	}

protected:
// Dialog Data
	//{{AFX_DATA(COffsetDatesDlg)
	enum { IDD = IDD_OFFSETDATES_DIALOG };
	BOOL    m_bOffsetStartDate;
	BOOL    m_bOffsetDueDate;
	BOOL    m_bOffsetDoneDate;
	int     m_bForward;
	int     m_nOffsetBy;
	int     m_nOffsetByUnits;
	BOOL    m_bOffsetSubtasks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetDatesDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetDatesDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFSETDATESDLG_H__9B2E2FE0_370B_41F4_98C1_2D3BB6D1526E__INCLUDED_)
