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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// TaskListCsvExporter.cpp: implementation of the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskListCsvExporter.h"
#include "TDLSchemaDef.h"
#include "Resource.h"
#include "RecurringTaskEdit.h"
#include "TDLCsvImportExportDlg.h"

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

bool CTaskListCsvExporter::ExportOutput(const TCHAR* szDestFilePath, const CString& sOutput)
{
	if (sOutput.IsEmpty())
	{
		return false;
	}

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		fileOut.WriteString(ColumnHeadings());
		fileOut.WriteString(sOutput);
		return true;
	}

	return false;
}

void CTaskListCsvExporter::InitConsts()
{
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
}

bool CTaskListCsvExporter::Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent)
{
	InitConsts();

	CString sOutput;

	for (int nTaskList = 0; nTaskList < pSrcTaskFile->GetTaskListCount(); nTaskList++)
	{
		const ITaskList8* pTasks8 = GetITLInterface<ITaskList8>(pSrcTaskFile->GetTaskList(nTaskList), IID_TASKLIST8);

		if (pTasks8)
		{
			// have we built the attribute list?
			if (nTaskList == 0)
			{
				CTDCAttributeArray aAttributes;
				BuildAttribList(pTasks8, NULL, aAttributes);

				CTDLCsvImportExportDlg dialog(szDestFilePath, aAttributes);

				if (!bSilent && dialog.DoModal() != IDOK)
				{
					return false;
				}

				dialog.GetColumnMapping(m_aColumnMapping);
				DELIM = dialog.GetDelimiter();
			}

			ExportTask(pTasks8, NULL, 0, 0, _T(""), sOutput);
		}
	}

	return ExportOutput(szDestFilePath, sOutput);
}

bool CTaskListCsvExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent)
{
	InitConsts();

	const ITaskList8* pTasks8 = GetITLInterface<ITaskList8>(pSrcTaskFile, IID_TASKLIST8);

	// build attribute list
	CTDCAttributeArray aAttributes;
	BuildAttribList(pTasks8, NULL, aAttributes);

	CTDLCsvImportExportDlg dialog(szDestFilePath, aAttributes);

	if (!bSilent && dialog.DoModal() != IDOK)
	{
		return false;
	}

	dialog.GetColumnMapping(m_aColumnMapping);

	CString sOutput;
	ExportTask(pTasks8, NULL, 0, 0, _T(""), sOutput);

	return ExportOutput(szDestFilePath, sOutput);
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

CString& CTaskListCsvExporter::ExportTask(const ITaskList8* pTasks, HTASKITEM hTask, int nDepth, int nPos,
	const CString& sParentPos, CString& sOutput) const
{
	if (!pTasks)
	{
		return sOutput;
	}

	CString sPos; // becomes parent pos for sub-tasks

	// handle locale specific decimal separator
	_tsetlocale(LC_NUMERIC, _T(""));

	if (hTask)
	{
		CString sTask;

		// same order as columns
		if (WantAttribute(TDCA_POSITION))
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

			AppendAttribute(sPos, sTask);
		}

		AppendAttribute(pTasks, hTask, TDCA_ID, TDL_TASKID, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_PARENTID, TDL_TASKPARENTID, NULL, sTask);

		// title is slightly different because we indent it for subtasks
		CString sPrefix = CalcIndent(nDepth);
		AppendAttribute(pTasks, hTask, TDCA_TASKNAME, TDL_TASKTITLE, NULL, sTask, sPrefix);

		AppendAttribute(pTasks, hTask, TDCA_PERCENT, TDL_TASKCALCCOMPLETION, TDL_TASKPERCENTDONE, sTask);

		// priority needs a little more care because -2 == <none>
		int nPriority = pTasks->GetTaskPriority(hTask, TRUE);

		if (nPriority >= 0)
		{
			AppendAttribute(pTasks, hTask, TDCA_PRIORITY, TDL_TASKHIGHESTPRIORITY, TDL_TASKPRIORITY, sTask);
		}

		else if (WantAttribute(TDCA_PRIORITY))
		{
			sTask += DELIM;
		}

		AppendAttribute(pTasks, hTask, TDCA_TIMEEST, TDL_TASKCALCTIMEESTIMATE, TDL_TASKTIMEESTIMATE, sTask);
		AppendAttribute(pTasks, hTask, TDCA_TIMESPENT, TDL_TASKCALCTIMESPENT, TDL_TASKTIMESPENT, sTask);
		AppendAttribute(pTasks, hTask, TDCA_CREATIONDATE, TDL_TASKCREATIONDATESTRING, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_CREATEDBY, TDL_TASKCREATEDBY, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_STARTDATE, TDL_TASKSTARTDATESTRING, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_DUEDATE, TDL_TASKEARLIESTDUEDATESTRING, TDL_TASKDUEDATESTRING, sTask);
		AppendAttribute(pTasks, hTask, TDCA_DONEDATE, TDL_TASKDONEDATESTRING, NULL, sTask);
		AppendAttributeList(pTasks, hTask, TDCA_ALLOCTO, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, sTask);
		AppendAttribute(pTasks, hTask, TDCA_ALLOCBY, TDL_TASKALLOCBY, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_VERSION, TDL_TASKVERSION, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_RECURRENCE, TDL_TASKRECURRENCE, NULL, sTask);
		AppendAttributeList(pTasks, hTask, TDCA_CATEGORY, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, sTask);
		AppendAttribute(pTasks, hTask, TDCA_STATUS, TDL_TASKSTATUS, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_RISK, TDL_TASKHIGHESTRISK, TDL_TASKRISK, sTask);
		AppendAttribute(pTasks, hTask, TDCA_EXTERNALID, TDL_TASKEXTERNALID, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_LASTMOD, TDL_TASKLASTMODSTRING, NULL, sTask);
		AppendAttribute(pTasks, hTask, TDCA_COST, TDL_TASKCALCCOST, TDL_TASKCOST, sTask);
		AppendAttribute(pTasks, hTask, TDCA_FILEREF, TDL_TASKFILEREFPATH, NULL, sTask);
		AppendAttributeList(pTasks, hTask, TDCA_DEPENDENCY, TDL_TASKNUMDEPENDENCY, TDL_TASKDEPENDENCY, sTask);

		// special case
		AppendComments(pTasks, hTask, sTask);

		// strip off trailing delimiter
		if (!sTask.IsEmpty())
		{
			sTask = sTask.Left(sTask.GetLength() - 1);
		}

		sOutput += sTask;
	}

	// add sub-tasks
	hTask = pTasks->GetFirstTask(hTask);

	if (hTask) // at least one sub-task
	{
		int nChildPos = 1;

		while (hTask)
		{
			CString sSubTask;
			ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sPos, sSubTask);

			sOutput += ENDL;
			sOutput += sSubTask;

			hTask = pTasks->GetNextTask(hTask);
		}
	}

	// restore decimal separator to '.'
	_tsetlocale(LC_NUMERIC, _T("English"));

	return sOutput;
}

void CTaskListCsvExporter::AppendAttributeList(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib,
	LPCTSTR szNumAttribName, LPCTSTR szAttribName, CString& sOutput) const
{
	if (!WantAttribute(attrib))
	{
		return;
	}

	int nItemCount = _ttoi(pTasks->GetTaskAttribute(hTask, ATL::CT2A(szNumAttribName)));

	if (nItemCount <= 1)
	{
		AppendAttribute(pTasks, hTask, attrib, szAttribName, NULL, sOutput);
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

void CTaskListCsvExporter::AppendAttribute(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib,
	LPCTSTR szAttribName, LPCTSTR szAltAttribName, CString& sOutput, LPCTSTR szPrefix) const
{
	if (!WantAttribute(attrib))
	{
		return;
	}

	if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
	{
		CString sAttrib(szPrefix);
		sAttrib += pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName));

		AppendAttribute(sAttrib, sOutput);
	}
	else if (szAltAttribName && pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAltAttribName)))
	{
		CString sAttrib(szPrefix);
		sAttrib += pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAltAttribName));

		AppendAttribute(sAttrib, sOutput);
	}
	else
	{
		sOutput += DELIM;
	}
}

void CTaskListCsvExporter::AppendComments(const ITaskList8* pTasks, HTASKITEM hTask, CString& sOutput) const
{
	if (WantAttribute(TDCA_COMMENTS))
	{
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS)))
		{
			CString sAttrib = pTasks->GetTaskAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS));
			AppendAttribute(sAttrib, sOutput, TRUE);
		}
		else
		{
			sOutput += DELIM;
		}
	}
}

void CTaskListCsvExporter::AppendAttribute(LPCTSTR szAttrib, CString& sOutput, BOOL bForceQuoted) const
{
	BOOL bNeedQuoting = bForceQuoted;
	CString sAttrib(szAttrib);

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

	sAttrib += DELIM;
	sOutput += sAttrib;
}

void CTaskListCsvExporter::AppendAttribute(double dAttrib, LPCTSTR szFormat, CString& sOutput) const
{
	CString sAttrib;

	sAttrib.Format(szFormat, dAttrib);
	AppendAttribute(sAttrib, sOutput);
}

void CTaskListCsvExporter::AppendAttribute(int nAttrib, LPCTSTR szFormat, CString& sOutput) const
{
	CString sAttrib;

	sAttrib.Format(szFormat, nAttrib);
	AppendAttribute(sAttrib, sOutput);
}

CString CTaskListCsvExporter::CheckGetColumnHeading(TDC_ATTRIBUTE attrib) const
{
	int nIndex = AttributeIndex(attrib);

	if (nIndex != -1)
	{
		return m_aColumnMapping[nIndex].sColumnName + DELIM;
	}

	// else
	return _T("");
}

CString CTaskListCsvExporter::ColumnHeadings() const
{
	ASSERT(m_aColumnMapping.GetSize());

	CString sHeadings;

	sHeadings += CheckGetColumnHeading(TDCA_POSITION);
	sHeadings += CheckGetColumnHeading(TDCA_ID);
	sHeadings += CheckGetColumnHeading(TDCA_PARENTID);
	sHeadings += CheckGetColumnHeading(TDCA_TASKNAME);
	sHeadings += CheckGetColumnHeading(TDCA_PERCENT);
	sHeadings += CheckGetColumnHeading(TDCA_PRIORITY);
	sHeadings += CheckGetColumnHeading(TDCA_TIMEEST);
	sHeadings += CheckGetColumnHeading(TDCA_TIMESPENT);
	sHeadings += CheckGetColumnHeading(TDCA_CREATIONDATE);
	sHeadings += CheckGetColumnHeading(TDCA_CREATEDBY);
	sHeadings += CheckGetColumnHeading(TDCA_STARTDATE);
	sHeadings += CheckGetColumnHeading(TDCA_DUEDATE);
	sHeadings += CheckGetColumnHeading(TDCA_DONEDATE);
	sHeadings += CheckGetColumnHeading(TDCA_ALLOCTO);
	sHeadings += CheckGetColumnHeading(TDCA_ALLOCBY);
	sHeadings += CheckGetColumnHeading(TDCA_VERSION);
	sHeadings += CheckGetColumnHeading(TDCA_RECURRENCE);
	sHeadings += CheckGetColumnHeading(TDCA_CATEGORY);
	sHeadings += CheckGetColumnHeading(TDCA_STATUS);
	sHeadings += CheckGetColumnHeading(TDCA_RISK);
	sHeadings += CheckGetColumnHeading(TDCA_EXTERNALID);
	sHeadings += CheckGetColumnHeading(TDCA_LASTMOD);
	sHeadings += CheckGetColumnHeading(TDCA_COST);
	sHeadings += CheckGetColumnHeading(TDCA_FILEREF);
	sHeadings += CheckGetColumnHeading(TDCA_DEPENDENCY);
	sHeadings += CheckGetColumnHeading(TDCA_COMMENTS);

	// strip off trailing delimiter
	if (!sHeadings.IsEmpty())
	{
		sHeadings = sHeadings.Left(sHeadings.GetLength() - 1);
	}

	return sHeadings;
}

int CTaskListCsvExporter::BuildAttribList(const ITaskList8* pTasks, HTASKITEM hTask, CTDCAttributeArray& aAttributes)
{
	if (hTask)
	{
		CheckAddAttribtoList(pTasks, hTask, TDCA_POSITION, TDL_TASKPOS, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_ID, TDL_TASKID, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_PARENTID, TDL_TASKPARENTID, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_TASKNAME, TDL_TASKTITLE, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_COMMENTS, TDL_TASKCOMMENTS, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_ALLOCTO, TDL_TASKALLOCTO, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_ALLOCBY, TDL_TASKALLOCBY, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_CATEGORY, TDL_TASKCATEGORY, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_STATUS, TDL_TASKSTATUS, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_FILEREF, TDL_TASKFILEREFPATH, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_CREATEDBY, TDL_TASKCREATEDBY, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_FLAG, TDL_TASKFLAG, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_DONEDATE, TDL_TASKDONEDATESTRING, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_DUEDATE, TDL_TASKDUEDATESTRING, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_STARTDATE, TDL_TASKSTARTDATESTRING, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_CREATIONDATE, TDL_TASKCREATIONDATESTRING, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_TIMEEST, TDL_TASKTIMEESTIMATE, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_TIMESPENT, TDL_TASKTIMESPENT, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_PERCENT, TDL_TASKPERCENTDONE, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_PRIORITY, TDL_TASKPRIORITY, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_RISK, TDL_TASKRISK, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_LASTMOD, TDL_TASKLASTMODSTRING, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_EXTERNALID, TDL_TASKEXTERNALID, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_COST, TDL_TASKCOST, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_DEPENDENCY, TDL_TASKDEPENDENCY, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_VERSION, TDL_TASKVERSION, aAttributes);
		CheckAddAttribtoList(pTasks, hTask, TDCA_RECURRENCE, TDL_TASKRECURRENCE, aAttributes);
	}

	// subtasks
	hTask = pTasks->GetFirstTask(hTask);

	while (hTask) // at least one sub-task
	{
		BuildAttribList(pTasks, hTask, aAttributes);

		// next subtask
		hTask = pTasks->GetNextTask(hTask);
	}

	return aAttributes.GetSize();
}

void CTaskListCsvExporter::CheckAddAttribtoList(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib,
	LPCTSTR szAttribName, CTDCAttributeArray& aAttributes)
{
	if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
	{
		// have we already got it ?
		int nAttrib = aAttributes.GetSize();

		while (nAttrib--)
		{
			if (aAttributes[nAttrib] == attrib)
			{
				return;   // already have it
			}
		}

		// add it
		aAttributes.Add(attrib);
	}
}

int CTaskListCsvExporter::AttributeIndex(TDC_ATTRIBUTE attrib) const
{
	int nAttrib = m_aColumnMapping.GetSize();

	while (nAttrib--)
	{
		const CSVCOLUMNMAPPING& col = m_aColumnMapping[nAttrib];

		if (col.nTDCAttrib == attrib && !col.sColumnName.IsEmpty())
		{
			break;
		}
	}

	return nAttrib;
}
