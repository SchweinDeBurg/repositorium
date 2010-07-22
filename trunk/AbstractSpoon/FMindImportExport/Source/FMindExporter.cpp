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

// FMindExporter.cpp: implementation of the CFMindExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FMindImportExportApp.h"
#include "FMindExporter.h"

#include "../../Common/XmlFileEx.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../ToDoList/Source/TDLSchemaDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFMindExporter::CFMindExporter()
{

}

CFMindExporter::~CFMindExporter()
{

}

bool CFMindExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL /*bSilent*/)
{
	CXmlFile fileDest("map");
	//fileDest.EnableFormattedOutput(TRUE);	

	CXmlItem *firstItem = fileDest.Root()->AddItem("node","");


	firstItem->AddItem("TEXT",pSrcTaskFile->GetProjectName());	
	CXmlItem * hookItem = firstItem->AddItem("hook","");
	hookItem->AddItem("NAME","accessories/plugins/AutomaticLayout.properties");

	//Attrib Manager settings
	//This will make the attribs not to be shown as a list view at every node;	
	CXmlItem *attribManItem = firstItem->AddItem("attribute_registry","");	
	attribManItem->AddItem("SHOW_ATTRIBUTES","hide");

	// export first task
	ExportTask((ITaskList7*)pSrcTaskFile, pSrcTaskFile->GetFirstTask(), firstItem , 0);

	// save result
	VERIFY (fileDest.Save(szDestFilePath));



	return true;
}

void CFMindExporter::ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent , int LEVEL)
{
	if (!hTask)
		return;

	char cTemp;

	// create a new item corresponding to pXITask at the dest
	CXmlItem* pXIDestItem = pXIDestParent->AddItem("node");

	// copy across the appropriate attributes
	//pXIDestItem->AddItem("id", (int)pSrcTaskFile->GetTaskID(hTask));
	pXIDestItem->AddItem("TEXT", pSrcTaskFile->GetTaskTitle(hTask));

	CString sModified;
	sModified.Format("%i",pSrcTaskFile->GetTaskLastModified(hTask) * 1000);
	pXIDestItem->AddItem("MODIFIED",sModified);
	int nTaskPrior = pSrcTaskFile->GetTaskPriority(hTask,FALSE);
	time_t tDone = pSrcTaskFile->GetTaskDoneDate(hTask);
	
	if (tDone)
	{		
		CXmlItem* pXIIcons = pXIDestItem->AddItem("icon");
		pXIIcons->AddItem("BUILTIN","button_ok");
	}

	if (nTaskPrior > 0)
	{		
		CString sIconName;
		sIconName.Format("full-%d",nTaskPrior);		
		CXmlItem* pXIIcons = pXIDestItem->AddItem("icon");
		pXIIcons->AddItem("BUILTIN",sIconName);
	}
	
	// comments
	CXmlItem* pXIAttribs = NULL; 
	CString sComments = pSrcTaskFile->GetTaskComments(hTask);
	
	if (!sComments.IsEmpty())
	{
		// for version 0.9 we export comments as rich NOTE
		CXmlItem* pXIComments = pXIDestItem->AddItem("richcontent", sComments);
		pXIComments->AddItem("TYPE", "NOTE");
		
		// for version 0.8 we export comments as private attribute
		pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","Comments");
		pXIAttribs->AddItem("VALUE",sComments);
	}

	//Start of Attrib List not Supported by FreeMind
	//fixme: Check how to use the resources and their index for the following 
	//functions
	AddTaskAllocTo(pSrcTaskFile, hTask, pXIDestItem);
	AddTaskCategories(pSrcTaskFile, hTask, pXIDestItem);
	AddTaskDependencies(pSrcTaskFile, hTask, pXIDestItem);

	//virtual bool SetTaskAllocatedBy(HTASKITEM hTask, const char* szAllocBy) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskAllocatedBy");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskAllocatedBy(hTask));

	//virtual bool SetTaskStatus(HTASKITEM hTask, const char* szStatus) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskStatus");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskStatus(hTask));

	//virtual bool SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskFileReferencePath");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskFileReferencePath(hTask));
	
	//virtual bool SetTaskColor(HTASKITEM hTask, unsigned long nColor) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskColor");
	pXIAttribs->AddItem("VALUE",(double)pSrcTaskFile->GetTaskColor(hTask));

	//virtual bool SetTaskPriority(HTASKITEM hTask, unsigned char nPriority) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskPriority");
	int nPriority = pSrcTaskFile->GetTaskPriority(hTask,FALSE);

	if (nPriority == -2) // none
		pXIAttribs->AddItem("VALUE", "");
	else
		pXIAttribs->AddItem("VALUE", nPriority);

	//virtual bool SetTaskRisk(HTASKITEM hTask, unsigned char nRisk) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskRisk");
	int nRisk = pSrcTaskFile->GetTaskRisk(hTask,FALSE);

	if (nRisk == -2) // none
		pXIAttribs->AddItem("VALUE", "");
	else
		pXIAttribs->AddItem("VALUE", nRisk);

	//virtual bool SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskPercentDone");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskPercentDone(hTask,FALSE));

	//virtual bool SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskTimeEstimate");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskTimeEstimate(hTask,cTemp,FALSE));

	//virtual bool SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskTimeSpent");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskTimeSpent(hTask,cTemp,FALSE));
	
	//virtual bool SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskDoneDate");
	CString sDate = FormatDate(pSrcTaskFile->GetTaskDoneDate(hTask));
	pXIAttribs->AddItem("VALUE", sDate);

	//virtual bool SetTaskDueDate(HTASKITEM hTask, time_t tDueDate) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskDueDate");
	sDate = FormatDate(pSrcTaskFile->GetTaskDueDate(hTask,FALSE));
	pXIAttribs->AddItem("VALUE", sDate);

	//virtual bool SetTaskStartDate(HTASKITEM hTask, time_t tStartDate) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskStartDate");
	sDate = FormatDate(pSrcTaskFile->GetTaskStartDate(hTask));
	pXIAttribs->AddItem("VALUE",sDate);
	
	//virtual bool SetTaskPosition(HTASKITEM hTask, unsigned long nPos) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskPosition");
	pXIAttribs->AddItem("VALUE",(int)pSrcTaskFile->GetTaskPosition(hTask));
	
	//virtual bool SetTaskFlag(HTASKITEM hTask, bool bFlag) = 0;
	
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskFlag");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->IsTaskFlagged(hTask));

	//virtual bool SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskCreatedBy");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskCreatedBy(hTask));

	//virtual bool SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskCreationDate");
	sDate = FormatDate(pSrcTaskFile->GetTaskCreationDate(hTask));
	pXIAttribs->AddItem("VALUE", sDate);


	//virtual bool SetTaskExternalID(HTASKITEM hTask, const char* szID) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskExternalID");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskExternalID(hTask));

	//virtual bool SetTaskCost(HTASKITEM hTask, double dCost) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskCost");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskCost(hTask,FALSE));

	//virtual bool SetTaskVersion(HTASKITEM hTask, const char* szVersion) = 0;
	pXIAttribs= pXIDestItem->AddItem("attribute");
	pXIAttribs->AddItem("NAME","TaskVersion");
	pXIAttribs->AddItem("VALUE",pSrcTaskFile->GetTaskVersion(hTask));

	//virtual bool SetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1, 
	//								DWORD& dwSpecific2, BOOL& bRecalcFromDue) const = 0;
	int nRegularity = 0, nReuse = 0;
	DWORD dwSpecific1 = 0, dwSpecific2 = 0;
	BOOL bRecalcFromDue = FALSE;
	
	if (pSrcTaskFile->GetTaskRecurrence(hTask, nRegularity, dwSpecific1, dwSpecific2, bRecalcFromDue, nReuse))
	{
		pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","nRegularity");
		pXIAttribs->AddItem("VALUE",nRegularity);
		
		pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","dwSpecific1");
		pXIAttribs->AddItem("VALUE",(int)dwSpecific1);

		pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","dwSpecific2");
		pXIAttribs->AddItem("VALUE",(int)dwSpecific2);

		pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","bRecalcFromDue");
		pXIAttribs->AddItem("VALUE",bRecalcFromDue);
	}
	
	//End of Attrib List not Supported by FreeMind

	static bool bRight = true;
	if (LEVEL == 0)
	{
		if (bRight)
			pXIDestItem->AddItem("POSITION","right");
		else
			pXIDestItem->AddItem("POSITION","left");
		bRight = ! bRight; 
	}

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), pXIDestItem , LEVEL + 1);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), pXIDestParent , LEVEL);
}

CString CFMindExporter::FormatDate(time_t tDate)
{
	if (tDate)
	{
		COleDateTime date(tDate);
		return CDateHelper::FormatDate(date, DHFD_ISO | DHFD_TIME);
	}

	// else
	return "";
}

void CFMindExporter::AddTaskAllocTo(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem)
{
	if (pSrcTaskFile->GetTaskAllocatedToCount(hTask))
	{
		CStringArray aItems;
		int nCount = pSrcTaskFile->GetTaskAllocatedToCount(hTask);

		while (nCount--)
			aItems.InsertAt(0, pSrcTaskFile->GetTaskAllocatedTo(hTask, nCount));

		CXmlItem* pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","AllocatedTo");
		pXIAttribs->AddItem("VALUE", Misc::FormatArray(aItems));
	}
}

void CFMindExporter::AddTaskCategories(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem)
{
	if (pSrcTaskFile->GetTaskCategoryCount(hTask))
	{
		CStringArray aItems;
		int nCount = pSrcTaskFile->GetTaskCategoryCount(hTask);

		while (nCount--)
			aItems.InsertAt(0, pSrcTaskFile->GetTaskCategory(hTask, nCount));

		CXmlItem* pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","Categories");
		pXIAttribs->AddItem("VALUE", Misc::FormatArray(aItems));
	}
}

void CFMindExporter::AddTaskDependencies(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem)
{
	if (pSrcTaskFile->GetTaskDependencyCount(hTask))
	{
		CStringArray aItems;
		int nCount = pSrcTaskFile->GetTaskDependencyCount(hTask);

		while (nCount--)
			aItems.InsertAt(0, pSrcTaskFile->GetTaskDependency(hTask, nCount));

		CXmlItem* pXIAttribs= pXIDestItem->AddItem("attribute");
		pXIAttribs->AddItem("NAME","Dependencies");
		pXIAttribs->AddItem("VALUE", Misc::FormatArray(aItems));
	}
}
