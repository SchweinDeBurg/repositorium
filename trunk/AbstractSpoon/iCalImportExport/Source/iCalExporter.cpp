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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// iCalExporter.cpp: implementation of the CiCalExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "iCalImportExportApp.h"
#include "iCalExporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT ONEDAY = 24 * 60 * 60;

CiCalExporter::CiCalExporter()
{
}

CiCalExporter::~CiCalExporter()
{
}

bool CiCalExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL /*bSilent*/)
{
	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		// header
		WriteString(fileOut, _T("BEGIN:VCALENDAR"));
		WriteString(fileOut, _T("PRODID:iCalExporter (c) AbstractSpoon 2009"));
		WriteString(fileOut, _T("VERSION:2.0.0"));

		// export first task
		ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), _T(""), fileOut);

		// footer
		WriteString(fileOut, _T("END:VCALENDAR"));

		return true;
	}

	return false;
}

void CiCalExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, const CString& sParentUID, CStdioFile& fileOut)
{
	if (!hTask)
	{
		return;
	}

	// attributes
	time_t tStartDate = pSrcTaskFile->GetTaskStartDate(hTask);
	time_t tDueDate = pSrcTaskFile->GetTaskDueDate(hTask);

	// if task only has a start date then make the due date the same as the start and vice versa
	if (tDueDate == 0 && tStartDate)
	{
		tDueDate = tStartDate;
	}

	else if (tStartDate == 0 && tDueDate)
	{
		tStartDate = tDueDate;
	}

	// construct a unique ID
	CString sUID, sFile = fileOut.GetFilePath();

	sFile.Replace(_T("\\"), _T(""));
	sFile.Replace(_T(":"), _T(""));
	sUID.Format(_T("%ld@%s.com"), pSrcTaskFile->GetTaskID(hTask), sFile);

	// tasks must have a start date or a due date or both
	if (tStartDate || tDueDate)
	{
		// header
		WriteString(fileOut, _T("BEGIN:VEVENT"));

		COleDateTime dtStart(tStartDate);
		WriteString(fileOut, _T("DTSTART;VALUE=DATE:%04d%02d%02d"), dtStart.GetYear(), dtStart.GetMonth(), dtStart.GetDay());

		// neither Google Calendar not Outlook pay any attention to the 'DUE' tag so we won't either.
		// instead we use 'DTEND' to mark the duration of the task. There is also no way to mark a
		// task as complete so we ignore our completion status

		// ics file dates are to 12am at the start of the day
		// so we need to increment the date by one day.
		COleDateTime dtDue(time_t(tDueDate + ONEDAY));
		WriteString(fileOut, _T("DTEND;VALUE=DATE:%04d%02d%02d"), dtDue.GetYear(), dtDue.GetMonth(), dtDue.GetDay());

		WriteString(fileOut, _T("SUMMARY:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskTitle(hTask)));
		WriteString(fileOut, _T("DESCRIPTION:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskComments(hTask)));
		WriteString(fileOut, _T("STATUS:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskStatus(hTask)));
		WriteString(fileOut, _T("CATEGORIES:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskCategory(hTask)));
		WriteString(fileOut, _T("URL:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskFileReferencePath(hTask)));
		WriteString(fileOut, _T("ORGANIZER:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskAllocatedBy(hTask)));
		WriteString(fileOut, _T("ATTENDEE:%s"), (LPTSTR)ATL::CA2T(pSrcTaskFile->GetTaskAllocatedTo(hTask)));
		WriteString(fileOut, _T("UID:%s"), (LPCTSTR)sUID);

		// parent child relationship
		WriteString(fileOut, _T("RELATED-TO;RELTYPE=PARENT:%s"), (LPCTSTR)sParentUID);

		// footer
		WriteString(fileOut, _T("END:VEVENT"));
	}

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), sUID, fileOut);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), sParentUID, fileOut);
}

void __cdecl CiCalExporter::WriteString(CStdioFile& fileOut, LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));
	CString sLine;

	va_list argList;
	va_start(argList, lpszFormat);
	sLine.FormatV(lpszFormat, argList);
	va_end(argList);

	sLine.TrimRight();

	// write line out in pieces no longer than 75 bytes
	while (sLine.GetLength() > 75)
	{
		CString sTemp = sLine.Left(75);
		sLine = sLine.Mid(75);

		fileOut.WriteString(sTemp);
		fileOut.WriteString(_T("\n "));
	}

	// write out whatever's left
	fileOut.WriteString(sLine);
	fileOut.WriteString(_T("\n"));
}
