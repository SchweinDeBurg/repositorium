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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// TaskListTxtExporter.cpp: implementation of the CTaskListTxtExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TaskListTxtExporter.h"
#include "TDLSchemaDef.h"
#include "RecurringTaskEdit.h"

#include "../../Common/Preferences.h"

#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = _T("\n");

CTaskListTxtExporter::CTaskListTxtExporter()
{
}

CTaskListTxtExporter::~CTaskListTxtExporter()
{
}

bool CTaskListTxtExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL /*bSilent*/)
{
	CPreferences prefs;

	if (prefs.GetProfileInt(_T("Preferences"), _T("UseSpaceIndents"), TRUE))
	{
		INDENT = CString(_T(' '), prefs.GetProfileInt(_T("Preferences"), _T("TextIndent"), 2));
	}
	else
	{
		INDENT = _T('\t');
	}

	if (prefs.GetProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), FALSE))
	{
		TEXTNOTES.Empty();
		int nLine = prefs.GetProfileInt(_T("Preferences"), _T("LineSpaces"), 8);

		if (nLine > 0)
		{
			while (nLine--)
			{
				TEXTNOTES += "\n";
			}
		}
	}

	ROUNDTIMEFRACTIONS = prefs.GetProfileInt(_T("Preferences"), _T("RoundTimeFractions"), FALSE);

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		const ITaskList6* pTasks6 = static_cast<const ITaskList6*>(pSrcTaskFile);
		CString sOutput;

		if (!ExportTask(pTasks6, NULL, 0, 0, _T(""), sOutput).IsEmpty())
		{
			fileOut.WriteString(sOutput);
			return true;
		}
	}

	return false;
}

CString& CTaskListTxtExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, const CString& sParentPos, CString& sOutput) const
{
	// handle locale specific decimal separator
	_tsetlocale(LC_NUMERIC, _T(""));

	// if depth == 0 then we're at the root so just add sub-tasks
	// else insert pItem first
	CString sPos;

	if (nDepth > 0)
	{
		// first create string to hold TABs
		CString sTabs;

		for (int nTab = 0; nTab < nDepth; nTab++)
		{
			sTabs += INDENT;
		}

		// if there is a POS child item then this replaces nPos
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPOS)))
		{
			nPos = pTasks->GetTaskPosition(hTask);
		}

		if (!sParentPos.IsEmpty())
		{
			sPos.Format(_T("%s.%d"), sParentPos, nPos);
		}
		else
		{
			sPos.Format(_T("%d"), nPos);
		}

		CString sID, sItem, sPriority, sStartDate, sDueDate, sDoneDate;
		CString sAllocTo, sAllocBy, sCategory, sStatus, sCost;
		CString sComments, sPercent, sTimeEst, sTimeSpent, sFileRef;
		CString sLastMod, sRisk, sExtID, sCreateDate, sCreateBy, sVersion;
		CString sRecurrence, sDepends;
		TCHAR cTemp;

		// ID
		FormatAttribute(pTasks, hTask, TDL_TASKID, _T("(ID: %s) "), sID);

		// title
		CString sTitle(pTasks->GetTaskTitle(hTask));

		// priority, start/due/done dates
		int nPercent = pTasks->GetTaskPercentDone(hTask, TRUE);
		BOOL bDone = pTasks->IsTaskDone(hTask) || (nPercent == 100);
		int nTimePlaces = ROUNDTIMEFRACTIONS ? 0 : 2;

		if (bDone)
		{
			FormatAttribute(pTasks, hTask, TDL_TASKDONEDATESTRING, _T(" (completed: %s)"), sDoneDate);
		}

		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKHIGHESTPRIORITY)) ||
				pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPRIORITY)))
		{
			int nPriority = pTasks->GetTaskPriority(hTask, TRUE);

			if (nPriority >= 0)
			{
				sPriority.Format(_T("[%d] "), nPriority);
			}
			else
			{
				sPriority = _T("[ ]");
			}
		}

		FormatAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, _T(" (start: %s)"), sStartDate);
		FormatAttribute(pTasks, hTask, TDL_TASKDUEDATESTRING, _T(" (due: %s)"), sDueDate);
		FormatAttribute(pTasks, hTask, TDL_TASKPERCENTDONE, _T(" (%s%%) "), sPercent);

		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMEESTIMATE)) ||
			pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMEESTIMATE)))
		{
			sTimeEst.Format(_T(" (time est: %.*f hrs)"), nTimePlaces, pTasks->GetTaskTimeEstimate(hTask, cTemp, TRUE));
		}

		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMESPENT)) ||
			pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMESPENT)))
		{
			sTimeEst.Format(_T(" (time spent: %.*f hrs)"), nTimePlaces, pTasks->GetTaskTimeSpent(hTask, cTemp, TRUE));
		}

		FormatAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, _T(" (created: %s)"), sCreateDate);
		FormatAttribute(pTasks, hTask, TDL_TASKCREATEDBY, _T(" (created by: %s)"), sCreateBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, _T(" (allocated to: %s)"), sAllocTo);
		FormatAttribute(pTasks, hTask, TDL_TASKALLOCBY, _T(" (allocated by: %s)"), sAllocBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, _T(" (category: %s)"), sCategory);
		FormatAttribute(pTasks, hTask, TDL_TASKSTATUS, _T(" (status: %s)"), sStatus);
		FormatAttribute(pTasks, hTask, TDL_TASKRISK, _T(" (risk: %s)"), sRisk);
		FormatAttribute(pTasks, hTask, TDL_TASKEXTERNALID, _T(" (ext.ID: %s)"), sExtID);
		FormatAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, _T(" (last mod: %s)"), sLastMod);
		FormatAttribute(pTasks, hTask, TDL_TASKCOST, _T(" (cost: %s)"), sCost);
		FormatAttribute(pTasks, hTask, TDL_TASKVERSION, _T(" (version: %s)"), sVersion);
		FormatAttribute(pTasks, hTask, TDL_TASKRECURRENCE, _T(" (occurs: %s)"), sRecurrence);
		FormatAttribute(pTasks, hTask, TDL_TASKDEPENDENCY, _T(" (depends on: %s)"), sDepends);

		// fileref
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKFILEREFPATH)))
		{
			sFileRef.Format(_T("%s%s(link: %s)"), ENDL, (LPCTSTR)sTabs, (LPTSTR)pTasks->GetTaskFileReferencePath(hTask));
		}

		// comments
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS)))
		{
			sComments.Format(_T("%s%s[%s]"), ENDL, (LPCTSTR)sTabs, (LPTSTR)pTasks->GetTaskComments(hTask));
		}

		sItem.Format(_T("%d. %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"),
			nPos, (LPCTSTR)sID, (LPCTSTR)sPriority, (LPCTSTR)sPercent, (LPCTSTR)sTitle, (LPCTSTR)sRisk,
			(LPCTSTR)sAllocTo, (LPCTSTR)sAllocBy, (LPCTSTR)sDepends, (LPCTSTR)sVersion, (LPCTSTR)sRecurrence,
			(LPCTSTR)sCategory, (LPCTSTR)sStatus, (LPCTSTR)sDoneDate, (LPCTSTR)sCreateDate, (LPCTSTR)sCreateBy,
			(LPCTSTR)sStartDate, (LPCTSTR)sDueDate, (LPCTSTR)sTimeEst, (LPCTSTR)sTimeSpent, (LPCTSTR)sCost,
			(LPCTSTR)sExtID, (LPCTSTR)sLastMod, (LPCTSTR)sFileRef, (LPCTSTR)sComments, ENDL);

		// notes section
		if (!bDone)
		{
			sItem += TEXTNOTES;
		}

		// indent to match depth
		sOutput += sTabs;
		sOutput += sItem;
	}
	else
	{
		// title and date
		const ITaskList4* pITL4 = static_cast<const ITaskList4*>(pTasks);

		CString sTitle = pITL4->GetReportTitle();
		CString sDate = pITL4->GetReportDate();

		// note: do not append a trailing ENDL as this will be added
		// by the following code
		if (!sTitle.IsEmpty())
		{
			sOutput += sTitle;

			if (!sDate.IsEmpty())
			{
				sOutput += ENDL;
				sOutput += sDate;
			}
		}
		else if (!sDate.IsEmpty())
		{
			sOutput += sDate;
		}
	}

	// begin new ordered list for sub-tasks
	hTask = pTasks->GetFirstTask(hTask);

	if (hTask) // at least one sub-task
	{
		int nChildPos = 1;

		while (hTask)
		{
			CString sTask;
			sOutput += ENDL;
			sOutput += ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sPos, sTask);

			hTask = pTasks->GetNextTask(hTask);
		}
	}

	// restore decimal separator to '.'
	_tsetlocale(LC_NUMERIC, _T("English"));

	return sOutput;
}

BOOL CTaskListTxtExporter::FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask,
	LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText)
{
	if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
	{
		sAttribText.Format(szFormat, (LPTSTR)pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName)));
		return TRUE;
	}

	return FALSE;
}


BOOL CTaskListTxtExporter::FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask,
	LPCTSTR szNumAttribName, LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText)
{
	int nItemCount = _ttoi(pTasks->GetTaskAttribute(hTask, ATL::CT2A(szNumAttribName)));

	if (nItemCount <= 1)
	{
		return FormatAttribute(pTasks, hTask, szAttribName, szFormat, sAttribText);
	}

	// else more than one (use plus sign as delimiter)
	CString sAttribs = pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName));

	for (int nItem = 1; nItem < nItemCount; nItem++)
	{
		CString sAttribName;
		sAttribName.Format(_T("%s%d"), szAttribName, nItem);

		sAttribs += _T('+');
		sAttribs += pTasks->GetTaskAttribute(hTask, ATL::CT2A(sAttribName));
	}

	sAttribText.Format(szFormat, sAttribs);
	return TRUE;
}
