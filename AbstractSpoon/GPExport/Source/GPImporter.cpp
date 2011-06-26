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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// GPImporter.cpp: implementation of the CGPImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GPExportApp.h"
#include "GPImporter.h"

#include "../../Common/XmlFileEx.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/EnString.h"
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

CGPImporter::CGPImporter():
m_bBumpTaskIDs(FALSE)
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

	ITaskList8* pITL8 = GetITLInterface<ITaskList8>(pDestTaskFile, IID_TASKLIST8);

	// GP supports tasks having an id of ZERO which is a big
	// no-no in TDL. so we need to detect this ahead of time
	// so we can bump all the ids by 1
	m_bBumpTaskIDs = HasZeroIDTask(pXISrcTask);

	if (!ImportTask(pXISrcTask, pITL8, NULL))
	{
		return false;
	}

	// fix up resource allocations
	FixupResourceAllocations(fileSrc.Root(), pITL8);

	// and dependencies
	FixupDependencies(pXISrcTask, pITL8);

	return true; // no tasks to import
}

BOOL CGPImporter::HasZeroIDTask(const CXmlItem* pXISrcTask) const
{
	ASSERT(!pXISrcTask || pXISrcTask->NameIs(_T("task")));

	if (!pXISrcTask)
	{
		return FALSE;
	}

	if (pXISrcTask->GetItemValueI(_T("id")) == 0)
	{
		return TRUE;
	}

	// children
	if (HasZeroIDTask(pXISrcTask->GetItem(_T("task"))))
	{
		return TRUE;
	}

	// siblings
	return HasZeroIDTask(pXISrcTask->GetSibling());
}

void CGPImporter::FixupResourceAllocations(const CXmlItem* pXISrcPrj, ITaskList8* pDestTaskFile)
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
			HTASKITEM hTask = pDestTaskFile->FindTask(nTaskID);

			if (hTask)
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

bool CGPImporter::ImportTask(const CXmlItem* pXISrcTask, ITaskList8* pDestTaskFile, HTASKITEM htDestParent)
{
	if (!pXISrcTask)
	{
		return true;
	}

	CString sName = pXISrcTask->GetItemValue(_T("name"));
	DWORD dwID = pXISrcTask->GetItemValueI(_T("id"));

	// do we need to bump it?
	if (m_bBumpTaskIDs)
	{
		dwID++;
	}

	HTASKITEM hTask = pDestTaskFile->NewTask(ATL::CT2A(sName), htDestParent, dwID);
	ASSERT(hTask);

	if (!hTask)
	{
		return false;
	}

	// completion
	int nPercentDone = pXISrcTask->GetItemValueI(_T("complete"));
	pDestTaskFile->SetTaskPercentDone(hTask, (unsigned char)nPercentDone);

	// dates
	time_t tStart;

	if (CDateHelper::DecodeISODate(pXISrcTask->GetItemValue(_T("start")), tStart))
	{
		pDestTaskFile->SetTaskStartDate(hTask, tStart);

		int nDuration = pXISrcTask->GetItemValueI(_T("duration"));

		// only add duration to leaf tasks else it'll double up
		if (nDuration && !pXISrcTask->HasItem(_T("task")))
		{
			if (nPercentDone == 100)
			{
				pDestTaskFile->SetTaskDoneDate(hTask, tStart + (nDuration - 1) * ONEDAY); // gp dates are inclusive
				pDestTaskFile->SetTaskTimeSpent(hTask, nDuration, _T('D'));
			}
			else
			{
				pDestTaskFile->SetTaskDueDate(hTask, tStart + (nDuration - 1) * ONEDAY); // gp dates are inclusive
				pDestTaskFile->SetTaskTimeEstimate(hTask, nDuration, _T('D'));
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
	// do this after we've imported all the tasks because GP does it
	// the opposite way round to TDL

	// children
	if (!ImportTask(pXISrcTask->GetItem(_T("task")), pDestTaskFile, hTask))
	{
		return false;
	}

	// siblings
	return ImportTask(pXISrcTask->GetSibling(), pDestTaskFile, htDestParent);
}

void CGPImporter::FixupDependencies(const CXmlItem* pXISrcTask, ITaskList8* pDestTaskFile)
{
	if (!pXISrcTask)
	{
		return;
	}

	const CXmlItem* pXIDepends = pXISrcTask->GetItem(_T("depend"));

	while (pXIDepends)
	{
		if (pXIDepends->GetItemValueI(_T("type")) == 2)
		{
			// GP records dependencies in reverse to TDL
			// so what appears to be the dependency target is actually the source
			DWORD dwSrcDependID = pXIDepends->GetItemValueI(_T("id"));

			// do we need to bump it?
			if (m_bBumpTaskIDs)
			{
				dwSrcDependID++;
			}

			HTASKITEM hTask = pDestTaskFile->FindTask(dwSrcDependID);

			if (hTask)
			{
				DWORD dwDestDependID = pXISrcTask->GetItemValueI(_T("id"));

				// do we need to bump it?
				if (m_bBumpTaskIDs)
				{
					dwDestDependID++;
				}

				pDestTaskFile->AddTaskDependency(hTask, dwDestDependID);
			}
		}

		// next dependency
		pXIDepends = pXIDepends->GetSibling();
	}

	// children
	FixupDependencies(pXISrcTask->GetItem(_T("task")), pDestTaskFile);

	// siblings
	FixupDependencies(pXISrcTask->GetSibling(), pDestTaskFile);
}

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
