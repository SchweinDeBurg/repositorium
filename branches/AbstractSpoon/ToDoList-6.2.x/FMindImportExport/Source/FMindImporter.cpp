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
//*****************************************************************************

// FMindImporter.cpp: implementation of the CFMindImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FMindImportExportApp.h"
#include "FMindImporter.h"

#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFMindImporter::CFMindImporter()
{
}

CFMindImporter::~CFMindImporter()
{
}

bool CFMindImporter::Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile)
{
	ITaskList7* pTL5 = 0;
	pDestTaskFile->QueryInterface(IID_TASKLIST5, reinterpret_cast<void**>(&pTL5));

	CXmlFile file;

	if (!file.Load(szSrcFilePath, _T("map")))
	{
		return false;
	}



	const CXmlItem* pXIMLOTree = file.Root();

	if (!pXIMLOTree)
	{
		return false;
	}

	//Get the Main Node
	const CXmlItem* pFMTask = pXIMLOTree->GetItem(_T("node"));

	if (!pFMTask)
	{
		return true;   // just means it was an empty tasklist
	}

	CString sTitle = pFMTask->GetItemValue(_T("TEXT"));
	pDestTaskFile->SetProjectName(ATL::CT2A(sTitle));

	//Get the Topics Node
	pFMTask = pFMTask->GetItem(_T("node"));
	if (!pFMTask)
	{
		return true;   // just means it was an empty tasklist
	}

	return ImportTask(pFMTask, pTL5, NULL); // NULL ==  root
}

bool CFMindImporter::ImportTask(const CXmlItem* pFMTask, ITaskList7* pDestTaskFile, HTASKITEM hParent) const
{
	ASSERT(pFMTask);

	CString sTask = pFMTask->GetItemValue(_T("TEXT"));

	if (sTask.IsEmpty())
	{
		// check for version 0.9 text
		const CXmlItem* pXIRich = pFMTask->GetItem(_T("richcontent")); // abstract into a function

		while (pXIRich && sTask.IsEmpty())
		{
			if (pXIRich->GetItemValue(_T("TYPE")) == _T("NODE"))
			{
				sTask = pXIRich->GetValue();
			}

			pXIRich = pXIRich->GetSibling();
		}
	}

	HTASKITEM hTask = pDestTaskFile->NewTask(ATL::CT2A(sTask), hParent);

	if (!hTask)
	{
		return false;
	}

	time_t tTimeCreated = GetDate(pFMTask, _T("CREATED")); // in tick count
	if (tTimeCreated != 0)
	{
		pDestTaskFile->SetTaskCreationDate(hTask, tTimeCreated);
	}

	// Modified date (== MODIFIED)
	time_t tTimeModified = GetDate(pFMTask, _T("MODIFIED")); // in tick count
	if (tTimeModified != 0)
	{
		pDestTaskFile->SetTaskLastModified(hTask, tTimeModified);
	}

	// Not Exposed Yet by todolist
	//BACKGROUND_COLOR="#ffff00"
	//long lBGColor = GetColor(pFMTask , "BACKGROUND_COLOR");
	//if (lBGColor != 0)
	//	pDestTaskFile->SetTaskColor(hTask, lBGColor);

	//COLOR="#00b439"
	long lFGColor = GetColor(pFMTask , _T("COLOR"));
	if (lFGColor != 0)
	{
		pDestTaskFile->SetTaskColor(hTask, lFGColor);
	}

	// Completion Status (== icon)
	const CXmlItem* pXIIcon = pFMTask->GetItem(_T("icon"));

	while (pXIIcon)
	{
		const CXmlItem* pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("button_ok"), TRUE);

		// For Completion Status - <icon BUILTIN="button_ok"/>
		//For Task Status - <icon BUILTIN="full-1"/>
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskDoneDate(hTask, tTimeModified);
			pDestTaskFile->SetTaskPercentDone(hTask, 100);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-1"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 1);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-2"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 2);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-3"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 3);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-4"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 4);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-5"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 5);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-6"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 6);
		}

		pXIIconItem = pXIIcon->FindItem(_T("BUILTIN"), _T("full-7"), TRUE);
		if (pXIIconItem != NULL)
		{
			pDestTaskFile->SetTaskPriority(hTask , 7);
		}

		pXIIcon = pXIIcon->GetSibling();
	}

	// check for version 0.9 comments
	CString sComments;
	const CXmlItem* pXIRich = pFMTask->GetItem(_T("richcontent"));

	while (pXIRich)
	{
		if (pXIRich->GetItemValue(_T("TYPE")) == _T("NOTE"))
		{
			sComments = pXIRich->GetValue();
		}

		pXIRich = pXIRich->GetSibling();
	}

	//Start of Attrib List not Supported by FreeMind

	//virtual bool SetTaskComments(HTASKITEM hTask, const char* szComments) = 0;
	if (sComments.IsEmpty())
	{
		sComments = GetAttribValueS(pFMTask, "Comments");
	}

	pDestTaskFile->SetTaskComments(hTask, ATL::CT2A(sComments));


	//virtual bool SetTaskStatus(HTASKITEM hTask, const char* szStatus) = 0;
	pDestTaskFile->SetTaskStatus(hTask, ATL::CT2A(GetAttribValueS(pFMTask, "TaskStatus")));


	//virtual bool SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath) = 0;
	pDestTaskFile->SetTaskFileReferencePath(hTask, ATL::CT2A(GetAttribValueS(pFMTask, "TaskFileReferencePath")));

	//Already Handled
	//virtual bool SetTaskColor(HTASKITEM hTask, unsigned long nColor) = 0;
	//pDestTaskFile->SetTaskColor(hTask,GetAttribValueI(pFMTask,"TaskColor"));

	//Already Handled
	//virtual bool SetTaskPriority(HTASKITEM hTask, unsigned char nPriority) = 0;
	//pDestTaskFile->SetTaskPriority(hTask,GetAttribValueI(pFMTask,"TaskPriority"));

	//Already Handled
	//virtual bool SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent) = 0;
	//pDestTaskFile->SetTaskPercentDone(hTask,GetAttribValueI(pFMTask,"TaskPercentDone"));

	//virtual bool SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits) = 0;
	pDestTaskFile->SetTaskTimeEstimate(hTask, GetAttribValueD(pFMTask, "TaskTimeEstimate"), 0);

	//virtual bool SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits) = 0;
	pDestTaskFile->SetTaskTimeSpent(hTask, GetAttribValueD(pFMTask, "TaskTimeSpent"), 0);

	//Already Handled
	//virtual bool SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate) = 0;
	//pDestTaskFile->SetTaskDoneDate(hTask,(time_t)GetAttribValueD(pFMTask,"TaskDoneDate"));

	//virtual bool SetTaskDueDate(HTASKITEM hTask, time_t tDueDate) = 0;
	pDestTaskFile->SetTaskDueDate(hTask, (time_t)GetAttribValueD(pFMTask, "TaskDueDate"));

	//virtual bool SetTaskStartDate(HTASKITEM hTask, time_t tStartDate) = 0;
	pDestTaskFile->SetTaskStartDate(hTask, (time_t)GetAttribValueD(pFMTask, "TaskStartDate"));

	//virtual bool SetTaskPosition(HTASKITEM hTask, unsigned long nPos) = 0;
	pDestTaskFile->SetTaskPosition(hTask, (long)GetAttribValueD(pFMTask, "TaskPosition"));

	//virtual bool SetTaskFlag(HTASKITEM hTask, bool bFlag) = 0;
	pDestTaskFile->SetTaskFlag(hTask, GetAttribValueB(pFMTask, "TaskFlag"));

	//virtual bool SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy) = 0;
	pDestTaskFile->SetTaskCreatedBy(hTask, ATL::CT2A(GetAttribValueS(pFMTask, "TaskCreatedBy")));

	//Already Handled
	//virtual bool SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate) = 0;
	//pDestTaskFile->SetTaskCreationDate(hTask,(time_t)GetAttribValueD(pFMTask,"TaskCreationDate"));


	//virtual bool SetTaskRisk(HTASKITEM hTask, unsigned char nRisk) = 0;
	pDestTaskFile->SetTaskRisk(hTask, (unsigned char)GetAttribValueI(pFMTask, "TaskRisk"));


	//virtual bool SetTaskExternalID(HTASKITEM hTask, const char* szID) = 0;
	pDestTaskFile->SetTaskExternalID(hTask, ATL::CT2A(GetAttribValueS(pFMTask, "TaskExternalID")));

	//virtual bool SetTaskCost(HTASKITEM hTask, double dCost) = 0;
	pDestTaskFile->SetTaskCost(hTask, GetAttribValueD(pFMTask, "TaskCost"));

	//virtual bool SetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1,
	//								DWORD& dwSpecific2, BOOL& bRecalcFromDue) const = 0;
	int nRegularity, nReuse = 0;
	DWORD dwSpecific1, dwSpecific2;
	BOOL bRecalcFromDue;

	nRegularity = GetAttribValueI(pFMTask, "nRegularity");
	dwSpecific1 = GetAttribValueI(pFMTask, "dwSpecific1");
	dwSpecific2 = GetAttribValueI(pFMTask, "dwSpecific2");
	bRecalcFromDue = GetAttribValueB(pFMTask, "bRecalcFromDue");

	pDestTaskFile->SetTaskRecurrence(hTask, nRegularity, dwSpecific1, dwSpecific2, bRecalcFromDue, nReuse);

	//virtual bool SetTaskVersion(HTASKITEM hTask, const char* szVersion) = 0;
	pDestTaskFile->SetTaskVersion(hTask, ATL::CT2A(GetAttribValueS(pFMTask, "TaskVersion")));

	//End of Attrib List not Supported by FreeMind


	// children
	const CXmlItem* pXIMLOSubTask = pFMTask->GetItem(_T("node"));

	if (pXIMLOSubTask)
	{
		if (!ImportTask(pXIMLOSubTask, pDestTaskFile, hTask))
		{
			return false;
		}
	}

	// siblings
	pFMTask = pFMTask->GetSibling();

	if (pFMTask)
	{
		return ImportTask(pFMTask, pDestTaskFile, hParent);
	}

	// else
	return true;
}

CString CFMindImporter::GetTaskRichContent(const CXmlItem* pFMTask , const char* szRichType) const
{
	UNREFERENCED_PARAMETER(szRichType);

	const CXmlItem* pXIRich = pFMTask->GetItem(_T("richcontent"));

	while (pXIRich)
	{
		if (pXIRich->GetItemValue(_T("TYPE")) == _T("NOTE"))
		{
			return pXIRich->GetValue();
		}

		pXIRich = pXIRich->GetSibling();
	}

	// else
	return _T("");
}

//Java Time date to CTime
long CFMindImporter::GetDate(const CXmlItem* pFMTask, LPCTSTR szColorField) const
{
	CString sDate = pFMTask->GetItemValue(szColorField);
	if (sDate.IsEmpty())
	{
		return 0;
	}
	//Date will be in Milliseconds from Jan 1, 1970.
	//Convert the Milliseconds to Sec
	sDate = sDate.Mid(0, sDate.GetLength() - 3);
	long lVal = _ttol(sDate);
	return lVal;
}


//Color Manipulation Code borrowed from - http://www.codeproject.com/gdi/ccolor.asp
long CFMindImporter::GetColor(const CXmlItem* pFMTask, LPCTSTR szColorField) const
{
	CString sColor = pFMTask->GetItemValue(szColorField);
	if (sColor.GetLength() < 2)
	{
		return 0;
	}
	if (sColor[0] == _T('#'))
	{
		LPTSTR szStop;
		ULONG ret = (ULONG)_tcstoul(sColor, &szStop, 16);
		return RGB(GetBValue(ret), GetGValue(ret), GetRValue(ret));
	}
	return 0;
}

CString CFMindImporter::GetAttribValueS(const CXmlItem* pFMTask , const char* szAttribName) const
{
	// Completion Status (== icon)
	const CXmlItem* pXIAttrib = pFMTask->GetItem(_T("attribute"));
	if (pXIAttrib == NULL)
	{
		return _T("");
	}

	while (pXIAttrib)
	{
		const CXmlItem* pXIAttribItemName = pXIAttrib->GetItem(_T("NAME"));
		if (pXIAttribItemName != NULL)
		{
			if (pXIAttribItemName->ValueMatches(ATL::CA2T(szAttribName)))
			{
				const CXmlItem* pXIAttribItemValue = pXIAttrib->GetItem(_T("VALUE"));
				if (pXIAttribItemValue != NULL)
				{
					return pXIAttribItemValue->GetValue();
				}
			}

		}

		pXIAttrib = pXIAttrib->GetSibling();
	}
	return _T("");
}

int CFMindImporter::GetAttribValueI(const CXmlItem* pFMTask , const char* szAttribName) const
{
	CString Result = GetAttribValueS(pFMTask, szAttribName);
	return _ttoi(Result);
}

bool CFMindImporter::GetAttribValueB(const CXmlItem* pFMTask , const char* szAttribName) const
{
	return (GetAttribValueI(pFMTask, szAttribName) != 0);

}

double CFMindImporter::GetAttribValueD(const CXmlItem* pFMTask , const char* szAttribName) const
{
	CString Result = GetAttribValueS(pFMTask, szAttribName);
	return Misc::Atof(Result);
}
