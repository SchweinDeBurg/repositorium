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
static char THIS_FILE[]=__FILE__;
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

bool CGPImporter::Import(const char* szSrcFilePath, ITaskList* pDestTaskFile)
{
	CXmlFile fileSrc;

	if (!fileSrc.Load(szSrcFilePath, "project"))
		return false;

	const CXmlItem* pXISrcTasks = fileSrc.GetItem("tasks");

	if (!pXISrcTasks) // must exist
		return false;

	const CXmlItem* pXISrcTask = pXISrcTasks->GetItem("task");

	if (!pXISrcTask) // must exist
		return false;

	ITaskList7* pITL7 = GetITLInterface<ITaskList7>(pDestTaskFile, IID_TASKLIST7);

	if (!ImportTask(pXISrcTask, pITL7, NULL))
		return false;

	// fix up resource allocations
	FixupResourceAllocations(fileSrc.Root(), pITL7);

	// and dependencies
	//FixupDependencies(pITL7, pITL7->GetFirstTask());

	return true; // no tasks to import
}

void CGPImporter::FixupResourceAllocations(const CXmlItem* pXISrcPrj, ITaskList7* pDestTaskFile)
{
	BuildResourceMap(pXISrcPrj);
			
	const CXmlItem* pXIAllocations = pXISrcPrj->GetItem("allocations");

	if (pXIAllocations && m_mapResources.GetCount())
	{
		const CXmlItem* pXIAlloc = pXIAllocations->GetItem("allocation");

		while (pXIAlloc)
		{
			int nTaskID = pXIAlloc->GetItemValueI("task-id");
			int nResID = pXIAlloc->GetItemValueI("resource-id");

			// look up task
			HTASKITEM hTask = NULL;

			if (m_mapTasks.Lookup(nTaskID, hTask) && hTask)
			{
				CString sRes;

				if (m_mapResources.Lookup(nResID, sRes) && !sRes.IsEmpty())
					pDestTaskFile->AddTaskAllocatedTo(hTask, sRes);
			}
			
			pXIAlloc = pXIAlloc->GetSibling();
		}
	}
}

bool CGPImporter::ImportTask(const CXmlItem* pXISrcTask, ITaskList7* pDestTaskFile, HTASKITEM htDestParent)
{
	if (!pXISrcTask)
		return true;

	CString sName = pXISrcTask->GetItemValue("name");
	HTASKITEM hTask = pDestTaskFile->NewTask(sName, htDestParent);
	ASSERT (hTask);

	if (!hTask)
		return false;

	// attributes

	// store GP id in external ID field and add to map to that we can 
	// set up the alloc to and dependencies afterwards
	pDestTaskFile->SetTaskExternalID(hTask, pXISrcTask->GetItemValue("id"));
	m_mapTasks[pXISrcTask->GetItemValueI("id")] = hTask;

	// completion
	int nPercentDone = pXISrcTask->GetItemValueI("complete");
	pDestTaskFile->SetTaskPercentDone(hTask, (unsigned char)nPercentDone);

	// dates
	time_t tStart;
	
	if (CDateHelper::DecodeISODate(pXISrcTask->GetItemValue("start"), tStart))
	{
		pDestTaskFile->SetTaskStartDate(hTask, tStart);

		int nDuration = pXISrcTask->GetItemValueI("duration");

		if (nDuration)
		{
			if (nPercentDone == 100)
				pDestTaskFile->SetTaskDoneDate(hTask, tStart + (nDuration - 1) * ONEDAY); // gp dates are inclusive
			else
				pDestTaskFile->SetTaskDueDate(hTask, tStart + (nDuration - 1) * ONEDAY); // gp dates are inclusive
		}
	}

	// priority
	int nPriority = pXISrcTask->GetItemValueI("priority");
	pDestTaskFile->SetTaskPriority(hTask, (unsigned char)(nPriority * 3 + 2)); // 2, 5, 8

	// file ref
	CString sFileRef = pXISrcTask->GetItemValue("webLink");
	sFileRef.TrimLeft();

	if (!sFileRef.IsEmpty())
	{
		// decode file paths
		if (sFileRef.Find("file://") == 0)
		{
			sFileRef = sFileRef.Mid(7);
			sFileRef.Replace("%20", "");
		}

		pDestTaskFile->SetTaskFileReferencePath(hTask, sFileRef);
	}

	// comments
	pDestTaskFile->SetTaskComments(hTask, pXISrcTask->GetItemValue("notes"));

	// dependency
	// we just set this to the GP id and fix it up later on once we
	// have finished mapping all the tasks
	const CXmlItem* pXIDepends = pXISrcTask->GetItem("depend");

	if (pXIDepends && pXIDepends->GetItemValueI("type") == 3)
		pDestTaskFile->SetTaskDependency(hTask, pXIDepends->GetItemValue("id"));

	// children (?)
	if (!ImportTask(pXISrcTask->GetItem("task"), pDestTaskFile, hTask))
		return false;

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

	const CXmlItem* pXIResources = pXISrcPrj->GetItem("resources");

	if (pXIResources)
	{
		const CXmlItem* pXIRes = pXIResources->GetItem("resource");

		while (pXIRes)
		{
			m_mapResources[pXIRes->GetItemValueI("id")] = pXIRes->GetItemValue("name");
			pXIRes = pXIRes->GetSibling();
		}
	}
}
