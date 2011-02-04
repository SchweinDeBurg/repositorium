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

// GPImporter.cpp: implementation of the CGPImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GPExportApp.h"
#include "GPImporter.h"

#include "../../Common/XmlFileEx.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../ToDoList/Source/TDLSchemaDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const UINT ONEDAY = 24 * 60 * 60;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPImporter::CGPImporter()
{
}

CGPImporter::~CGPImporter()
{
}

bool CGPImporter::Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile)
{
	CXmlFile fileSrc;

	if (!fileSrc.Load(szSrcFilePath, _T("project")))
	{
		return false;
	}

	const CXmlItem* pXISrcTasks = fileSrc.GetItem(_T("tasks"));

	if (!pXISrcTasks) // must exist
	{
		return false;
	}

	const CXmlItem* pXISrcTask = pXISrcTasks->GetItem(_T("task"));

	if (!pXISrcTask) // must exist
	{
		return false;
	}

	ITaskList7* pITL7 = GetITLInterface<ITaskList7>(pDestTaskFile, IID_TASKLIST7);

	if (!ImportTask(pXISrcTask, pITL7, NULL))
	{
		return false;
	}

	// fix up resource allocations
	FixupResourceAllocations(fileSrc.Root(), pITL7);

	// and dependencies
	//FixupDependencies(pITL7, pITL7->GetFirstTask());

	return true; // no tasks to import
}

void CGPImporter::FixupResourceAllocations(const CXmlItem* pXISrcPrj, ITaskList7* pDestTaskFile)
{
	BuildResourceMap(pXISrcPrj);

	const CXmlItem* pXIAllocations = pXISrcPrj->GetItem(_T("allocations"));

	if (pXIAllocations && m_mapResources.GetCount())
	{
		const CXmlItem* pXIAlloc = pXIAllocations->GetItem(_T("allocation"));

		while (pXIAlloc)
		{
			int nTaskID = pXIAlloc->GetItemValueI(_T("task-id"));
			int nResID = pXIAlloc->GetItemValueI(_T("resource-id"));

			// look up task
			HTASKITEM hTask = NULL;

			if (m_mapTasks.Lookup(nTaskID, hTask) && hTask)
			{
				CString sRes;

				if (m_mapResources.Lookup(nResID, sRes) && !sRes.IsEmpty())
				{
					pDestTaskFile->AddTaskAllocatedTo(hTask, ATL::CT2A(sRes));
				}
			}

			pXIAlloc = pXIAlloc->GetSibling();
		}
	}
}

bool CGPImporter::ImportTask(const CXmlItem* pXISrcTask, ITaskList7* pDestTaskFile, HTASKITEM htDestParent)
{
	if (!pXISrcTask)
	{
		return true;
	}

	CString sName = pXISrcTask->GetItemValue(_T("name"));
	HTASKITEM hTask = pDestTaskFile->NewTask(ATL::CT2A(sName), htDestParent);
	ASSERT(hTask);

	if (!hTask)
	{
		return false;
	}

	// attributes

	// store GP id in external ID field and add to map to that we can
	// set up the alloc to and dependencies afterwards
	pDestTaskFile->SetTaskExternalID(hTask, ATL::CT2A(pXISrcTask->GetItemValue(_T("id"))));
	m_mapTasks[pXISrcTask->GetItemValueI(_T("id"))] = hTask;

	// completion
	int nPercentDone = pXISrcTask->GetItemValueI(_T("complete"));
	pDestTaskFile->SetTaskPercentDone(hTask, (unsigned char)nPercentDone);

	// dates
	time_t tStart;

	if (CDateHelper::DecodeISODate(pXISrcTask->GetItemValue(_T("start")), tStart))
	{
		pDestTaskFile->SetTaskStartDate(hTask, tStart);

		int nDuration = pXISrcTask->GetItemValueI(_T("duration"));

		if (nDuration)
		{
			if (nPercentDone == 100)
			{
				pDestTaskFile->SetTaskDoneDate(hTask, tStart + (nDuration - 1) * ONEDAY);   // gp dates are inclusive
			}
			else
			{
				pDestTaskFile->SetTaskDueDate(hTask, tStart + (nDuration - 1) * ONEDAY);   // gp dates are inclusive
			}
		}
	}

	// priority
	int nPriority = pXISrcTask->GetItemValueI(_T("priority"));
	pDestTaskFile->SetTaskPriority(hTask, (unsigned char)(nPriority * 3 + 2)); // 2, 5, 8

	// file ref
	CString sFileRef = pXISrcTask->GetItemValue(_T("webLink"));
	sFileRef.TrimLeft();

	if (!sFileRef.IsEmpty())
	{
		// decode file paths
		if (sFileRef.Find(_T("file://")) == 0)
		{
			sFileRef = sFileRef.Mid(7);
			sFileRef.Replace(_T("%20"), _T(""));
		}

		pDestTaskFile->SetTaskFileReferencePath(hTask, ATL::CT2A(sFileRef));
	}

	// comments
	pDestTaskFile->SetTaskComments(hTask, ATL::CT2A(pXISrcTask->GetItemValue(_T("notes"))));

	// dependency
	// we just set this to the GP id and fix it up later on once we
	// have finished mapping all the tasks
	const CXmlItem* pXIDepends = pXISrcTask->GetItem(_T("depend"));

	if (pXIDepends && pXIDepends->GetItemValueI(_T("type")) == 3)
	{
		pDestTaskFile->SetTaskDependency(hTask, ATL::CT2A(pXIDepends->GetItemValue(_T("id"))));
	}

	// children (?)
	if (!ImportTask(pXISrcTask->GetItem(_T("task")), pDestTaskFile, hTask))
	{
		return false;
	}

	// siblings
	return ImportTask(pXISrcTask->GetSibling(), pDestTaskFile, htDestParent);
}

/*
void CGPImporter::FixupDependencies(ITaskList7* pDestTaskFile, HTASKITEM hTask)
{
	if (!hTask)
		return;

	CString sDepends = pDestTaskFile->GetTaskDependency(hTask, 0);

	if (!sDepends.IsEmpty())
	{
		int nDepends = atoi(sDepends);

		// look up task
		HTASKITEM hDepends = NULL;

		if (m_mapTasks.Lookup(nDepends, hDepends) && hDepends)
		{
			// extract correct depends task ID
			nDepends = pDestTaskFile->GetTaskID(hDepends);
			sDepends.Format("%d", nDepends);

			pDestTaskFile->SetTaskDependency(hTask, sDepends);
		}
	}

	// children
	FixupDependencies(pDestTaskFile, pDestTaskFile->GetFirstTask(hTask));

	// siblings
	FixupDependencies(pDestTaskFile, pDestTaskFile->GetNextTask(hTask));
}
*/

void CGPImporter::BuildResourceMap(const CXmlItem* pXISrcPrj)
{
	m_mapResources.RemoveAll();

	const CXmlItem* pXIResources = pXISrcPrj->GetItem(_T("resources"));

	if (pXIResources)
	{
		const CXmlItem* pXIRes = pXIResources->GetItem(_T("resource"));

		while (pXIRes)
		{
			m_mapResources[pXIRes->GetItemValueI(_T("id"))] = pXIRes->GetItemValue(_T("name"));
			pXIRes = pXIRes->GetSibling();
		}
	}
}
