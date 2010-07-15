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

// TaskFile.cpp: implementation of the CTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskFile.h"
#include "TDLSchemaDef.h"
#include "MergeToDoList.h"
#include "TDCStruct.h"

#include "../../Common/TimeEdit.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../Common/Base64Coder.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/FileMisc.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// private class to load header
class CTFHeaderParse : public IXmlParse
{
public:
	CTFHeaderParse() : m_sTask(TDL_TASK) {}
	
	virtual BOOL Continue(LPCTSTR szItem, LPCTSTR /*szValue*/) const
	{
		return (m_sTask.CompareNoCase(szItem) != 0);
	}
	
protected:
	CString m_sTask;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define GET_TASK(t, h, r) { t = TaskFromHandle(h); if (!t) return r; }

CTaskFile::CTaskFile(LPCTSTR szPassword) : m_dwNextUniqueID(0),
#ifdef NO_TL_ENCRYPTDECRYPT
	CXmlFile(TDL_ROOT)
#else
	CXmlFileEx(TDL_ROOT, szPassword)
#endif
{
}

CTaskFile::~CTaskFile()
{
}

HRESULT CTaskFile::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;

	// compare riid against our supported version numbers
	if (IsEqualIID(riid, IID_TASKLIST2))
	{
		*ppvObject = reinterpret_cast<ITaskList2*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST3))
	{
		*ppvObject = reinterpret_cast<ITaskList3*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST4))
	{
		*ppvObject = reinterpret_cast<ITaskList4*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST5))
	{
		*ppvObject = reinterpret_cast<ITaskList5*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST6))
	{
		*ppvObject = reinterpret_cast<ITaskList6*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_TASKLIST7))
	{
		*ppvObject = reinterpret_cast<ITaskList7*>(this);
		AddRef();
	}
	
	return (*ppvObject ? S_OK : E_NOTIMPL);
}

#ifndef NO_TL_ENCRYPTDECRYPT

BOOL CTaskFile::Decrypt(LPCTSTR szPassword)
{
	BOOL bWasEncrypted = IsEncrypted();
	BOOL bResult = CXmlFileEx::Decrypt(szPassword);

	if (bResult && bWasEncrypted)
	{
		// fix corrupted tasklist where the root item has an ID
		CXmlItem* pXI = GetItem(TDL_TASKID);

		while (pXI)
		{
			DeleteItem(pXI);
			pXI = GetItem(TDL_TASKID);
		}

		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);

		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0

		BuildHandleMap();
	}

	return bResult;
}

#endif

BOOL CTaskFile::Load(LPCTSTR szFilePath, IXmlParse* pCallback, BOOL bDecrypt)
{
	
#ifdef NO_TL_ENCRYPTDECRYPT
	BOOL bRes = CXmlFile::Load(szFilePath, TDL_ROOT, pCallback);
#else
	BOOL bRes = CXmlFileEx::Load(szFilePath, TDL_ROOT, pCallback, bDecrypt);
#endif
	
	if (bRes)
	{
		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);
		
		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0
		
		BuildHandleMap();
	}
	
	return bRes;
}

BOOL CTaskFile::LoadHeader(LPCTSTR szFilePath)
{
	CTFHeaderParse tfhp;

	return Load(szFilePath, &tfhp, FALSE);
}

BOOL CTaskFile::LoadEx(IXmlParse* pCallback)
{
	BOOL bResult = XMLBASE::LoadEx(TDL_ROOT, pCallback);

#ifdef NO_TL_ENCRYPTDECRYPT
	if (bResult)
#else
	if (bResult && !IsEncrypted())
#endif
	{
		// fix corrupted tasklist where the root item has an ID
		CXmlItem* pXI = GetItem(TDL_TASKID);

		while (pXI)
		{
			DeleteItem(pXI);
			pXI = GetItem(TDL_TASKID);
		}

		m_dwNextUniqueID = (DWORD)GetItemValueI(TDL_NEXTUNIQUEID);

		if (m_dwNextUniqueID <= 0)
			m_dwNextUniqueID = 1; // always > 0

		BuildHandleMap();
	}

	return bResult;
}

BOOL CTaskFile::SaveEx()
{
	return XMLBASE::SaveEx();
}

BOOL CTaskFile::Copy(const CTaskFile& tasks)
{
	XMLBASE::Copy(tasks);
	BuildHandleMap();

	return TRUE;
}

BOOL CTaskFile::Copy(const ITaskList* pTasks)
{
	Reset();
	m_dwNextUniqueID = 1;

	// copy top level tasks
	if (CopyTask(NULL, pTasks, NULL))
   {
	   BuildHandleMap();
	   return TRUE;
   }

   return FALSE;
}

BOOL CTaskFile::CopyTask(HTASKITEM hSrcTask, const ITaskList* pSrcTasks, HTASKITEM hDestParent)
{
	HTASKITEM hDestTask = NULL; // our root

	if (hSrcTask) // NULL if src root
	{
		hDestTask = NewTask(pSrcTasks->GetTaskTitle(hSrcTask), hDestParent);
		ASSERT (hDestTask);

		if (!hDestTask)
			return FALSE;

		// the rest of the attributes
		SetTaskID(hDestTask, pSrcTasks->GetTaskID(hSrcTask));
		SetTaskComments(hDestTask, pSrcTasks->GetTaskComments(hSrcTask));
		SetTaskAllocatedTo(hDestTask, pSrcTasks->GetTaskAllocatedTo(hSrcTask));
		SetTaskAllocatedBy(hDestTask, pSrcTasks->GetTaskAllocatedBy(hSrcTask));
		SetTaskCategory(hDestTask, pSrcTasks->GetTaskCategory(hSrcTask));
		SetTaskStatus(hDestTask, pSrcTasks->GetTaskStatus(hSrcTask));
		SetTaskFileReferencePath(hDestTask, pSrcTasks->GetTaskFileReferencePath(hSrcTask));
		SetTaskColor(hDestTask, pSrcTasks->GetTaskColor(hSrcTask));
		SetTaskPriority(hDestTask, pSrcTasks->GetTaskPriority(hSrcTask, FALSE));
		SetTaskPercentDone(hDestTask, pSrcTasks->GetTaskPercentDone(hSrcTask, FALSE));
		SetTaskLastModified(hDestTask, pSrcTasks->GetTaskLastModified(hSrcTask));
		SetTaskDoneDate(hDestTask, pSrcTasks->GetTaskDoneDate(hSrcTask));
		SetTaskDueDate(hDestTask, pSrcTasks->GetTaskDueDate(hSrcTask));
		SetTaskStartDate(hDestTask, pSrcTasks->GetTaskStartDate(hSrcTask));
		SetTaskFlag(hDestTask, pSrcTasks->IsTaskFlagged(hSrcTask));

		char cUnits;
		double dTime;
		
		dTime = pSrcTasks->GetTaskTimeEstimate(hSrcTask, cUnits, FALSE);
		SetTaskTimeEstimate(hDestTask, dTime, cUnits);
		dTime = pSrcTasks->GetTaskTimeSpent(hSrcTask, cUnits, FALSE);
		SetTaskTimeSpent(hDestTask, dTime, cUnits);

		const ITaskList2* pTL2 = GetITLInterface<ITaskList2>(pSrcTasks, IID_TASKLIST2);

		if (pTL2)
		{
			SetTaskCreatedBy(hDestTask, pTL2->GetTaskCreatedBy(hSrcTask));
			SetTaskCreationDate(hDestTask, pTL2->GetTaskCreationDate(hSrcTask));
		}

		const ITaskList3* pTL3 = GetITLInterface<ITaskList3>(pSrcTasks, IID_TASKLIST3);

		if (pTL3)
		{
			SetTaskRisk(hDestTask, pTL3->GetTaskRisk(hSrcTask, FALSE));
			SetTaskExternalID(hDestTask, pTL3->GetTaskExternalID(hSrcTask));
		}

		const ITaskList4* pTL4 = GetITLInterface<ITaskList4>(pSrcTasks, IID_TASKLIST4);

		if (pTL4)
		{
			SetTaskDependency(hDestTask, pTL4->GetTaskDependency(hSrcTask));
		}

		const ITaskList6* pTL6 = GetITLInterface<ITaskList6>(pSrcTasks, IID_TASKLIST6);

		if (pTL6)
		{
			SetTaskVersion(hDestTask, pTL6->GetTaskVersion(hSrcTask));
		}
	}

	// children
	HTASKITEM hSrcChildTask = pSrcTasks->GetFirstTask(hSrcTask);

	while (hSrcChildTask)
	{
		if (!CopyTask(hSrcChildTask, pSrcTasks, hDestTask))
			return FALSE;

		hSrcChildTask = pSrcTasks->GetNextTask(hSrcChildTask);
	}

	return TRUE;
}

#ifndef NO_TL_MERGE
int CTaskFile::Merge(const CTaskFile& tasks, BOOL bByID, BOOL bMoveExist)
{
	CMergeToDoList mtdl(bByID ? TDLM_BYID : TDLM_BYTITLE, 
						bMoveExist ? TDLM_MOVE : TDLM_LEAVE);

	int nMerge = mtdl.Merge(tasks.Root(), Root());
	BuildHandleMap();

	// update next uniqueID
	SetNextUniqueID(mtdl.GetNextID());

	return nMerge;
}

int CTaskFile::Merge(LPCTSTR szTaskFilePath, BOOL bByID, BOOL bMoveExist)
{
	CMergeToDoList mtdl(bByID ? TDLM_BYID : TDLM_BYTITLE, 
						bMoveExist ? TDLM_MOVE : TDLM_LEAVE);
	
	int nMerge = mtdl.Merge(szTaskFilePath, Root());
	BuildHandleMap();

	// update next uniqueID
	SetNextUniqueID(mtdl.GetNextID());

	return nMerge;
}
#endif

/*
void CTaskFile::SortTasksByID()
{
	SortItemsI(TDL_TASK, TDL_TASKID);
}
*/

void CTaskFile::SortTasksByPos()
{
	SortItemsI(TDL_TASK, TDL_TASKPOS);
}

DWORD CTaskFile::GetNextUniqueID() const
{
	return m_dwNextUniqueID;
}

void CTaskFile::BuildHandleMap()
{
	m_mapHandles.RemoveAll();

	AddTaskToMap(GetItem(TDL_TASK), TRUE); // first top level item
}

void CTaskFile::AddTaskToMap(CXmlItem* pXITask, BOOL bRecurse)
{
	if (pXITask)
	{
		// sanity check
		ASSERT (pXITask->NameIs(TDL_TASK));
		
		m_mapHandles[(HTASKITEM)pXITask] = pXITask;

		if (bRecurse)
		{
			// next item
			AddTaskToMap(pXITask->GetSibling(), TRUE);

			// children
			// note: we only need do the first child
			AddTaskToMap(pXITask->GetItem(TDL_TASK), TRUE);
		}
	}
}

void CTaskFile::RemoveTaskFromMap(CXmlItem* pXITask)
{
	if (pXITask)
	{
		// sanity check
		ASSERT (pXITask->NameIs(TDL_TASK));
		
		m_mapHandles.RemoveKey((HTASKITEM)pXITask);

		// children
		CXmlItem* pXIChild = pXITask->GetItem(TDL_TASK);

		while (pXIChild)
		{
			RemoveTaskFromMap(pXIChild);
			pXIChild = pXIChild->GetSibling();
		}
	}
}

CXmlItem* CTaskFile::TaskFromHandle(HTASKITEM hTask) const
{
#ifdef _DEBUG
	CXmlItem* pXITask = NULL;

	if (hTask)
		m_mapHandles.Lookup(hTask, pXITask);

	return pXITask;
#else
	return static_cast<CXmlItem*>(hTask);
#endif
}

bool CTaskFile::IsArchive() const
{
	return (NULL != GetItem(TDL_ARCHIVE));
}

bool CTaskFile::IsCheckedOut() const
{
	return (Misc::GetComputerName() == GetCheckOutTo());
}	

bool CTaskFile::IsSourceControlled() const
{
	return (NULL != GetItem(TDL_CHECKEDOUTTO));
}

const char* CTaskFile::GetProjectName() const
{
	return GetItemValue(TDL_PROJECTNAME);
}

const char* CTaskFile::GetReportTitle() const
{
	return GetItemValue(TDL_REPORTTITLE);
}

const char* CTaskFile::GetHtmlCharSet() const
{
	return GetItemValue(TDL_CHARSET);
}

const char* CTaskFile::GetAttribute(const char* szAttrib) const
{
	return GetItemValue(szAttrib);
}

const char* CTaskFile::GetReportDate() const
{
	return GetItemValue(TDL_REPORTDATE);
}

const char* CTaskFile::GetCheckOutTo() const
{
	return GetItemValue(TDL_CHECKEDOUTTO);
}	

unsigned long CTaskFile::GetFileFormat() const
{
	return GetItemValueI(TDL_FILEFORMAT);
}

unsigned long CTaskFile::GetFileVersion() const
{
	return GetItemValueI(TDL_FILEVERSION);
}
	
time_t CTaskFile::GetLastModified() const
{
	COleDateTime date(GetItemValueF(TDL_LASTMODIFIED));

	tm time = { date.GetHour(), 
		        0, 
				date.GetDay(), 
				date.GetMinute(), 
				date.GetMonth(), 
				date.GetSecond(), 
				0, 
				0, 
				date.GetYear() - 1900 };

	return mktime(&time);
}

BOOL CTaskFile::SetEarliestDueDate(const COleDateTime& date)
{
	return (NULL != SetItemValue(TDL_TASKEARLIESTDUEDATE, date.m_dt));
}

BOOL CTaskFile::GetEarliestDueDate(COleDateTime& date) const
{
	const CXmlItem* pXItem = GetItem(TDL_TASKEARLIESTDUEDATE);

	if (!pXItem)
		return FALSE;

	date.m_dt = pXItem->GetValueF();
	return TRUE;
}

/*
BOOL CTaskFile::SetCommentsType(LPCTSTR szID)
{
	return (SetItemValue(TDL_COMMENTSTYPE, szID) != NULL);
}
*/

CString CTaskFile::GetCommentsType() const
{
	return GetItemValue(TDL_COMMENTSTYPE);
}

bool CTaskFile::SetProjectName(const char* szName)
{
	return (NULL != SetItemValue(TDL_PROJECTNAME, szName));
}

BOOL CTaskFile::SetArchive(BOOL bArchive)
{
	CXmlItem* pXItem = GetItem(TDL_ARCHIVE);

	if (pXItem)
	{
		if (bArchive)
			pXItem->SetValue((int)1);
		else
			DeleteItem(pXItem);

		return true;
	}
	else if (bArchive)
		return (NULL != AddItem(TDL_ARCHIVE, (int)1));

	// else
	return TRUE;
}

bool CTaskFile::SetFileVersion(unsigned long nVersion)
{
	return (NULL != SetItemValue(TDL_FILEVERSION, (int)nVersion));
}

BOOL CTaskFile::SetCheckedOutTo(const CString& sCheckedOutTo)
{
	return (NULL != SetItemValue(TDL_CHECKEDOUTTO, sCheckedOutTo));
}

BOOL CTaskFile::SetCharSet(LPCTSTR szCharSet)
{
	CXmlItem* pXItem = GetItem(TDL_CHARSET);

	if (pXItem)
	{
		if (szCharSet && *szCharSet)
			pXItem->SetValue(szCharSet);
		else
			DeleteItem(pXItem);
	
		return TRUE;
	}
	else if (szCharSet && *szCharSet)
		return (NULL != AddItem(TDL_CHARSET, szCharSet));

	// else
	return TRUE;
}

BOOL CTaskFile::SetFileName(LPCTSTR szFilename)
{
	CXmlItem* pXItem = GetItem(TDL_FILENAME);

	if (pXItem)
	{
		if (szFilename && *szFilename)
			pXItem->SetValue(szFilename);
		else
			DeleteItem(pXItem);
	
		return TRUE;
	}
	else if (szFilename && *szFilename)
		return (NULL != AddItem(TDL_FILENAME, szFilename));

	// else
	return TRUE;
}

BOOL CTaskFile::SetCategoryNames(const CStringArray& aCategories)
{
	return SetArray(TDL_TASKCATEGORY, aCategories);
}

BOOL CTaskFile::SetStatusNames(const CStringArray& aStatuses)
{
	return SetArray(TDL_TASKSTATUS, aStatuses);
}

BOOL CTaskFile::SetAllocToNames(const CStringArray& aAllocTo)
{
	return SetArray(TDL_TASKALLOCTO, aAllocTo);
}

BOOL CTaskFile::SetAllocByNames(const CStringArray& aAllocBy)
{
	return SetArray(TDL_TASKALLOCBY, aAllocBy);
}

BOOL CTaskFile::SetVersionNames(const CStringArray& aVersions)
{
	return SetArray(TDL_TASKVERSION, aVersions);
}

int CTaskFile::GetCategoryNames(CStringArray& aCategories) const
{
	return GetArray(TDL_TASKCATEGORY, aCategories);
}

int CTaskFile::GetStatusNames(CStringArray& aStatuses) const
{
	return GetArray(TDL_TASKSTATUS, aStatuses);
}

int CTaskFile::GetAllocToNames(CStringArray& aAllocTo) const
{
	return GetArray(TDL_TASKALLOCTO, aAllocTo);
}

int CTaskFile::GetAllocByNames(CStringArray& aAllocBy) const
{
	return GetArray(TDL_TASKALLOCBY, aAllocBy);
}

int CTaskFile::GetVersionNames(CStringArray& aVersions) const
{
	return GetArray(TDL_TASKVERSION, aVersions);
}

BOOL CTaskFile::SetFileFormat(unsigned long lFormat)
{
	return (NULL != SetItemValue(TDL_FILEFORMAT, (int)lFormat));
}

BOOL CTaskFile::SetNextUniqueID(DWORD dwNextID)
{
	ASSERT (dwNextID >= m_dwNextUniqueID);

	BOOL bResult = FALSE;
	CXmlItem* pXItem = GetItem(TDL_NEXTUNIQUEID);

	if (pXItem)
	{
		pXItem->SetValue((int)dwNextID);
		bResult = TRUE;
	}
	else
		bResult = (NULL != AddItem(TDL_NEXTUNIQUEID, (int)dwNextID));

	if (bResult)
		m_dwNextUniqueID = dwNextID;

	return bResult;
}

BOOL CTaskFile::SetLastModified(const CString& sLastMod)
{
	return (NULL != SetItemValue(TDL_LASTMODIFIED, sLastMod));
}

BOOL CTaskFile::CheckOut()
{
	CString sTemp;
	return CheckOut(sTemp);
}

BOOL CTaskFile::CheckOut(CString& sCheckedOutTo)
{
	// make sure its got a filepath attached
	if (GetFilePath().IsEmpty())
		return FALSE;
	
	sCheckedOutTo = GetCheckOutTo();
	
	if (!sCheckedOutTo.IsEmpty())
	{
		if (sCheckedOutTo == Misc::GetComputerName()) // its us
			return TRUE;
		
		else if (!sCheckedOutTo.IsEmpty()) // someone else
			return FALSE;
		
		// else check it out
		SetCheckedOutTo(Misc::GetComputerName());
	}
	else // check it out
		SetCheckedOutTo(Misc::GetComputerName());
	
	// and rewrite the file but keeping the same timestamp
	FILETIME ftMod;
	::GetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);
	
//	SortTasksByID();
	BOOL bCheckedOut = SaveEx();
	
	::SetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);
	
	if (bCheckedOut)
		sCheckedOutTo = Misc::GetComputerName();
	
	return bCheckedOut;
}

BOOL CTaskFile::SetReportAttributes(LPCTSTR szTitle, const COleDateTime& date)
{
	BOOL bRes = TRUE;

	CXmlItem* pXItem = GetItem(TDL_REPORTTITLE);

	if (pXItem)
		pXItem->SetValue(szTitle);
	else
		bRes &= (NULL != AddItem(TDL_REPORTTITLE, szTitle));

	if (date.m_dt > 0)
	{
		// date value
		pXItem = GetItem(TDL_REPORTDATEOLE);

		if (pXItem)
			pXItem->SetValue(date.m_dt);
		else
			bRes &= (NULL != AddItem(TDL_REPORTDATE, date.m_dt));

		// date string
		CString sDate = date.Format(VAR_DATEVALUEONLY);
		pXItem = GetItem(TDL_REPORTDATE);

		if (pXItem)
			pXItem->SetValue(sDate);
		else
			bRes &= (NULL != AddItem(TDL_REPORTDATE, sDate));
	}

	return bRes;
}

BOOL CTaskFile::DeleteTaskAttributes(HTASKITEM hTask)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	POSITION pos = pXITask->GetFirstItemPos();

	while (pos)
	{
		CXmlItem* pXIChild = pXITask->GetNextItem(pos);

		if (!pXIChild->NameMatches(TDL_TASK))
			pXITask->DeleteItem(pXIChild);
	}

	return TRUE;
}

HTASKITEM CTaskFile::FindTask(DWORD dwTaskID) const
{
	const CXmlItem* pXI = FindItem(TDL_TASKID, (int)dwTaskID);

	return pXI ? (HTASKITEM)(pXI->GetParent()) : NULL;
}

BOOL CTaskFile::DeleteTask(HTASKITEM hTask)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIParent = pXITask->GetParent();
	ASSERT (pXIParent);

	// remove task from map first
	RemoveTaskFromMap(pXITask);

	return pXIParent->DeleteItem(pXITask);
}

BOOL CTaskFile::SetTaskCustomComments(HTASKITEM hTask, const CString& sContent, const CString& sType)
{
	if (!TaskFromHandle(hTask))
		return FALSE;

	BOOL bRes = TRUE;

	if (!sContent.IsEmpty())
	{
		Base64Coder b64;
		b64.Encode((const PBYTE)(LPCTSTR)sContent, sContent.GetLength());

		bRes = SetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS, b64.EncodedMessage(), XIT_ATTRIB);
	}

	if (bRes)
		bRes = SetTaskCChar(hTask, TDL_TASKCOMMENTSTYPE, sType);

	return bRes;
}

BOOL CTaskFile::SetTaskHtmlComments(HTASKITEM hTask, const CString& sContent, BOOL bForTransform)
{
	if (!TaskFromHandle(hTask))
		return FALSE;

   XI_TYPE nType = bForTransform ? XIT_ATTRIB : XIT_CDATA;

	return SetTaskCChar(hTask, TDL_TASKHTMLCOMMENTS, sContent, nType);
}

BOOL CTaskFile::GetTaskCustomComments(HTASKITEM hTask, CString& sContent, CString& sType) const
{
	// comments type
	sType = GetTaskCChar(hTask, TDL_TASKCOMMENTSTYPE);

	// custom comments
	CString sTemp = GetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS);

	if (sTemp.IsEmpty())
		return FALSE;

	Base64Coder b64;
	b64.Decode(sTemp);

	unsigned long nLenContent;
	PBYTE pContent = b64.DecodedMessage(nLenContent);

	PBYTE szContent = (PBYTE)sContent.GetBuffer(nLenContent);
	CopyMemory(szContent, pContent, nLenContent);
	sContent.ReleaseBuffer(nLenContent);
	
	return nLenContent;
}

BOOL CTaskFile::SetTaskCategories(HTASKITEM hTask, const CStringArray& aCategories)
{
	return SetTaskArray(hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, aCategories);
}

BOOL CTaskFile::SetTaskDependencies(HTASKITEM hTask, const CStringArray& aDepends)
{
	return SetTaskArray(hTask, TDL_TASKNUMDEPENDENCY, TDL_TASKDEPENDENCY, aDepends);
}

BOOL CTaskFile::SetTaskAllocatedTo(HTASKITEM hTask, const CStringArray& aAllocTo)
{
	return SetTaskArray(hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, aAllocTo);
}

bool CTaskFile::AddTaskDependency(HTASKITEM hTask, const char* szDepends)
{
	return AddTaskArrayItem(hTask, TDL_TASKNUMDEPENDENCY, TDL_TASKDEPENDENCY, szDepends);
}

bool CTaskFile::AddTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo)
{
	return AddTaskArrayItem(hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, szAllocTo);
}

bool CTaskFile::AddTaskCategory(HTASKITEM hTask, const char* szCategory)
{
	return AddTaskArrayItem(hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, szCategory);
}

int CTaskFile::GetTaskCategories(HTASKITEM hTask, CStringArray& aCategories) const
{
	return GetTaskArray(hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, aCategories);
}

int CTaskFile::GetTaskDependencies(HTASKITEM hTask, CStringArray& aDepends) const
{
	return GetTaskArray(hTask, TDL_TASKNUMDEPENDENCY, TDL_TASKDEPENDENCY, aDepends);
}

int CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask, CStringArray& aAllocTo) const
{
	return GetTaskArray(hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, aAllocTo);
}

CXmlItem* CTaskFile::NewItem(LPCTSTR szName)
{
	return new CXmlItem(NULL, szName);
}

///////////////////////////////////////////////////////////////////////

HTASKITEM CTaskFile::NewTask(const char* szTitle, HTASKITEM hParent, DWORD dwID)
{
	CXmlItem* pXIParent = hParent ? TaskFromHandle(hParent) : Root();

	if (!pXIParent)
	{
		// handle the fact that hParent might already point to Root()
		if (hParent == (HTASKITEM)Root())
			pXIParent = Root();
		else
			return NULL;
	}

	CXmlItem* pXINew = NewItem(TDL_TASK);

	if (pXINew)
	{
		pXIParent->AddItem(pXINew);
		AddTaskToMap(pXINew, FALSE);

		// set ID and name
		SetTaskTitle((HTASKITEM)pXINew, szTitle);

		if (dwID <= 0)
			dwID = m_dwNextUniqueID++;
		else
			m_dwNextUniqueID = max(m_dwNextUniqueID, dwID + 1);

		SetTaskID((HTASKITEM)pXINew, dwID);
	}

	return (HTASKITEM)pXINew;
}

HTASKITEM CTaskFile::NewTask(const char* szTitle, HTASKITEM hParent)
{
	return NewTask(szTitle, hParent, 0); // 0 means 'allocate new ID'
}

HTASKITEM CTaskFile::GetFirstTask(HTASKITEM hParent) const
{
	const CXmlItem* pXIParent = hParent ? TaskFromHandle(hParent) : Root();

	if (!pXIParent)
	{
		// handle the fact that hParent might already point to Root()
		if (hParent == (HTASKITEM)Root())
			pXIParent = Root();
		else
			return NULL;
	}

	return (HTASKITEM)pXIParent->GetItem(TDL_TASK);
}

HTASKITEM CTaskFile::GetNextTask(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetSibling();
}

const char* CTaskFile::GetTaskTitle(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKTITLE);
}

int CTaskFile::GetTaskIconIndex(HTASKITEM hTask) const
{
	if (TaskHasAttribute(hTask, TDL_TASKICONINDEX))
		return (int)GetTaskULong(hTask, TDL_TASKICONINDEX);
	else
		return -1;
}

const char* CTaskFile::GetTaskComments(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCOMMENTS);
}

const char* CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCTO);
}

const char* CTaskFile::GetTaskAllocatedBy(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCBY);
}

const char* CTaskFile::GetTaskCategory(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCATEGORY);
}

unsigned char CTaskFile::GetTaskCategoryCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMCATEGORY);
}

const char* CTaskFile::GetTaskCategory(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, nIndex);
}

unsigned char CTaskFile::GetTaskDependencyCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMDEPENDENCY);
}

const char* CTaskFile::GetTaskDependency(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, TDL_TASKNUMDEPENDENCY, TDL_TASKDEPENDENCY, nIndex);
}

unsigned char CTaskFile::GetTaskAllocatedToCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMALLOCTO);
}

const char* CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, nIndex);
}

const char* CTaskFile::GetTaskDependency(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDEPENDENCY);
}

const char* CTaskFile::GetTaskStatus(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKSTATUS);
}

const char* CTaskFile::GetTaskFileReferencePath(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKFILEREFPATH);
}

const char* CTaskFile::GetTaskCreatedBy(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCREATEDBY);
}

unsigned long CTaskFile::GetTaskID(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKID);
}

bool CTaskFile::IsTaskFlagged(HTASKITEM hTask) const
{
	return (GetTaskUChar(hTask, TDL_TASKFLAG) > 0);
}

unsigned long CTaskFile::GetTaskColor(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKCOLOR);
}

const char* CTaskFile::GetTaskWebColor(HTASKITEM hTask) const
{
	DWORD color = 0;

	if (TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR))
		color = GetTaskULong(hTask, TDL_TASKTEXTCOLOR);
	else
		color = GetTaskULong(hTask, TDL_TASKCOLOR);
	
	return GetWebColor((COLORREF)color);
}

const char* CTaskFile::GetTaskPriorityWebColor(HTASKITEM hTask) const
{
	return GetWebColor(GetTaskPriorityColor(hTask));
}

unsigned long CTaskFile::GetTaskPriorityColor(HTASKITEM hTask) const
{
	DWORD color = RGB(255, 255, 255);

	if (TaskHasAttribute(hTask, TDL_TASKPRIORITYCOLOR))
		color = GetTaskULong(hTask, TDL_TASKPRIORITYCOLOR);

	return color;
}

int CTaskFile::GetTaskPriority(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, TDL_TASKHIGHESTPRIORITY))
		return GetTaskInt(hTask, TDL_TASKHIGHESTPRIORITY);

	return GetTaskInt(hTask, TDL_TASKPRIORITY);
}

unsigned char CTaskFile::GetTaskPercentDone(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCCOMPLETION))
		return GetTaskUChar(hTask, TDL_TASKCALCCOMPLETION);

	return GetTaskUChar(hTask, TDL_TASKPERCENTDONE);
}

double CTaskFile::GetTaskCost(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCCOST))
		return GetTaskDouble(hTask, TDL_TASKCALCCOST);

	// else
	return GetTaskDouble(hTask, TDL_TASKCOST);
}

BOOL CTaskFile::GetTaskRecurrence(HTASKITEM hTask, TDIRECURRENCE& tr) const
{
	return GetTaskRecurrence(hTask, (int&)tr.nRegularity, tr.dwSpecific1, 
							tr.dwSpecific2, tr.bRecalcFromDue, (int&)tr.nReuse);
}

bool CTaskFile::GetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1, 
									DWORD& dwSpecific2, BOOL& bRecalcFromDue, int& nReuse) const
{
	CXmlItem* pXI = TaskFromHandle(hTask);

	if (pXI)
	{
		CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

		if (pXIRecur)
		{
			nRegularity = pXIRecur->GetItemValueI(TDL_TASKRECURRENCEREG);
			dwSpecific1 = pXIRecur->GetItemValueI(TDL_TASKRECURRENCESPEC1);
			dwSpecific2 = pXIRecur->GetItemValueI(TDL_TASKRECURRENCESPEC2);
			bRecalcFromDue = pXIRecur->GetItemValueI(TDL_TASKRECURRENCEFROMDUE);
			nReuse = pXIRecur->GetItemValueI(TDL_TASKRECURRENCEREUSE);

			return true;
		}
	}

	return false;
}

const char* CTaskFile::GetTaskVersion(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKVERSION);
}

double CTaskFile::GetTaskTimeEstimate(HTASKITEM hTask, char& cUnits, BOOL bCalc) const
{
	cUnits = 0;
	
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCTIMEESTIMATE))
	{
		cUnits = 'H';
		return GetTaskDouble(hTask, TDL_TASKCALCTIMEESTIMATE);
	}
	else if (TaskHasAttribute(hTask, TDL_TASKTIMEESTUNITS))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMEESTUNITS);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	
	if (!cUnits)
		cUnits = 'H';
	
	return GetTaskDouble(hTask, TDL_TASKTIMEESTIMATE);
}

double CTaskFile::GetTaskTimeSpent(HTASKITEM hTask, char& cUnits, BOOL bCalc) const
{
	cUnits = 0;
	
	if (bCalc && TaskHasAttribute(hTask, TDL_TASKCALCTIMESPENT))
	{
		cUnits = 'H';
		return GetTaskDouble(hTask, TDL_TASKCALCTIMESPENT);
	}
	else if (TaskHasAttribute(hTask, TDL_TASKTIMESPENTUNITS))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	// backwards compatibility
	else if (TaskHasAttribute(hTask, TDL_TASKTIMESPENTUNITS_OLD))
	{
		const char* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS_OLD);
		
		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (char)atoi(szUnits);
			
			if (!cUnits)
				cUnits = szUnits[0];
		}
	}
	
	if (!cUnits)
		cUnits = 'H';

	if (TaskHasAttribute(hTask, TDL_TASKTIMESPENT))
		return GetTaskDouble(hTask, TDL_TASKTIMESPENT);

	// else for backwards compatibility
		return GetTaskDouble(hTask, TDL_TASKTIMESPENT_OLD);
}

time_t CTaskFile::GetTaskLastModified(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKLASTMOD, TRUE);
}

time_t CTaskFile::GetTaskCreationDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKCREATIONDATE, FALSE);
}

time_t CTaskFile::GetTaskDoneDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKDONEDATE, FALSE);
}

time_t CTaskFile::GetTaskDueDate(HTASKITEM hTask) const
{
	return GetTaskDueDate(hTask, FALSE);
}

time_t CTaskFile::GetTaskDueDate(HTASKITEM hTask, BOOL bEarliest) const
{
	if (bEarliest && TaskHasAttribute(hTask, TDL_TASKEARLIESTDUEDATE))
		return GetTaskDate(hTask, TDL_TASKEARLIESTDUEDATE, TRUE);

	return GetTaskDate(hTask, TDL_TASKDUEDATE, TRUE);
}

const char* CTaskFile::GetTaskDueDateString(HTASKITEM hTask, BOOL bEarliest) const
{
	if (bEarliest && TaskHasAttribute(hTask, TDL_TASKEARLIESTDUEDATESTRING))
		return GetTaskCChar(hTask, TDL_TASKEARLIESTDUEDATESTRING);

	return GetTaskCChar(hTask, TDL_TASKDUEDATESTRING);
}

time_t CTaskFile::GetTaskStartDate(HTASKITEM hTask) const
{
	return GetTaskDate(hTask, TDL_TASKSTARTDATE, TRUE);
}

COleDateTime CTaskFile::GetTaskLastModifiedOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKLASTMOD, TRUE);
}

COleDateTime CTaskFile::GetTaskDoneDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKDONEDATE, TRUE);
}

COleDateTime CTaskFile::GetTaskDueDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKDUEDATE, TRUE);
}

COleDateTime CTaskFile::GetTaskStartDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKSTARTDATE, TRUE);
}

COleDateTime CTaskFile::GetTaskCreationDateOle(HTASKITEM hTask) const
{
	return GetTaskDateOle(hTask, TDL_TASKCREATIONDATE, FALSE);
}

const char* CTaskFile::GetTaskDoneDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDONEDATESTRING);
}

const char* CTaskFile::GetTaskCreationDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCREATIONDATESTRING);
}

const char* CTaskFile::GetTaskDueDateString(HTASKITEM hTask) const
{
	return GetTaskDueDateString(hTask, FALSE);
}

const char* CTaskFile::GetTaskStartDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKSTARTDATESTRING);
}

unsigned long CTaskFile::GetTaskPosition(HTASKITEM hTask) const
{
	return GetTaskULong(hTask, TDL_TASKPOS);
}

bool CTaskFile::IsTaskDone(HTASKITEM hTask) const
{
	return (GetTaskDoneDateOle(hTask).m_dt > 0.0 ||
			GetTaskPercentDone(hTask, FALSE) == 100);
}

bool CTaskFile::IsTaskDue(HTASKITEM hTask) const
{
	double dDue = GetTaskDueDateOle(hTask);

	if (dDue > 0.0)
	{
		double dToday = floor(COleDateTime::GetCurrentTime());
		return dDue <= dToday;
	}

	return false;
}

bool CTaskFile::TaskHasAttribute(HTASKITEM hTask, const char* szAttrib) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);
	
	CXmlItem* pXIAttrib = pXITask->GetItem(szAttrib);
	
	if (!pXIAttrib || !pXIAttrib->GetValueLen()) // not found
	{
		// some fallbacks
		if (lstrcmp(szAttrib, TDL_TASKCOLOR) == 0)
			return TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR);
		
		else if (lstrcmp(szAttrib, TDL_TASKWEBCOLOR) == 0)
			return TaskHasAttribute(hTask, TDL_TASKTEXTWEBCOLOR);

		// else
		return FALSE;
	}
	
	// finally check for hidden attribute
	return (pXIAttrib->GetItemValueI("HIDE") == 0);
}

const char* CTaskFile::GetTaskAttribute(HTASKITEM hTask, const char* szAttrib) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, "");

	return (pXITask->GetItemValue(szAttrib));
}

HTASKITEM CTaskFile::GetTaskParent(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetParent();
}

unsigned long CTaskFile::GetTaskTextColor(HTASKITEM hTask) const
{
	if (TaskHasAttribute(hTask, TDL_TASKTEXTCOLOR))
		return GetTaskULong(hTask, TDL_TASKTEXTCOLOR);

	else if (TaskHasAttribute(hTask, TDL_TASKCOLOR))
		return GetTaskULong(hTask, TDL_TASKCOLOR);

	// else
	return 0;
}

int CTaskFile::GetTaskRisk(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, TDL_TASKHIGHESTRISK))
		return GetTaskInt(hTask, TDL_TASKHIGHESTRISK);

	return GetTaskInt(hTask, TDL_TASKRISK);
}

const char* CTaskFile::GetTaskExternalID(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKEXTERNALID);
}

///////////////////////////////////////////////

bool CTaskFile::SetTaskTitle(HTASKITEM hTask, const char* szTitle)
{
	return SetTaskCChar(hTask, TDL_TASKTITLE, szTitle);
}

bool CTaskFile::SetTaskComments(HTASKITEM hTask, const char* szComments)
{
	// if the first two chars are '\r\n' then we need to translate them 
	// because otherwise MSXML will remove them
	


//	CString sComments(szComments);
//	sComments.Replace("\r\n", "&#13;&#10;");
//	return SetTaskCChar(hTask, TDL_TASKCOMMENTS, sComments, XIT_ITEM);

	return SetTaskCChar(hTask, TDL_TASKCOMMENTS, szComments, XIT_ATTRIB);
}

bool CTaskFile::SetTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo)
{
	return SetTaskCChar(hTask, TDL_TASKALLOCTO, szAllocTo);
}

bool CTaskFile::SetTaskDependency(HTASKITEM hTask, const char* szDepends)
{
	return SetTaskCChar(hTask, TDL_TASKDEPENDENCY, szDepends);
}

bool CTaskFile::SetTaskAllocatedBy(HTASKITEM hTask, const char* szAllocBy)
{
	return SetTaskCChar(hTask, TDL_TASKALLOCBY, szAllocBy);
}

bool CTaskFile::SetTaskCategory(HTASKITEM hTask, const char* szCategory)
{
	return SetTaskCChar(hTask, TDL_TASKCATEGORY, szCategory);
}

bool CTaskFile::SetTaskStatus(HTASKITEM hTask, const char* szStatus)
{
	return SetTaskCChar(hTask, TDL_TASKSTATUS, szStatus);
}

bool CTaskFile::SetTaskFlag(HTASKITEM hTask, bool bFlag)
{
	return SetTaskUChar(hTask, TDL_TASKFLAG, (unsigned char)(bFlag ? 1 : 0));
}

bool CTaskFile::SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath)
{
	return SetTaskCChar(hTask, TDL_TASKFILEREFPATH, szFileRefpath);
}

bool CTaskFile::SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy)
{
	return SetTaskCChar(hTask, TDL_TASKCREATEDBY, szCreatedBy);
}

BOOL CTaskFile::SetTaskID(HTASKITEM hTask, unsigned long nID, BOOL bVisible)
{
	if (SetTaskULong(hTask, TDL_TASKID, nID))
	{
		// update m_dwNextUniqueID
		m_dwNextUniqueID = max(m_dwNextUniqueID, nID + 1);

		HideAttribute(hTask, TDL_TASKID, !bVisible);

		return true;
	}
	
	return false;
}

BOOL CTaskFile::HideAttribute(HTASKITEM hTask, const char* szAttrib, BOOL bHide)
      {
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIAttrib = pXITask->GetItem(szAttrib);

	if (pXIAttrib)
	{
		CXmlItem* pXIHide = pXIAttrib->GetItem("HIDE");

		// if not visible add 'HIDE' attribute
		if (bHide)
		{
			if (pXIHide)
				pXIHide->SetValue(1);
			else
				pXIAttrib->AddItem("HIDE", 1);
		}
		else if (pXIHide)
			pXIAttrib->DeleteItem(pXIHide);
		
		return TRUE;
	}
	
	return FALSE;
}

bool CTaskFile::SetTaskColor(HTASKITEM hTask, unsigned long nColor)
{
	return SetTaskULong(hTask, TDL_TASKCOLOR, nColor) &&
			SetTaskCChar(hTask, TDL_TASKWEBCOLOR, GetWebColor((COLORREF)nColor));
}

bool CTaskFile::SetTaskPriority(HTASKITEM hTask, unsigned char nPriority)
{
	return SetTaskUChar(hTask, TDL_TASKPRIORITY, (unsigned char)max(0, min(10, nPriority)));
}

bool CTaskFile::SetTaskPriority(HTASKITEM hTask, int nPriority)
{
	if (nPriority != FT_NOPRIORITY)
		nPriority = (char)max(0, min(10, nPriority));

	return SetTaskInt(hTask, TDL_TASKPRIORITY, nPriority);
}

bool CTaskFile::SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent)
{
	return SetTaskUChar(hTask, TDL_TASKPERCENTDONE, nPercent);
}

bool CTaskFile::SetTaskCost(HTASKITEM hTask, double dCost)
{
	return SetTaskDouble(hTask, TDL_TASKCOST, dCost);
}

bool CTaskFile::SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMEESTIMATE, dTimeEst,
						TDL_TASKTIMEESTUNITS, cUnits);
}

bool CTaskFile::SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMESPENT, dTimeSpent,
						TDL_TASKTIMESPENTUNITS, cUnits);
}

bool CTaskFile::SetTaskLastModified(HTASKITEM hTask, time_t tLastMod)
{
	return SetTaskDate(hTask, TDL_TASKLASTMOD, tLastMod, TRUE);
}

bool CTaskFile::SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate)
{
	return SetTaskDate(hTask, TDL_TASKDONEDATE, tDoneDate, TRUE);
}

bool CTaskFile::SetTaskDueDate(HTASKITEM hTask, time_t tDueDate)
{
	return SetTaskDate(hTask, TDL_TASKDUEDATE, tDueDate, TRUE);
}

bool CTaskFile::SetTaskStartDate(HTASKITEM hTask, time_t tStartDate)
{
	return SetTaskDate(hTask, TDL_TASKSTARTDATE, tStartDate, TRUE);
}

bool CTaskFile::SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate)
{
	return SetTaskDate(hTask, TDL_TASKCREATIONDATE, tCreationDate, TRUE);
}

BOOL CTaskFile::SetTaskLastModified(HTASKITEM hTask, const COleDateTime& tLastMod)
{
	if (SetTaskDate(hTask, TDL_TASKLASTMOD, tLastMod, TRUE))
	{
		DWORD dwFmt = DHFD_TIME | DHFD_NOSEC | DHFD_ISO;
		
		SetTaskCChar(hTask, TDL_TASKLASTMODSTRING, CDateHelper::FormatDate(tLastMod, dwFmt));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskCreationDate(HTASKITEM hTask, const COleDateTime& date)
{
	if (SetTaskDate(hTask, TDL_TASKCREATIONDATE, date, TRUE))
	{
		DWORD dwFmt = DHFD_TIME | DHFD_NOSEC | DHFD_ISO;

		SetTaskCChar(hTask, TDL_TASKCREATIONDATESTRING, CDateHelper::FormatDate(date, dwFmt));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskDoneDate(HTASKITEM hTask, const COleDateTime& date)
{
	if (SetTaskDate(hTask, TDL_TASKDONEDATE, date, TRUE))
	{
		DWORD dwFmt = DHFD_ISO;

		if (CDateHelper::GetTimeOnly(date) > 0.0)
			dwFmt |= DHFD_TIME | DHFD_NOSEC;

		SetTaskCChar(hTask, TDL_TASKDONEDATESTRING, CDateHelper::FormatDate(date, dwFmt));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskDueDate(HTASKITEM hTask, const COleDateTime& date)
{
	if (SetTaskDate(hTask, TDL_TASKDUEDATE, date, TRUE))
	{
		DWORD dwFmt = DHFD_ISO;

		if (CDateHelper::GetTimeOnly(date) > 0.0)
			dwFmt |= DHFD_TIME | DHFD_NOSEC;

		SetTaskCChar(hTask, TDL_TASKDUEDATESTRING, CDateHelper::FormatDate(date, dwFmt));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskStartDate(HTASKITEM hTask, const COleDateTime& date)
{
	if (SetTaskDate(hTask, TDL_TASKSTARTDATE, date, TRUE))
	{
		DWORD dwFmt = DHFD_ISO;
		
		if (CDateHelper::GetTimeOnly(date) > 0.0)
			dwFmt |= DHFD_TIME | DHFD_NOSEC;

		SetTaskCChar(hTask, TDL_TASKSTARTDATESTRING, CDateHelper::FormatDate(date, dwFmt));
		return true;
	}

	return FALSE;
}

BOOL CTaskFile::SetTaskRecurrence(HTASKITEM hTask, const TDIRECURRENCE& tr, LPCTSTR szRegularity)
{
	if (SetTaskRecurrence(hTask, (int)tr.nRegularity, tr.dwSpecific1, tr.dwSpecific2, tr.bRecalcFromDue, tr.nReuse))
	{
		SetTaskCChar(hTask, TDL_TASKRECURRENCE, szRegularity, XIT_ELEMENT); // set human readable
		return true;
	}

	return false;
}

bool CTaskFile::SetTaskRecurrence(HTASKITEM hTask, int nRegularity, DWORD dwSpecific1, 
									DWORD dwSpecific2, BOOL bRecalcFromDue, int nReuse)
{
	CXmlItem* pXI = TaskFromHandle(hTask);

	if (pXI)
	{
		CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

		if (!pXIRecur)
			pXIRecur = pXI->AddItem(TDL_TASKRECURRENCE);
		
		pXIRecur->SetItemValue(TDL_TASKRECURRENCEREG, nRegularity);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC1, (int)dwSpecific1);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC2, (int)dwSpecific2);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCEREUSE, nReuse);

		if (bRecalcFromDue)
			pXIRecur->SetItemValue(TDL_TASKRECURRENCEFROMDUE, TRUE);
		else
			pXIRecur->DeleteItem(TDL_TASKRECURRENCEFROMDUE);

		return true;
	}

	return false;
}

bool CTaskFile::SetTaskVersion(HTASKITEM hTask, const char* szVersion)
{
	return SetTaskCChar(hTask, TDL_TASKVERSION, szVersion);
}

BOOL CTaskFile::SetTaskTextColor(HTASKITEM hTask, COLORREF color)
{
	return SetTaskULong(hTask, TDL_TASKTEXTCOLOR, color) &&
			SetTaskCChar(hTask, TDL_TASKTEXTWEBCOLOR, GetWebColor(color));
}

BOOL CTaskFile::SetTaskIconIndex(HTASKITEM hTask, int nIndex)
{
	return SetTaskULong(hTask, TDL_TASKICONINDEX, nIndex);
}

BOOL CTaskFile::SetTaskPriorityColor(HTASKITEM hTask, COLORREF color)
{
	return SetTaskULong(hTask, TDL_TASKPRIORITYCOLOR, color) &&
			SetTaskCChar(hTask, TDL_TASKPRIORITYWEBCOLOR, GetWebColor(color));
}

BOOL CTaskFile::SetTaskCalcTimeEstimate(HTASKITEM hTask, double dTime)
{
	return SetTaskDouble(hTask, TDL_TASKCALCTIMEESTIMATE, dTime);
}

BOOL CTaskFile::SetTaskCalcTimeSpent(HTASKITEM hTask, double dTime)
{
	return SetTaskDouble(hTask, TDL_TASKCALCTIMESPENT, dTime);
}

BOOL CTaskFile::SetTaskEarliestDueDate(HTASKITEM hTask, const COleDateTime& date)
{
	return SetTaskDate(hTask, TDL_TASKEARLIESTDUEDATE, date, FALSE);
}

BOOL CTaskFile::SetTaskCalcCompletion(HTASKITEM hTask, int nPercent)
{
	return SetTaskUChar(hTask, TDL_TASKCALCCOMPLETION, (unsigned char)nPercent);
}

BOOL CTaskFile::SetTaskHighestPriority(HTASKITEM hTask, int nPriority)
{
	if (nPriority != FT_NOPRIORITY)
		nPriority = (char)max(0, min(10, nPriority));

	return SetTaskInt(hTask, TDL_TASKHIGHESTPRIORITY, nPriority);
}

BOOL CTaskFile::SetTaskHighestRisk(HTASKITEM hTask, int nRisk)
{
	if (nRisk != FT_NORISK)
		nRisk = (char)max(0, min(10, nRisk));

	return SetTaskInt(hTask, TDL_TASKHIGHESTRISK, nRisk);
}

BOOL CTaskFile::SetTaskCalcCost(HTASKITEM hTask, double dCost)
{
	return SetTaskDouble(hTask, TDL_TASKCALCCOST, dCost);
}

bool CTaskFile::SetTaskPosition(HTASKITEM hTask, unsigned long nPos)
{
	return SetTaskULong(hTask, TDL_TASKPOS, nPos);
}

bool CTaskFile::SetTaskRisk(HTASKITEM hTask, unsigned char nRisk)
{
	return SetTaskUChar(hTask, TDL_TASKRISK, (unsigned char)max(0, min(10, nRisk)));
}

bool CTaskFile::SetTaskRisk(HTASKITEM hTask, int nRisk)
{
	if (nRisk != FT_NORISK)
		nRisk = (char)max(-1, min(10, nRisk));

	return SetTaskInt(hTask, TDL_TASKRISK, nRisk);
}

bool CTaskFile::SetTaskExternalID(HTASKITEM hTask, const char* szID)
{
	return SetTaskCChar(hTask, TDL_TASKEXTERNALID, szID);
}

////////////////////////////////////////////////
// utility functions

time_t CTaskFile::GetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const
{
	COleDateTime date = GetTaskDateOle(hTask, szDateItem, bIncTime);

	if (date.m_dt)
	{
		tm time = { date.GetSecond(), 
					date.GetMinute(),
					date.GetHour(), 
					date.GetDay(),
					date.GetMonth() - 1, 
					date.GetYear() - 1900, 
					date.GetDayOfWeek() - 1, 
					date.GetDayOfYear(), 
					-1};

		time_t timet = mktime(&time);

		return (timet == -1) ? 0 : timet;
	}

	return 0;
}

COleDateTime CTaskFile::GetTaskDateOle(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const
{
	double dDate = GetTaskDouble(hTask, szDateItem);

	if (!bIncTime)
		dDate = floor(dDate);

	return dDate;
}

unsigned char CTaskFile::GetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem) const
{
	unsigned long lVal = GetTaskULong(hTask, szUCharItem);

	return (unsigned char)min(max(0, lVal), 255);
}

unsigned long CTaskFile::GetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, 0);

	return (unsigned long)pXITask->GetItemValueI(szULongItem);
}

int CTaskFile::GetTaskInt(HTASKITEM hTask, LPCTSTR szIntItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, 0);

	return pXITask->GetItemValueI(szIntItem);
}

const char* CTaskFile::GetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return pXITask->GetItemValue(szCCharItem);
}

double CTaskFile::GetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, 0.0);

	return pXITask->GetItemValueF(szDoubleItem);
}

double CTaskFile::GetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem) const
{
	return GetTaskDouble(hTask, szTimeItem);
}

char CTaskFile::GetTaskTimeUnits(HTASKITEM hTask, LPCTSTR szUnitsItem) const
{
	return (char)GetTaskUChar(hTask, szUnitsItem);
}

////////////////////////////////////////////////////////////////////

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, const COleDateTime& tVal, 
							BOOL bIncTime)
{
	double dDate = bIncTime ? tVal.m_dt : floor(tVal.m_dt);
	
	return SetTaskDouble(hTask, szDateItem, dDate);
}

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, time_t tVal, BOOL bIncTime)
{
	if (tVal == 0) 
        return SetTaskDate(hTask, szDateItem, COleDateTime(0.0), bIncTime);

	//fabio_2005
#if _MSC_VER >= 1400
    errno_t err;
	tm time;
	tm* pTime = &time;
        // Get time as 64-bit integer.
        // Convert to local time
	err= localtime_s(pTime ,&tVal  );
#else
	tm* pTime = localtime(&tVal);
#endif

	if (!pTime)
		return false;

	// else
	COleDateTime date(pTime->tm_year + 1900,
					  pTime->tm_mon + 1,
					  pTime->tm_mday,
					  pTime->tm_hour,
					  pTime->tm_min,
					  pTime->tm_sec);
	
	return SetTaskDate(hTask, szDateItem, date, bIncTime);
}

bool CTaskFile::SetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem, unsigned char cVal)
{
	return SetTaskULong(hTask, szUCharItem, cVal);
}

bool CTaskFile::SetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem, unsigned long lVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szULongItem, (int)lVal, XIT_ATTRIB) != NULL);
}

bool CTaskFile::SetTaskInt(HTASKITEM hTask, LPCTSTR szIntItem, int iVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szIntItem, iVal, XIT_ATTRIB) != NULL);
}

bool CTaskFile::SetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem, const char* szVal, XI_TYPE nType)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szCCharItem, szVal, nType) != NULL);
}

bool CTaskFile::SetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem, double dVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

   return (pXITask->SetItemValue(szDoubleItem, dVal, XIT_ATTRIB) != NULL);
}

bool CTaskFile::SetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem, double dTime,
							LPCTSTR szUnitsItem, char cUnits)
{
  char* szUnits;

	switch (cUnits)
	{
	case 'i': 
	case 'I': szUnits = "I"; // minutes
    break;

	case 'h': 
	case 'H': szUnits = "H";
    break;

	case 'd': 
	case 'D': szUnits = "D";
    break;

	case 'w':
	case 'W': szUnits = "W";
    break;

	case 'm': 
	case 'M': szUnits = "M";
    break;

	case 'y': 
	case 'Y': szUnits = "Y";
    break;

	default: // all else
		return false;
	}

	return SetTaskDouble(hTask, szTimeItem, dTime) &&
		  	SetTaskCChar(hTask, szUnitsItem, szUnits);
}

CString CTaskFile::GetWebColor(COLORREF color)
{
	unsigned char cRed = GetRValue(color);
	unsigned char cGreen = GetGValue(color);
	unsigned char cBlue = GetBValue(color);
	
	static CString sColor;
	sColor.Format("#%02X%02X%02X", cRed, cGreen, cBlue);
	
	return sColor;
}

const char* CTaskFile::GetTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, 
										const char* szItemTag, int nIndex) const
{
	if (nIndex == 0)
		return GetTaskCChar(hTask, szItemTag); // first item
	
	// else
	int nCount = GetTaskUChar(hTask, szNumItemTag);
	
	if (!nCount || nIndex < 1 || nIndex > nCount - 1)
		return NULL;
	
	CString sItem;
	sItem.Format("%s%d", szItemTag, nIndex);
	
	return GetTaskCChar(hTask, sItem);
}

bool CTaskFile::DeleteTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
								const char* szItemTag)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);
	
	// delete any existing items
	pXITask->DeleteItem(szItemTag);

	int nItem, nNumExists = pXITask->GetItemValueI(szNumItemTag);

	for (nItem = 1; nItem < nNumExists; nItem++)
	{
		CString sItem;
		sItem.Format("%s%d", szItemTag, nItem);
		pXITask->DeleteItem(sItem);
	}

	pXITask->DeleteItem(szNumItemTag);

	return true;
}

bool CTaskFile::AddTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, 
								 const char* szItemTag, const char* szItem)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);
	
	int nCount = pXITask->GetItemValueI(szNumItemTag);
	
	// see if it already exists
	if (nCount || pXITask->GetItem(szItemTag))
	{
		CString sValue = pXITask->GetItemValue(szItemTag);
		
		if (sValue.CompareNoCase(szItem) == 0)
			return false; // already exists
		
		// the rest have numbers after their names
		for (int nItem = 1; nItem < nCount; nItem++)
		{
			CString sItem;
			sItem.Format("%s%d", szItemTag, nItem);
			
			CString sValue = pXITask->GetItemValue(sItem);
			
			if (sValue.CompareNoCase(szItem) == 0)
				return false; // already exists
		}
	}
	
	// does the task have an existing item?
	if (!pXITask->GetItem(szItemTag))
	{
		pXITask->AddItem(szNumItemTag, 1); // num
		pXITask->AddItem(szItemTag, szItem); // first item
	}
	else // append
	{
		// increment item count
		pXITask->SetItemValue(szNumItemTag, nCount + 1);
		
		CString sItem;
		sItem.Format("%s%d", szItemTag, nCount);
		
		pXITask->AddItem(sItem, szItem);
	}
	
	return true;
}

int CTaskFile::GetTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
				  	 const char* szItemTag, CStringArray& aItems) const
{
	aItems.RemoveAll();

	// first item
	CString sItem = GetTaskCChar(hTask, szItemTag);

	if (!sItem.IsEmpty())
	{
		aItems.Add(sItem);

		// rest
		int nCount = GetTaskUChar(hTask, szNumItemTag);

		for (int nItem = 1; nItem < nCount; nItem++)
			aItems.Add(GetTaskArrayItem(hTask, szNumItemTag, szItemTag, nItem));
	}

	return aItems.GetSize();

}

BOOL CTaskFile::SetTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
				  	 const char* szItemTag, const CStringArray& aItems)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	// delete any existing items
	DeleteTaskArray(hTask, szNumItemTag, szItemTag);

	// then add these
	int nCount = aItems.GetSize();
	
	for (int nItem = 0; nItem < nCount; nItem++)
		AddTaskArrayItem(hTask, szNumItemTag, szItemTag, aItems[nItem]);

	return TRUE;
}

int CTaskFile::GetArray(const char* szItemTag, CStringArray& aItems) const
{
	aItems.RemoveAll();

	const CXmlItem* pXI = GetItem(szItemTag);

	if (pXI)
	{
		int nCount = pXI->GetItemCount();

		for (int nItem = 0; nItem < nCount; nItem++)
		{
			CString sItem;
			sItem.Format("%s%d", szItemTag, nItem);

			aItems.Add(pXI->GetItemValue(sItem));
		}
	}

	return aItems.GetSize();
}

BOOL CTaskFile::SetArray(const char* szItemTag, const CStringArray& aItems)
{
	// delete existing items
	DeleteItem(szItemTag);

	// and start again
	if (aItems.GetSize())
	{
		CXmlItem* pXI = AddItem(szItemTag);

		if (pXI)
		{
			int nCount = aItems.GetSize();
			
			for (int nItem = 0; nItem < nCount; nItem++)
			{
				CString sItem;
				sItem.Format("%s%d", szItemTag, nItem);

				pXI->AddItem(sItem, aItems[nItem]);
			}
		}
		else
			return FALSE;
	}

	return TRUE;
}
