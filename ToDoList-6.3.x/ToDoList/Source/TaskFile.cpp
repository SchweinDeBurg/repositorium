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
// - merged with ToDoList version 6.1.2-6.2.2 sources
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
#include "../../../Naughter/Source/Base64Coder.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/FileMisc.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// private class to load header
class CTFHeaderParse : public IXmlParse
{
public:
	CTFHeaderParse():
	m_sTask(TDL_TASK)
	{
	}

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

CTaskFile::CTaskFile(LPCTSTR szPassword):
m_dwNextUniqueID(1),
m_bISODates(TRUE),
#ifdef NO_TL_ENCRYPTDECRYPT
CXmlFile(TDL_ROOT)
#else
CXmlFileEx(TDL_ROOT, szPassword)
#endif
{
}

CTaskFile::CTaskFile(const CTaskFile& tasks, LPCTSTR szPassword):
m_dwNextUniqueID(1),
m_bISODates(TRUE),
#ifdef NO_TL_ENCRYPTDECRYPT
CXmlFile(TDL_ROOT)
#else
CXmlFileEx(TDL_ROOT, szPassword)
#endif
{
	Copy(tasks);
}

CTaskFile::CTaskFile(const ITaskList* pTasks, LPCTSTR szPassword):
m_dwNextUniqueID(1),
m_bISODates(TRUE),
#ifdef NO_TL_ENCRYPTDECRYPT
CXmlFile(TDL_ROOT)
#else
CXmlFileEx(TDL_ROOT, szPassword)
#endif
{
	Copy(pTasks);
}

CTaskFile::~CTaskFile()
{
}

HRESULT CTaskFile::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject)
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
	else if (IsEqualIID(riid, IID_TASKLIST8))
	{
		*ppvObject = reinterpret_cast<ITaskList8*>(this);
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
		{
			m_dwNextUniqueID = 1;   // always > 0
		}

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
		{
			m_dwNextUniqueID = 1;   // always > 0
		}

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
		{
			m_dwNextUniqueID = 1;   // always > 0
		}

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
	m_dwNextUniqueID = tasks.GetNextUniqueID();

	BuildHandleMap();

	return TRUE;
}

BOOL CTaskFile::Copy(const ITaskList* pTasks)
{
	Reset();
	m_dwNextUniqueID = 1;

	// copy top level tasks
	// m_dwNextUniqueID is kept up to date in CopyTask
	if (CopyTask(NULL, pTasks, NULL))
	{
		BuildHandleMap();

		return TRUE;
	}

	return FALSE;
}

void CTaskFile::Reset()
{
	XMLBASE::Reset();

	m_mapHandles.RemoveAll();
}

BOOL CTaskFile::CopyTask(HTASKITEM hSrcTask, const ITaskList* pSrcTasks, HTASKITEM hDestParent)
{
	HTASKITEM hDestTask = NULL; // our root

	if (hSrcTask) // NULL if src root
	{
		hDestTask = NewTask(ATL::CT2A(pSrcTasks->GetTaskTitle(hSrcTask)), hDestParent);
		ASSERT(hDestTask);

		if (!hDestTask)
		{
			return FALSE;
		}

		// Task ID. Keep track of highest taskID
		DWORD dwTask = pSrcTasks->GetTaskID(hSrcTask);
		SetTaskID(hDestTask, dwTask);
		m_dwNextUniqueID = dwTask + 1;

		// the rest of the attributes
		SetTaskComments(hDestTask, ATL::CT2A(pSrcTasks->GetTaskComments(hSrcTask)));
		SetTaskAllocatedTo(hDestTask, ATL::CT2A(pSrcTasks->GetTaskAllocatedTo(hSrcTask)));
		SetTaskAllocatedBy(hDestTask, ATL::CT2A(pSrcTasks->GetTaskAllocatedBy(hSrcTask)));
		SetTaskCategory(hDestTask, ATL::CT2A(pSrcTasks->GetTaskCategory(hSrcTask)));
		SetTaskStatus(hDestTask, ATL::CT2A(pSrcTasks->GetTaskStatus(hSrcTask)));
		SetTaskFileReferencePath(hDestTask, ATL::CT2A(pSrcTasks->GetTaskFileReferencePath(hSrcTask)));
		SetTaskColor(hDestTask, pSrcTasks->GetTaskColor(hSrcTask));
		SetTaskPriority(hDestTask, pSrcTasks->GetTaskPriority(hSrcTask, FALSE));
		SetTaskPercentDone(hDestTask, pSrcTasks->GetTaskPercentDone(hSrcTask, FALSE));
		SetTaskLastModified(hDestTask, pSrcTasks->GetTaskLastModified(hSrcTask));
		SetTaskDoneDate(hDestTask, pSrcTasks->GetTaskDoneDate(hSrcTask));
		SetTaskDueDate(hDestTask, pSrcTasks->GetTaskDueDate(hSrcTask));
		SetTaskStartDate(hDestTask, pSrcTasks->GetTaskStartDate(hSrcTask));
		SetTaskFlag(hDestTask, pSrcTasks->IsTaskFlagged(hSrcTask));

		TCHAR cUnits;
		double dTime;

		dTime = pSrcTasks->GetTaskTimeEstimate(hSrcTask, cUnits, FALSE);
		SetTaskTimeEstimate(hDestTask, dTime, cUnits);
		dTime = pSrcTasks->GetTaskTimeSpent(hSrcTask, cUnits, FALSE);
		SetTaskTimeSpent(hDestTask, dTime, cUnits);

		const ITaskList2* pTL2 = GetITLInterface<ITaskList2>(pSrcTasks, IID_TASKLIST2);

		if (pTL2)
		{
			SetTaskCreatedBy(hDestTask, ATL::CT2A(pTL2->GetTaskCreatedBy(hSrcTask)));
			SetTaskCreationDate(hDestTask, pTL2->GetTaskCreationDate(hSrcTask));
		}

		const ITaskList3* pTL3 = GetITLInterface<ITaskList3>(pSrcTasks, IID_TASKLIST3);

		if (pTL3)
		{
			SetTaskRisk(hDestTask, pTL3->GetTaskRisk(hSrcTask, FALSE));
			SetTaskExternalID(hDestTask, ATL::CT2A(pTL3->GetTaskExternalID(hSrcTask)));
		}

		const ITaskList4* pTL4 = GetITLInterface<ITaskList4>(pSrcTasks, IID_TASKLIST4);

		if (pTL4)
		{
			SetTaskDependency(hDestTask, ATL::CT2A(pTL4->GetTaskDependency(hSrcTask)));
		}

		const ITaskList6* pTL6 = GetITLInterface<ITaskList6>(pSrcTasks, IID_TASKLIST6);

		if (pTL6)
		{
			SetTaskVersion(hDestTask, ATL::CT2A(pTL6->GetTaskVersion(hSrcTask)));

			int nRegularity, nReuse;
			DWORD dwSpecific1, dwSpecific2;
			BOOL bRecalcFromDue;

			if (GetTaskRecurrence(hSrcTask, nRegularity, dwSpecific1, dwSpecific2, bRecalcFromDue, nReuse))
			{
				SetTaskRecurrence(hDestTask, nRegularity, dwSpecific1, dwSpecific2, bRecalcFromDue, nReuse);
			}
		}

		const ITaskList7* pTL7 = GetITLInterface<ITaskList7>(pSrcTasks, IID_TASKLIST7);

		if (pTL7)
		{
			int nNumDepends = GetTaskDependencyCount(hSrcTask);

			for (int nDepend = 0; nDepend < nNumDepends; nDepend++)
			{
				AddTaskDependency(hDestTask, ATL::CT2A(GetTaskDependency(hSrcTask, nDepend)));
			}

			int nNumAllocTo = GetTaskAllocatedToCount(hSrcTask);

			for (int nAlloc = 0; nAlloc < nNumAllocTo; nAlloc++)
			{
				AddTaskAllocatedTo(hDestTask, ATL::CT2A(GetTaskAllocatedTo(hSrcTask, nAlloc)));
			}
		}

		const ITaskList8* pTL8 = GetITLInterface<ITaskList8>(pSrcTasks, IID_TASKLIST8);

		if (pTL8)
		{
			// nothing to do
		}
	}

	// children
	HTASKITEM hSrcChildTask = pSrcTasks->GetFirstTask(hSrcTask);

	while (hSrcChildTask)
	{
		if (!CopyTask(hSrcChildTask, pSrcTasks, hDestTask))
		{
			return FALSE;
		}

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
		ASSERT(pXITask->NameIs(TDL_TASK));

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
		ASSERT(pXITask->NameIs(TDL_TASK));

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
	{
		m_mapHandles.Lookup(hTask, pXITask);
	}

	return pXITask;
#else
	return static_cast<CXmlItem*>(hTask);
#endif
}

bool CTaskFile::IsArchive() const
{
	return (NULL != GetItem(TDL_ARCHIVE));
}

BOOL CTaskFile::IsCheckedOutTo(const CString& sCheckedOutTo) const
{
	return (sCheckedOutTo.CompareNoCase(GetCheckOutTo()) == 0);
}

bool CTaskFile::IsSourceControlled() const
{
	return (NULL != GetItem(TDL_CHECKEDOUTTO));
}

const TCHAR* CTaskFile::GetProjectName() const
{
	return GetItemValue(TDL_PROJECTNAME);
}

const TCHAR* CTaskFile::GetReportTitle() const
{
	return GetItemValue(TDL_REPORTTITLE);
}

const TCHAR* CTaskFile::GetHtmlCharSet() const
{
	return GetItemValue(TDL_CHARSET);
}

const TCHAR* CTaskFile::GetAttribute(const char* szAttrib) const
{
	return GetItemValue(ATL::CA2T(szAttrib));
}

const TCHAR* CTaskFile::GetReportDate() const
{
	return GetItemValue(TDL_REPORTDATE);
}

const TCHAR* CTaskFile::GetCheckOutTo() const
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

	tm time =
	{
		date.GetHour(),
		0,
		date.GetDay(),
		date.GetMinute(),
		date.GetMonth(),
		date.GetSecond(),
		0,
		0,
		date.GetYear() - 1900
	};

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
	{
		return FALSE;
	}

	date.m_dt = pXItem->GetValueF();
	return TRUE;
}

CString CTaskFile::GetCommentsType() const
{
	return GetItemValue(TDL_COMMENTSTYPE);
}

bool CTaskFile::SetProjectName(const char* szName)
{
	return (NULL != SetItemValue(TDL_PROJECTNAME, ATL::CA2T(szName)));
}

BOOL CTaskFile::SetArchive(BOOL bArchive)
{
	CXmlItem* pXItem = GetItem(TDL_ARCHIVE);

	if (pXItem)
	{
		if (bArchive)
		{
			pXItem->SetValue((int)1);
		}
		else
		{
			DeleteItem(pXItem);
		}

		return true;
	}
	else if (bArchive)
	{
		return (NULL != AddItem(TDL_ARCHIVE, (int)1));
	}

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
		{
			pXItem->SetValue(szCharSet);
		}
		else
		{
			DeleteItem(pXItem);
		}

		return TRUE;
	}
	else if (szCharSet && *szCharSet)
	{
		return (NULL != AddItem(TDL_CHARSET, szCharSet));
	}

	// else
	return TRUE;
}

BOOL CTaskFile::SetFileName(LPCTSTR szFilename)
{
	CXmlItem* pXItem = GetItem(TDL_FILENAME);

	if (pXItem)
	{
		if (szFilename && *szFilename)
		{
			pXItem->SetValue(szFilename);
		}
		else
		{
			DeleteItem(pXItem);
		}

		return TRUE;
	}
	else if (szFilename && *szFilename)
	{
		return (NULL != AddItem(TDL_FILENAME, szFilename));
	}

	// else
	return TRUE;
}

BOOL CTaskFile::SetCategoryNames(const CStringArray& aCategories)
{
	return SetArray(ATL::CT2A(TDL_TASKCATEGORY), aCategories);
}

BOOL CTaskFile::SetStatusNames(const CStringArray& aStatuses)
{
	return SetArray(ATL::CT2A(TDL_TASKSTATUS), aStatuses);
}

BOOL CTaskFile::SetAllocToNames(const CStringArray& aAllocTo)
{
	return SetArray(ATL::CT2A(TDL_TASKALLOCTO), aAllocTo);
}

BOOL CTaskFile::SetAllocByNames(const CStringArray& aAllocBy)
{
	return SetArray(ATL::CT2A(TDL_TASKALLOCBY), aAllocBy);
}

BOOL CTaskFile::SetVersionNames(const CStringArray& aVersions)
{
	return SetArray(ATL::CT2A(TDL_TASKVERSION), aVersions);
}

int CTaskFile::GetCategoryNames(CStringArray& aCategories) const
{
	return GetArray(ATL::CT2A(TDL_TASKCATEGORY), aCategories);
}

int CTaskFile::GetStatusNames(CStringArray& aStatuses) const
{
	return GetArray(ATL::CT2A(TDL_TASKSTATUS), aStatuses);
}

int CTaskFile::GetAllocToNames(CStringArray& aAllocTo) const
{
	return GetArray(ATL::CT2A(TDL_TASKALLOCTO), aAllocTo);
}

int CTaskFile::GetAllocByNames(CStringArray& aAllocBy) const
{
	return GetArray(ATL::CT2A(TDL_TASKALLOCBY), aAllocBy);
}

int CTaskFile::GetVersionNames(CStringArray& aVersions) const
{
	return GetArray(ATL::CT2A(TDL_TASKVERSION), aVersions);
}

BOOL CTaskFile::SetFileFormat(unsigned long lFormat)
{
	return (NULL != SetItemValue(TDL_FILEFORMAT, (int)lFormat));
}

BOOL CTaskFile::SetNextUniqueID(DWORD dwNextID)
{
	ASSERT(dwNextID >= m_dwNextUniqueID);

	BOOL bResult = FALSE;
	CXmlItem* pXItem = GetItem(TDL_NEXTUNIQUEID);

	if (pXItem)
	{
		pXItem->SetValue((int)dwNextID);
		bResult = TRUE;
	}
	else
	{
		bResult = (NULL != AddItem(TDL_NEXTUNIQUEID, (int)dwNextID));
	}

	if (bResult)
	{
		m_dwNextUniqueID = dwNextID;
	}

	return bResult;
}

BOOL CTaskFile::SetLastModified(const CString& sLastMod)
{
	return (NULL != SetItemValue(TDL_LASTMODIFIED, sLastMod));
}

BOOL CTaskFile::CheckOut(LPCTSTR szCheckOutTo)
{
	CString sTemp;
	return CheckOut(szCheckOutTo, sTemp);
}

BOOL CTaskFile::CheckOut(LPCTSTR szCheckOutTo, CString& sCheckedOutTo)
{
	// make sure its got a filepath attached
	if (GetFilePath().IsEmpty())
	{
		return FALSE;
	}

	sCheckedOutTo = GetCheckOutTo();

	if (!sCheckedOutTo.IsEmpty())
	{
		if (sCheckedOutTo.CompareNoCase(szCheckOutTo) == 0) // its us
		{
			return TRUE;
		}

		else if (!sCheckedOutTo.IsEmpty()) // someone else
		{
			return FALSE;
		}

		// else check it out
		SetCheckedOutTo(szCheckOutTo);
	}
	else // check it out
	{
		SetCheckedOutTo(szCheckOutTo);
	}

	// and rewrite the file but keeping the same timestamp
	FILETIME ftMod;
	::GetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);

	BOOL bCheckedOut = SaveEx();

	::SetFileTime((HANDLE)GetFileHandle(), NULL, NULL, &ftMod);

	if (bCheckedOut)
	{
		sCheckedOutTo = szCheckOutTo;
	}

	return bCheckedOut;
}

BOOL CTaskFile::SetReportAttributes(LPCTSTR szTitle, const COleDateTime& date)
{
	BOOL bRes = TRUE;

	CXmlItem* pXItem = GetItem(TDL_REPORTTITLE);

	if (pXItem)
	{
		pXItem->SetValue(szTitle);
	}
	else
	{
		bRes &= (NULL != AddItem(TDL_REPORTTITLE, szTitle));
	}

	if (date.m_dt > 0)
	{
		// date value
		pXItem = GetItem(TDL_REPORTDATEOLE);

		if (pXItem)
		{
			pXItem->SetValue(date.m_dt);
		}
		else
		{
			bRes &= (NULL != AddItem(TDL_REPORTDATE, date.m_dt));
		}

		// date string
		CString sDate = date.Format(VAR_DATEVALUEONLY);
		pXItem = GetItem(TDL_REPORTDATE);

		if (pXItem)
		{
			pXItem->SetValue(sDate);
		}
		else
		{
			bRes &= (NULL != AddItem(TDL_REPORTDATE, sDate));
		}
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
		{
			pXITask->DeleteItem(pXIChild);
		}
	}

	return TRUE;
}

unsigned long CTaskFile::GetTaskParentID(HTASKITEM hTask) const
{
	return GetTaskID(GetTaskParent(hTask));
}

HTASKITEM CTaskFile::FindTask(unsigned long dwTaskID) const
{
	if (dwTaskID <= 0)
	{
		return NULL;
	}

	// find taskID attribute
	const CXmlItem* pXI = FindItem(TDL_TASKID, (int)dwTaskID);

	// then take it's parent as the task itself
	return pXI ? (HTASKITEM)(pXI->GetParent()) : NULL;
}

BOOL CTaskFile::DeleteTask(HTASKITEM hTask)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIParent = pXITask->GetParent();
	ASSERT(pXIParent);

	// remove task from map first
	RemoveTaskFromMap(pXITask);

	return pXIParent->DeleteItem(pXITask);
}

BOOL CTaskFile::SetTaskCustomComments(HTASKITEM hTask, const CString& sContent, const CString& sType)
{
	if (!TaskFromHandle(hTask))
	{
		return FALSE;
	}

	BOOL bRes = TRUE;

	if (!sContent.IsEmpty())
	{
		CBase64Coder b64;
		b64.Encode((const PBYTE)(LPCTSTR)sContent, sContent.GetLength());

		bRes = SetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS, b64.EncodedMessage(), XIT_ATTRIB);
	}

	if (bRes)
	{
		bRes = SetTaskCChar(hTask, TDL_TASKCOMMENTSTYPE, ATL::CT2A(sType));
	}

	return bRes;
}

BOOL CTaskFile::SetTaskHtmlComments(HTASKITEM hTask, const CString& sContent, BOOL bForTransform)
{
	if (!TaskFromHandle(hTask))
	{
		return FALSE;
	}

	XI_TYPE nType = bForTransform ? XIT_ATTRIB : XIT_CDATA;

	return SetTaskCChar(hTask, TDL_TASKHTMLCOMMENTS, ATL::CT2A(sContent), nType);
}

BOOL CTaskFile::GetTaskCustomComments(HTASKITEM hTask, CString& sContent, CString& sType) const
{
	// comments type
	sType = GetTaskCChar(hTask, TDL_TASKCOMMENTSTYPE);

	// custom comments
	CString sTemp = GetTaskCChar(hTask, TDL_TASKCUSTOMCOMMENTS);

	if (sTemp.IsEmpty())
	{
		return FALSE;
	}

	CBase64Coder b64;
	b64.Decode(ATL::CT2A(sTemp));

	unsigned long nLenContent;
	PBYTE pContent = b64.DecodedMessage(nLenContent);

#if defined(UNICODE) || defined(_UNICODE)
	sContent = ATL::CA2W((PCHAR)pContent);
#else
	PBYTE szContent = (PBYTE)sContent.GetBuffer(nLenContent);
	CopyMemory(szContent, pContent, nLenContent);
	sContent.ReleaseBuffer(nLenContent);
#endif	// UNICODE || _UNICODE

	return nLenContent;
}

BOOL CTaskFile::SetTaskCategories(HTASKITEM hTask, const CStringArray& aCategories)
{
	return SetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMCATEGORY), ATL::CT2A(TDL_TASKCATEGORY), aCategories);
}

BOOL CTaskFile::SetTaskDependencies(HTASKITEM hTask, const CStringArray& aDepends)
{
	return SetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMDEPENDENCY), ATL::CT2A(TDL_TASKDEPENDENCY), aDepends);
}

BOOL CTaskFile::SetTaskAllocatedTo(HTASKITEM hTask, const CStringArray& aAllocTo)
{
	return SetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMALLOCTO), ATL::CT2A(TDL_TASKALLOCTO), aAllocTo);
}

bool CTaskFile::AddTaskDependency(HTASKITEM hTask, const char* szDepends)
{
	return AddTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMDEPENDENCY), ATL::CT2A(TDL_TASKDEPENDENCY), szDepends);
}

bool CTaskFile::AddTaskDependency(HTASKITEM hTask, unsigned long dwID)
{
	CString sID;
	sID.Format(_T("%ld"), dwID);

	return AddTaskDependency(hTask, ATL::CT2A(sID));
}

bool CTaskFile::AddTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo)
{
	return AddTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMALLOCTO), ATL::CT2A(TDL_TASKALLOCTO), szAllocTo);
}

bool CTaskFile::AddTaskCategory(HTASKITEM hTask, const char* szCategory)
{
	return AddTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMCATEGORY), ATL::CT2A(TDL_TASKCATEGORY), szCategory);
}

int CTaskFile::GetTaskCategories(HTASKITEM hTask, CStringArray& aCategories) const
{
	return GetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMCATEGORY), ATL::CT2A(TDL_TASKCATEGORY), aCategories);
}

int CTaskFile::GetTaskDependencies(HTASKITEM hTask, CStringArray& aDepends) const
{
	return GetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMDEPENDENCY), ATL::CT2A(TDL_TASKDEPENDENCY), aDepends);
}

int CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask, CStringArray& aAllocTo) const
{
	return GetTaskArray(hTask, ATL::CT2A(TDL_TASKNUMALLOCTO), ATL::CT2A(TDL_TASKALLOCTO), aAllocTo);
}

CXmlItem* CTaskFile::NewItem(LPCTSTR szName)
{
	return new CXmlItem(NULL, szName);
}

///////////////////////////////////////////////////////////////////////

HTASKITEM CTaskFile::NewTask(const char* szTitle, HTASKITEM hParent, DWORD dwID)
{
	// check no task exists having dwID
	ASSERT(FindTask(dwID) == 0);

	CXmlItem* pXIParent = hParent ? TaskFromHandle(hParent) : Root();

	if (!pXIParent)
	{
		// handle the fact that hParent might already point to Root()
		if (hParent == (HTASKITEM)Root())
		{
			pXIParent = Root();
		}
		else
		{
			return NULL;
		}
	}

	CXmlItem* pXINew = NewItem(TDL_TASK);

	if (pXINew)
	{
		pXIParent->AddItem(pXINew);
		AddTaskToMap(pXINew, FALSE);

		// set ID and name
		SetTaskTitle((HTASKITEM)pXINew, szTitle);

		if (dwID <= 0)
		{
			dwID = m_dwNextUniqueID++;
		}
		else
		{
			m_dwNextUniqueID = max(m_dwNextUniqueID, dwID + 1);
		}

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
		{
			pXIParent = Root();
		}
		else
		{
			return NULL;
		}
	}

	return (HTASKITEM)pXIParent->GetItem(TDL_TASK);
}

HTASKITEM CTaskFile::GetNextTask(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetSibling();
}

const TCHAR* CTaskFile::GetTaskTitle(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKTITLE);
}

int CTaskFile::GetTaskIconIndex(HTASKITEM hTask) const
{
	if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKICONINDEX)))
	{
		return (int)GetTaskULong(hTask, TDL_TASKICONINDEX);
	}
	else
	{
		return -1;
	}
}

const TCHAR* CTaskFile::GetTaskComments(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCOMMENTS);
}

const TCHAR* CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCTO);
}

const TCHAR* CTaskFile::GetTaskAllocatedBy(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKALLOCBY);
}

const TCHAR* CTaskFile::GetTaskCategory(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCATEGORY);
}

unsigned char CTaskFile::GetTaskCategoryCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMCATEGORY);
}

const TCHAR* CTaskFile::GetTaskCategory(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMCATEGORY), ATL::CT2A(TDL_TASKCATEGORY), nIndex);
}

unsigned char CTaskFile::GetTaskDependencyCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMDEPENDENCY);
}

const TCHAR* CTaskFile::GetTaskDependency(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMDEPENDENCY), ATL::CT2A(TDL_TASKDEPENDENCY), nIndex);
}

unsigned char CTaskFile::GetTaskAllocatedToCount(HTASKITEM hTask) const
{
	return GetTaskUChar(hTask, TDL_TASKNUMALLOCTO);
}

const TCHAR* CTaskFile::GetTaskAllocatedTo(HTASKITEM hTask, int nIndex) const
{
	return GetTaskArrayItem(hTask, ATL::CT2A(TDL_TASKNUMALLOCTO), ATL::CT2A(TDL_TASKALLOCTO), nIndex);
}

const TCHAR* CTaskFile::GetTaskDependency(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDEPENDENCY);
}

const TCHAR* CTaskFile::GetTaskStatus(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKSTATUS);
}

const TCHAR* CTaskFile::GetTaskFileReferencePath(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKFILEREFPATH);
}

const TCHAR* CTaskFile::GetTaskCreatedBy(HTASKITEM hTask) const
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

const TCHAR* CTaskFile::GetTaskWebColor(HTASKITEM hTask) const
{
	DWORD color = 0;

	if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTEXTCOLOR)))
	{
		color = GetTaskULong(hTask, TDL_TASKTEXTCOLOR);
	}
	else
	{
		color = GetTaskULong(hTask, TDL_TASKCOLOR);
	}

	return GetWebColor((COLORREF)color);
}

const TCHAR* CTaskFile::GetTaskPriorityWebColor(HTASKITEM hTask) const
{
	return GetWebColor(GetTaskPriorityColor(hTask));
}

unsigned long CTaskFile::GetTaskPriorityColor(HTASKITEM hTask) const
{
	DWORD color = RGB(255, 255, 255);

	if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPRIORITYCOLOR)))
	{
		color = GetTaskULong(hTask, TDL_TASKPRIORITYCOLOR);
	}

	return color;
}

int CTaskFile::GetTaskPriority(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKHIGHESTPRIORITY)))
	{
		return GetTaskInt(hTask, TDL_TASKHIGHESTPRIORITY);
	}

	return GetTaskInt(hTask, TDL_TASKPRIORITY);
}

unsigned char CTaskFile::GetTaskPercentDone(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCCOMPLETION)))
	{
		return GetTaskUChar(hTask, TDL_TASKCALCCOMPLETION);
	}

	return GetTaskUChar(hTask, TDL_TASKPERCENTDONE);
}

double CTaskFile::GetTaskCost(HTASKITEM hTask, BOOL bCalc) const
{
	if (bCalc && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCCOST)))
	{
		return GetTaskDouble(hTask, TDL_TASKCALCCOST);
	}

	// else
	return GetTaskDouble(hTask, TDL_TASKCOST);
}

BOOL CTaskFile::GetTaskRecurrence(HTASKITEM hTask, TDIRECURRENCE& tr) const
{
	if (GetTaskRecurrence(hTask, (int&)tr.nRegularity, tr.dwSpecific1, tr.dwSpecific2, tr.bRecalcFromDue, (int&)tr.nReuse))
	{
		CXmlItem* pXI = TaskFromHandle(hTask);

		if (pXI)
		{
			CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

			if (pXIRecur)
			{
				tr.sRegularity = pXIRecur->GetValue();
			}
		}

		return TRUE;
	}

	// else
	return FALSE;
}

bool CTaskFile::GetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1, DWORD& dwSpecific2,
	BOOL& bRecalcFromDue, int& nReuse) const
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

const TCHAR* CTaskFile::GetTaskVersion(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKVERSION);
}

double CTaskFile::GetTaskTimeEstimate(HTASKITEM hTask, TCHAR& cUnits, BOOL bCalc) const
{
	cUnits = 0;

	if (bCalc && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMEESTIMATE)))
	{
		cUnits = _T('H');
		return GetTaskDouble(hTask, TDL_TASKCALCTIMEESTIMATE);
	}
	else if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMEESTUNITS)))
	{
		const TCHAR* szUnits = GetTaskCChar(hTask, TDL_TASKTIMEESTUNITS);

		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (TCHAR)_ttoi(szUnits);

			if (!cUnits)
			{
				cUnits = szUnits[0];
			}
		}
	}

	if (!cUnits)
	{
		cUnits = _T('H');
	}

	return GetTaskDouble(hTask, TDL_TASKTIMEESTIMATE);
}

double CTaskFile::GetTaskTimeSpent(HTASKITEM hTask, TCHAR& cUnits, BOOL bCalc) const
{
	cUnits = 0;

	if (bCalc && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMESPENT)))
	{
		cUnits = 'H';
		return GetTaskDouble(hTask, TDL_TASKCALCTIMESPENT);
	}
	else if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMESPENTUNITS)))
	{
		const TCHAR* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS);

		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (TCHAR)_ttoi(szUnits);

			if (!cUnits)
			{
				cUnits = szUnits[0];
			}
		}
	}
	// backwards compatibility
	else if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMESPENTUNITS_OLD)))
	{
		const TCHAR* szUnits = GetTaskCChar(hTask, TDL_TASKTIMESPENTUNITS_OLD);

		if (szUnits && *szUnits)
		{
			// bug fix
			cUnits = (TCHAR)_ttoi(szUnits);

			if (!cUnits)
			{
				cUnits = szUnits[0];
			}
		}
	}

	if (!cUnits)
	{
		cUnits = _T('H');
	}

	if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMESPENT)))
	{
		return GetTaskDouble(hTask, TDL_TASKTIMESPENT);
	}

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
	if (bEarliest && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKEARLIESTDUEDATE)))
	{
		return GetTaskDate(hTask, TDL_TASKEARLIESTDUEDATE, TRUE);
	}

	return GetTaskDate(hTask, TDL_TASKDUEDATE, TRUE);
}

const TCHAR* CTaskFile::GetTaskDueDateString(HTASKITEM hTask, BOOL bEarliest) const
{
	if (bEarliest && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKEARLIESTDUEDATESTRING)))
	{
		return GetTaskCChar(hTask, TDL_TASKEARLIESTDUEDATESTRING);
	}

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
	return GetTaskDateOle(hTask, TDL_TASKCREATIONDATE, TRUE);
}

const TCHAR* CTaskFile::GetTaskDoneDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKDONEDATESTRING);
}

const TCHAR* CTaskFile::GetTaskCreationDateString(HTASKITEM hTask) const
{
	return GetTaskCChar(hTask, TDL_TASKCREATIONDATESTRING);
}

const TCHAR* CTaskFile::GetTaskDueDateString(HTASKITEM hTask) const
{
	return GetTaskDueDateString(hTask, FALSE);
}

const TCHAR* CTaskFile::GetTaskStartDateString(HTASKITEM hTask) const
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

	CXmlItem* pXIAttrib = pXITask->GetItem(ATL::CA2T(szAttrib));

	if (!pXIAttrib || !pXIAttrib->GetValueLen()) // not found
	{
		// some fallbacks
		if (strcmp(szAttrib, ATL::CT2A(TDL_TASKCOLOR)) == 0)
		{
			return TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTEXTCOLOR));
		}

		else if (strcmp(szAttrib, ATL::CT2A(TDL_TASKWEBCOLOR)) == 0)
		{
			return TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTEXTWEBCOLOR));
		}

		else if (strcmp(szAttrib, ATL::CT2A(TDL_TASKPARENTID)) == 0)
		{
			return TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKID));
		}

		// else
		return FALSE;
	}

	// finally check for hidden attribute
	return (pXIAttrib->GetItemValueI(_T("HIDE")) == 0);
}

const TCHAR* CTaskFile::GetTaskAttribute(HTASKITEM hTask, const char* szAttrib) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, _T(""));

	CString sValue = pXITask->GetItemValue(ATL::CA2T(szAttrib));

	// special case: Parent ID
	if (sValue.IsEmpty() && strcmp(szAttrib, ATL::CT2A(TDL_TASKPARENTID)) == 0 &&
		TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKID)))
	{
		static CString sPID;
		sPID.Format(_T("%d"), GetTaskParentID(hTask));
		sValue = sPID;
	}

	return sValue;
}

HTASKITEM CTaskFile::GetTaskParent(HTASKITEM hTask) const
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, NULL);

	return (HTASKITEM)pXITask->GetParent();
}

unsigned long CTaskFile::GetTaskTextColor(HTASKITEM hTask) const
{
	if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTEXTCOLOR)))
	{
		return GetTaskULong(hTask, TDL_TASKTEXTCOLOR);
	}

	else if (TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCOLOR)))
	{
		return GetTaskULong(hTask, TDL_TASKCOLOR);
	}

	// else
	return 0;
}

int CTaskFile::GetTaskRisk(HTASKITEM hTask, BOOL bHighest) const
{
	if (bHighest && TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKHIGHESTRISK)))
	{
		return GetTaskInt(hTask, TDL_TASKHIGHESTRISK);
	}

	return GetTaskInt(hTask, TDL_TASKRISK);
}

const TCHAR* CTaskFile::GetTaskExternalID(HTASKITEM hTask) const
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

		HideAttribute(hTask, ATL::CT2A(TDL_TASKID), !bVisible);

		return true;
	}

	return false;
}

BOOL CTaskFile::HideAttribute(HTASKITEM hTask, const char* szAttrib, BOOL bHide)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	CXmlItem* pXIAttrib = pXITask->GetItem(ATL::CA2T(szAttrib));

	if (pXIAttrib)
	{
		CXmlItem* pXIHide = pXIAttrib->GetItem(_T("HIDE"));

		// if not visible add 'HIDE' attribute
		if (bHide)
		{
			if (pXIHide)
			{
				pXIHide->SetValue(1);
			}
			else
			{
				pXIAttrib->AddItem(_T("HIDE"), 1);
			}
		}
		else if (pXIHide)
		{
			pXIAttrib->DeleteItem(pXIHide);
		}

		return TRUE;
	}

	return FALSE;
}

bool CTaskFile::SetTaskColor(HTASKITEM hTask, unsigned long nColor)
{
	return SetTaskULong(hTask, TDL_TASKCOLOR, nColor) &&
		SetTaskCChar(hTask, TDL_TASKWEBCOLOR, ATL::CT2A(GetWebColor((COLORREF)nColor)));
}

bool CTaskFile::SetTaskPriority(HTASKITEM hTask, unsigned char nPriority)
{
	return SetTaskUChar(hTask, TDL_TASKPRIORITY, (unsigned char)max(0, min(10, nPriority)));
}

bool CTaskFile::SetTaskPriority(HTASKITEM hTask, int nPriority)
{
	if (nPriority != FT_NOPRIORITY)
	{
		nPriority = (char)max(0, min(10, nPriority));
	}

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

bool CTaskFile::SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, TCHAR cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMEESTIMATE, dTimeEst, TDL_TASKTIMEESTUNITS, cUnits);
}

bool CTaskFile::SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, TCHAR cUnits)
{
	return SetTaskTime(hTask, TDL_TASKTIMESPENT, dTimeSpent, TDL_TASKTIMESPENTUNITS, cUnits);
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
	return (BOOL)SetTaskDate(hTask, TDL_TASKLASTMOD, tLastMod, TRUE, TDL_TASKLASTMODSTRING);
}

BOOL CTaskFile::SetTaskCreationDate(HTASKITEM hTask, const COleDateTime& date)
{
	return (BOOL)SetTaskDate(hTask, TDL_TASKCREATIONDATE, date, TRUE, TDL_TASKCREATIONDATESTRING);
}

BOOL CTaskFile::SetTaskDoneDate(HTASKITEM hTask, const COleDateTime& date)
{
	return (BOOL)SetTaskDate(hTask, TDL_TASKDONEDATE, date, TRUE, TDL_TASKDONEDATESTRING);
}

BOOL CTaskFile::SetTaskDueDate(HTASKITEM hTask, const COleDateTime& date)
{
	return (BOOL)SetTaskDate(hTask, TDL_TASKDUEDATE, date, TRUE, TDL_TASKDUEDATESTRING);
}

BOOL CTaskFile::SetTaskStartDate(HTASKITEM hTask, const COleDateTime& date)
{
	return (BOOL)SetTaskDate(hTask, TDL_TASKSTARTDATE, date, TRUE, TDL_TASKSTARTDATESTRING);
}

BOOL CTaskFile::SetTaskRecurrence(HTASKITEM hTask, const TDIRECURRENCE& tr)
{
	if (SetTaskRecurrence(hTask, (int)tr.nRegularity, tr.dwSpecific1, tr.dwSpecific2, tr.bRecalcFromDue, tr.nReuse))
	{
		SetTaskCChar(hTask, TDL_TASKRECURRENCE, ATL::CT2A(tr.sRegularity), XIT_ELEMENT); // set human readable
		return true;
	}

	return false;
}

bool CTaskFile::SetTaskRecurrence(HTASKITEM hTask, int nRegularity, DWORD dwSpecific1, DWORD dwSpecific2,
	BOOL bRecalcFromDue, int nReuse)
{
	CXmlItem* pXI = TaskFromHandle(hTask);

	if (pXI)
	{
		CXmlItem* pXIRecur = pXI->GetItem(TDL_TASKRECURRENCE);

		if (!pXIRecur)
		{
			pXIRecur = pXI->AddItem(TDL_TASKRECURRENCE);
		}

		pXIRecur->SetItemValue(TDL_TASKRECURRENCEREG, nRegularity);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC1, (int)dwSpecific1);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCESPEC2, (int)dwSpecific2);
		pXIRecur->SetItemValue(TDL_TASKRECURRENCEREUSE, nReuse);

		if (bRecalcFromDue)
		{
			pXIRecur->SetItemValue(TDL_TASKRECURRENCEFROMDUE, TRUE);
		}
		else
		{
			pXIRecur->DeleteItem(TDL_TASKRECURRENCEFROMDUE);
		}

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
		SetTaskCChar(hTask, TDL_TASKTEXTWEBCOLOR, ATL::CT2A(GetWebColor(color)));
}

BOOL CTaskFile::SetTaskIconIndex(HTASKITEM hTask, int nIndex)
{
	return SetTaskULong(hTask, TDL_TASKICONINDEX, nIndex);
}

BOOL CTaskFile::SetTaskPriorityColor(HTASKITEM hTask, COLORREF color)
{
	return SetTaskULong(hTask, TDL_TASKPRIORITYCOLOR, color) &&
		SetTaskCChar(hTask, TDL_TASKPRIORITYWEBCOLOR, ATL::CT2A(GetWebColor(color)));
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
	{
		nPriority = (char)max(0, min(10, nPriority));
	}

	return SetTaskInt(hTask, TDL_TASKHIGHESTPRIORITY, nPriority);
}

BOOL CTaskFile::SetTaskHighestRisk(HTASKITEM hTask, int nRisk)
{
	if (nRisk != FT_NORISK)
	{
		nRisk = (char)max(0, min(10, nRisk));
	}

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
	{
		nRisk = (char)max(-1, min(10, nRisk));
	}

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
		tm time =
		{
			date.GetSecond(),
			date.GetMinute(),
			date.GetHour(),
			date.GetDay(),
			date.GetMonth() - 1,
			date.GetYear() - 1900,
			date.GetDayOfWeek() - 1,
			date.GetDayOfYear(),
			-1
		};

		time_t timet = mktime(&time);

		return (timet == -1) ? 0 : timet;
	}

	return 0;
}

COleDateTime CTaskFile::GetTaskDateOle(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const
{
	double dDate = GetTaskDouble(hTask, szDateItem);

	if (!bIncTime)
	{
		dDate = floor(dDate);
	}

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

const TCHAR* CTaskFile::GetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem) const
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

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, const COleDateTime& date, BOOL bIncTime,
	LPCTSTR szDateStringItem)
{
	double dDate = bIncTime ? date.m_dt : floor(date.m_dt);

	if (SetTaskDouble(hTask, szDateItem, dDate))
	{
		if (szDateStringItem)
		{
			DWORD dwFmt = m_bISODates ? DHFD_ISO : 0;

			if (bIncTime && CDateHelper::GetTimeOnly(date) > 0.0)
			{
				dwFmt |= DHFD_TIME | DHFD_NOSEC;
			}

			return SetTaskCChar(hTask, szDateStringItem, ATL::CT2A(CDateHelper::FormatDate(date, dwFmt)));
		}

		return true;
	}

	// else
	return false;
}

bool CTaskFile::SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, time_t tVal, BOOL bIncTime)
{
	if (tVal == 0)
	{
		return SetTaskDate(hTask, szDateItem, COleDateTime(0.0), bIncTime);
	}

	//fabio_2005
#if _MSC_VER >= 1400
	errno_t err;
	tm time;
	tm* pTime = &time;
	// Get time as 64-bit integer.
	// Convert to local time
	err = localtime_s(pTime, &tVal);
#else
	tm* pTime = localtime(&tVal);
#endif

	if (!pTime)
	{
		return false;
	}

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

	return (pXITask->SetItemValue(szCCharItem, ATL::CA2T(szVal), nType) != NULL);
}

bool CTaskFile::SetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem, double dVal)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

	return (pXITask->SetItemValue(szDoubleItem, dVal, XIT_ATTRIB) != NULL);
}

bool CTaskFile::SetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem, double dTime, LPCTSTR szUnitsItem, TCHAR cUnits)
{
	TCHAR* szUnits;

	switch (cUnits)
	{
	case _T('i'):
	case _T('I'):
		szUnits = _T("I"); // minutes
		break;

	case _T('h'):
	case _T('H'):
		szUnits = _T("H");
		break;

	case _T('d'):
	case _T('D'):
		szUnits = _T("D");
		break;

	case _T('w'):
	case _T('W'):
		szUnits = _T("W");
		break;

	case _T('m'):
	case _T('M'):
		szUnits = _T("M");
		break;

	case _T('y'):
	case _T('Y'):
		szUnits = _T("Y");
		break;

	default: // all else
		return false;
	}

	return SetTaskDouble(hTask, szTimeItem, dTime) &&
		SetTaskCChar(hTask, szUnitsItem, ATL::CT2A(szUnits));
}

CString CTaskFile::GetWebColor(COLORREF color)
{
	unsigned char cRed = GetRValue(color);
	unsigned char cGreen = GetGValue(color);
	unsigned char cBlue = GetBValue(color);

	static CString sColor;
	sColor.Format(_T("#%02X%02X%02X"), cRed, cGreen, cBlue);

	return sColor;
}

const TCHAR* CTaskFile::GetTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, const char* szItemTag,
	int nIndex) const
{
	if (nIndex == 0)
	{
		return GetTaskCChar(hTask, ATL::CA2T(szItemTag));   // first item
	}

	// else
	int nCount = GetTaskUChar(hTask, ATL::CA2T(szNumItemTag));

	if (!nCount || nIndex < 1 || nIndex > nCount - 1)
	{
		return NULL;
	}

	CString sItem;
	sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nIndex);

	return GetTaskCChar(hTask, sItem);
}

bool CTaskFile::DeleteTaskArray(HTASKITEM hTask, const char* szNumItemTag, const char* szItemTag)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

	// delete any existing items
	pXITask->DeleteItem(ATL::CA2T(szItemTag));

	int nItem, nNumExists = pXITask->GetItemValueI(ATL::CA2T(szNumItemTag));

	for (nItem = 1; nItem < nNumExists; nItem++)
	{
		CString sItem;
		sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nItem);
		pXITask->DeleteItem(sItem);
	}

	pXITask->DeleteItem(ATL::CA2T(szNumItemTag));

	return true;
}

bool CTaskFile::AddTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, const char* szItemTag, const char* szItem)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, false);

	int nCount = pXITask->GetItemValueI(ATL::CA2T(szNumItemTag));

	// see if it already exists
	if (nCount || pXITask->GetItem(ATL::CA2T(szItemTag)))
	{
		CString sValue = pXITask->GetItemValue(ATL::CA2T(szItemTag));

		if (sValue.CompareNoCase(ATL::CA2T(szItem)) == 0)
		{
			return false;   // already exists
		}

		// the rest have numbers after their names
		for (int nItem = 1; nItem < nCount; nItem++)
		{
			CString sItem;
			sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nItem);

			CString sValue = pXITask->GetItemValue(sItem);

			if (sValue.CompareNoCase(ATL::CA2T(szItem)) == 0)
			{
				return false;   // already exists
			}
		}
	}

	// does the task have an existing item?
	if (!pXITask->GetItem(ATL::CA2T(szItemTag)))
	{
		pXITask->AddItem(ATL::CA2T(szNumItemTag), 1); // num
		pXITask->AddItem(ATL::CA2T(szItemTag), ATL::CA2T(szItem)); // first item
	}
	else // append
	{
		// increment item count
		pXITask->SetItemValue(ATL::CA2T(szNumItemTag), nCount + 1);

		CString sItem;
		sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nCount);

		pXITask->AddItem(sItem, ATL::CA2T(szItem));
	}

	return true;
}

int CTaskFile::GetTaskArray(HTASKITEM hTask, const char* szNumItemTag, const char* szItemTag,
	CStringArray& aItems) const
{
	aItems.RemoveAll();

	// first item
	CString sItem = GetTaskCChar(hTask, ATL::CA2T(szItemTag));

	if (!sItem.IsEmpty())
	{
		aItems.Add(sItem);

		// rest
		int nCount = GetTaskUChar(hTask, ATL::CA2T(szNumItemTag));

		for (int nItem = 1; nItem < nCount; nItem++)
		{
			aItems.Add(GetTaskArrayItem(hTask, szNumItemTag, szItemTag, nItem));
		}
	}

	return aItems.GetSize();

}

BOOL CTaskFile::SetTaskArray(HTASKITEM hTask, const char* szNumItemTag, const char* szItemTag,
	const CStringArray& aItems)
{
	CXmlItem* pXITask = NULL;
	GET_TASK(pXITask, hTask, FALSE);

	// delete any existing items
	DeleteTaskArray(hTask, szNumItemTag, szItemTag);

	// then add these
	int nCount = aItems.GetSize();

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		AddTaskArrayItem(hTask, szNumItemTag, szItemTag, ATL::CT2A(aItems[nItem]));
	}

	return TRUE;
}

int CTaskFile::GetArray(const char* szItemTag, CStringArray& aItems) const
{
	aItems.RemoveAll();

	const CXmlItem* pXI = GetItem(ATL::CA2T(szItemTag));

	if (pXI)
	{
		int nCount = pXI->GetItemCount();

		for (int nItem = 0; nItem < nCount; nItem++)
		{
			CString sItem;
			sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nItem);

			aItems.Add(pXI->GetItemValue(sItem));
		}
	}

	return aItems.GetSize();
}

BOOL CTaskFile::SetArray(const char* szItemTag, const CStringArray& aItems)
{
	// delete existing items
	DeleteItem(ATL::CA2T(szItemTag));

	// and start again
	if (aItems.GetSize())
	{
		CXmlItem* pXI = AddItem(ATL::CA2T(szItemTag));

		if (pXI)
		{
			int nCount = aItems.GetSize();

			for (int nItem = 0; nItem < nCount; nItem++)
			{
				CString sItem;
				sItem.Format(_T("%s%d"), (LPTSTR)ATL::CA2T(szItemTag), nItem);

				pXI->AddItem(sItem, aItems[nItem]);
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
