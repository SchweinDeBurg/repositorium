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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// RecurringTaskEdit.h: interface for the CRecurringTaskEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_)
#define AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../../CodeProject/Source/EnEdit.h"
#include "../../../CodeProject/Source/PropertyPageHost.h"
#include "../../../CodeProject/Source/MonthComboBox.h"
#include "../../../CodeProject/Source/CheckListBoxEx.h"

#include "ToDoCtrlData.h"

const int REBTN_OPTIONS = 1;

class CTDLRecurringTaskEdit : public CEnEdit
{
public:
	CTDLRecurringTaskEdit();
	virtual ~CTDLRecurringTaskEdit();

	void GetRecurrenceOptions(TDIRECURRENCE& tr) const;
	void SetRecurrenceOptions(const TDIRECURRENCE& tr);

	void SetDefaultDate(const COleDateTime& date)
	{
		m_dtDefault = date;
	}

	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
	void DoEdit();

	static CString GetRegularity(const TDIRECURRENCE& tr, BOOL bIncOnce = TRUE);
	static int CalcMaxRegularityWidth(CDC* pDC, BOOL bIncOnce = TRUE);

protected:
	TDIRECURRENCE m_tr;
	BOOL m_bReadOnly;
	COleDateTime m_dtDefault;
	BOOL m_bInOnSetReadOnly;

protected:
	virtual void OnBtnClick(UINT nID);
	virtual void PreSubclassWindow();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecurringTaskEdit)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnSetReadOnly(WPARAM wp, LPARAM lp);
	afx_msg void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg BOOL OnReflectChangeDisplayText();
	DECLARE_MESSAGE_MAP()

protected:
	static BOOL IsDefaultString(const CString& sRegularity);
	static CString GetRegularity(TDI_REGULARITY nRegularity, BOOL bIncOnce = TRUE);
};

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg dialog

class CTDLRecurringTaskOptionDlg : public CDialog
{
// Construction
public:
	CTDLRecurringTaskOptionDlg(const TDIRECURRENCE& tr, const COleDateTime& dtDefault, CWnd* pParent = NULL);   // standard constructor

	void GetRecurrenceOptions(TDIRECURRENCE& tr) const;

protected:
// Dialog Data
	//{{AFX_DATA(CRecurringTaskOptionDlg)
	int     m_nRegularity;
	int     m_bRecalcFromDue;
	int     m_nReuse;
	//}}AFX_DATA
	int     m_nNumDays;
	CCheckListBoxEx m_lbWeekdays;
	int     m_nNumWeeks;
	DWORD m_dwWeekdays;
	CMonthComboBox  m_cbMonths;
	int     m_nMonth;
	int     m_nYearMonthDay;
	int     m_nNumMonths;
	int     m_nMonthDay;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecurringTaskOptionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecurringTaskOptionDlg)
	afx_msg void OnSelchangeRegularity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnChangeDailyValues();
	afx_msg void OnChangeMonthlyValues();
	afx_msg void OnChangeWeeklyValues();
	afx_msg void OnChangeYearlyValues();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL HasValidData();
};

#endif // !defined(AFX_RECURRENCEEDIT_H__4EE655E3_F4B1_44EA_8AAA_39DD459AD8A8__INCLUDED_)
