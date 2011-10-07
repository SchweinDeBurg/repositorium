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
// - merged with ToDoList version 6.2.4 sources
//*****************************************************************************

// TaskListCsvImporter.cpp: implementation of the CTaskListCsvImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskListCsvImporter.h"
#include "TDLSchemaDef.h"
#include "Resource.h"
#include "RecurringTaskEdit.h"
#include "TDLCsvImportExportDlg.h"

#include <locale.h>

#include "../../../CodeProject/Source/TimeHelper.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/FileMisc.h"
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

// private structure for sorting
struct CSVSORT
{
	void Set(DWORD id, DWORD parentID, int originalLineIndex)
	{
		dwID = id;
		dwParentID = parentID;
		nOriginalLineIndex = originalLineIndex;
	}

	DWORD dwID;
	DWORD dwParentID;
	int nOriginalLineIndex;
};

////////////////////////////////////////////////////////

CTaskListCsvImporter::CTaskListCsvImporter()
{
}

CTaskListCsvImporter::~CTaskListCsvImporter()
{
}

void CTaskListCsvImporter::InitConsts()
{
	CPreferences prefs;

	DELIM = Misc::GetListSeparator();

	if (prefs.GetProfileInt(_T("Preferences"), _T("UseSpaceIndents"), TRUE))
	{
		INDENT = CString(_T(' '), prefs.GetProfileInt(_T("Preferences"), _T("TextIndent"), 2));
	}
	else
	{
		INDENT = _T("  ");   // don't use tabs - excel strips them off
	}
}

void CTaskListCsvImporter::PreProcessFileLines(CStringArray& aLines) const
{
	// if we have parent ID then we need to sort the lines so that
	// the parent task occurs before its subtasks
	int nParentIDIndex = AttributeIndex(TDCA_PARENTID);

	if (nParentIDIndex == -1)
	{
		return;
	}

	int nIDIndex = AttributeIndex(TDCA_ID);

	// build an array that contains enough information to sort
	CArray<CSVSORT, CSVSORT&> aSortArray;
	int nLine, nNumLines = aLines.GetSize();

	aSortArray.SetSize(nNumLines);

	for (nLine = 0; nLine < nNumLines; nLine++)
	{
		CStringArray aColumns;
		Misc::Split(aLines[nLine], aColumns, TRUE, DELIM);

		aSortArray[nLine].Set(_ttoi(aColumns[nIDIndex]), _ttoi(aColumns[nParentIDIndex]), nLine);
	}

	qsort(aSortArray.GetData(), aSortArray.GetSize(), sizeof(CSVSORT), SortProc);

	// assume there was a change and rebuild aLines
	CStringArray aSortedLines;
	aSortedLines.SetSize(nNumLines);

	for (nLine = 0; nLine < nNumLines; nLine++)
	{
		int nOrgLine = aSortArray[nLine].nOriginalLineIndex;
		aSortedLines[nLine] = aLines[nOrgLine];
	}

	// finally copy back
	aLines.Copy(aSortedLines);
}

int CTaskListCsvImporter::SortProc(const void* item1, const void* item2)
{
	const CSVSORT* pItem1 = (const CSVSORT*)item1;
	const CSVSORT* pItem2 = (const CSVSORT*)item2;

	// if pItem2's ID is the parent ID of pItem1 then
	// we must switch them so that the parent occurs before the child
	if (pItem2->dwID == pItem1->dwParentID)
	{
		return 1;
	}

	// all else no change
	return 0;
}

bool CTaskListCsvImporter::Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile)
{
	InitConsts();

	CTDLCsvImportExportDlg dialog(szSrcFilePath);

	if (dialog.DoModal() != IDOK)
	{
		return false;
	}

	dialog.GetColumnMapping(m_aColumnMapping);
	DELIM = dialog.GetDelimiter();

	// Load csv and sort the elements so that parents are above children
	CStringArray aLines;
	FileMisc::LoadFileLines(szSrcFilePath, aLines);

	// remove header line
	aLines.RemoveAt(0);

	// sort task data to ensure parent preceeds children
	//PreProcessFileLines(aLines);

	ITaskList8* pTasks8 = GetITLInterface<ITaskList8>(pDestTaskFile, IID_TASKLIST8);

	for (int nLine = 0; nLine < aLines.GetSize(); nLine++)
	{
		CString sLine = GetLine(aLines, nLine);
		ImportTask(pTasks8, sLine);
	}

	return true;
}

CString CTaskListCsvImporter::GetLine(const CStringArray& aLines, int& nLine)
{
	CEnString sLine(aLines[nLine]);

	// if the line contains an odd number of double-quotes
	// then assume we're in a comment and keep appending lines
	// until we hit the closing double-quote
	if (sLine.GetCharacterCount(_T('\"')) % 2)
	{
		while (++nLine < aLines.GetSize())
		{
			CEnString sNextLine(aLines[nLine]);
			sLine += _T("\n") + sNextLine;

			if (sNextLine.GetCharacterCount(_T('\"')) % 2)
			{
				break;
			}
		}
	}

	return sLine;
}

void CTaskListCsvImporter::GetTaskAndParentIDs(const CStringArray& sValues, DWORD& dwTaskID, DWORD& dwParentID) const
{
	dwTaskID = dwParentID = 0;

	int nCol = AttributeIndex(TDCA_ID);

	if (nCol != -1)
	{
		dwTaskID = _ttoi(sValues[nCol]);
	}

	nCol = AttributeIndex(TDCA_PARENTID);

	if (nCol != -1)
	{
		dwParentID = _ttoi(sValues[nCol]);
	}
}

CString CTaskListCsvImporter::GetTaskTitle(const CStringArray& sValues) const
{
	int nCol = AttributeIndex(TDCA_TASKNAME);

	if (nCol != -1)
	{
		return sValues[nCol];
	}

	// else
	static CString sEmpty;
	return sEmpty;
}

BOOL CTaskListCsvImporter::ImportTask(ITaskList8* pTasks, const CString& sLine) const
{
	CStringArray aValues;
	Misc::Split(sLine, aValues, TRUE, DELIM);

	ASSERT(aValues.GetSize() == m_aColumnMapping.GetSize());

	if (aValues.GetSize() != m_aColumnMapping.GetSize())
	{
		return FALSE;   // then we can report this when we've finished importing
	}

	// get taskID and ParentID
	DWORD dwTaskID, dwParentID;
	GetTaskAndParentIDs(aValues, dwTaskID, dwParentID);

	// find the parent task
	HTASKITEM hParent = pTasks->FindTask(dwParentID);

	// create task
	HTASKITEM hTask = pTasks->NewTask(ATL::CT2A(GetTaskTitle(aValues)), hParent, dwTaskID);

	AddAttributeToTask(pTasks, hTask, TDCA_CREATEDBY, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_CATEGORY, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_STATUS, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_EXTERNALID, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_ALLOCBY, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_ALLOCTO, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_VERSION, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_FILEREF, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_DEPENDENCY, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_COMMENTS, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_PRIORITY, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_RISK, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_FLAG, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_TIMEEST, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_TIMESPENT, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_COST, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_STARTDATE, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_DUEDATE, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_DONEDATE, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_LASTMOD, aValues);
	AddAttributeToTask(pTasks, hTask, TDCA_CREATIONDATE, aValues);

	return TRUE;
}

int CTaskListCsvImporter::AttributeIndex(TDC_ATTRIBUTE attrib) const
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

int CTaskListCsvImporter::GetDepth(const CString& sLine)
{
	if (INDENT.IsEmpty() || sLine.IsEmpty())
	{
		return 0;
	}

	// else
	int nDepth = 0;

	if (INDENT == _T("\t"))
	{
		while (nDepth < sLine.GetLength())
		{
			if (sLine[nDepth] == _T('\t'))
			{
				nDepth++;
			}
			else
			{
				break;
			}
		}
	}
	else // one or more spaces
	{
		int nPos = 0;

		while (nPos < sLine.GetLength())
		{
			if (sLine.Find(INDENT, nPos) == nPos)
			{
				nDepth++;
			}
			else
			{
				break;
			}

			// next
			nPos = nDepth * INDENT.GetLength();
		}
	}

	// set root depth if not set
	if (ROOTDEPTH == -1)
	{
		ROOTDEPTH = nDepth;
	}

	// and take allowance for it
	nDepth -= ROOTDEPTH;

	return nDepth;
}

time_t CTaskListCsvImporter::String2Date(const CString& sDate)
{
	time_t tDate = 0;

	// date format is either ISO or regional short
	if (!CDateHelper::DecodeISODate(sDate, tDate))
	{
		CDateHelper::DecodeLocalShortDate(sDate, tDate);
	}

	return tDate;
}

void CTaskListCsvImporter::AddAttributeToTask(ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE nAttrib,
	const CStringArray& aValues) const
{
	int nCol = AttributeIndex(nAttrib);

	if (nCol == -1)
	{
		return;
	}

	CString sValue = aValues[nCol];

	switch (nAttrib)
	{
	case TDCA_CREATEDBY:
		pTasks->SetTaskCreatedBy(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_CATEGORY:
		{
			CStringArray aValues;

			if (Misc::Split(sValue, _T('+'), aValues))
			{
				for (int nVal = 0; nVal < aValues.GetSize(); nVal++)
				{
					pTasks->AddTaskCategory(hTask, ATL::CT2A(aValues[nVal]));
				}
			}
		}
		break;

	case TDCA_STATUS:
		pTasks->SetTaskStatus(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_EXTERNALID:
		pTasks->SetTaskExternalID(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_ALLOCBY:
		pTasks->SetTaskAllocatedBy(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_ALLOCTO:
		{
			CStringArray aValues;

			if (Misc::Split(sValue, _T('+'), aValues))
			{
				for (int nVal = 0; nVal < aValues.GetSize(); nVal++)
				{
					pTasks->AddTaskAllocatedTo(hTask, ATL::CT2A(aValues[nVal]));
				}
			}
		}
		break;

	case TDCA_VERSION:
		pTasks->SetTaskVersion(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_FILEREF:
		pTasks->SetTaskFileReferencePath(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_DEPENDENCY:
		{
			CStringArray aValues;

			if (Misc::Split(sValue, _T('+'), aValues))
			{
				for (int nVal = 0; nVal < aValues.GetSize(); nVal++)
				{
					pTasks->AddTaskDependency(hTask, ATL::CT2A(aValues[nVal]));
				}
			}
		}
		break;

	case TDCA_COMMENTS:
		pTasks->SetTaskComments(hTask, ATL::CT2A(sValue));
		break;

	case TDCA_STARTDATE:
		pTasks->SetTaskStartDate(hTask, String2Date(sValue));
		break;

	case TDCA_DUEDATE:
		pTasks->SetTaskDueDate(hTask, String2Date(sValue));
		break;

	case TDCA_DONEDATE:
		pTasks->SetTaskDoneDate(hTask, String2Date(sValue));
		break;

	case TDCA_LASTMOD:
		pTasks->SetTaskLastModified(hTask, String2Date(sValue));
		break;

	case TDCA_CREATIONDATE:
		pTasks->SetTaskCreationDate(hTask, String2Date(sValue));
		break;

	case TDCA_PRIORITY:
		pTasks->SetTaskPriority(hTask, (unsigned char)_ttoi(sValue));
		break;

	case TDCA_RISK:
		pTasks->SetTaskRisk(hTask, (unsigned char)_ttoi(sValue));
		break;

	case TDCA_FLAG:
		pTasks->SetTaskFlag(hTask, (_ttoi(sValue) != 0));
		break;

	case TDCA_COST:
		pTasks->SetTaskCost(hTask, _tstof(sValue));
		break;

	case TDCA_TIMEEST:
		pTasks->SetTaskTimeEstimate(hTask, _tstof(sValue), _T('H'));
		break;

	case TDCA_TIMESPENT:
		pTasks->SetTaskTimeSpent(hTask, _tstof(sValue), _T('H'));
		break;
	}
}

