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

// GPExporter.cpp: implementation of the CGPExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GPExportApp.h"
#include "GPExporter.h"

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

CGPExporter::CGPExporter()
{

}

CGPExporter::~CGPExporter() 
{

}

bool CGPExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL /*bSilent*/)
{
	CXmlFile fileDest("project");

	const ITaskList7* pITL7 = GetITLInterface<ITaskList7>(pSrcTaskFile, IID_TASKLIST7);
	ASSERT (pITL7);

	// export resource allocations
	ExportResources(pITL7, fileDest.Root());

	// export tasks
	CXmlItem* pXITasks = fileDest.AddItem("tasks");
	CXmlItem* pXIAllocations = fileDest.AddItem("allocations");

	if (!ExportTask(pITL7, pSrcTaskFile->GetFirstTask(), pXITasks, pXIAllocations))
		return false;

	// important display stuff for GP
	SetupDisplay(fileDest.Root());
	SetupCalendar(fileDest.Root());

	// save result
	VERIFY (fileDest.Save(szDestFilePath));

	return true;
}

void CGPExporter::SetupDisplay(CXmlItem* pDestPrj)
{
	CXmlItem* pXIDisplay = pDestPrj->AddItem("taskdisplaycolumns");

	CXmlItem* pXIColumn = pXIDisplay->AddItem("displaycolumn");
	pXIColumn->SetItemValue("property-id", "tpd3");
	pXIColumn->SetItemValue("order", "0");
	pXIColumn->SetItemValue("width", "75");

	pXIColumn = pXIDisplay->AddItem("displaycolumn");
	pXIColumn->SetItemValue("property-id", "tpd4");
	pXIColumn->SetItemValue("order", "1");
	pXIColumn->SetItemValue("width", "75");

	pXIColumn = pXIDisplay->AddItem("displaycolumn");
	pXIColumn->SetItemValue("property-id", "tpd5");
	pXIColumn->SetItemValue("order", "2");
	pXIColumn->SetItemValue("width", "75");
}

void CGPExporter::SetupCalendar(CXmlItem* pDestPrj)
{
	/*
	<calendars>
	<day-types>
	<day-type id="0"/> 
	<day-type id="1"/> 
	<calendar id="1" name="default">
	<default-week sun="1" mon="0" tue="0" wed="0"
	thu="0" fri="0" sat="1"/>
	</calendar>
	</day-types>
	</calendars>
	*/
	
	CXmlItem* pXICals = pDestPrj->AddItem("calendars");
	CXmlItem* pXIDayTypes = pXICals->AddItem("day-types");
	
	CXmlItem* pXIDay = pXICals->AddItem("day-type");
	pXIDay->SetItemValue("id", "0"); // weekday
	
	pXIDay = pXICals->AddItem("day-type");
	pXIDay->SetItemValue("id", "1"); // weekend
	
	CXmlItem* pXICal = pXIDayTypes->AddItem("calendar");
	
	pXICal->SetItemValue("id", "1");
	pXICal->SetItemValue("name", "default");
	
	CXmlItem* pXIWeek = pXICal->AddItem("default-week");
	
	pXIWeek->SetItemValue("sun", "1");
	pXIWeek->SetItemValue("mon", "0");
	pXIWeek->SetItemValue("tue", "0");
	pXIWeek->SetItemValue("wed", "0");
	pXIWeek->SetItemValue("thu", "0");
	pXIWeek->SetItemValue("fri", "0");
	pXIWeek->SetItemValue("sat", "1");
}


bool CGPExporter::ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, 
							 CXmlItem* pXIDestParent, CXmlItem* pXIAllocations)
{
	if (!hTask)
		return true;

	// create a new item corresponding to pXITask at the dest
	CXmlItem* pXIDestItem = pXIDestParent->AddItem("task");

	if (!pXIDestItem)
		return false;

	// copy across the appropriate attributes
	int nTaskID = (int)pSrcTaskFile->GetTaskID(hTask);
	pXIDestItem->AddItem("id", nTaskID);
	pXIDestItem->AddItem("name", pSrcTaskFile->GetTaskTitle(hTask));

	// colour
	pXIDestItem->AddItem("color", pSrcTaskFile->GetTaskAttribute(hTask, TDL_TASKTEXTWEBCOLOR));

	// dates
	time_t tStart = pSrcTaskFile->GetTaskStartDate(hTask);
	time_t tDue = pSrcTaskFile->GetTaskDueDate(hTask, FALSE);
	time_t tDone = pSrcTaskFile->GetTaskDoneDate(hTask);

	// if no start date then make something up
	if (!tStart)
	{
		if (tDue)
			tStart = tDue;
		else if (tDone)
			tStart = tDone;

		// else there are no dates so leave as-is
	}

	// if no due date then make something up
	if (tStart && !tDue && !tDone)
		tDue = tStart;

	if (tStart && (tDue || tDone))
	{
		COleDateTime start(tStart);
		pXIDestItem->AddItem("start", CDateHelper::FormatDate(start, DHFD_ISO));

		if (tDone >= tStart) // completion date takes precedence
		{
			int nDays = CDateHelper::CalcDaysFromTo(start, tDone, TRUE);
			pXIDestItem->AddItem("duration", nDays);
		}
		else if (tDue >= tStart)
		{
			int nDays = CDateHelper::CalcDaysFromTo(start, tDue, TRUE);
			pXIDestItem->AddItem("duration", nDays);
		}
	}

	// completion
	if (tDone)
	{
		pXIDestItem->AddItem("complete", "100");
		pXIDestItem->AddItem("priority", "-1");
	}
	else
	{
		pXIDestItem->AddItem("complete", pSrcTaskFile->GetTaskPercentDone(hTask, TRUE));

		int nPriority = pSrcTaskFile->GetTaskPriority(hTask, TRUE);
		
		if (nPriority < 0)
			pXIDestItem->AddItem("priority", "-1");
		else if (nPriority <= 3)
			pXIDestItem->AddItem("priority", 0); // low
		else if (nPriority <= 6)
			pXIDestItem->AddItem("priority", 1); // medium
		else
			pXIDestItem->AddItem("priority", 2); // high
	}
	
/*
	// dependency
	CString sDepends = pSrcTaskFile->GetTaskAttribute(hTask, TDL_TASKDEPENDENCY);

	if (!sDepends.IsEmpty())
	{
		int nDelim = sDepends.Find('?');

		if (nDelim == -1) // must be a dependency in the same tasklist
		{
			CXmlItem* pXIDepends = pXIDestItem->AddItem("depend");

			pXIDepends->AddItem("id", sDepends);
			pXIDepends->AddItem("type", "3");
		}
	}
*/

	// file/weblink
	CString sFileRef = pSrcTaskFile->GetTaskFileReferencePath(hTask);
	sFileRef.TrimLeft();

	if (!sFileRef.IsEmpty())
	{
		// web or file link?
		if (sFileRef.Find(":\\") == 1 || sFileRef.Find("\\\\") == 0)
			sFileRef = "file://" + sFileRef;

		sFileRef.Replace(" ", "%20");

		pXIDestItem->AddItem("webLink", sFileRef);
	}

	// comments
	const char* szComments = pSrcTaskFile->GetTaskComments(hTask);

	if (szComments && *szComments)
		pXIDestItem->AddItem("notes", szComments, XIT_CDATA);

	// resource allocation
	int nAllocTo = pSrcTaskFile->GetTaskAllocatedToCount(hTask);

	while (nAllocTo--)
	{
		CString sAllocTo = pSrcTaskFile->GetTaskAllocatedTo(hTask, nAllocTo);
		int nResID;

		if (m_mapResources.Lookup(sAllocTo, nResID))
		{
			CXmlItem* pXIAllocTo = pXIAllocations->AddItem("allocation");

			if (pXIAllocTo)
			{
				pXIAllocTo->AddItem("task-id", nTaskID);
				pXIAllocTo->AddItem("resource-id", nResID);
				pXIAllocTo->AddItem("responsible", "true");
				pXIAllocTo->AddItem("load", "100.0");
				pXIAllocTo->AddItem("function", "Default:0");
			}
		}
	}
	
	// copy across first child
	if (!ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pXIDestItem, pXIAllocations))
		return false;

	// copy across first sibling
	return ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pXIDestParent, pXIAllocations);
}

void CGPExporter::BuildResourceMap(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pDestPrj)
{
	if (!hTask)
		return;

	int nAllocTo = pSrcTaskFile->GetTaskAllocatedToCount(hTask);
	
	while (nAllocTo--)
	{
		CString sAllocTo = pSrcTaskFile->GetTaskAllocatedTo(hTask, nAllocTo);
		int nID = 0;
		
		if (!m_mapResources.Lookup(sAllocTo, nID))
		{
			nID = m_mapResources.GetCount();
			m_mapResources[sAllocTo] = nID;
		}
	}

	// children
	BuildResourceMap(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pDestPrj);

	// siblings
	BuildResourceMap(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pDestPrj);
}

void CGPExporter::ExportResources(const ITaskList7* pSrcTaskFile, CXmlItem* pDestPrj)
{
	BuildResourceMap(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), pDestPrj);

	if (m_mapResources.GetCount())
	{
		CXmlItem* pXIResources = pDestPrj->AddItem("resources");
		CString sRes;
		int nResID;

		POSITION pos = m_mapResources.GetStartPosition();

		while (pos)
		{
			m_mapResources.GetNextAssoc(pos, sRes, nResID);
			
			CXmlItem* pXIRes = pXIResources->AddItem("resource");

			if (pXIRes)
			{
				pXIRes->AddItem("name", sRes);
				pXIRes->AddItem("id", nResID);
				pXIRes->AddItem("function", "Default:0"); // crucial
			}
		}
	}
}