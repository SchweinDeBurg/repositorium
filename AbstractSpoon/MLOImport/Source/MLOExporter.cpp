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

// MLOExporter.cpp: implementation of the CMLOExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MLOImportApp.h"
#include "MLOExporter.h"

#include "../../Common/XmlFileEx.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/TimeHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMLOExporter::CMLOExporter()
{

}

CMLOExporter::~CMLOExporter() 
{

}

bool CMLOExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL /*bSilent*/)
{
	CXmlFile fileDest("MyLifeOrganized-xml");

	const ITaskList7* pITL7 = GetITLInterface<ITaskList7>(pSrcTaskFile, IID_TASKLIST7);
	ASSERT (pITL7);

	// export tasks
	CXmlItem* pXITasks = fileDest.AddItem("TaskTree");

	if (!ExportTask(pITL7, pSrcTaskFile->GetFirstTask(), pXITasks))
		return false;

	// export resource allocations
	ExportPlaces(pITL7, fileDest.Root());

	// save result
	VERIFY (fileDest.Save(szDestFilePath));

	return true;
}

bool CMLOExporter::ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, 
							 CXmlItem* pXIDestParent)
{
	if (!hTask)
		return true;

	// create a new item corresponding to pXITask at the dest
	CXmlItem* pXIDestItem = pXIDestParent->AddItem("TaskNode");

	if (!pXIDestItem)
		return false;

	// copy across the appropriate attributes
	pXIDestItem->AddItem("Caption", pSrcTaskFile->GetTaskTitle(hTask));

   // priority
   int nPriority = pSrcTaskFile->GetTaskPriority(hTask, FALSE);
   int nImportance = max(nPriority, 0) * 10;

	pXIDestItem->AddItem("Importance", nImportance);

	// dates
	time_t tDue = pSrcTaskFile->GetTaskDueDate(hTask, FALSE);
	time_t tDone = pSrcTaskFile->GetTaskDoneDate(hTask);

	if (tDone)
		pXIDestItem->AddItem("CompletionDateTime", CDateHelper::FormatDate(tDone, DHFD_ISO));

	if (tDue)
		pXIDestItem->AddItem("DueDateTime", CDateHelper::FormatDate(tDue, DHFD_ISO));

   // time estimate
   char cTimeUnits;
  	double dTimeEst = pSrcTaskFile->GetTaskTimeEstimate(hTask, cTimeUnits, FALSE);
   
   if (dTimeEst > 0.0)
      pXIDestItem->AddItem("EstimateMax", CTimeHelper().GetTime(dTimeEst, cTimeUnits, THU_DAYS));
	
	// comments
	const char* szComments = pSrcTaskFile->GetTaskComments(hTask);

	if (szComments && *szComments)
		pXIDestItem->AddItem("Note", szComments);

	// copy across first child
	if (!ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pXIDestItem))
		return false;

	// copy across first sibling
	return ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pXIDestParent);
}

void CMLOExporter::BuildPlacesMap(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CMapStringToString& mapPlaces)
{
	if (!hTask)
		return;

	int nCat = pSrcTaskFile->GetTaskCategoryCount(hTask);
	
	while (nCat--)
	{
		CString sCat = pSrcTaskFile->GetTaskCategory(hTask, nCat);
      CString sCatUpper(sCat);
      sCat.MakeUpper();

      mapPlaces[sCatUpper] = sCat;
	}

	// children
	BuildPlacesMap(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), mapPlaces);

	// siblings
	BuildPlacesMap(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), mapPlaces);
}

void CMLOExporter::ExportPlaces(const ITaskList7* pSrcTaskFile, CXmlItem* pDestPrj)
{
   CMapStringToString mapPlaces;
	BuildPlacesMap(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), mapPlaces);

	if (mapPlaces.GetCount())
	{
		CXmlItem* pXIResources = pDestPrj->AddItem("PlacesList");
		CString sPlace, sPlaceUpper;

		POSITION pos = mapPlaces.GetStartPosition();

		while (pos)
		{
			mapPlaces.GetNextAssoc(pos, sPlaceUpper, sPlace);
			
			CXmlItem* pXIRes = pXIResources->AddItem("TaskPlace");

			if (pXIRes)
				pXIRes->AddItem("Caption", sPlace);
		}
	}
}
