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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// TimeEdit.h: interface for the CTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_)
#define AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CodeProject/Source/EnEdit.h"
#include "../../CodeProject/Source/TimeHelper.h"

const UINT WM_TEN_UNITSCHANGE = ::RegisterWindowMessage(_T("WM_TEN_UNITSCHANGE")); // wParam == <CtrlID>, lParam = <prev units>

const int TEBTN_UNITS = 1;

class CTimeEdit : public CEnEdit, public CTimeHelper
{
public:
	CTimeEdit(int nUnits = THU_HOURS, int nMaxDecPlaces = 9);
	virtual ~CTimeEdit();

	double GetTime() const;
	double GetTime(int nUnits) const;

	void SetTime(double dTime);
	void SetTime(double dTime, int nUnits);

	inline int GetUnits() const
	{
		return m_nUnits;
	}
	void SetUnits(int nUnits);

	inline int GetMaxDecimalPlaces() const
	{
		return m_nMaxDecPlaces;
	}
	void SetMaxDecimalPlaces(int nMaxDecPlaces);

	CString FormatTimeHMS() const;
	CString FormatTime(BOOL bUnits) const;

	void EnableNegativeTimes(BOOL bEnable);

	static void SetUnits(int nUnits, LPCTSTR szLongUnits, LPCTSTR szAbbrevUnits);

protected:
	int m_nUnits, m_nMaxDecPlaces;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeEdit)
	//}}AFX_VIRTUAL
	virtual void PreSubclassWindow();
	virtual void OnBtnClick(UINT nID);
	virtual void OnSetReadOnly(BOOL bReadOnly);

// Implementation

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	static void RemoveTrailingZeros(CString& sTime);
};

#endif // !defined(AFX_TIMEEDIT_H__2CCFE44D_9578_4E38_B2BF_091C172C85A5__INCLUDED_)
