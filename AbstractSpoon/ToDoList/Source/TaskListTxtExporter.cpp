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
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = "\n";

CTaskListTxtExporter::CTaskListTxtExporter()
{
}

CTaskListTxtExporter::~CTaskListTxtExporter()
{

}

bool CTaskListTxtExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL /*bSilent*/)
{
	CPreferences prefs;

	if (prefs.GetProfileInt("Preferences", "UseSpaceIndents", TRUE))
		INDENT = CString(' ', prefs.GetProfileInt("Preferences", "TextIndent", 2));
	else
		INDENT = '\t';

	if (prefs.GetProfileInt("Preferences", "ExportSpaceForNotes", FALSE))
	{
		TEXTNOTES.Empty();
		int nLine = prefs.GetProfileInt("Preferences", "LineSpaces", 8);

		if (nLine > 0)
		{
			while (nLine--)
				TEXTNOTES += "\n";
		}
	}

	ROUNDTIMEFRACTIONS = prefs.GetProfileInt("Preferences", "RoundTimeFractions", FALSE);

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		const ITaskList6* pTasks6 = static_cast<const ITaskList6*>(pSrcTaskFile);
		CString sOutput;

		if (!ExportTask(pTasks6, NULL, 0, 0, sOutput).IsEmpty())
		{
			fileOut.WriteString(sOutput);
			return true;
		}
	}

	return false;
}

CString& CTaskListTxtExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, CString& sOutput) const
{
	// handle locale specific decimal separator
	setlocale(LC_NUMERIC, "");

	// if depth == 0 then we're at the root so just add sub-tasks
	// else insert pItem first
	if (nDepth > 0)
	{
		// first create string to hold TABs
		CString sTabs;

		for (int nTab = 0; nTab < nDepth; nTab++)
			sTabs += INDENT;

		// if there is a POS child item then this replaces nPos
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKPOS))
			nPos = pTasks->GetTaskPosition(hTask);

		CString sID, sItem, sPriority, sStartDate, sDueDate, sDoneDate;
		CString sAllocTo, sAllocBy, sCategory, sStatus, sCost;
		CString sComments, sPercent, sTimeEst, sTimeSpent, sFileRef;
		CString sLastMod, sRisk, sExtID, sCreateDate, sCreateBy, sVersion;
		CString sRecurrence, sDepends;
		char cTemp;

		// ID
		FormatAttribute(pTasks, hTask, TDL_TASKID, "(ID: %s) ", sID);

		// title
		CString sTitle(pTasks->GetTaskTitle(hTask));

		// priority, start/due/done dates
		int nPercent = pTasks->GetTaskPercentDone(hTask, TRUE);
		BOOL bDone = pTasks->IsTaskDone(hTask) || (nPercent == 100);
		int nTimePlaces = ROUNDTIMEFRACTIONS ? 0 : 2;

		if (!bDone || !FormatAttribute(pTasks, hTask, TDL_TASKDONEDATESTRING, " (completed: %s)", sDoneDate))
		{
			if (pTasks->TaskHasAttribute(hTask, TDL_TASKHIGHESTPRIORITY) ||
				pTasks->TaskHasAttribute(hTask, TDL_TASKPRIORITY))
			{
				int nPriority = pTasks->GetTaskPriority(hTask, TRUE);

				if (nPriority >= 0)
					sPriority.Format("[%d] ", nPriority);
				else
					sPriority = "[ ]";
			}

			FormatAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, " (start: %s)", sStartDate);
			FormatAttribute(pTasks, hTask, TDL_TASKDUEDATESTRING, " (due: %s)", sDueDate);
			FormatAttribute(pTasks, hTask, TDL_TASKPERCENTDONE, " (%s%%) ", sPercent);

			if (pTasks->TaskHasAttribute(hTask, TDL_TASKTIMEESTIMATE) ||
				pTasks->TaskHasAttribute(hTask, TDL_TASKCALCTIMEESTIMATE))
				sTimeEst.Format(" (time est: %.*f hrs)", nTimePlaces, pTasks->GetTaskTimeEstimate(hTask, cTemp, TRUE));
		}
		
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKTIMESPENT) ||
			pTasks->TaskHasAttribute(hTask, TDL_TASKCALCTIMESPENT))
			sTimeEst.Format(" (time spent: %.*f hrs)", nTimePlaces, pTasks->GetTaskTimeSpent(hTask, cTemp, TRUE));

		FormatAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, " (created: %s)", sCreateDate);
		FormatAttribute(pTasks, hTask, TDL_TASKCREATEDBY, " (created by: %s)", sCreateBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, " (allocated to: %s)", sAllocTo);
		FormatAttribute(pTasks, hTask, TDL_TASKALLOCBY, " (allocated by: %s)", sAllocBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, " (category: %s)", sCategory);
		FormatAttribute(pTasks, hTask, TDL_TASKSTATUS, " (status: %s)", sStatus);
		FormatAttribute(pTasks, hTask, TDL_TASKRISK, " (risk: %s)", sRisk);
		FormatAttribute(pTasks, hTask, TDL_TASKEXTERNALID, " (ext.ID: %s)", sExtID);
		FormatAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, " (last mod: %s)", sLastMod);
		FormatAttribute(pTasks, hTask, TDL_TASKCOST, " (cost: %s)", sCost);
		FormatAttribute(pTasks, hTask, TDL_TASKVERSION, " (version: %s)", sVersion);
		FormatAttribute(pTasks, hTask, TDL_TASKRECURRENCE, " (occurs: %s)", sRecurrence);
		FormatAttribute(pTasks, hTask, TDL_TASKDEPENDENCY, " (depends on: %s)", sDepends);

		// fileref
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKFILEREFPATH))
			sFileRef.Format("%s%s(link: %s)", ENDL, sTabs, pTasks->GetTaskFileReferencePath(hTask));
		
		// comments
		if (pTasks->TaskHasAttribute(hTask, TDL_TASKCOMMENTS))
			sComments.Format("%s%s[%s]", ENDL, sTabs, pTasks->GetTaskComments(hTask));

		sItem.Format("%d. %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
					nPos, sID, sPriority, sPercent, sTitle, sRisk, 
					sAllocTo, sAllocBy, sDepends, sVersion, sRecurrence, sCategory, sStatus, 
					sDoneDate, sCreateDate, sCreateBy, sStartDate, sDueDate, 
					sTimeEst, sTimeSpent, sCost,
					sExtID, sLastMod, sFileRef, sComments, ENDL);

		// notes section
		if (!bDone)
			sItem += TEXTNOTES;

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
			sOutput += sDate;
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
			sOutput += ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sTask);

			hTask = pTasks->GetNextTask(hTask);
		}
	}

	// restore decimal separator to '.'
	setlocale(LC_NUMERIC, "English");

	return sOutput;
}

BOOL CTaskListTxtExporter::FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText)
{
	if (pTasks->TaskHasAttribute(hTask, szAttribName))
	{
		sAttribText.Format(szFormat, pTasks->GetTaskAttribute(hTask, szAttribName));
		return TRUE;
	}

	return FALSE;
}


BOOL CTaskListTxtExporter::FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                           LPCTSTR szFormat, CString& sAttribText)
{
	int nItemCount = atoi(pTasks->GetTaskAttribute(hTask, szNumAttribName));

	if (nItemCount <= 1)
		return FormatAttribute(pTasks, hTask, szAttribName, szFormat, sAttribText);

	// else more than one (use plus sign as delimiter)
	CString sAttribs = pTasks->GetTaskAttribute(hTask, szAttribName);
	
	for (int nItem = 1; nItem < nItemCount; nItem++)
	{
		CString sAttribName;
		sAttribName.Format("%s%d", szAttribName, nItem);
		
		sAttribs += '+';
		sAttribs += pTasks->GetTaskAttribute(hTask, sAttribName);
	}
	
	sAttribText.Format(szFormat, sAttribs);
	return TRUE;
}
