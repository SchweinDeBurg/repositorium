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
// - merged with ToDoList version 6.1.2-6.1.7 sources
//*****************************************************************************

// TaskListCsvExporter.cpp: implementation of the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskListCsvExporter.h"
#include "TDLSchemaDef.h"
#include "Resource.h"
#include "RecurringTaskEdit.h"

#include <locale.h>

#include "../../../CodeProject/Source/TimeHelper.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR SPACE = _T(" ");
const LPCTSTR ENDL = _T("\n");
const LPCTSTR ONEDBLQUOTE = _T("\"");
const LPCTSTR TWODBLQUOTE = _T("\"\"");

CTaskListCsvExporter::CTaskListCsvExporter()
{
}

CTaskListCsvExporter::~CTaskListCsvExporter()
{
}

bool CTaskListCsvExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL /*bSilent*/)
{
	const ITaskList6* pTasks6 = GetITLInterface<ITaskList6>(pSrcTaskFile, IID_TASKLIST6);

	if (!pTasks6)
	{
		return false;
	}

	CPreferences prefs;

	DELIM = Misc::GetListSeparator();
	ROUNDTIMEFRACTIONS = prefs.GetProfileInt(_T("Preferences"), _T("RoundTimeFractions"), FALSE);

	if (prefs.GetProfileInt(_T("Preferences"), _T("UseSpaceIndents"), TRUE))
	{
		INDENT = CString(_T(' '), prefs.GetProfileInt(_T("Preferences"), _T("TextIndent"), 2));
	}
	else
	{
		INDENT = _T("  ");   // don't use tabs - excel strips them off
	}

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		// clear attrib list before rebuilding
		m_aAttribs.RemoveAll();
		BuildAttribList(pTasks6, NULL);

		CString sOutput;

		if (!ExportTask(pTasks6, NULL, 0, 0, _T(""), sOutput).IsEmpty())
		{
			fileOut.WriteString(ColumnHeadings());
			fileOut.WriteString(sOutput);
			return true;
		}
	}

	return false;
}

CString CTaskListCsvExporter::CalcIndent(int nDepth) const
{
	CString sIndent;

	for (int nLevel = 1; nLevel < nDepth; nLevel++)
	{
		sIndent += INDENT;
	}

	return sIndent;
}

void CTaskListCsvExporter::RemoveTrailingDelimiter(CString& sOutput) const
{
	// strip off trailing delimiter
	if (sOutput.Right(1) == DELIM)
	{
		sOutput = sOutput.Mid(0, sOutput.GetLength() - 1);
	}
}

CString& CTaskListCsvExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth,
	int nPos, const CString& sParentPos, CString& sOutput) const
{
	CString sPos; // becomes parent pos for sub-tasks

	if (hTask)
	{
		// same order as columns
		if (WantAttribute(TDL_TASKPOS))
		{
			// if there is a POS child item then this replaces nPos
			if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPOS)))
			{
				nPos = pTasks->GetTaskPosition(hTask);
			}

			if (nDepth == 1) // top level tasks
			{
				sPos.Format(_T("%d"), nPos);
			}
			else
			{
				sPos.Format(_T("%s.%d"), (LPCTSTR)sParentPos, nPos);
			}

			AppendAttribute(sPos, sOutput);
		}

		AppendAttribute(pTasks, hTask, TDL_TASKID, NULL, sOutput);

		// title is slightly different because we indent it for subtasks
		CString sPrefix = CalcIndent(nDepth);
		AppendAttribute(pTasks, hTask, TDL_TASKTITLE, NULL, sOutput, AT_TEXT, sPrefix);

		AppendAttribute(pTasks, hTask, TDL_TASKCALCCOMPLETION, TDL_TASKPERCENTDONE, sOutput);

		// priority needs a little more care because -2 == <none>
		int nPriority = pTasks->GetTaskPriority(hTask, TRUE);

		if (nPriority >= 0)
		{
			AppendAttribute(pTasks, hTask, TDL_TASKHIGHESTPRIORITY, TDL_TASKPRIORITY, sOutput);
		}

		else if (WantAttribute(TDL_TASKPRIORITY) || WantAttribute(TDL_TASKHIGHESTPRIORITY))
		{
			sOutput += DELIM;
		}

		AppendAttribute(pTasks, hTask, TDL_TASKCALCTIMEESTIMATE, TDL_TASKTIMEESTIMATE, sOutput, AT_TIME);
		AppendAttribute(pTasks, hTask, TDL_TASKCALCTIMESPENT, TDL_TASKTIMESPENT, sOutput, AT_TIME);
		AppendAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCREATEDBY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKEARLIESTDUEDATESTRING, TDL_TASKDUEDATESTRING, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKDONEDATESTRING, NULL, sOutput);
		AppendAttributeList(pTasks, hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKALLOCBY, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKVERSION, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKRECURRENCE, NULL, sOutput);
		AppendAttributeList(pTasks, hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKSTATUS, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKHIGHESTRISK, TDL_TASKRISK, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKEXTERNALID, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKCALCCOST, TDL_TASKCOST, sOutput, AT_COST);
		AppendAttribute(pTasks, hTask, TDL_TASKFILEREFPATH, NULL, sOutput);
		AppendAttribute(pTasks, hTask, TDL_TASKDEPENDENCY, NULL, sOutput);

		// special case
		AppendComments(pTasks, hTask, sOutput);

		RemoveTrailingDelimiter(sOutput);
	}

	// add sub-tasks
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

	return sOutput;
}

void CTaskListCsvExporter::CheckAddAttribtoList(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName)
{
	if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
	{
		// have we already got it ?
		if (!WantAttribute(szAttribName))
		{
			m_aAttribs.Add(szAttribName);   // add it
		}
	}
}

void CTaskListCsvExporter::AppendAttributeList(const ITaskList6* pTasks, HTASKITEM hTask,
	LPCTSTR szNumAttribName, LPCTSTR szAttribName,
	CString& sOutput) const
{
	int nItemCount = _ttoi(pTasks->GetTaskAttribute(hTask, ATL::CT2A(szNumAttribName)));

	if (nItemCount <= 1)
	{
		AppendAttribute(pTasks, hTask, szAttribName, NULL, sOutput);
	}

	else // more than one (use plus sign as delimiter)
	{
		CString sAttribs = pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName));

		for (int nItem = 1; nItem < nItemCount; nItem++)
		{
			CString sAttribName;
			sAttribName.Format(_T("%s%d"), szAttribName, nItem);

			sAttribs += _T('+');
			sAttribs += pTasks->GetTaskAttribute(hTask, ATL::CT2A(sAttribName));
		}

		AppendAttribute(sAttribs, sOutput);
	}
}

void CTaskListCsvExporter::AppendAttribute(const ITaskList6* pTasks, HTASKITEM hTask,
	LPCTSTR szAttribName, LPCTSTR szAltAttribName,
	CString& sOutput, ATTRIBTYPE at, LPCTSTR szPrefix) const
{
	if (WantAttribute(szAttribName) || (szAltAttribName && WantAttribute(szAltAttribName)))
	{
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
		{
			CString sAttrib(szPrefix);
			sAttrib += pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName));

			AppendAttribute(sAttrib, sOutput, at);
		}
		else if (szAltAttribName && pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAltAttribName)))
		{
			CString sAttrib(szPrefix);
			sAttrib += pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAltAttribName));

			AppendAttribute(sAttrib, sOutput, at);
		}
		else
		{
			sOutput += DELIM;
		}
	}
}

void CTaskListCsvExporter::AppendComments(const ITaskList6* pTasks, HTASKITEM hTask, CString& sOutput) const
{
	if (WantAttribute(TDL_TASKCOMMENTS) && pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS)))
	{
		CString sAttrib = pTasks->GetTaskAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS));
		AppendAttribute(sAttrib, sOutput, AT_TEXT, TRUE);
	}
}

void CTaskListCsvExporter::AppendAttribute(LPCTSTR szAttrib, CString& sOutput, ATTRIBTYPE at, BOOL bForceQuoted) const
{
	CString sAttrib(szAttrib);

	if (at == AT_TEXT)
	{
		BOOL bNeedQuoting = bForceQuoted;

		// double up quotes
		if (sAttrib.Find(ONEDBLQUOTE) != -1)
		{
			sAttrib.Replace(ONEDBLQUOTE, TWODBLQUOTE);
			bNeedQuoting = TRUE;
		}

		// look for commas or whatever is the list delimiter
		if (sAttrib.Find(DELIM) != -1)
		{
			bNeedQuoting = TRUE;
		}

		if (bNeedQuoting)
		{
			sAttrib = ONEDBLQUOTE + sAttrib + ONEDBLQUOTE;
		}

		// replace carriage returns
		sAttrib.Replace(ENDL, SPACE);
	}
	else if (at == AT_COST)
	{
		sAttrib = Misc::Format(_tstof(sAttrib));
	}
	else if (at == AT_TIME)
	{
		if (ROUNDTIMEFRACTIONS)
		{
			sAttrib.Format(_T("%d"), _ttoi(sAttrib));
		}
		else
		{
			sAttrib = Misc::Format(_tstof(sAttrib));
		}
	}

	sAttrib += DELIM;
	sOutput += sAttrib;
}

CString CTaskListCsvExporter::CheckGetColumnHeading(LPCTSTR szAttribName, UINT nIDHeading) const
{
	if (WantAttribute(szAttribName))
	{
		return CEnString(nIDHeading) + DELIM;
	}

	// else
	return _T("");
}

CString CTaskListCsvExporter::ColumnHeadings() const
{
	ASSERT(m_aAttribs.GetSize());

	CString sHeadings;

	sHeadings += CheckGetColumnHeading(TDL_TASKPOS, IDS_TDLBC_POS);
	sHeadings += CheckGetColumnHeading(TDL_TASKID, IDS_TDLBC_ID);
	sHeadings += CheckGetColumnHeading(TDL_TASKTITLE, IDS_TDLBC_TITLE);
	sHeadings += CheckGetColumnHeading(TDL_TASKPERCENTDONE, IDS_TDLBC_PERCENT);
	sHeadings += CheckGetColumnHeading(TDL_TASKPRIORITY, IDS_TDLBC_PRIORITY);
	sHeadings += CheckGetColumnHeading(TDL_TASKTIMEESTIMATE, IDS_TDLBC_TIMEEST);
	sHeadings += CheckGetColumnHeading(TDL_TASKTIMESPENT, IDS_TDLBC_TIMESPENT);
	sHeadings += CheckGetColumnHeading(TDL_TASKCREATIONDATESTRING, IDS_TDLBC_CREATEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCREATEDBY, IDS_TDLBC_CREATEDBY);
	sHeadings += CheckGetColumnHeading(TDL_TASKSTARTDATESTRING, IDS_TDLBC_STARTDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKDUEDATESTRING, IDS_TDLBC_DUEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKDONEDATESTRING, IDS_TDLBC_DONEDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKALLOCTO, IDS_TDLBC_ALLOCTO);
	sHeadings += CheckGetColumnHeading(TDL_TASKALLOCBY, IDS_TDLBC_ALLOCBY);
	sHeadings += CheckGetColumnHeading(TDL_TASKVERSION, IDS_TDLBC_VERSION);
	sHeadings += CheckGetColumnHeading(TDL_TASKRECURRENCE, IDS_TDLBC_RECURRENCE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCATEGORY, IDS_TDLBC_CATEGORY);
	sHeadings += CheckGetColumnHeading(TDL_TASKSTATUS, IDS_TDLBC_STATUS);
	sHeadings += CheckGetColumnHeading(TDL_TASKRISK, IDS_TDLBC_RISK);
	sHeadings += CheckGetColumnHeading(TDL_TASKEXTERNALID, IDS_TDLBC_EXTERNALID);
	sHeadings += CheckGetColumnHeading(TDL_TASKLASTMODSTRING, IDS_TDLBC_MODIFYDATE);
	sHeadings += CheckGetColumnHeading(TDL_TASKCOST, IDS_TDLBC_COST);
	sHeadings += CheckGetColumnHeading(TDL_TASKFILEREFPATH, IDS_TDLBC_FILEREF);
	sHeadings += CheckGetColumnHeading(TDL_TASKDEPENDENCY, IDS_TDLBC_DEPENDS);
	sHeadings += CheckGetColumnHeading(TDL_TASKCOMMENTS, IDS_TDLBC_COMMENTS);

	RemoveTrailingDelimiter(sHeadings);

	return sHeadings;
}

int CTaskListCsvExporter::BuildAttribList(const ITaskList6* pTasks, HTASKITEM hTask)
{
	if (hTask)
	{
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPOS);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKID);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTITLE);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCOMMENTS);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKALLOCTO);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKALLOCBY);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCATEGORY);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKSTATUS);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKFILEREFPATH);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCREATEDBY);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKFLAG);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDONEDATESTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDUEDATESTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKSTARTDATESTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCREATIONDATESTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTIMEESTIMATE);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKTIMESPENT);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPERCENTDONE);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKPRIORITY);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKRISK);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKLASTMODSTRING);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKEXTERNALID);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKCOST);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKDEPENDENCY);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKVERSION);
		CheckAddAttribtoList(pTasks, hTask, TDL_TASKRECURRENCE);
	}

	// subtasks
	hTask = pTasks->GetFirstTask(hTask);

	while (hTask) // at least one sub-task
	{
		BuildAttribList(pTasks, hTask);

		// next subtask
		hTask = pTasks->GetNextTask(hTask);
	}

	return m_aAttribs.GetSize();
}

BOOL CTaskListCsvExporter::WantAttribute(LPCTSTR szAttribName) const
{
	int nAttrib = m_aAttribs.GetSize();

	while (nAttrib--)
	{
		if (m_aAttribs[nAttrib] == szAttribName)
		{
			return TRUE;
		}
	}

	// nope
	return FALSE;
}
