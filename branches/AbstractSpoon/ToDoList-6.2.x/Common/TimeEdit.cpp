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

// TimeEdit.cpp: implementation of the CTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TimeEdit.h"
#include "StringRes.h"
#include "../../CodeProject/Source/Misc.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int BTNWIDTHDLU = 11;
const int LABELLEN = 10;

struct TIMEUNIT
{
	int nUnits;
	TCHAR szLabel[LABELLEN];
	TCHAR cAbbrLabel;
	UINT nMenuID;
};

enum
{
	ID_MINS = 0x8000,
	ID_HOURS,
	ID_DAYS,
	ID_WEEKS,
	ID_MONTHS,
	ID_YEARS,
};

static TIMEUNIT TIMEUNITS[] =
{
	{ THU_MINS,     _T(""), TIME_MIN_ABBREV,    ID_MINS },
	{ THU_HOURS,    _T(""), TIME_HOUR_ABBREV,   ID_HOURS },
	{ THU_DAYS,     _T(""), TIME_DAY_ABBREV,    ID_DAYS },
	{ THU_WEEKS,    _T(""), TIME_WEEK_ABBREV,   ID_WEEKS },
	{ THU_MONTHS,   _T(""), TIME_MONTH_ABBREV,  ID_MONTHS },
	{ THU_YEARS,    _T(""), TIME_YEAR_ABBREV,   ID_YEARS },
};

static LPCTSTR UNITLABELS[] =
{
	TIME_MINS,
	TIME_HOURS,
	TIME_DAYS,
	TIME_WEEKS,
	TIME_MONTHS,
	TIME_YEARS
};

const int NUM_UNITS = sizeof(TIMEUNITS) / sizeof(TIMEUNIT);

const TIMEUNIT& GetTimeUnit(int nUnits)
{
	int nItem = NUM_UNITS;

	while (nItem--)
	{
		if (TIMEUNITS[nItem].nUnits == nUnits)
		{
			return TIMEUNITS[nItem];
		}
	}

	return TIMEUNITS[0]; // hours
}

/////////////////////////////////////////////////////////////////////////////////////

CTimeEdit::CTimeEdit(int nUnits, int nMaxDecPlaces) : m_nUnits(nUnits), m_nMaxDecPlaces(nMaxDecPlaces)
{
	// init static units
	for (int nUnit = 0; nUnit < NUM_UNITS; nUnit++)
	{
		TIMEUNIT& tu = TIMEUNITS[nUnit];

		if (tu.szLabel[0] == 0) // empty string
		{
			//fabio_2005
#if _MSC_VER >= 1400
			_tcsncpy_s(tu.szLabel, UNITLABELS[nUnit], LABELLEN - 1);
#else
			_tcsncpy(tu.szLabel, UNITLABELS[nUnit], LABELLEN - 1);
#endif

			tu.szLabel[LABELLEN - 1] = 0;
		}
	}

	SetMask(_T(".0123456789"), ME_LOCALIZEDECIMAL);

	CString sLabel;
	sLabel.Format(_T("%c"), GetTimeUnit(nUnits).cAbbrLabel);

	AddButton(TEBTN_UNITS, sLabel, TIME_UNITS, CALC_BTNWIDTH);
	SetDropMenuButton(TEBTN_UNITS);
}

CTimeEdit::~CTimeEdit()
{
}

BEGIN_MESSAGE_MAP(CTimeEdit, CEnEdit)
	//{{AFX_MSG_MAP(CTimeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTimeEdit::EnableNegativeTimes(BOOL bEnable)
{
	if (bEnable)
	{
		SetMask(_T("-.0123456789"), ME_LOCALIZEDECIMAL);
	}
	else
	{
		SetMask(_T(".0123456789"), ME_LOCALIZEDECIMAL);
	}
}

void CTimeEdit::PreSubclassWindow()
{
	CEnEdit::PreSubclassWindow();

	CString sLabel;
	sLabel.Format(_T("%c"), GetTimeUnit(m_nUnits).cAbbrLabel);
	SetButtonCaption(1, sLabel);
}

double CTimeEdit::GetTime() const
{
	CString sTime;
	GetWindowText(sTime);
	return Misc::Atof(sTime);
}

void CTimeEdit::SetTime(double dTime)
{
	CString sTime = CTimeHelper::FormatTime(dTime, m_nMaxDecPlaces);
	RemoveTrailingZeros(sTime);

	SetWindowText(sTime);
}

void CTimeEdit::SetTime(double dTime, int nUnits)
{
	if (dTime != GetTime())
	{
		SetTime(dTime);
		SetUnits(nUnits);
	}
}

void CTimeEdit::SetUnits(int nUnits)
{
	if (nUnits != m_nUnits)
	{
		m_nUnits = nUnits;

		if (GetSafeHwnd())
		{
			CString sLabel;
			sLabel.Format(_T("%c"), GetTimeUnit(nUnits).cAbbrLabel);
			SetButtonCaption(1, sLabel);
		}
	}
}

void CTimeEdit::SetMaxDecimalPlaces(int nMaxDecPlaces)
{
	if (m_nMaxDecPlaces != nMaxDecPlaces)
	{
		m_nMaxDecPlaces = nMaxDecPlaces;

		SetTime(GetTime());
	}
}

void CTimeEdit::OnBtnClick(UINT nID)
{
	if (nID != TEBTN_UNITS)
	{
		return;
	}

	CMenu menu;

	if (menu.CreatePopupMenu())
	{
		int nUnit = 0;
		for (nUnit = 0; nUnit < NUM_UNITS; nUnit++)
		{
			const TIMEUNIT& tu = TIMEUNITS[nUnit];

			menu.AppendMenu(MF_STRING, tu.nMenuID, tu.szLabel);

			if (tu.nUnits == m_nUnits)
			{
				menu.CheckMenuItem(nUnit, MF_CHECKED | MF_BYPOSITION);
			}
		}
		CRect rButton = GetButtonRect(nID);

		TPMPARAMS tpmp;
		tpmp.cbSize = sizeof(TPMPARAMS);
		tpmp.rcExclude = rButton;

		UINT nID = ::TrackPopupMenuEx(menu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
			rButton.right, rButton.top, *this, &tpmp);

		// handle result
		for (nUnit = 0; nUnit < NUM_UNITS; nUnit++)
		{
			const TIMEUNIT& tu = TIMEUNITS[nUnit];

			if (tu.nMenuID == nID)
			{
				if (m_nUnits != tu.nUnits)
				{
					// update the units
					int nPrevUnits = m_nUnits;
					SetUnits(tu.nUnits);

					// inform parent and check for cancel
					if (GetParent()->SendMessage(WM_TEN_UNITSCHANGE, (WPARAM)GetDlgCtrlID(), nPrevUnits))
					{
						SetUnits(nPrevUnits);
					}
				}
				break;
			}
		}
	}
}

double CTimeEdit::GetTime(int nUnits) const
{
	return CTimeHelper().GetTime(GetTime(), m_nUnits, nUnits);
}

CString CTimeEdit::FormatTime(BOOL bUnits) const
{
	return CTimeHelper().FormatTime(GetTime(), bUnits ? m_nUnits : 0, m_nMaxDecPlaces);
}

CString CTimeEdit::FormatTimeHMS() const
{
	return CTimeHelper().FormatTimeHMS(GetTime(), GetUnits(), TRUE);
}

void CTimeEdit::OnSetReadOnly(BOOL bReadOnly)
{
	EnableButton(1, !bReadOnly && IsWindowEnabled());
}

void CTimeEdit::SetUnits(int nUnits, LPCTSTR szLongUnits, LPCTSTR szAbbrevUnits)
{
	for (int nUnit = 0; nUnit < NUM_UNITS; nUnit++)
	{
		TIMEUNIT& tu = TIMEUNITS[nUnit];

		if (tu.nUnits == nUnits)
		{
			if (szLongUnits && *szLongUnits)
			{
				//fabio_2005
#if _MSC_VER >= 1300
				_tcsncpy_s(tu.szLabel, szLongUnits, LABELLEN - 1);
#else
				_tcsncpy(tu.szLabel, szLongUnits, LABELLEN - 1);
#endif

				tu.szLabel[LABELLEN - 1] = 0;
			}

			if (szAbbrevUnits && *szAbbrevUnits)
			{
				tu.cAbbrLabel = szAbbrevUnits[0];
			}
		}
	}

	// abbrev units
	CTimeHelper::SetUnits(nUnits, szAbbrevUnits);
}

void CTimeEdit::RemoveTrailingZeros(CString& sTime)
{
	sTime.TrimRight();

	while (!sTime.IsEmpty())
	{
		int nLen = sTime.GetLength();

		if (sTime[nLen - 1] == _T('0'))
		{
			sTime = sTime.Left(nLen - 1);
		}

		else if (sTime[nLen - 1] == _T('.'))
		{
			sTime = sTime.Left(nLen - 1);
			break;
		}
		else
		{
			break;
		}
	}
}
