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

// MLOImporter.cpp: implementation of the CMLOImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MLOImporter.h"

#include <time.h>
#include <unknwn.h>

#include "../../Common/ITaskList.h"
#include "../../Common/XmlFile.h"
#include "../../../CodeProject/Source/DateHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMLOImporter::CMLOImporter()
{
}

CMLOImporter::~CMLOImporter()
{
}

bool CMLOImporter::Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile)
{
	ITaskList5* pTL5 = GetITLInterface<ITaskList5>(pDestTaskFile, IID_TASKLIST5);

	if (!pTL5)
	{
		return false;
	}

	CXmlFile file;

	if (!file.Load(szSrcFilePath, _T("MyLifeOrganized-xml")))
	{
		return false;
	}

	const CXmlItem* pXIMLOTree = file.GetItem(_T("TaskTree"));

	if (!pXIMLOTree)
	{
		return false;
	}

	const CXmlItem* pXIMLOTask = pXIMLOTree->GetItem(_T("TaskNode"));

	if (!pXIMLOTask)
	{
		return false;
	}

	// this first node always seems to be untitled
	// so we get the first subnode
	pXIMLOTask = pXIMLOTask->GetItem(_T("TaskNode"));

	if (!pXIMLOTask)
	{
		return true;   // just means it was an empty tasklist
	}

	return ImportTask(pXIMLOTask, pTL5, NULL); // NULL ==  root
}

bool CMLOImporter::ImportTask(const CXmlItem* pXIMLOTask, ITaskList5* pDestTaskFile, HTASKITEM hParent) const
{
	ASSERT(pXIMLOTask);

	HTASKITEM hTask = pDestTaskFile->NewTask(ATL::CT2A(pXIMLOTask->GetItemValue(_T("Caption"))), hParent);

	if (!hTask)
	{
		return false;
	}

	// priority (== Importance)
	int nPriority = (pXIMLOTask->GetItemValueI(_T("Importance")) * 10) / 100;

	pDestTaskFile->SetTaskPriority(hTask, (unsigned char)nPriority);

	// categories (== places)
	const CXmlItem* pXIPlace = pXIMLOTask->GetItem(_T("Places"), _T("Place"));

	while (pXIPlace)
	{
		pDestTaskFile->AddTaskCategory(hTask, ATL::CT2A(pXIPlace->GetValue()));
		pXIPlace = pXIPlace->GetSibling();
	}

	// estimated time (== EstimateMax)
	double dTimeEst = pXIMLOTask->GetItemValueF(_T("EstimateMax")); // in days

	pDestTaskFile->SetTaskTimeEstimate(hTask, dTimeEst, 'D');

	// due date (== DueDateTime)
	pDestTaskFile->SetTaskDueDate(hTask, GetDate(pXIMLOTask, _T("DueDateTime")));

	// completion (== CompletionDateTime)
	pDestTaskFile->SetTaskDoneDate(hTask, GetDate(pXIMLOTask, _T("CompletionDateTime")));

	// comments (== Note)
	CString sComments = pXIMLOTask->GetItemValue(_T("Note"));

	pDestTaskFile->SetTaskComments(hTask, ATL::CT2A(sComments));

	// children
	const CXmlItem* pXIMLOSubTask = pXIMLOTask->GetItem(_T("TaskNode"));

	if (pXIMLOSubTask)
	{
		if (!ImportTask(pXIMLOSubTask, pDestTaskFile, hTask))
		{
			return false;
		}
	}

	// siblings
	pXIMLOTask = pXIMLOTask->GetSibling();

	if (pXIMLOTask)
	{
		return ImportTask(pXIMLOTask, pDestTaskFile, hParent);
	}

	// else
	return true;
}

time_t CMLOImporter::GetDate(const CXmlItem* pXIMLOTask, LPCTSTR szDateField) const
{
	CString sDate = pXIMLOTask->GetItemValue(szDateField);
	time_t date = 0;

	if (!CDateHelper::DecodeISODate(sDate, date))
	{
		date = 0;
	}

	return date;
}
