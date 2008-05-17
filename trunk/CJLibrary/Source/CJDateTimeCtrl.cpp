// CJDateTimeCtrl.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new 
// IE4 controls' http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:53 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJDateTimeCtrl.cpp $
 *
 * $History: CJDateTimeCtrl.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:53
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC, Visual C++ 6 and vc5 Tech. Preview. Added to extend
 * functionality to Visual C++ 5 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJDateTimeCtrl.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Helper functions

static BOOL GetAsSystemTime(const CTime& tm, SYSTEMTIME& timeDest)
{
	struct tm* ptm = tm.GetLocalTm(NULL);
	if (ptm == NULL)
		return FALSE;
	
	timeDest.wYear = (WORD) (1900 + ptm->tm_year);
	timeDest.wMonth = (WORD) (1 + ptm->tm_mon);
	timeDest.wDay = (WORD) ptm->tm_mday;
	timeDest.wHour = (WORD) ptm->tm_hour;
	timeDest.wMinute = (WORD) ptm->tm_min;
	timeDest.wSecond = (WORD) ptm->tm_sec;
	timeDest.wMilliseconds = 0;
	
	return TRUE;
}

static BOOL GetAsSystemTime(const CTime* ptm, SYSTEMTIME& timeDest)
{
	return GetAsSystemTime(*ptm, timeDest);
}

// Verifies will fail if the needed buffer size is too large
#define MAX_TIME_BUFFER_SIZE    128         // matches that in timecore.cpp
#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
static int nMonthDays[13] =
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest)
{
	// The legal range does not actually span year 0 to 9999.
	if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
		return FALSE;
	
	long nDays;             // Number of days since Dec. 30, 1899
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
	long nMinutesInDay;     // Minutes in day
	
	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
	//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)
	BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year
	
	double dblDate = dtSrc; // tempory serial date
	
	// If a valid date, then this conversion should not overflow
	nDays = (long)dblDate;
	
	// Round to the second
	dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);
	
	nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899
	
	dblDate = fabs(dblDate);
	nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);
	
	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;
	
	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);
	
	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;
	
	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);
	
	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;
		
		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);
		
		if (n4Years != 0)
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = FALSE;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}
	
	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;
		
		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}
	
	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	tmDest.tm_yday = (int)n4Day + 1;
	tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;
	
	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			tmDest.tm_mon = 2;
			tmDest.tm_mday = 29;
			goto DoTime;
		}
		
		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}
	
	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;
	
	// Month number always >= n/32, so save some loop time */
	for (tmDest.tm_mon = (n4Day >> 5) + 1;
	n4Day > nMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);
    
    tmDest.tm_mday = (int)(n4Day - nMonthDays[tmDest.tm_mon-1]);
    
DoTime:
    if (nSecsInDay == 0)
		tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
    else
    {
		tmDest.tm_sec = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		tmDest.tm_min = (int)nMinutesInDay % 60;
		tmDest.tm_hour = (int)nMinutesInDay / 60;
    }
    
    return TRUE;
}

static BOOL GetAsSystemTime(const COleDateTime& dtm, SYSTEMTIME& sysTime)
{
	BOOL bRetVal = FALSE;
	if (dtm.GetStatus() == COleDateTime::valid)
	{
		struct tm tmTemp;
		if (TmFromOleDate(dtm, tmTemp))
		{
			sysTime.wYear = (WORD) tmTemp.tm_year;
			sysTime.wMonth = (WORD) tmTemp.tm_mon;
			sysTime.wDayOfWeek = (WORD) tmTemp.tm_wday;
			sysTime.wDay = (WORD) tmTemp.tm_mday;
			sysTime.wHour = (WORD) tmTemp.tm_hour;
			sysTime.wMinute = (WORD) tmTemp.tm_min;
			sysTime.wSecond = (WORD) tmTemp.tm_sec;
			sysTime.wMilliseconds = 0;
			
			bRetVal = TRUE;
		}
	}
	
	return bRetVal;
}

static BOOL GetAsSystemTime(const COleDateTime* pdtm, SYSTEMTIME& sysTime)
{
	return GetAsSystemTime(*pdtm, sysTime);
}

/////////////////////////////////////////////////////////////////////////////
// CCJDateTimeCtrl

BOOL CCJDateTimeCtrl::Create(DWORD dwStyle, const RECT& rect,
                           CWnd* pParentWnd, UINT nID)
{
   // initialize common controls
#ifdef _VC_VERSION_5
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));
#else
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_DATE_REG));
#endif

	CWnd* pWnd = this;
	return pWnd->Create(DATETIMEPICK_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
}

DWORD CCJDateTimeCtrl::GetRange(CTime* pMinTime, CTime* pMaxTime) const
{
	ASSERT(::IsWindow(m_hWnd));
	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwResult = ::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (pMinTime != NULL)
	{
		if (dwResult & GDTR_MIN)
			*pMinTime = CTime(sysTimes[0]);
	}

	if (pMaxTime != NULL)
	{
		if (dwResult & GDTR_MAX)
			*pMaxTime = CTime(sysTimes[1]);
	}

	return dwResult;

}

DWORD CCJDateTimeCtrl::GetRange(COleDateTime* pMinTime,
	COleDateTime* pMaxTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwResult = ::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM) &sysTimes);
	if (pMinTime != NULL)
	{
		if (dwResult & GDTR_MIN)
			*pMinTime = COleDateTime(sysTimes[0]);
		else
			pMinTime->SetStatus(COleDateTime::null);
	}

	if (pMaxTime != NULL)
	{
		if (dwResult & GDTR_MAX)
			*pMaxTime = COleDateTime(sysTimes[1]);
		else
			pMaxTime->SetStatus(COleDateTime::null);
	}

	return dwResult;
}

BOOL CCJDateTimeCtrl::SetRange(const CTime* pMinTime, const CTime* pMaxTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];

	WPARAM wFlags = 0;
	if (pMinTime != NULL && GetAsSystemTime(pMinTime, sysTimes[0]))
		wFlags |= GDTR_MIN;

	if (pMaxTime != NULL && GetAsSystemTime(pMaxTime, sysTimes[1]))
		wFlags |= GDTR_MAX;

	return (BOOL) ::SendMessage(m_hWnd, DTM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

BOOL CCJDateTimeCtrl::SetRange(const COleDateTime* pMinTime, const COleDateTime* pMaxTime)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pMinTime == NULL || pMinTime->GetStatus() != COleDateTime::invalid);
	ASSERT(pMaxTime == NULL || pMaxTime->GetStatus() != COleDateTime::invalid);

	SYSTEMTIME sysTime[2];

	WPARAM wFlags = 0;
	if (pMinTime != NULL && pMinTime->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMinTime, sysTime[0]))
			wFlags |= GDTR_MIN;
	}

	if (pMaxTime != NULL && pMaxTime->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMaxTime, sysTime[1]))
			wFlags |= GDTR_MAX;
	}

	return (BOOL) ::SendMessage(m_hWnd, DTM_SETRANGE, wFlags, (LPARAM) &sysTime);
}

BOOL CCJDateTimeCtrl::SetTime(LPSYSTEMTIME pTimeNew /* = NULL */)
{
	ASSERT(::IsWindow(m_hWnd));
	WPARAM wParam = (pTimeNew == NULL) ? GDT_NONE : GDT_VALID;
	return (BOOL) ::SendMessage(m_hWnd, DTM_SETSYSTEMTIME,
		wParam, (LPARAM) pTimeNew);
}

BOOL CCJDateTimeCtrl::SetTime(const COleDateTime& timeNew)
{
	BOOL bRetVal = FALSE;

	// make sure the time isn't invalid
	ASSERT(timeNew.GetStatus() != COleDateTime::invalid);
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	WPARAM wParam = GDT_NONE;
	if (timeNew.GetStatus() == COleDateTime::valid &&
		GetAsSystemTime(timeNew, sysTime))
	{
		wParam = GDT_VALID;
	}

	bRetVal = (BOOL) ::SendMessage(m_hWnd,
			DTM_SETSYSTEMTIME, wParam, (LPARAM) &sysTime);

	return bRetVal;
}

BOOL CCJDateTimeCtrl::SetTime(const CTime* pTimeNew)
{
	BOOL bRetVal = FALSE;

	// make sure the time isn't invalid
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	WPARAM wParam = GDT_NONE;
	if (pTimeNew != NULL && GetAsSystemTime(pTimeNew, sysTime))
	{
		wParam = GDT_VALID;
	}

	bRetVal = (BOOL) ::SendMessage(m_hWnd,
			DTM_SETSYSTEMTIME, wParam, (LPARAM) &sysTime);

	return bRetVal;
}

BOOL CCJDateTimeCtrl::GetTime(COleDateTime& timeDest) const
{
	SYSTEMTIME sysTime;
	BOOL bRetVal = TRUE;

	LRESULT result = ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime);
	if (result == GDT_VALID)
	{
		timeDest = COleDateTime(sysTime);
		bRetVal = TRUE;
		ASSERT(timeDest.GetStatus() == COleDateTime::valid);
	}
	else if (result == GDT_NONE)
	{
		timeDest.SetStatus(COleDateTime::null);
		bRetVal = TRUE;
	}
	else
		timeDest.SetStatus(COleDateTime::invalid);
	return bRetVal;
}

DWORD CCJDateTimeCtrl::GetTime(CTime& timeDest) const
{
	SYSTEMTIME sysTime;
	DWORD dwResult = (DWORD)
		::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime);

	if (dwResult == GDT_VALID)
		timeDest = CTime(sysTime);

	return dwResult;
}

CCJDateTimeCtrl::~CCJDateTimeCtrl()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CCJMonthCalCtrl

BOOL CCJMonthCalCtrl::Create(DWORD dwStyle, const RECT& rect,
	CWnd* pParentWnd, UINT nID)
{
	// initialize common controls
#ifdef _VC_VERSION_5
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));
#else
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_DATE_REG));
#endif

	CWnd* pWnd = this;
	return pWnd->Create(MONTHCAL_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
}

BOOL CCJMonthCalCtrl::Create(DWORD dwStyle, const POINT& pt,
	CWnd* pParentWnd, UINT nID)
{
	BOOL bWasVisible = (dwStyle & WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	CRect rect(pt.x, pt.y, 0, 0);

	BOOL bRetVal = FALSE;
   if (Create(dwStyle, rect, pParentWnd, nID))
	{
		if (SizeMinReq())
		{
			if (bWasVisible)
				ShowWindow(SW_SHOWNA);
			bRetVal = TRUE;
		}
		else
			DestroyWindow();
	}

	return bRetVal;
}

BOOL CCJMonthCalCtrl::SizeMinReq(BOOL bRepaint /* = TRUE */)
{
	CRect rect;
	BOOL bRetVal = FALSE;
	if (GetMinReqRect(rect))
	{
		DWORD dwFlags = SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOMOVE | SWP_NOACTIVATE;
		if (!bRepaint)
			dwFlags |= SWP_NOREDRAW;
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), dwFlags);
		bRetVal = TRUE;
	}

	return bRetVal;
}

void CCJMonthCalCtrl::SetToday(const COleDateTime& refTime)
{
	ASSERT_VALID(this);

	// make sure the time isn't invalid
	ASSERT(refTime.GetStatus() != COleDateTime::invalid);
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	LPSYSTEMTIME pSysTime = NULL;
	WPARAM wParam = GDT_NONE;

	// if the passed time is null or out of range,
	// we'll set the control to NULL

	if (GetAsSystemTime(refTime, sysTime))
	{
		pSysTime = &sysTime;
		wParam = GDT_VALID;
	}

	if (::IsWindow(m_hWnd))
		::SendMessage(m_hWnd, MCM_SETTODAY, wParam, (LPARAM) pSysTime);
}

void CCJMonthCalCtrl::SetToday(const CTime* pDateTime)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT_VALID(this);

	// if the passed time is NULL, we'll set the
	// control to NULL

	WPARAM wParam = GDT_NONE;
	LPSYSTEMTIME pSysTime = NULL;
	SYSTEMTIME sysTime;

	if (pDateTime != NULL && GetAsSystemTime(pDateTime, sysTime))
	{
		wParam = GDT_VALID;
		pSysTime = &sysTime;
	}

	if (::IsWindow(m_hWnd))
		::SendMessage(m_hWnd, MCM_SETTODAY, wParam, (LPARAM) pSysTime);
}

BOOL CCJMonthCalCtrl::SetCurSel(const COleDateTime& refTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	BOOL bRetVal = FALSE;

	// if the passed time is null or out of range,
	// we'll set the control to NULL

	if (GetAsSystemTime(refTime, sysTime) &&
		refTime.GetStatus() == COleDateTime::valid)
	{
		bRetVal = (BOOL)
			::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) &sysTime);
	}

	return bRetVal;
}

BOOL CCJMonthCalCtrl::SetCurSel(const CTime& refTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	BOOL bRetVal = FALSE;

	if (GetAsSystemTime(refTime, sysTime))
	{
		bRetVal = (BOOL)
			::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) &sysTime);
	}

	return bRetVal;
}

BOOL CCJMonthCalCtrl::GetCurSel(COleDateTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = COleDateTime(sysTime);

	return bResult;
}

BOOL CCJMonthCalCtrl::GetToday(COleDateTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = COleDateTime(sysTime);

	return bResult;
}

BOOL CCJMonthCalCtrl::GetCurSel(CTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = CTime(sysTime);

	return bResult;
}

BOOL CCJMonthCalCtrl::GetToday(CTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = CTime(sysTime);

	return bResult;
}

CCJMonthCalCtrl::~CCJMonthCalCtrl()
{
	DestroyWindow();
}

int CCJMonthCalCtrl::GetFirstDayOfWeek(BOOL* pbLocal /* = NULL */) const
{
	ASSERT(::IsWindow(m_hWnd));
	DWORD dwResult;
	dwResult = (DWORD) ::SendMessage(m_hWnd, MCM_GETFIRSTDAYOFWEEK, 0, 0);

	// set *pbLocal to reflect if the first day of week
	// matches current locale setting

	if (pbLocal)
		*pbLocal = HIWORD(dwResult);
	return LOWORD(dwResult);
}

BOOL CCJMonthCalCtrl::SetFirstDayOfWeek(int iDay, int* lpnOld /* = NULL */)
{
	ASSERT(::IsWindow(m_hWnd));
	DWORD dwResult;
	dwResult = (DWORD) ::SendMessage(m_hWnd, MCM_SETFIRSTDAYOFWEEK, 0, (WPARAM) iDay);

	if (lpnOld != NULL)
		*lpnOld = LOWORD(dwResult);

	return (BOOL) HIWORD(dwResult);
}

BOOL CCJMonthCalCtrl::SetDayState(int nMonths, LPMONTHDAYSTATE pStates)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(AfxIsValidAddress(pStates, nMonths * sizeof(MONTHDAYSTATE), FALSE));
	return (BOOL) ::SendMessage(m_hWnd, MCM_SETDAYSTATE, (WPARAM) nMonths, (LPARAM) pStates);
}

BOOL CCJMonthCalCtrl::SetRange(const COleDateTime* pMinRange,
	const COleDateTime* pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pMinRange == NULL || pMinRange->GetStatus() != COleDateTime::invalid);
	ASSERT(pMaxRange == NULL || pMaxRange->GetStatus() != COleDateTime::invalid);

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;

	if (pMinRange != NULL && pMinRange->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMinRange, sysTimes[0]))
			wFlags |= GDTR_MIN;
	}

	if (pMaxRange != NULL && pMaxRange->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMaxRange, sysTimes[1]))
			wFlags |= GDTR_MAX;
	}

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

BOOL CCJMonthCalCtrl::SetRange(const LPSYSTEMTIME pMinRange,
	const LPSYSTEMTIME pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;

	if (pMinRange != NULL)
	{
		memcpy(&sysTimes[0], pMinRange, sizeof(SYSTEMTIME));
		wFlags |= GDTR_MIN;
	}

	if (pMaxRange != NULL)
	{
		memcpy(&sysTimes[1], pMaxRange, sizeof(SYSTEMTIME));
		wFlags |= GDTR_MAX;
	}

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

DWORD CCJMonthCalCtrl::GetRange(COleDateTime* pMinRange,
	COleDateTime* pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
		*pMinRange = COleDateTime(sysTimes[0]);

	if (dwRanges & GDTR_MAX && pMaxRange)
		*pMaxRange = COleDateTime(sysTimes[1]);

	return dwRanges;
}

DWORD CCJMonthCalCtrl::GetRange(LPSYSTEMTIME pMinRange,
	LPSYSTEMTIME pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
		memcpy(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));

	if (dwRanges & GDTR_MAX && pMaxRange)
		memcpy(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));

	return dwRanges;
}

BOOL CCJMonthCalCtrl::SetRange(const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;
	if (pMinRange != NULL && GetAsSystemTime(pMinRange, sysTimes[0]))
		wFlags |= GDTR_MIN;

	if (pMaxRange != NULL && GetAsSystemTime(pMaxRange, sysTimes[1]))
		wFlags |= GDTR_MAX;

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

DWORD CCJMonthCalCtrl::GetRange(CTime* pMinRange, CTime* pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
		*pMinRange = CTime(sysTimes[0]);

	if (dwRanges & GDTR_MAX && pMaxRange)
		*pMaxRange = CTime(sysTimes[1]);

	return dwRanges;
}

int CCJMonthCalCtrl::GetMonthRange(COleDateTime& refMinRange,
	COleDateTime& refMaxRange, DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(dwFlags == GMR_DAYSTATE || dwFlags == GMR_VISIBLE);

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags, (LPARAM) &sysTimes);

	refMinRange = COleDateTime(sysTimes[0]);
	refMaxRange = COleDateTime(sysTimes[1]);

	return nCount;
}

int CCJMonthCalCtrl::GetMonthRange(LPSYSTEMTIME pMinRange,
	LPSYSTEMTIME pMaxRange, DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];

	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags, (LPARAM) &sysTimes);

	memcpy(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));
	memcpy(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));

	return nCount;
}

int CCJMonthCalCtrl::GetMonthRange(CTime& refMinRange, CTime& refMaxRange,
	DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(dwFlags == GMR_DAYSTATE || dwFlags == GMR_VISIBLE);

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags,   (LPARAM) &sysTimes);

	refMinRange = CTime(sysTimes[0]);
	refMaxRange = CTime(sysTimes[1]);

	return nCount;
}

BOOL CCJMonthCalCtrl::GetSelRange(LPSYSTEMTIME pMinRange,
	LPSYSTEMTIME pMaxRange) const
{
	ASSERT(m_hWnd != NULL);
	ASSERT((GetStyle() & MCS_MULTISELECT));

	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];
	BOOL bReturn = (BOOL) ::SendMessage(m_hWnd, MCM_GETSELRANGE,
										0, (LPARAM) &sysTimes);

	if (bReturn)
	{
		memcpy(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));
		memcpy(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));
	}

	return bReturn;
}

BOOL CCJMonthCalCtrl::SetSelRange(const LPSYSTEMTIME pMinRange,
	const LPSYSTEMTIME pMaxRange)
{
	ASSERT(m_hWnd != NULL);
	ASSERT((GetStyle() & MCS_MULTISELECT));

	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];
	memcpy(&sysTimes[0], pMinRange, sizeof(SYSTEMTIME));
	memcpy(&sysTimes[1], pMaxRange, sizeof(SYSTEMTIME));

	return (BOOL) ::SendMessage(m_hWnd, MCM_SETSELRANGE,
		0, (LPARAM) &sysTimes);
}

BOOL CCJMonthCalCtrl::SetSelRange(const COleDateTime& refMinRange,
	const COleDateTime& refMaxRange)
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	BOOL bResult = FALSE;

	if (refMinRange.GetStatus() == COleDateTime::valid &&
		refMinRange.GetStatus() == COleDateTime::valid)
	{
		if (GetAsSystemTime(refMinRange, sysTimes[0]) &&
			GetAsSystemTime(refMaxRange, sysTimes[1]))
		{
			bResult = (BOOL)
				::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)sysTimes);
		}
	}

	return bResult;
}

BOOL CCJMonthCalCtrl::GetSelRange(COleDateTime& refMinRange,
	COleDateTime& refMaxRange) const
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM) &sysTimes);

	if (bResult)
	{
		refMinRange = COleDateTime(sysTimes[0]);
		refMaxRange = COleDateTime(sysTimes[1]);
	}
	return bResult;
}

BOOL CCJMonthCalCtrl::SetSelRange(const CTime& refMinRange,
	const CTime& refMaxRange)
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	BOOL bResult = FALSE;

	if (GetAsSystemTime(refMinRange, sysTimes[0]) &&
		GetAsSystemTime(refMaxRange, sysTimes[1]))
	{
		bResult = (BOOL)
			::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)sysTimes);
	}

	return bResult;
}

BOOL CCJMonthCalCtrl::GetSelRange(CTime& refMinRange, CTime& refMaxRange) const
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM) &sysTimes);

	if (bResult)
	{
		refMinRange = CTime(sysTimes[0]);
		refMaxRange = CTime(sysTimes[1]);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// DDX_ routines

void AFXAPI DDX_CJDateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CCJDateTimeCtrl* pWnd = (CCJDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetTime(value);
	else
		pWnd->SetTime(value);
}

void AFXAPI DDX_CJDateTimeCtrl(CDataExchange* pDX, int nIDC, CTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CCJDateTimeCtrl* pWnd = (CCJDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetTime(value);
	else
		pWnd->SetTime(&value);
}

void AFXAPI DDX_CJMonthCalCtrl(CDataExchange* pDX, int nIDC,
	COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CCJMonthCalCtrl* pWnd = (CCJMonthCalCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetCurSel(value);
	else
		pWnd->SetCurSel(value);
}

void AFXAPI DDX_CJMonthCalCtrl(CDataExchange* pDX, int nIDC, CTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CCJMonthCalCtrl* pWnd = (CCJMonthCalCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetCurSel(value);
	else
		pWnd->SetCurSel(value);
}

void AFXAPI DDV_CJMinMaxDateTime(CDataExchange* pDX, CTime& refValue,
	const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);

	#if _MSC_VER >= 1300		// VC 7.0 (.NET)
		CCJDateTimeCtrl* pWnd = static_cast<CCJDateTimeCtrl*>(pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl) );
	#else						// VC 5.0/6.0
		CCJDateTimeCtrl* pWnd =	(CCJDateTimeCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
	#endif

	if (!pDX->m_bSaveAndValidate)
	{
		if ( (pMinRange != NULL && *pMinRange > refValue) ||
			  (pMaxRange != NULL && *pMaxRange < refValue))
		{
#ifdef _DEBUG
			#if _MSC_VER >= 1300		// VC 7.0 (.NET)
				int nIDC = pDX->m_idLastControl;
			#else						// VC 5.0/6.0
				int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
			#endif

			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
#endif
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

void AFXAPI DDV_CJMinMaxDateTime(CDataExchange* pDX,  COleDateTime& refValue,
	const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);

	#if _MSC_VER >= 1300		// VC 7.0 (.NET)
		CCJDateTimeCtrl* pWnd = static_cast<CCJDateTimeCtrl*>(pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl) );
	#else						// VC 5.0/6.0
		CCJDateTimeCtrl* pWnd = (CCJDateTimeCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
	#endif

	if (!pDX->m_bSaveAndValidate)
	{
		if ( (pMinRange != NULL && *pMinRange > refValue) ||
			  (pMaxRange != NULL && *pMaxRange < refValue))
		{
#if _MSC_VER >= 1300		// VC 7.0 (.NET)
			int nIDC = pDX->m_idLastControl;
#else						// VC 5.0/6.0
			int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
#endif
			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

void AFXAPI DDV_CJMinMaxMonth(CDataExchange* pDX, CTime& refValue,
	const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);

	#if _MSC_VER >= 1300		// VC 7.0 (.NET)
		CCJMonthCalCtrl* pWnd = static_cast<CCJMonthCalCtrl*>(pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl) );
	#else						// VC 5.0/6.0
		CCJMonthCalCtrl* pWnd = (CCJMonthCalCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
	#endif

	if (!pDX->m_bSaveAndValidate)
	{
		if ( (pMinRange != NULL && *pMinRange > refValue) ||
			  (pMaxRange != NULL && *pMaxRange < refValue))
		{
			#ifdef _DEBUG
				#if _MSC_VER >= 1300		// VC 7.0 (.NET)
					int nIDC = pDX->m_idLastControl;
				#else						// VC 5.0/6.0
					int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
				#endif

				TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
			#endif

			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

void AFXAPI DDV_CJMinMaxMonth(CDataExchange* pDX, COleDateTime& refValue,
	const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);

	 #if _MSC_VER >= 1300		// VC 7.0 (.NET)
		CCJMonthCalCtrl* pWnd = static_cast<CCJMonthCalCtrl*>(pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl) );
	#else						// VC 5.0/6.0
		CCJMonthCalCtrl* pWnd = (CCJMonthCalCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
	#endif

	if (!pDX->m_bSaveAndValidate)
	{
		if ( (pMinRange != NULL && *pMinRange > refValue) ||
			  (pMaxRange != NULL && *pMaxRange < refValue))
		{
			#ifdef _DEBUG
				#if _MSC_VER >= 1300		// VC 7.0 (.NET)
					int nIDC = pDX->m_idLastControl;
				#else						// VC 5.0/6.0
					int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
				#endif

				TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
			#endif

			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCJDateTimeCtrl, CWnd)
IMPLEMENT_DYNAMIC(CCJMonthCalCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
