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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// TimeHelper.h: interface for the CTimeHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_)
#define AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

enum
{
	THU_MINS    = 'I',
	THU_HOURS   = 'H',
	THU_DAYS    = 'D',
	THU_WEEKS   = 'W',
	THU_MONTHS  = 'M',
	THU_YEARS   = 'Y',
};

class CTimeHelper
{
public:
	CTimeHelper(); // uses statically defined hours and days
	CTimeHelper(double dHoursInDay, double dDaysInWeek);

	double GetTime(double dTime, int nFromUnits, int nToUnits);
	CString FormatTimeHMS(double dTime, int nUnitsFrom, BOOL bDecPlaces = TRUE);
	CString FormatTime(double dTime, int nUnits, int nDecPlaces);
	CString FormatTime(double dTime, int nDecPlaces);

public:
	static BOOL SetHoursInOneDay(double dHours);
	static BOOL SetDaysInOneWeek(double dDays);
	static double GetHoursInOneDay()
	{
		return HOURS2DAYS;
	}
	static double GetDaysInOneWeek()
	{
		return DAYS2WEEKS;
	}
	static void SetUnits(int nUnits, LPCTSTR szUnits);
	static void SetUnits(int nUnits, TCHAR cUnits);
	static TCHAR GetUnits(int nUnits);

	static CString FormatISOTime(int nHour, int nMin, int nSec = 0, BOOL bIncSeconds = FALSE);
	static CString Format24HourTime(int nHour, int nMin, int nSec = 0, BOOL bIncSeconds = FALSE);

protected:
	double m_dHours2Days, m_dDaysToWeeks;

protected:
	static double HOURS2DAYS, DAYS2WEEKS; // user definable
	static CMap<int, int, TCHAR, TCHAR&> MAPUNIT2CH; // user definable

protected:
	static BOOL Compare(int nFromUnits, int nToUnits); // 0=same, -1=nFrom < nTo else 1
	static CString FormatTimeHMS(double dTime, int nUnits, int nLeftOverUnits,
		double dLeftOverMultiplier, BOOL bDecPlaces);
};

#endif // !defined(AFX_TIMEHELPER_H__BA0C1E67_FAAA_4E65_8EF3_01B011ACFBBC__INCLUDED_)
