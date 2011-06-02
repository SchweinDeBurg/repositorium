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

// DateHelper.h: interface for the CDateHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
#define AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum DH_DATE
{
	DHD_TODAY,
	DHD_TOMORROW,
	DHD_ENDTHISWEEK, // start of week + 7
	DHD_ENDNEXTWEEK, // DHD_ENDTHISWEEK + 7
	DHD_ENDTHISMONTH, // beginning of next month - 1
	DHD_ENDNEXTMONTH, // get's trickier :)
	DHD_ENDTHISYEAR,
	DHD_ENDNEXTYEAR,
};

enum DH_UNITS
{
	DHU_DAYS	= 'D',
	DHU_WEEKS	= 'W',
	DHU_MONTHS	= 'M',
	DHU_YEARS	= 'Y',
};

enum
{
	DHFD_ISO	= 0x0001,
	DHFD_DOW	= 0x0002,
	DHFD_TIME	= 0x0004,
	DHFD_NOSEC	= 0x0008,
};

class CDateHelper
{
public:
	static int CalcDaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive);
	static int CalcDaysFromTo(const COleDateTime& dateFrom, DH_DATE nTo, BOOL bInclusive);
	static int CalcDaysFromTo(DH_DATE nFrom, DH_DATE nTo, BOOL bInclusive);
	static double GetDate(DH_DATE nDate); // 12am

	static int CalcWeekdaysFromTo(const COleDateTime& dateFrom, const COleDateTime& dateTo, BOOL bInclusive);
	static void OffsetDate(COleDateTime& date, int nAmount, DH_UNITS nUnits);

	// DOW = 'day of week'
	static CString FormatDate(const COleDateTime& date, DWORD dwFlags = 0);
	static CString FormatCurrentDate(DWORD dwFlags = 0);

	static BOOL DecodeISODate(const CString& sDate, COleDateTime& date);
	static BOOL DecodeISODate(const CString& sDate, time_t& date);

	static BOOL DecodeLocalShortDate(const CString& sDate, COleDateTime& date);
	static BOOL DecodeLocalShortDate(const CString& sDate, time_t& date);

	static int FirstDayOfWeek();
	static int LastDayOfWeek();
	static int NextDayOfWeek(int nDOW);
	static int GetDaysInMonth(int nMonth, int nYear); // 1-12

	static CString GetWeekday(int nWeekday, BOOL bShort); // 1-7, sun-sat
	static CString GetMonth(int nMonth, BOOL bShort); // 1-12, jan-nov
	static void GetWeekdays(BOOL bShort, CStringArray& aWeekDays); // sun-sat
	static void GetMonths(BOOL bShort, CStringArray& aMonths); // jan-nov
	static int CalcLongestWeekday(CDC* pDC, BOOL bShort);

	static BOOL IsLeapYear(const COleDateTime& date = COleDateTime::GetCurrentTime());
	static BOOL IsLeapYear(int nYear);

	static void SplitDate(const COleDateTime& date, double& dDateOnly, double& dTimeOnly);
	static double MakeDate(const COleDateTime& dtTimeOnly, const COleDateTime& dtDateOnly);

	static double GetTimeOnly(const COleDateTime& date);
	static double GetDateOnly(const COleDateTime& date);
};

#endif // !defined(AFX_DATEHELPER_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
