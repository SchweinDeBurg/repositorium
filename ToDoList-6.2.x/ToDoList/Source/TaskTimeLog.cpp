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
// - merged with ToDoList version 6.1.2-6.1.7 sources
//*****************************************************************************

// TaskTimeLog.cpp: implementation of the CTaskTimeLog class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskTimeLog.h"

#include <locale.h>

#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/DateHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR COLUMNHEADINGS[] = { _T("Task ID"), _T("Title"), _T("Time Spent (Hrs)"), _T("User ID"), _T("End Date/Time"), _T("Start Date/Time") };
const UINT NUM_COLUMNHEADINGS = sizeof(COLUMNHEADINGS) / sizeof(LPCTSTR);

const LPCTSTR LOGFORMAT[] = { _T("%ld"), _T("%s"), _T("%.3f"), _T("%s"), _T("%s"), _T("%s") };
const UINT NUM_LOGFORMATS = sizeof(LOGFORMAT) / sizeof(LPCTSTR);

CTaskTimeLog::CTaskTimeLog(LPCTSTR szRefPath) : m_sRefPath(szRefPath)
{
}

CTaskTimeLog::~CTaskTimeLog()
{
}

BOOL CTaskTimeLog::LogTime(DWORD dwTaskID, LPCTSTR szTaskTitle, double dTime, BOOL bLogSeparately)
{
	return LogTime(dwTaskID, szTaskTitle, dTime, COleDateTime::GetCurrentTime(), bLogSeparately);
}

BOOL CTaskTimeLog::LogTime(DWORD dwTaskID, LPCTSTR szTaskTitle, double dTime, COleDateTime dtWhen, BOOL bLogSeparately)
{
	CString sLogPath = GetLogPath(dwTaskID, bLogSeparately);

	// if the file doesn't exist then we insert the column headings as the first line
	if (!FileMisc::FileExists(sLogPath))
	{
		FileMisc::AppendLineToFile(sLogPath, BuildColumnHeader());
	}

	// then log the time spent
	CString sLog, sLogFormat(BuildRowFormat());

	COleDateTime dtEnd = dtWhen;
	COleDateTime dtStart = dtEnd - COleDateTime(dTime / 24); // dTime is in hours

	sLog.Format(sLogFormat,
		dwTaskID,
		szTaskTitle,
		dTime,
		static_cast<LPCTSTR>(Misc::GetUserName()),
		static_cast<LPCTSTR>(CDateHelper::FormatDate(dtEnd, DHFD_ISO | DHFD_TIME)),
		static_cast<LPCTSTR>(CDateHelper::FormatDate(dtStart, DHFD_ISO | DHFD_TIME)));

	return FileMisc::AppendLineToFile(sLogPath, sLog);
}

CString CTaskTimeLog::GetLogPath(DWORD dwTaskID, BOOL bLogSeparately)
{
	CString sLogPath, sDrive, sFolder, sFileName;

	// use ref filename as the basis for the log filename
	FileMisc::SplitPath(m_sRefPath, &sDrive, &sFolder, &sFileName);

	if (bLogSeparately)
	{
		sLogPath.Format(_T("%s%s%s\\%ld_Log.csv"), (LPCTSTR)sDrive, (LPCTSTR)sFolder, (LPCTSTR)sFileName, dwTaskID);
	}
	else
	{
		sLogPath.Format(_T("%s%s%s_Log.csv"), (LPCTSTR)sDrive, (LPCTSTR)sFolder, (LPCTSTR)sFileName);
	}

	return sLogPath;
}

double CTaskTimeLog::CalcAccumulatedTime(DWORD dwTaskID, BOOL bLogSeparately)
{
	double dTotalTime = 0;
	CStdioFile file;
	CString sLogPath = GetLogPath(dwTaskID, bLogSeparately);

	if (file.Open(sLogPath, CFile::modeRead | CFile::typeText))
	{
		CString sLine;
		double dLogTime;
		DWORD dwLogID;

		while (file.ReadString(sLine))
		{
			// decode it
			//fabio_2005
#if _MSC_VER >= 1400
			if (_stscanf_s(_T("%ld,%.3f"), sLine, &dwLogID, &dLogTime) == 2)
#else
			if (_stscanf(_T("%ld,%.3f"), sLine, &dwLogID, &dLogTime) == 2)
#endif

			{
				if (dwLogID == dwTaskID)
				{
					dTotalTime += dLogTime;
				}
			}
		}
	}

	return dTotalTime;
}

CString CTaskTimeLog::BuildColumnHeader()
{
	ASSERT(NUM_COLUMNHEADINGS == NUM_LOGFORMATS);

	CString sColumnHeader, sDelim = Misc::GetListSeparator();

	for (int nCol = 0; nCol < NUM_COLUMNHEADINGS; nCol++)
	{
		sColumnHeader += COLUMNHEADINGS[nCol];

		if (nCol < NUM_COLUMNHEADINGS - 1)
		{
			sColumnHeader += sDelim + _T('\x20');
		}
	}

	return sColumnHeader;
}

CString CTaskTimeLog::BuildRowFormat()
{
	ASSERT(NUM_COLUMNHEADINGS == NUM_LOGFORMATS);

	CString sRowFormat, sDelim = Misc::GetListSeparator();

	for (int nRow = 0; nRow < NUM_LOGFORMATS; nRow++)
	{
		sRowFormat += LOGFORMAT[nRow];

		if (nRow < NUM_LOGFORMATS - 1)
		{
			sRowFormat += sDelim + _T('\x20');
		}
	}

	return sRowFormat;
}
