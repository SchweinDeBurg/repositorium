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
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// ToDoItem.cpp: implementation of the CToDoItem class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoItem.h"
#include "TDCEnum.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

// C4995: name was marked as #pragma deprecated
#pragma warning(disable: 4995)
IMPLEMENT_FIXED_ALLOC(TODOITEM, 1024);
#pragma warning(default: 4995)

TODOITEM::TODOITEM(LPCTSTR szTitle, LPCTSTR szComments) :
sTitle(szTitle),
sComments(szComments),
color(0),
nPriority(5),
nRisk(0),
nPercentDone(0),
dTimeEstimate(0),
dTimeSpent(0),
nTimeEstUnits(TDITU_HOURS),
nTimeSpentUnits(TDITU_HOURS),
dCost(0),
bFlagged(FALSE),
dateCreated(COleDateTime::GetCurrentTime()),
nIconIndex(-1),
dwCalculated(0),
nCalcPriority(0),
nCalcPriorityIncDue(0),
nCalcPercent(0),
nCalcRisk(0),
dCalcTimeEstimate(0),
dCalcTimeSpent(0),
dCalcCost(0),
bGoodAsDone(0),
nSubtasksCount(0),
nSubtasksDone(0)
{
}

TODOITEM::TODOITEM() :
color(0),
nPriority(5),
nRisk(0),
nPercentDone(0),
dTimeEstimate(0),
dTimeSpent(0),
nTimeEstUnits(TDITU_HOURS),
nTimeSpentUnits(TDITU_HOURS),
dCost(0),
bFlagged(FALSE),
dateCreated(COleDateTime::GetCurrentTime()),
nIconIndex(-1),
dwCalculated(0),
nCalcPriority(0),
nCalcPriorityIncDue(0),
nCalcPercent(0),
nCalcRisk(0),
dCalcTimeEstimate(0),
dCalcTimeSpent(0),
dCalcCost(0),
bGoodAsDone(0),
nSubtasksCount(0),
nSubtasksDone(0)
{
}

TODOITEM::TODOITEM(const TODOITEM& tdi)
{
	*this = tdi;

	if (dateCreated.m_dt == 0.0)
	{
		dateCreated = COleDateTime::GetCurrentTime();
	}
}

TODOITEM::TODOITEM(const TODOITEM* pTDI)
{
	if (pTDI)
	{
		*this = *pTDI;

		if (dateCreated.m_dt == 0.0)
		{
			dateCreated = COleDateTime::GetCurrentTime();
		}
	}
}

const TODOITEM& TODOITEM::operator=(const TODOITEM& tdi)
{
	sTitle = tdi.sTitle;
	sComments = tdi.sComments;
	sCustomComments = tdi.sCustomComments;
	sCommentsTypeID = tdi.sCommentsTypeID;
	color = tdi.color;
	sFileRefPath = tdi.sFileRefPath;
	sAllocBy = tdi.sAllocBy;
	sStatus = tdi.sStatus;
	nPriority = tdi.nPriority;
	nPercentDone = tdi.nPercentDone;
	dTimeEstimate = tdi.dTimeEstimate;
	dTimeSpent = tdi.dTimeSpent;
	nTimeEstUnits = tdi.nTimeEstUnits;
	nTimeSpentUnits = tdi.nTimeSpentUnits;
	dCost = tdi.dCost;
	dateStart = tdi.dateStart;
	dateDue = tdi.dateDue;
	dateDone = tdi.dateDone;
	dateCreated = tdi.dateCreated;
	bFlagged = tdi.bFlagged;
	sCreatedBy = tdi.sCreatedBy;
	nRisk = tdi.nRisk;
	sExternalID = tdi.sExternalID;
	trRecurrence = tdi.trRecurrence;
	tLastMod = tdi.tLastMod;
	sVersion = tdi.sVersion;
	nIconIndex = tdi.nIconIndex,

	aCategories.Copy(tdi.aCategories);
	aAllocTo.Copy(tdi.aAllocTo);
	aDependencies.Copy(tdi.aDependencies);

	dwCalculated = 0; // reset calcs
	nCalcPriority = 0;
	nCalcPriorityIncDue = 0;
	nCalcPercent = 0;
	nCalcRisk = 0;
	dCalcTimeEstimate = 0;
	dCalcTimeSpent = 0;
	dCalcCost = 0;
	bGoodAsDone = 0;
	nSubtasksCount = 0;
	nSubtasksDone = 0;

	return *this;
}

BOOL TODOITEM::AttribNeedsRecalc(DWORD dwAttrib) const
{
	return ((dwCalculated & dwAttrib) == 0);
}

void TODOITEM::SetAttribNeedsRecalc(DWORD dwAttrib, BOOL bSet) const
{
	if (bSet)
	{
		dwCalculated &= ~dwAttrib;
	}
	else
	{
		dwCalculated |= dwAttrib;
	}
}

BOOL TODOITEM::HasCreation() const
{
	return (dateCreated.m_dt > 0.0) ? TRUE : FALSE;
}

BOOL TODOITEM::HasLastMod() const
{
	return (tLastMod.m_dt > 0.0) ? TRUE : FALSE;
}

BOOL TODOITEM::HasStart() const
{
	return (dateStart.m_dt > 0.0) ? TRUE : FALSE;
}

BOOL TODOITEM::HasStartTime() const
{
	return HasTime(dateStart);
}

BOOL TODOITEM::HasDue() const
{
	return (dateDue.m_dt > 0.0) ? TRUE : FALSE;
}

BOOL TODOITEM::HasDueTime() const
{
	return HasTime(dateDue);
}

BOOL TODOITEM::HasTime(const COleDateTime& date)
{
	return (CDateHelper::GetTimeOnly(date.m_dt) > 0.0) ? TRUE : FALSE;
}

BOOL TODOITEM::IsDone() const
{
	return (dateDone.m_dt > 0) ? TRUE : FALSE;
}

BOOL TODOITEM::HasDoneTime() const
{
	return HasTime(dateDone);
}

void TODOITEM::ClearStart()
{
	dateStart.m_dt = 0;
}

void TODOITEM::ClearDue()
{
	dateDue.m_dt = 0;
}

void TODOITEM::ClearDone()
{
	dateDone.m_dt = 0;
}

BOOL TODOITEM::IsDue() const
{
	return IsDue(COleDateTime::GetCurrentTime());
}

BOOL TODOITEM::IsDue(const COleDateTime& dateDueBy) const
{
	if (IsDone() || !HasDue())
	{
		return FALSE;
	}

	return (floor(dateDue.m_dt) <= floor(dateDueBy.m_dt));
}

void TODOITEM::SetModified()
{
	tLastMod = COleDateTime::GetCurrentTime();
}

void TODOITEM::ResetCalcs() const
{
	dwCalculated = 0;
}

CString TODOITEM::GetFirstCategory() const
{
	return aCategories.GetSize() ? aCategories[0] : _T("");
}

CString TODOITEM::GetFirstAllocTo() const
{
	return aAllocTo.GetSize() ? aAllocTo[0] : _T("");
}

CString TODOITEM::GetFirstDependency() const
{
	return aDependencies.GetSize() ? aDependencies[0] : _T("");
}

BOOL TODOITEM::GetNextOccurence(COleDateTime& dtNext) const
{
	if (trRecurrence.bRecalcFromDue && HasDue())
	{
		return trRecurrence.GetNextOccurence(dateDue, dtNext);
	}

	// else use completed date but with the current due time
	if (trRecurrence.GetNextOccurence(COleDateTime::GetCurrentTime(), dtNext))
	{
		// restore the due time to be whatever it was
		double dDueTime = HasDue() ? CDateHelper::GetTimeOnly(dateDue) : 0.0;
		dtNext = CDateHelper::GetDateOnly(dtNext) + dDueTime;

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL TODOITEM::IsRecentlyEdited(const COleDateTimeSpan& dtSpan) const
{
	if (!HasLastMod())
	{
		return FALSE;
	}

	// else
	return (COleDateTime::GetCurrentTime() - tLastMod < dtSpan);
}

COleDateTimeSpan TODOITEM::GetRemainingDueTime() const
{
	return GetRemainingDueTime(dateDue);
}

COleDateTimeSpan TODOITEM::GetRemainingDueTime(const COleDateTime& date)
{
	COleDateTimeSpan dtsRemaining = date - COleDateTime::GetCurrentTime();

	if (!HasTime(date))
	{
		dtsRemaining += 1;   // midnight on the day
	}

	return dtsRemaining;
}

void TODOITEM::ParseTaskLink(const CString& sLink, DWORD& dwTaskID, CString& sFile)
{
	sFile.Empty();
	dwTaskID = 0;

	int nDiv = sLink.Find('?');

	if (nDiv >= 0)
	{
		sFile = sLink.Left(nDiv);
		sFile.TrimLeft();
		sFile.TrimRight();

		CString sTaskID = sLink.Mid(nDiv + 1);
		dwTaskID = _ttoi(sTaskID);
	}
	else if (!sLink.IsEmpty())
	{
		if (isdigit(sLink[0])) // number
		{
			dwTaskID = _ttoi(sLink);
			sFile.Empty();
		}
		else
		{
			dwTaskID = 0;
			sFile = sLink;
		}
	}
}

CString TODOITEM::MakeTaskLink(DWORD dwTaskID, const CString& sFile)
{
	CString sLink;

	if (sFile.IsEmpty() && dwTaskID > 0)
	{
		sLink.Format(_T("%d"), dwTaskID);
	}

	else if (!sFile.IsEmpty())
	{
		if (dwTaskID > 0)
		{
			sLink.Format(_T("%s?%d"), sFile, dwTaskID);
		}
		else
		{
			sLink = sFile;
		}
	}

	return sLink;
}

//////////////////////////////////////////////////////////////////////////////////////////////

TODOSTRUCTURE::TODOSTRUCTURE(DWORD dwID) : m_dwID(dwID), m_pTDSParent(NULL)
{
	ASSERT(dwID);
}

TODOSTRUCTURE::TODOSTRUCTURE(const TODOSTRUCTURE& tds)
{
	*this = tds; // invoke assignment operator
}

TODOSTRUCTURE::~TODOSTRUCTURE()
{
	CleanUp();
}

const TODOSTRUCTURE& TODOSTRUCTURE::operator=(const TODOSTRUCTURE& tds)
{
	// reset our own contents
	CleanUp();

	// copy target
	m_dwID = tds.m_dwID;

	// clear parent
	m_pTDSParent = NULL; // caller must add to parent explicitly

	// copy children
	for (int nSubTask = 0; nSubTask < tds.GetSubTaskCount(); nSubTask++)
	{
		const TODOSTRUCTURE* pTDSOther = tds.GetSubTask(nSubTask);
		ASSERT(pTDSOther);

		TODOSTRUCTURE* pTDSChild = new TODOSTRUCTURE(*pTDSOther); // this will copy the children's children
		m_aSubTasks.Add(pTDSChild);

		// set parent
		pTDSChild->m_pTDSParent = this;
	}

	return *this;
}

TODOSTRUCTURE* TODOSTRUCTURE::GetSubTask(int nPos) const
{
	if (nPos >= 0 && nPos < GetSubTaskCount())
	{
		return m_aSubTasks[nPos];
	}

	// else
	ASSERT(0);
	return NULL;
}

int TODOSTRUCTURE::GetSubTaskPos(TODOSTRUCTURE* pTDS) const
{
	for (int nSubTask = 0; nSubTask < GetSubTaskCount(); nSubTask++)
	{
		if (GetSubTaskID(nSubTask) == pTDS->GetTaskID())
		{
			return nSubTask;
		}
	}

	// else
	ASSERT(0);
	return -1;
}

DWORD TODOSTRUCTURE::GetSubTaskID(int nPos) const
{
	const TODOSTRUCTURE* pTDS = GetSubTask(nPos);
	return pTDS ? pTDS->GetTaskID() : 0;
}

int TODOSTRUCTURE::GetSubTaskPosition(DWORD dwID) const
{
	ASSERT(dwID);

	if (!dwID)
	{
		return -1;
	}

	for (int nSubTask = 0; nSubTask < GetSubTaskCount(); nSubTask++)
	{
		if (GetSubTaskID(nSubTask) == dwID)
		{
			return nSubTask;
		}
	}

	// not found
	return -1;
}

int TODOSTRUCTURE::GetPosition() const
{
	if (m_pTDSParent == NULL) // root
	{
		return -1;
	}

	// get the position of 'this' task in its parent
	return m_pTDSParent->GetSubTaskPosition(GetTaskID());
}

DWORD TODOSTRUCTURE::GetParentTaskID() const
{
	if (m_pTDSParent == NULL) // root
	{
		return NULL;
	}

	return m_pTDSParent->GetTaskID();
}

TODOSTRUCTURE* TODOSTRUCTURE::GetParentTask() const
{
	return m_pTDSParent;
}

DWORD TODOSTRUCTURE::GetPreviousSubTaskID(int nPos)
{
	if (nPos <= 0 || nPos >= GetSubTaskCount())
	{
		return 0;
	}

	// else
	return GetSubTaskID(nPos - 1);
}

// protected helper
BOOL TODOSTRUCTURE::InsertSubTask(TODOSTRUCTURE* pTDS, int nPos)
{
	// sanity checks
	ASSERT(pTDS && pTDS->GetTaskID());

	if (!pTDS)
	{
		return FALSE;
	}

	ASSERT(nPos >= 0 && nPos <= GetSubTaskCount());

	if (nPos < 0 || nPos > GetSubTaskCount())
	{
		return FALSE;
	}

	// check task with this ID does not already exist
	if (GetSubTaskPosition(pTDS->GetTaskID()) != -1)
	{
		ASSERT(0);
		return FALSE;
	}

	if (nPos == GetSubTaskCount())
	{
		m_aSubTasks.Add(pTDS);
	}
	else
	{
		m_aSubTasks.InsertAt(nPos, pTDS);
	}

	// setup ourselves as parent
	pTDS->m_pTDSParent = this;

	return TRUE;
}

BOOL TODOSTRUCTURE::DeleteSubTask(int nPos)
{
	ASSERT(nPos >= 0 && nPos < GetSubTaskCount());

	if (nPos < 0 || nPos >= GetSubTaskCount())
	{
		return FALSE;
	}

	delete GetSubTask(nPos);
	m_aSubTasks.RemoveAt(nPos);

	return TRUE;
}

void TODOSTRUCTURE::CleanUp()
{
	// clean up children
	for (int nSubTask = 0; nSubTask < GetSubTaskCount(); nSubTask++)
	{
		TODOSTRUCTURE* pTDSChild = GetSubTask(nSubTask);
		ASSERT(pTDSChild);

		delete pTDSChild;
	}

	m_aSubTasks.RemoveAll();
}

int TODOSTRUCTURE::MoveSubTask(int nPos, TODOSTRUCTURE* pTDSDestParent, int nDestPos)
{
	// check destination is okay
	ASSERT(pTDSDestParent && nDestPos >= 0 && nDestPos <= pTDSDestParent->GetSubTaskCount());

	if (!pTDSDestParent || nDestPos < 0 || nDestPos > pTDSDestParent->GetSubTaskCount())
	{
		return -1;
	}

	TODOSTRUCTURE* pTDS = GetSubTask(nPos);
	ASSERT(pTDS);

	if (!pTDS)
	{
		return -1;
	}

	m_aSubTasks.RemoveAt(nPos); // remove from 'this' TODOSTRUCTURE

	// special case: the the source and destination are the same and the source
	// pos precedes the destination then we need to decrement the destination
	// to allow for having just deleted the source
	if (this == pTDSDestParent && nPos < nDestPos)
	{
		nDestPos--;
	}

	// add to destination
	pTDSDestParent->InsertSubTask(pTDS, nDestPos);

	return nDestPos;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CToDoCtrlStructure::CToDoCtrlStructure(const CToDoCtrlStructure& tds)
{
	*this = tds;
}

CToDoCtrlStructure::~CToDoCtrlStructure()
{
	m_mapStructure.RemoveAll();
}

const CToDoCtrlStructure& CToDoCtrlStructure::operator=(const CToDoCtrlStructure& tds)
{
	TODOSTRUCTURE::operator=(tds);

	BuildMap();

	return *this;
}

DWORD CToDoCtrlStructure::GetPreviousTaskID(DWORD dwID) const
{
	TODOSTRUCTURE* pTDSParent = NULL;
	int nPos = -1;

	if (!FindTask(dwID, pTDSParent, nPos))
	{
		return 0;
	}

	// else
	return pTDSParent->GetPreviousSubTaskID(nPos);
}

DWORD CToDoCtrlStructure::GetParentTaskID(DWORD dwID) const
{
	TODOSTRUCTURE* pTDSParent = GetParentTask(dwID);

	if (!pTDSParent)
	{
		return 0;
	}

	return pTDSParent->GetTaskID();
}

TODOSTRUCTURE* CToDoCtrlStructure::GetParentTask(DWORD dwID) const
{
	TODOSTRUCTURE* pTDSParent = NULL;
	int nPos = -1;

	if (!FindTask(dwID, pTDSParent, nPos))
	{
		return NULL;
	}

	return pTDSParent;
}

BOOL CToDoCtrlStructure::AddTask(DWORD dwID, TODOSTRUCTURE* pTDSParent)
{
	TODOSTRUCTURE* pTDSChild = new TODOSTRUCTURE(dwID);

	if (pTDSParent->InsertSubTask(pTDSChild, pTDSParent->GetSubTaskCount()))
	{
		AddToMap(pTDSChild);
		return TRUE;
	}

	// else
	delete pTDSChild;
	return FALSE;
}

BOOL CToDoCtrlStructure::DeleteTask(DWORD dwID)
{
	ASSERT(dwID);

	if (!dwID)
	{
		return FALSE;
	}

	TODOSTRUCTURE* pTDS = FindTask(dwID);
	ASSERT(pTDS);

	if (!pTDS)
	{
		return FALSE;
	}

	TODOSTRUCTURE* pTDSParent = pTDS->GetParentTask();
	ASSERT(pTDSParent);

	if (!pTDSParent)
	{
		return FALSE;
	}

	int nPos = pTDSParent->GetSubTaskPos(pTDS);
	ASSERT(nPos != -1);

	if (nPos == -1)
	{
		return FALSE;
	}

	m_mapStructure.RemoveKey(dwID);

	return pTDSParent->DeleteSubTask(nPos);
}

TODOSTRUCTURE* CToDoCtrlStructure::FindTask(DWORD dwID) const
{
	TODOSTRUCTURE* pTDS = NULL;

	return (dwID && m_mapStructure.Lookup(dwID, pTDS)) ? pTDS : NULL;
}

BOOL CToDoCtrlStructure::FindTask(DWORD dwID, TODOSTRUCTURE*& pTDSParent, int& nPos) const
{
	pTDSParent = NULL;
	nPos = -1;

	TODOSTRUCTURE* pTDS = FindTask(dwID);

	if (!pTDS)
	{
		return FALSE;
	}

	pTDSParent = pTDS->GetParentTask();
	ASSERT(pTDSParent);

	if (!pTDSParent)
	{
		return FALSE;
	}

	nPos = pTDSParent->GetSubTaskPos(pTDS);
	ASSERT(nPos != -1);

	return (nPos != -1);
}

BOOL CToDoCtrlStructure::InsertTask(DWORD dwID, TODOSTRUCTURE* pTDSParent, int nPos)
{
	return InsertTask(TODOSTRUCTURE(dwID), pTDSParent, nPos);
}

BOOL CToDoCtrlStructure::InsertTask(const TODOSTRUCTURE& tds, TODOSTRUCTURE* pTDSParent, int nPos)
{
	TODOSTRUCTURE* pTDSChild = new TODOSTRUCTURE(tds);

	if (!InsertTask(pTDSChild, pTDSParent, nPos))
	{
		delete pTDSChild;
		return FALSE;
	}

	return TRUE;
}

BOOL CToDoCtrlStructure::InsertTask(TODOSTRUCTURE* pTDS, TODOSTRUCTURE* pTDSParent, int nPos)
{
	if (pTDSParent->InsertSubTask(pTDS, nPos))
	{
		AddToMap(pTDS);
		return TRUE;
	}

	// else
	return FALSE;
}

void CToDoCtrlStructure::BuildMap()
{
	for (int nSubTask = 0; nSubTask < GetSubTaskCount(); nSubTask++)
	{
		TODOSTRUCTURE* pTDSChild = GetSubTask(nSubTask);
		ASSERT(pTDSChild);

		AddToMap(pTDSChild);
	}
}

void CToDoCtrlStructure::AddToMap(const TODOSTRUCTURE* pTDS)
{
	ASSERT(!pTDS->IsRoot());

	if (pTDS->IsRoot())
	{
		return;
	}

	m_mapStructure[pTDS->GetTaskID()] = const_cast<TODOSTRUCTURE*>(pTDS);

	// children
	for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
	{
		TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
		ASSERT(pTDSChild);

		AddToMap(pTDSChild);
	}
}

void CToDoCtrlStructure::RemoveFromMap(const TODOSTRUCTURE* pTDS)
{
	ASSERT(!pTDS->IsRoot());

	if (pTDS->IsRoot())
	{
		return;
	}

	m_mapStructure.RemoveKey(pTDS->GetTaskID());

	// children
	for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
	{
		TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
		ASSERT(pTDSChild);

		RemoveFromMap(pTDSChild);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
