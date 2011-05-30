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
// - merged with ToDoList versions 6.1.2-6.1.10 sources
//*****************************************************************************

// ToDoCtrlData.cpp: implementation of the CToDoCtrlData class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoCtrlData.h"
#include "Resource.h"

#include "../../Common/XmlFile.h"
#include "../../../CodeProject/Source/TimeHelper.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/TreeCtrlHelper.h"
#include "../../../CodeProject/Source/TreeDragDropHelper.h"

#include <float.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SAVE_UNDO(op, id, pid, psid) AddUndoElement(op, id, pid, psid)
#define SAVE_UNDOEDIT(id, tdi, a) if (m_undo.IsActive()) m_undo.SaveElement(TDCUEO_EDIT, id, 0, 0, (WORD)a, tdi)

CUndoAction::CUndoAction(CToDoCtrlData& data, TDCUNDOACTIONTYPE nType) : m_data(data)
{
	m_bSuccess = m_data.BeginNewUndoAction(nType);
}

CUndoAction::~CUndoAction()
{
	if (m_bSuccess)
	{
		m_data.EndCurrentUndoAction();
	}
}

//////////////////////////////////////////////////////////////////////

CToDoCtrlData::CToDoCtrlData(const CWordArray& aStyles) : m_aStyles(aStyles)
{
}

CToDoCtrlData::~CToDoCtrlData()
{
	DeleteAllTasks();
}

TODOITEM* CToDoCtrlData::NewTask(const TODOITEM* pTDIRef)
{
	if (pTDIRef)
	{
		return new TODOITEM(*pTDIRef);
	}
	else
	{
		return new TODOITEM;
	}
}

TODOITEM* CToDoCtrlData::GetTask(DWORD dwTaskID) const
{
	TODOITEM* pTDI;

	return (dwTaskID && m_mapID2TDI.Lookup(dwTaskID, pTDI)) ? pTDI : NULL;
}

BOOL CToDoCtrlData::Locate(DWORD dwParentID, DWORD dwPrevSiblingID, TODOSTRUCTURE*& pTDSParent, int& nPos) const
{
	pTDSParent = NULL;
	nPos = -1;

	if (dwPrevSiblingID)
	{
		m_struct.FindTask(dwPrevSiblingID, pTDSParent, nPos);
	}
	else if (dwParentID)
	{
		pTDSParent = m_struct.FindTask(dwParentID);
	}
	else
	{
		pTDSParent = const_cast<CToDoCtrlStructure*>(&m_struct);   // root
	}

	ASSERT(pTDSParent);

	return (pTDSParent != NULL);
}

BOOL CToDoCtrlData::LocateTask(DWORD dwTaskID, TODOSTRUCTURE*& pTDSParent, int& nPos) const
{
	return m_struct.FindTask(dwTaskID, pTDSParent, nPos);
}

TODOSTRUCTURE* CToDoCtrlData::LocateTask(DWORD dwTaskID) const
{
	// special case
	if (dwTaskID == 0)
	{
		return const_cast<CToDoCtrlStructure*>(&m_struct);
	}

	// else
	return m_struct.FindTask(dwTaskID);
}

void CToDoCtrlData::AddTask(DWORD dwTaskID, TODOITEM* pTDI, DWORD dwParentID, DWORD dwPrevSiblingID)
{
	if (dwTaskID && pTDI)
	{
		// must delete duplicates else we'll get a memory leak
		TODOITEM* pExist = GetTask(dwTaskID);

		if (pExist)
		{
			m_mapID2TDI.RemoveKey(dwTaskID);
			delete pExist;
		}
	}

	// add to structure
	TODOSTRUCTURE* pTDSParent = NULL;
	int nPrevSibling = -1;

	if (!Locate(dwParentID, dwPrevSiblingID, pTDSParent, nPrevSibling))
	{
		return;
	}

	m_struct.InsertTask(dwTaskID, pTDSParent, nPrevSibling + 1);
	m_mapID2TDI.SetAt(dwTaskID, pTDI);

	SAVE_UNDO(TDCUEO_ADD, dwTaskID, dwParentID, dwPrevSiblingID);
}

void CToDoCtrlData::DeleteAllTasks()
{
	while (m_struct.GetSubTaskCount())
	{
		DeleteTask(&m_struct, 0);
	}

	ASSERT(GetTaskCount() == 0);
}

CString CToDoCtrlData::GetTaskTitle(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sTitle;
	}

	return _T("");
}

int CToDoCtrlData::GetTaskIconIndex(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->nIconIndex;
	}

	return -1;
}

CString CToDoCtrlData::GetTaskComments(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sComments;
	}

	return _T("");
}

CString CToDoCtrlData::GetTaskCustomComments(DWORD dwTaskID, CString& sCommentsTypeID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		sCommentsTypeID = pTDI->sCommentsTypeID;
		return pTDI->sCustomComments;
	}

	// else
	sCommentsTypeID.Empty();
	return _T("");
}

double CToDoCtrlData::GetTaskCost(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->dCost;
	}

	return 0;
}

double CToDoCtrlData::GetTaskTimeEstimate(DWORD dwTaskID, int& nUnits) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		nUnits = pTDI->nTimeEstUnits;
		return pTDI->dTimeEstimate;
	}

	return 0;
}

double CToDoCtrlData::GetTaskTimeSpent(DWORD dwTaskID, int& nUnits) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		nUnits = pTDI->nTimeSpentUnits;
		return pTDI->dTimeSpent;
	}

	return 0;
}

int CToDoCtrlData::GetTaskAllocTo(DWORD dwTaskID, CStringArray& aAllocTo) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);
	aAllocTo.RemoveAll();

	if (pTDI)
	{
		aAllocTo.Copy(pTDI->aAllocTo);
	}

	return aAllocTo.GetSize();
}

CString CToDoCtrlData::GetTaskAllocBy(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sAllocBy;
	}

	return _T("");
}

CString CToDoCtrlData::GetTaskVersion(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sVersion;
	}

	return _T("");
}

CString CToDoCtrlData::GetTaskStatus(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sStatus;
	}

	return _T("");
}

int CToDoCtrlData::GetTaskCategories(DWORD dwTaskID, CStringArray& aCategories) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);
	aCategories.RemoveAll();

	if (pTDI)
	{
		aCategories.Copy(pTDI->aCategories);
	}

	return aCategories.GetSize();
}

CString CToDoCtrlData::GetTaskPath(DWORD dwTaskID, BOOL bIncludeTaskName, int nMaxLen) const
{
	if (nMaxLen == 0)
	{
		return _T("");
	}

	TODOSTRUCTURE* pTDS = LocateTask(dwTaskID);

	if (!pTDS)
	{
		return _T("");
	}

	int nMaxElementLen = -1;
	CString sPath;

	do
	{
		const TODOSTRUCTURE* pTDSParent = pTDS->GetParentTask();
		int nMaxParentLen = 0;

		sPath.Empty();

		while (pTDSParent && !pTDSParent->IsRoot())
		{
			TODOITEM* pTDIParent = GetTask(pTDSParent);
			ASSERT(pTDIParent);

			if (!pTDIParent)
			{
				return _T("");
			}

			CString sParent = pTDIParent->sTitle;
			nMaxParentLen = max(nMaxParentLen, sParent.GetLength());

			if (nMaxElementLen != -1 && sParent.GetLength() > nMaxElementLen)
			{
				sParent = sParent.Left(nMaxElementLen) + _T("...");
			}

			sParent.TrimLeft();
			sParent.TrimRight();
			sPath = sParent + _T(" / ") + sPath;

			pTDSParent = pTDSParent->GetParentTask();
		}

		// add taskname?
		if (bIncludeTaskName)
		{
			sPath += GetTaskTitle(dwTaskID);
		}

		if (nMaxLen > 0 && sPath.GetLength() > nMaxLen)
		{
			if (nMaxElementLen == -1)
			{
				nMaxElementLen = nMaxParentLen;
			}

			nMaxElementLen--; // try again
		}
		else
		{
			break;
		}
	}
	while (nMaxElementLen > 0);

	return sPath;
}

int CToDoCtrlData::GetTaskDependencies(DWORD dwTaskID, CStringArray& aDependencies) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);
	aDependencies.RemoveAll();

	if (pTDI)
	{
		aDependencies.Copy(pTDI->aDependencies);
	}

	return aDependencies.GetSize();
}

int CToDoCtrlData::GetTaskDependents(DWORD dwTaskID, CDWordArray& aDependents) const
{
	if (!dwTaskID)
	{
		return 0;
	}

	POSITION pos = m_mapID2TDI.GetStartPosition();
	CEnString sTaskID(_T("%d"), dwTaskID);

	CDWordArray aTemp;

	while (pos)
	{
		TODOITEM* pTDI;
		DWORD dwDependsID;

		m_mapID2TDI.GetNextAssoc(pos, dwDependsID, pTDI);

		if (dwDependsID != dwTaskID && Misc::Find(pTDI->aDependencies, sTaskID) != -1)
		{
			aDependents.Add(dwDependsID);
		}
	}

	return aDependents.GetSize();
}

int CToDoCtrlData::GetTaskDependencies(DWORD dwTaskID, CDWordArray& aDepends) const
{
	static int nDepth = 0;

	// note: only clear aDepends if first call
	if (nDepth == 0)
	{
		aDepends.RemoveAll();
	}

	nDepth++;

	TODOITEM* pTDI = GetTask(dwTaskID);
	BOOL bContinue = TRUE;

	if (pTDI)
	{
		for (int nDepends = 0; nDepends < pTDI->aDependencies.GetSize(); nDepends++)
		{
			// we only handle 'same file' links
			DWORD dwIDLink = _ttoi(pTDI->aDependencies[nDepends]);

			if (dwIDLink && dwIDLink != dwTaskID)
			{
				// stop if we already have this ID because it means there's a
				// circular dependency
				int nLink = aDepends.GetSize();

				while (nLink--)
				{
					if (aDepends[nLink] == dwIDLink)
					{
						bContinue = FALSE;
						break;
					}
				}

				if (bContinue)
				{
					// insert dependency at head of list
					aDepends.InsertAt(0, dwIDLink);

					// add this links dependencies too
					GetTaskDependencies(dwIDLink, aDepends);
				}
			}
		}
	}

	nDepth--;

	return aDepends.GetSize();
}

BOOL CToDoCtrlData::TaskHasCircularDependencies(DWORD dwTaskID) const
{
	// trace each of this tasks dependencies to see if it ever comes
	// back to itself.
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		for (int nDepends = 0; nDepends < pTDI->aDependencies.GetSize(); nDepends++)
		{
			// we only handle 'same file' links
			DWORD dwIDLink = _ttoi(pTDI->aDependencies[nDepends]);

			if (dwIDLink && FindDependency(dwIDLink, dwTaskID))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CToDoCtrlData::FindDependency(DWORD dwTaskID, DWORD dwDependsID) const
{
	// simple checks
	if (!dwTaskID)
	{
		return FALSE;
	}

	if (dwTaskID == dwDependsID)
	{
		return TRUE;
	}

	TODOITEM* pTDI = GetTask(dwTaskID);
	ASSERT(pTDI);

	if (!pTDI)
	{
		return FALSE;
	}

	for (int nDepends = 0; nDepends < pTDI->aDependencies.GetSize(); nDepends++)
	{
		// we only handle 'same file' links
		DWORD dwIDLink = _ttoi(pTDI->aDependencies[nDepends]);

		if (dwIDLink && FindDependency(dwIDLink, dwDependsID))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CString CToDoCtrlData::GetTaskExtID(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sExternalID;
	}

	return _T("");
}

CString CToDoCtrlData::GetTaskCreatedBy(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sCreatedBy;
	}

	return _T("");
}

COLORREF CToDoCtrlData::GetTaskColor(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->color;
	}

	return 0;
}

int CToDoCtrlData::GetTaskPriority(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->nPriority;
	}

	return 0;
}

int CToDoCtrlData::GetTaskRisk(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->nRisk;
	}

	return 0;
}

BOOL CToDoCtrlData::IsTaskFlagged(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->bFlagged;
	}

	return FALSE;
}

BOOL CToDoCtrlData::GetTaskRecurrence(DWORD dwTaskID, TDIRECURRENCE& tr) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		tr = pTDI->trRecurrence;
		return TRUE;
	}

	return FALSE;
}

BOOL CToDoCtrlData::GetTaskNextOccurrence(DWORD dwTaskID, COleDateTime& dtNext) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->GetNextOccurence(dtNext);
	}

	// else
	return FALSE;
}

COleDateTime CToDoCtrlData::GetTaskDate(DWORD dwTaskID, TDC_DATE nDate) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		switch (nDate)
		{
		case TDCD_CREATE:
			return pTDI->dateCreated;
		case TDCD_START:
			return pTDI->dateStart;
		case TDCD_STARTDATE:
			return CDateHelper::GetDateOnly(pTDI->dateStart);
		case TDCD_STARTTIME:
			return CDateHelper::GetTimeOnly(pTDI->dateStart);
		case TDCD_DUE:
			return pTDI->dateDue;
		case TDCD_DUEDATE:
			return CDateHelper::GetDateOnly(pTDI->dateDue);
		case TDCD_DUETIME:
			return CDateHelper::GetTimeOnly(pTDI->dateDue);
		case TDCD_DONE:
			return pTDI->dateDone;
		case TDCD_DONEDATE:
			return CDateHelper::GetDateOnly(pTDI->dateDone);
		case TDCD_DONETIME:
			return CDateHelper::GetTimeOnly(pTDI->dateDone);
		}
	}

	ASSERT(0);
	return COleDateTime();
}

int CToDoCtrlData::GetTaskPercent(DWORD dwTaskID, BOOL bCheckIfDone) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		if (bCheckIfDone)
		{
			return pTDI->IsDone() ? 100 : pTDI->nPercentDone;
		}
		else
		{
			return pTDI->nPercentDone;
		}
	}

	return 0;
}

CString CToDoCtrlData::GetTaskFileRef(DWORD dwTaskID) const
{
	TODOITEM* pTDI = GetTask(dwTaskID);

	if (pTDI)
	{
		return pTDI->sFileRefPath;
	}

	return _T("");
}

BOOL CToDoCtrlData::DeleteTask(DWORD dwTaskID)
{
	TODOSTRUCTURE* pTDSParent = NULL;
	int nPos = -1;

	if (!m_struct.FindTask(dwTaskID, pTDSParent, nPos))
	{
		ASSERT(0);
		return FALSE;
	}

	DeleteTask(pTDSParent, nPos);

	return TRUE;
}

BOOL CToDoCtrlData::DeleteTask(TODOSTRUCTURE* pTDSParent, int nPos)
{
	TODOSTRUCTURE* pTDS = pTDSParent->GetSubTask(nPos);
	ASSERT(pTDS);

	// do children first to ensure entire branch is deleted
	while (pTDS->GetSubTaskCount() > 0)
	{
		DeleteTask(pTDS, 0);
	}

	// save undo
	DWORD dwTaskID = pTDS->GetTaskID();
	DWORD dwParentID = pTDSParent->GetTaskID();
	DWORD dwPrevSiblingID = nPos ? pTDSParent->GetSubTaskID(nPos - 1) : 0;

	AddUndoElement(TDCUEO_DELETE, dwTaskID, dwParentID, dwPrevSiblingID);

	// then this item
	delete GetTask(dwTaskID);
	m_mapID2TDI.RemoveKey(dwTaskID);
	m_struct.DeleteTask(dwTaskID);

	return TRUE;
}

void CToDoCtrlData::ResetCachedCalculations() const
{
	// sets the bNeedRecalc flag on all items
	POSITION pos = m_mapID2TDI.GetStartPosition();
	TODOITEM* pTDI;
	DWORD dwTaskID;

	while (pos)
	{
		m_mapID2TDI.GetNextAssoc(pos, dwTaskID, pTDI);
		pTDI->ResetCalcs();
	}
}

int CToDoCtrlData::CopyTaskAttributes(DWORD dwToTaskID, DWORD dwFromTaskID, const CTDCAttributeArray& aAttribs)
{
	if (aAttribs.GetSize() == 0)
	{
		return SET_NOCHANGE;
	}

	TODOITEM* pToTDI = GetTask(dwToTaskID);

	int nRes = CopyTaskAttributes(pToTDI, dwFromTaskID, aAttribs);

	// save undo info
	if (nRes == SET_CHANGE)
	{
		SAVE_UNDOEDIT(dwToTaskID, pToTDI, aAttribs[0]);
	}

	return nRes;
}

int CToDoCtrlData::CopyTaskAttributes(TODOITEM* pToTDI, DWORD dwFromTaskID, const CTDCAttributeArray& aAttribs)
{
	TODOITEM* pFromTDI = GetTask(dwFromTaskID);

	if (!pToTDI || !pFromTDI)
	{
		return SET_NOCHANGE;
	}

	int nRes = SET_NOCHANGE;

	// helper macros
#define COPYATTRIB(a) if (pToTDI->a != pFromTDI->a) { pToTDI->a = pFromTDI->a; nRes = SET_CHANGE; }
#define COPYATTRIBARR(a) if (!Misc::ArraysMatch(pToTDI->a, pFromTDI->a)) { pToTDI->a.Copy(pFromTDI->a); nRes = SET_CHANGE; }

	// note: we don't use the public SetTask* methods purely so we can
	// capture all the edits as a single atomic change that can be undone
	for (int nAtt = 0; nAtt < aAttribs.GetSize(); nAtt++)
	{
		TDC_ATTRIBUTE nAttrib = aAttribs[nAtt];

		switch (nAttrib)
		{
		case TDCA_TASKNAME:
			COPYATTRIB(sTitle);
			break;
		case TDCA_DONEDATE:
			COPYATTRIB(dateDone);
			break;
		case TDCA_DUEDATE:
			COPYATTRIB(dateDue);
			break;
		case TDCA_STARTDATE:
			COPYATTRIB(dateStart);
			break;
		case TDCA_PRIORITY:
			COPYATTRIB(nPriority);
			break;
		case TDCA_RISK:
			COPYATTRIB(nRisk);
			break;
		case TDCA_COLOR:
			COPYATTRIB(color);
			break;
		case TDCA_ALLOCBY:
			COPYATTRIB(sAllocBy);
			break;
		case TDCA_STATUS:
			COPYATTRIB(sStatus);
			break;
		case TDCA_PERCENT:
			COPYATTRIB(nPercentDone);
			break;
		case TDCA_FILEREF:
			COPYATTRIB(sFileRefPath);
			break;
		case TDCA_VERSION:
			COPYATTRIB(sVersion);
			break;
		case TDCA_EXTERNALID:
			COPYATTRIB(sExternalID);
			break;
		case TDCA_FLAG:
			COPYATTRIB(bFlagged);
			break;

		case TDCA_TIMEEST:
			COPYATTRIB(dTimeEstimate);
			COPYATTRIB(nTimeEstUnits);
			break;
		case TDCA_TIMESPENT:
			COPYATTRIB(dTimeSpent);
			COPYATTRIB(nTimeSpentUnits);
			break;

		case TDCA_COMMENTS:
			COPYATTRIB(sComments);
			COPYATTRIB(sCustomComments);
			COPYATTRIB(sCommentsTypeID);
			break;

		case TDCA_ALLOCTO:
			COPYATTRIBARR(aAllocTo);
			break;
		case TDCA_CATEGORY:
			COPYATTRIBARR(aCategories);
			break;
		}
	}

	return nRes;
}

BOOL CToDoCtrlData::ApplyLastChangeToSubtasks(DWORD dwTaskID, TDC_ATTRIBUTE nAttrib)
{
	TODOITEM* pTDI = GetTask(dwTaskID);
	TODOSTRUCTURE* pTDS = m_struct.FindTask(dwTaskID);

	return ApplyLastChangeToSubtasks(pTDI, pTDS, nAttrib);
}

BOOL CToDoCtrlData::ApplyLastChangeToSubtasks(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, TDC_ATTRIBUTE nAttrib)
{
	ASSERT(pTDI && pTDS);

	if (!pTDI || !pTDS)
	{
		return FALSE;
	}

	for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
	{
		DWORD dwChildID = pTDS->GetSubTaskID(nSubTask);
		TODOITEM* pTDIChild = GetTask(dwChildID);
		ASSERT(pTDIChild);

		if (!pTDIChild)
		{
			return FALSE;
		}

		SAVE_UNDOEDIT(dwChildID, pTDIChild, nAttrib);

		// apply the change based on nAttrib
		TRACE(_T("ApplyLastChangeToSubtask(%d)\n"), dwChildID);

		switch (nAttrib)
		{
		case TDCA_DONEDATE:
			pTDIChild->dateDone = pTDI->dateDone;
			break;

		case TDCA_DUEDATE:
			pTDIChild->dateDue = pTDI->dateDue;
			break;

		case TDCA_STARTDATE:
			pTDIChild->dateStart = pTDI->dateStart;
			break;

		case TDCA_PRIORITY:
			pTDIChild->nPriority = pTDI->nPriority;
			break;

		case TDCA_COLOR:
			pTDIChild->color = pTDI->color;
			break;

		case TDCA_ALLOCTO:
			pTDIChild->aAllocTo.Copy(pTDI->aAllocTo);
			break;

		case TDCA_ALLOCBY:
			pTDIChild->sAllocBy = pTDI->sAllocBy;
			break;

		case TDCA_STATUS:
			pTDIChild->sStatus = pTDI->sStatus;
			break;

		case TDCA_CATEGORY:
			pTDIChild->aCategories.Copy(pTDI->aCategories);
			break;

		case TDCA_PERCENT:
			pTDIChild->nPercentDone = pTDI->nPercentDone;
			break;

		case TDCA_TIMEEST:
			pTDIChild->dTimeEstimate = pTDI->dTimeEstimate;
			break;

		case TDCA_FILEREF:
			pTDIChild->sFileRefPath = pTDI->sFileRefPath;
			break;

		case TDCA_VERSION:
			pTDIChild->sVersion = pTDI->sVersion;
			break;

		case TDCA_FLAG:
			pTDIChild->bFlagged = pTDI->bFlagged;
			break;

		default:
			ASSERT(0);
			return FALSE;
		}

		// and its children too
		ApplyLastChangeToSubtasks(pTDIChild, pTDS->GetSubTask(nSubTask), nAttrib);
	}

	return TRUE;
}

int CToDoCtrlData::SetTaskColor(DWORD dwTaskID, COLORREF color)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			// if the color is 0 then add 1 to discern from unset
			if (color == 0)
			{
				color = 1;
			}

			else if (color == (COLORREF) - 1) // and treat -1 as meaning unset color
			{
				color = 0;
			}

			if (pTDI->color != color)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_COLOR);

				pTDI->color = color;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskComments(DWORD dwTaskID, LPCTSTR szComments, const CString& sCustomComments, LPCTSTR szCommentsTypeID)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			BOOL bCommentsChange = (pTDI->sComments != szComments);
			BOOL bCustomCommentsChange = bCommentsChange;

			if (!bCustomCommentsChange)
			{
				int nLen = sCustomComments.GetLength();

				bCustomCommentsChange = (pTDI->sCommentsTypeID != szCommentsTypeID) ||
					(pTDI->sCustomComments.GetLength() != nLen) ||
					(memcmp((LPCTSTR)pTDI->sCustomComments, (LPCTSTR)sCustomComments, nLen) != 0);
			}

			if (bCommentsChange || bCustomCommentsChange)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_COMMENTS);

				if (bCommentsChange)
				{
					pTDI->sComments = szComments;
				}

				if (bCustomCommentsChange)
				{
					// if we're changing comments type we clear the custom comments
					if (pTDI->sCommentsTypeID != szCommentsTypeID)
					{
						pTDI->sCustomComments.Empty();
					}
					else
					{
						pTDI->sCustomComments = sCustomComments;
					}

					pTDI->sCommentsTypeID = szCommentsTypeID;
				}

				pTDI->SetModified();
				return SET_CHANGE;
			}

			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCommentsType(DWORD dwTaskID, LPCTSTR szCommentsTypeID)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{

			if (pTDI->sCommentsTypeID != szCommentsTypeID)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_COMMENTS);

				pTDI->sCustomComments.Empty();
				pTDI->sCommentsTypeID = szCommentsTypeID;

				pTDI->SetModified();
				return SET_CHANGE;
			}

			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTitle(DWORD dwTaskID, LPCTSTR szTitle)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sTitle != szTitle)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_TASKNAME);

				pTDI->sTitle = szTitle;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskIconIndex(DWORD dwTaskID, int nIndex)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->nIconIndex != nIndex)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_ICON);

				pTDI->nIconIndex = nIndex;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskFlag(DWORD dwTaskID, BOOL bFlagged)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->bFlagged != bFlagged)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_FLAG);

				pTDI->bFlagged = bFlagged;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskRecurrence(DWORD dwTaskID, const TDIRECURRENCE& tr)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->trRecurrence != tr)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_RECURRENCE);

				pTDI->trRecurrence = tr;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskPriority(DWORD dwTaskID, int nPriority)
{
	if (dwTaskID && (nPriority == FT_NOPRIORITY || (nPriority >= 0 && nPriority <= 10)))
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->nPriority != nPriority)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_PRIORITY);

				pTDI->nPriority = nPriority;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskRisk(DWORD dwTaskID, int nRisk)
{
	if (dwTaskID && (nRisk == FT_NORISK || (nRisk >= 0 && nRisk <= 10)))
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->nRisk != nRisk)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_RISK);

				pTDI->nRisk = nRisk;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

TDC_ATTRIBUTE CToDoCtrlData::MapDateToAttribute(TDC_DATE nDate)
{
	switch (nDate)
	{
	case TDCD_CREATE:
		return TDCA_CREATIONDATE;
	case TDCD_START:
	case TDCD_STARTDATE:
	case TDCD_STARTTIME:
		return TDCA_STARTDATE;
	case TDCD_DUE:
	case TDCD_DUEDATE:
	case TDCD_DUETIME:
		return TDCA_DUEDATE;
	case TDCD_DONE:
	case TDCD_DONEDATE:
	case TDCD_DONETIME:
		return TDCA_DONEDATE;
	}

	// else
	ASSERT(0);
	return TDCA_NONE;
}

int CToDoCtrlData::SetTaskDate(DWORD dwTaskID, TDC_DATE nDate, const COleDateTime& date)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			// whole days only (except for start & due & done dates)
			COleDateTime dtDate = date;

			if (nDate == TDCD_CREATE)
			{
				dtDate = CDateHelper::GetDateOnly(dtDate);
			}

			COleDateTime dtCur = GetTaskDate(dwTaskID, nDate);

			if (dtCur != dtDate)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, MapDateToAttribute(nDate));

				switch (nDate)
				{
				case TDCD_CREATE:
					pTDI->dateCreated = dtDate;
					break;

				case TDCD_START:
					pTDI->dateStart = dtDate;
					break;

				case TDCD_STARTDATE:
					// add date to existing time component unless date is 0.0
					if (dtDate.m_dt == 0.0 || !pTDI->HasStart())
					{
						pTDI->dateStart = CDateHelper::GetDateOnly(dtDate);
					}
					else
					{
						pTDI->dateStart = CDateHelper::MakeDate(pTDI->dateStart, dtDate);
					}
					break;

				case TDCD_STARTTIME:
					// add time to date component only if it exists
					if (pTDI->HasStart())
					{
						pTDI->dateStart = CDateHelper::MakeDate(dtDate, pTDI->dateStart);
					}
					break;

				case TDCD_DUE:
					pTDI->dateDue = dtDate;
					break;

				case TDCD_DUEDATE:
					// add date to existing time component unless date is 0.0
					if (dtDate.m_dt == 0.0 || !pTDI->HasDue())
					{
						pTDI->dateDue = CDateHelper::GetDateOnly(dtDate);
					}
					else
					{
						pTDI->dateDue = CDateHelper::MakeDate(pTDI->dateDue, dtDate);
					}
					break;

				case TDCD_DUETIME:
					// add time to date component only if it exists
					if (pTDI->HasDue())
					{
						pTDI->dateDue = CDateHelper::MakeDate(dtDate, pTDI->dateDue);
					}
					break;

				case TDCD_DONE:
					{
						BOOL bWasDone = pTDI->IsDone();
						pTDI->dateDone = dtDate;

						// restore last known percentage if going from done
						// to not-done unless is 100%
						if (bWasDone && !pTDI->IsDone() && pTDI->nPercentDone == 100)
						{
							pTDI->nPercentDone = 0;
						}
					}
					break;

				case TDCD_DONEDATE:
					// add date to existing time component unless date is 0.0
					if (dtDate.m_dt == 0.0 || !pTDI->IsDone())
					{
						pTDI->dateDone = CDateHelper::GetDateOnly(dtDate);
					}
					else
					{
						pTDI->dateDone = CDateHelper::MakeDate(pTDI->dateDone, dtDate);
					}
					break;

				case TDCD_DONETIME:
					// add time to date component only if it exists
					if (pTDI->IsDone())
					{
						pTDI->dateDone = CDateHelper::MakeDate(dtDate, pTDI->dateDone);
					}
					break;

				default:
					ASSERT(0);
					return SET_FAILED;
				}

				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::OffsetTaskDate(DWORD dwTaskID, TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks)
{
	DH_UNITS nDHUnits = (DH_UNITS)nUnits;
	ASSERT(nDHUnits == DHU_DAYS || nDHUnits == DHU_WEEKS || nDHUnits == DHU_MONTHS);

	COleDateTime date = GetTaskDate(dwTaskID, nDate);
	CDateHelper::OffsetDate(date, nAmount, nDHUnits);

	int nRes = SET_NOCHANGE;
	int nItemRes = SetTaskDate(dwTaskID, nDate, date);

	if (nItemRes == SET_CHANGE)
	{
		nRes = SET_CHANGE;
	}

	// children
	if (bAndSubtasks)
	{
		TODOSTRUCTURE* pTDS = m_struct.FindTask(dwTaskID);
		ASSERT(pTDS);

		for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
		{
			DWORD dwChildID = pTDS->GetSubTaskID(nSubTask);
			nItemRes = OffsetTaskDate(dwChildID, nDate, nAmount, nUnits, TRUE);

			if (nItemRes == SET_CHANGE)
			{
				nRes = SET_CHANGE;
			}
		}
	}

	return nRes;
}

int CToDoCtrlData::SetTaskPercent(DWORD dwTaskID, int nPercent)
{
	if (nPercent < 0 || nPercent > 100)
	{
		return FALSE;
	}

	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->nPercentDone != nPercent)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_PERCENT);

				pTDI->nPercentDone = nPercent;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCost(DWORD dwTaskID, double dCost)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->dCost != dCost)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_COST);

				pTDI->dCost = dCost;
				pTDI->SetModified();
				return SET_CHANGE;
			}

			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTimeEstimate(DWORD dwTaskID, double dTime, int nUnits)
{
	if (dTime < 0)
	{
		return FALSE;
	}

	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->dTimeEstimate != dTime || pTDI->nTimeEstUnits != nUnits)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_TIMEEST);

				pTDI->dTimeEstimate = dTime;
				pTDI->nTimeEstUnits = nUnits;
				pTDI->SetModified();
				return SET_CHANGE;
			}

			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskTimeSpent(DWORD dwTaskID, double dTime, int nUnits)
{
	if (dTime < 0)
	{
		return FALSE;
	}

	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->dTimeSpent != dTime || pTDI->nTimeSpentUnits != nUnits)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_TIMESPENT);

				pTDI->dTimeSpent = dTime;
				pTDI->nTimeSpentUnits = nUnits;
				pTDI->SetModified();
				return SET_CHANGE;
			}

			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::ResetTaskTimeSpent(DWORD dwTaskID, BOOL bAndChildren)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI;
		TODOSTRUCTURE* pTDS;

		if (GetTask(dwTaskID, pTDI, pTDS))
		{
			int nChange = SET_NOCHANGE;

			if (pTDI->dTimeSpent != 0.0)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_TIMESPENT);

				pTDI->dTimeSpent = 0;
				pTDI->SetModified();

				nChange = SET_CHANGE;
			}

			// children?
			if (bAndChildren)
			{
				for (int nSubtask = 0; nSubtask < pTDS->GetSubTaskCount(); nSubtask++)
				{
					DWORD dwSubtaskID = pTDS->GetSubTaskID(nSubtask);

					if (ResetTaskTimeSpent(dwSubtaskID, TRUE) == SET_CHANGE)
					{
						nChange = SET_CHANGE;
					}
				}
			}

			return nChange;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskAllocTo(DWORD dwTaskID, const CStringArray& aAllocTo)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (!Misc::ArraysMatch(pTDI->aAllocTo, aAllocTo))
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_ALLOCTO);

				pTDI->aAllocTo.Copy(aAllocTo);
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskAllocBy(DWORD dwTaskID, LPCTSTR szAllocBy)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sAllocBy != szAllocBy)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_ALLOCBY);

				pTDI->sAllocBy = szAllocBy;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskVersion(DWORD dwTaskID, LPCTSTR szVersion)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sVersion != szVersion)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_VERSION);

				pTDI->sVersion = szVersion;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskStatus(DWORD dwTaskID, LPCTSTR szStatus)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sStatus != szStatus)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_STATUS);

				pTDI->sStatus = szStatus;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskCategories(DWORD dwTaskID, const CStringArray& aCategories)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (!Misc::ArraysMatch(pTDI->aCategories, aCategories))
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_CATEGORY);

				pTDI->aCategories.Copy(aCategories);
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskDependencies(DWORD dwTaskID, const CStringArray& aDepends)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (!Misc::ArraysMatch(pTDI->aDependencies, aDepends))
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_DEPENDENCY);

				pTDI->aDependencies.Copy(aDepends);
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskExtID(DWORD dwTaskID, LPCTSTR szID)
{
	if (dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sExternalID != szID)
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_EXTERNALID);

				pTDI->sExternalID = szID;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::SetTaskFileRef(DWORD dwTaskID, LPCTSTR szFilePath)
{
	if (dwTaskID && szFilePath)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		if (pTDI)
		{
			if (pTDI->sFileRefPath.CompareNoCase(szFilePath))
			{
				SAVE_UNDOEDIT(dwTaskID, pTDI, TDCA_FILEREF);

				pTDI->sFileRefPath = szFilePath;
				pTDI->SetModified();
				return SET_CHANGE;
			}
			return SET_NOCHANGE;
		}
	}

	return SET_FAILED;
}

int CToDoCtrlData::MapTimeUnits(const CString& sUnits)
{
	if (sUnits.IsEmpty())
	{
		return TDITU_HOURS;   // default
	}

	switch (sUnits[0])
	{
	case _T('I'):
	case _T('i'):
		return TDITU_MINS;

	case _T('D'):
	case _T('d'):
		return TDITU_DAYS;

	case _T('W'):
	case _T('w'):
		return TDITU_WEEKS;

	case _T('M'):
	case _T('m'):
		return TDITU_MONTHS;

	case _T('Y'):
	case _T('y'):
		return TDITU_YEARS;
	}

	// all else
	return TDITU_HOURS;
}

CString CToDoCtrlData::MapTimeUnits(int nUnits)
{
	switch (nUnits)
	{
	case TDITU_MINS:
		return _T("I");
	case TDITU_DAYS:
		return _T("D");
	case TDITU_WEEKS:
		return _T("W");
	case TDITU_MONTHS:
		return _T("M");
	case TDITU_YEARS:
		return _T("Y");
	}

	// all else
	return _T("H");
}

BOOL CToDoCtrlData::BeginNewUndoAction(TDCUNDOACTIONTYPE nType)
{
	return m_undo.BeginNewAction(nType);
}

BOOL CToDoCtrlData::EndCurrentUndoAction()
{
	return m_undo.EndCurrentAction();
}

BOOL CToDoCtrlData::AddUndoElement(TDCUNDOELMOP nOp, DWORD dwTaskID, DWORD dwParentID, DWORD dwPrevSiblingID, WORD wFlags)
{
	if (!m_undo.IsActive())
	{
		return FALSE;
	}

	TODOITEM* pTDI = GetTask(dwTaskID);
	ASSERT(pTDI);

	if (!pTDI)
	{
		return FALSE;
	}

	// save task state
	VERIFY(m_undo.SaveElement(nOp, dwTaskID, dwParentID, dwPrevSiblingID, wFlags, pTDI));

	// save children recursively if an add
	if (nOp == TDCUEO_ADD)
	{
		const TODOSTRUCTURE* pTDS = m_struct.FindTask(dwTaskID);
		dwPrevSiblingID = 0; // reuse
		dwParentID = dwTaskID; // reuse

		for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
		{
			TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
			ASSERT(pTDSChild);

			dwTaskID = pTDSChild->GetTaskID(); // reuse
			VERIFY(AddUndoElement(nOp, dwTaskID, dwParentID, dwPrevSiblingID));

			dwPrevSiblingID = dwTaskID;
		}
	}

	return TRUE;
}

int CToDoCtrlData::GetLastUndoActionTaskIDs(BOOL bUndo, CDWordArray& aIDs) const
{
	return bUndo ? m_undo.GetLastUndoActionTaskIDs(aIDs) : m_undo.GetLastRedoActionTaskIDs(aIDs);
}

TDCUNDOACTIONTYPE CToDoCtrlData::GetLastUndoActionType(BOOL bUndo) const
{
	return (bUndo ? m_undo.GetLastUndoType() : m_undo.GetLastRedoType());
}

BOOL CToDoCtrlData::DeleteLastUndoAction()
{
	return m_undo.DeleteLastUndoAction();
}

BOOL CToDoCtrlData::UndoLastAction(BOOL bUndo, CArrayUndoElements& aElms)
{
	aElms.RemoveAll();
	TDCUNDOACTION* pAction = bUndo ? m_undo.UndoLastAction() : m_undo.RedoLastAction();

	if (!pAction)
	{
		return FALSE;
	}

	// restore elements
	int nNumElm = pAction->aElements.GetSize();

	// note: if we are undoing then we need to undo in the reverse order
	// of the initial edits unless it was a move because moves always
	// work off the first item.
	int nStart = 0, nEnd = nNumElm, nInc = 1;

	if (bUndo && pAction->nType != TDCUAT_MOVE)
	{
		nStart = nNumElm - 1;
		nEnd = -1;
		nInc = -1;
	}

	// copy the structure because we're going to be changing it and we need
	// to be able to lookup the original previous sibling IDs for undo info
	CToDoCtrlStructure tdsCopy(m_struct);

	// now undo
	for (int nElm = nStart; nElm != nEnd; nElm += nInc)
	{
		TDCUNDOELEMENT& elm = pAction->aElements.ElementAt(nElm);

		if (elm.nOp == TDCUEO_EDIT)
		{
			TODOITEM* pTDI = GetTask(elm.dwTaskID);
			ASSERT(pTDI);

			if (!pTDI)
			{
				return FALSE;
			}

			// copy current task state so we can update redo info
			TODOITEM tdiRedo = *pTDI;
			*pTDI = elm.tdi;
			elm.tdi = tdiRedo;

			// no need to return this item nothing to be done
		}
		else if ((elm.nOp == TDCUEO_ADD && bUndo) || (elm.nOp == TDCUEO_DELETE && !bUndo))
		{
			// this is effectively a delete so make the returned elem that way
			TDCUNDOELEMENT elmRet(TDCUEO_DELETE, elm.dwTaskID);
			aElms.Add(elmRet);

			DeleteTask(elm.dwTaskID);
		}
		else if ((elm.nOp == TDCUEO_DELETE && bUndo) || (elm.nOp == TDCUEO_ADD && !bUndo))
		{
			// this is effectively an add so make the returned elem that way
			TDCUNDOELEMENT elmRet(TDCUEO_ADD, elm.dwTaskID, elm.dwParentID, elm.dwPrevSiblingID);
			aElms.Add(elmRet);

			// restore task
			TODOITEM* pTDI = GetTask(elm.dwTaskID);
			ASSERT(pTDI == NULL);

			if (!pTDI)
			{
				pTDI = NewTask(&elm.tdi);
				AddTask(elm.dwTaskID, pTDI, elm.dwParentID, elm.dwPrevSiblingID);
			}
		}
		else if (elm.nOp == TDCUEO_MOVE)
		{
			TDCUNDOELEMENT elmRet(TDCUEO_MOVE, elm.dwTaskID, elm.dwParentID, elm.dwPrevSiblingID);
			aElms.Add(elmRet);

			MoveTask(elm.dwTaskID, elm.dwParentID, elm.dwPrevSiblingID);

			// adjust undo element so these changes can be undone
			elm.dwParentID = tdsCopy.GetParentTaskID(elm.dwTaskID);
			elm.dwPrevSiblingID = tdsCopy.GetPreviousTaskID(elm.dwTaskID);
		}
		else
		{
			return FALSE;
		}
	}

	ResetCachedCalculations();

	return TRUE;
}

BOOL CToDoCtrlData::CanUndoLastAction(BOOL bUndo) const
{
	return bUndo ? m_undo.CanUndo() : m_undo.CanRedo();
}

BOOL CToDoCtrlData::MoveTask(DWORD dwTaskID, DWORD dwDestParentID, DWORD dwDestPrevSiblingID)
{
	// get source location
	TODOSTRUCTURE* pTDSParent = NULL;
	int nPos = 0;

	if (!LocateTask(dwTaskID, pTDSParent, nPos))
	{
		ASSERT(0);
		return FALSE;
	}

	DWORD dwPrevSiblingID = pTDSParent->GetPreviousSubTaskID(nPos);

	// get destination
	TODOSTRUCTURE* pTDSDestParent = NULL;
	int nDestPos = -1;

	if (!Locate(dwDestParentID, dwDestPrevSiblingID, pTDSDestParent, nDestPos))
	{
		ASSERT(0);
		return FALSE;
	}
	// we want the location after the dest previous sibling
	else
	{
		nDestPos++;
	}

	return MoveTask(pTDSParent, nPos, dwPrevSiblingID, pTDSDestParent, nDestPos);
}

BOOL CToDoCtrlData::MoveTasks(const CDWordArray& aTaskIDs, DWORD dwDestParentID, DWORD dwDestPrevSiblingID)
{
	if (aTaskIDs.GetSize() == 0)
	{
		return FALSE;
	}

	else if (aTaskIDs.GetSize() == 1)
	{
		return MoveTask(aTaskIDs[0], dwDestParentID, dwDestPrevSiblingID);
	}

	// copy the structure because we're going to be changing it and we need
	// to be able to lookup the original previous sibling IDs for undo info
	CToDoCtrlStructure tdsCopy(m_struct);

	// get destination location
	TODOSTRUCTURE* pTDSDestParent = NULL;
	int nDestPos = -1;

	if (!Locate(dwDestParentID, dwDestPrevSiblingID, pTDSDestParent, nDestPos))
	{
		ASSERT(0);
		return FALSE;
	}
	// we want the location after the dest previous sibling
	else
	{
		nDestPos++;
	}

	// move source tasks
	for (int nTask = 0; nTask < aTaskIDs.GetSize(); nTask++, nDestPos++)
	{
		TODOSTRUCTURE* pTDSSrcParent = NULL;
		int nSrcPos = -1;
		DWORD dwTaskID = aTaskIDs[nTask];

		if (!LocateTask(dwTaskID, pTDSSrcParent, nSrcPos))
		{
			ASSERT(0);
			return FALSE;
		}

		// get previous subtask ID
		// for this we must use our copy of the task structure because
		// the original task structure may already have been altered
		TODOSTRUCTURE* pTDSDummy = NULL;
		int nDummyPos = -1;
		tdsCopy.FindTask(dwTaskID, pTDSDummy, nDummyPos);

		DWORD dwPrevSiblingID = pTDSDummy->GetPreviousSubTaskID(nDummyPos);

		nDestPos = MoveTask(pTDSSrcParent, nSrcPos, dwPrevSiblingID, pTDSDestParent, nDestPos);
	}

	return TRUE;
}

int CToDoCtrlData::MoveTask(TODOSTRUCTURE* pTDSSrcParent, int nSrcPos, DWORD dwSrcPrevSiblingID,
	TODOSTRUCTURE* pTDSDestParent, int nDestPos)
{
	DWORD dwTaskID = pTDSSrcParent->GetSubTaskID(nSrcPos);
	DWORD dwParentID = pTDSSrcParent->GetTaskID();

	// save undo info
	AddUndoElement(TDCUEO_MOVE, dwTaskID, dwParentID, dwSrcPrevSiblingID);

	return pTDSSrcParent->MoveSubTask(nSrcPos, pTDSDestParent, nDestPos);
}

BOOL CToDoCtrlData::IsParentTaskDone(DWORD dwTaskID) const
{
	const TODOSTRUCTURE* pTDS = LocateTask(dwTaskID);
	return IsParentTaskDone(pTDS);
}

BOOL CToDoCtrlData::IsParentTaskDone(const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS);

	if (!pTDS || pTDS->ParentIsRoot())
	{
		return FALSE;
	}

	TODOSTRUCTURE* pTDSParent = pTDS->GetParentTask();
	TODOITEM* pTDIParent = GetTask(pTDSParent);

	ASSERT(pTDIParent && pTDSParent);

	if (!pTDIParent || !pTDSParent)
	{
		return FALSE;
	}

	if (pTDIParent->IsDone())
	{
		return TRUE;
	}

	// else check parent's parent
	return IsParentTaskDone(pTDSParent);
}

int CToDoCtrlData::AreChildTasksDone(DWORD dwTaskID) const
{
	const TODOSTRUCTURE* pTDS = LocateTask(dwTaskID);
	return AreChildTasksDone(pTDS);
}

int CToDoCtrlData::AreChildTasksDone(const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS);

	if (!pTDS || !pTDS->HasSubTasks())
	{
		return -1;
	}

	// check children and their children recursively
	for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
	{
		const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
		const TODOITEM* pTDIChild = GetTask(pTDSChild);

		ASSERT(pTDSChild && pTDIChild);

		if (!pTDIChild || !pTDSChild)
		{
			return -1;
		}

		BOOL bDone = IsTaskDone(pTDIChild, pTDSChild, TDCCHECKCHILDREN);

		if (!bDone)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CToDoCtrlData::GetSubtaskTotals(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS,
	int& nSubtasksCount, int& nSubtasksDone) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDS->HasSubTasks() || !pTDI)
	{
		return FALSE;
	}

	// do we need to recalc?
	if (pTDI->AttribNeedsRecalc(TDIR_SUBTASKSCOUNT) ||
		pTDI->AttribNeedsRecalc(TDIR_SUBTASKSDONE))
	{
		nSubtasksDone = nSubtasksCount = 0;

		for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
		{
			nSubtasksCount++;

			const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
			const TODOITEM* pTDIChild = GetTask(pTDSChild);

			if (IsTaskDone(pTDIChild, pTDSChild, TDCCHECKCHILDREN))
			{
				nSubtasksDone++;
			}
		}

		pTDI->nSubtasksDone = nSubtasksDone;
		pTDI->SetAttribNeedsRecalc(TDIR_SUBTASKSDONE, FALSE);

		pTDI->nSubtasksCount = nSubtasksCount;
		pTDI->SetAttribNeedsRecalc(TDIR_SUBTASKSCOUNT, FALSE);
	}
	else // use cached values
	{
		nSubtasksDone = pTDI->nSubtasksDone;
		nSubtasksCount = pTDI->nSubtasksCount;
	}

	return TRUE;
}

int CToDoCtrlData::CalcPercentDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0;
	}

	// do we need to recalc?
	if (pTDI->AttribNeedsRecalc(TDIR_PERCENT))
	{
		int nPercent = 0;

		if (!HasStyle(TDCS_AVERAGEPERCENTSUBCOMPLETION) || !pTDS->HasSubTasks())
		{
			if (pTDI->IsDone())
			{
				nPercent = 100;
			}

			else if (HasStyle(TDCS_AUTOCALCPERCENTDONE))
			{
				nPercent = CalcPercentFromTime(pTDI, pTDS);
			}

			else
			{
				nPercent = pTDI->nPercentDone;
			}
		}
		else
		{
			// else
			double dTotalPercent = 0, dTotalWeighting = 0, dPercent = 0;

			SumPercentDone(pTDI, pTDS, dTotalPercent, dTotalWeighting);

			if (dTotalWeighting > 0)
			{
				dPercent = dTotalPercent / dTotalWeighting;
			}
			else
			{
				ASSERT(dTotalPercent == 0);   // sanity check
			}

			nPercent = (int)dPercent;
		}

		// update calc'ed value
		pTDI->nCalcPercent = nPercent;
		pTDI->SetAttribNeedsRecalc(TDIR_PERCENT, FALSE);
	}

	return pTDI->nCalcPercent;
}

int CToDoCtrlData::CalcPercentFromTime(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0;
	}

	ASSERT(HasStyle(TDCS_AUTOCALCPERCENTDONE));  // sanity check

	double dSpent = CalcTimeSpent(pTDI, pTDS, THU_HOURS);
	double dEstimate = CalcTimeEstimate(pTDI, pTDS, THU_HOURS);

	if (dSpent > 0 && dEstimate > 0)
	{
		return min(100, (int)(100 * dSpent / dEstimate));
	}
	else
	{
		return 0;
	}
}

void CToDoCtrlData::SumPercentDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS,
	double& dTotalPercent, double& dTotalWeighting) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return;
	}

	ASSERT(HasStyle(TDCS_AVERAGEPERCENTSUBCOMPLETION));  // sanity check

	if (!pTDS->HasSubTasks() || pTDI->IsDone())
	{
		double dWeighting = 1; // default
		double dPercent = 0;

		// base percent
		if (pTDI->IsDone())
		{
			dPercent = 100;
		}

		else if (HasStyle(TDCS_AUTOCALCPERCENTDONE))
		{
			dPercent = CalcPercentFromTime(pTDI, pTDS);
		}

		else
		{
			dPercent = pTDI->nPercentDone;
		}

		dTotalWeighting += dWeighting;
		dTotalPercent += dPercent;
	}
	else // aggregate child percentages
	{
		double dChildWeighting = 0;
		double dChildPercent = 0;

		for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
		{
			const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
			ASSERT(pTDSChild);

			TODOITEM* pTDIChild = GetTask(pTDSChild);

			if (!pTDIChild->IsDone() || HasStyle(TDCS_INCLUDEDONEINAVERAGECALC))
			{
				SumPercentDone(pTDIChild, pTDSChild, dChildPercent, dChildWeighting);
			}
		}

		if (HasStyle(TDCS_WEIGHTPERCENTCALCBYNUMSUB))
		{
			dTotalPercent += dChildPercent;
			dTotalWeighting += dChildWeighting;
		}
		else
		{
			dTotalPercent += (dChildPercent / dChildWeighting);
			dTotalWeighting += 1;
		}
	}
}

double CToDoCtrlData::CalcCost(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0.0;
	}

	// do we need to recalc?
	if (pTDI->AttribNeedsRecalc(TDIR_COST))
	{
		double dCost = pTDI->dCost; // parent's own cost

		if (pTDS->HasSubTasks())
		{
			for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
			{
				const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
				const TODOITEM* pTDIChild = GetTask(pTDSChild);

				dCost += CalcCost(pTDIChild, pTDSChild);
			}
		}

		// update calc'ed value
		pTDI->dCalcCost = dCost;
		pTDI->SetAttribNeedsRecalc(TDIR_COST, FALSE);
	}

	return pTDI->dCalcCost;
}

double CToDoCtrlData::CalcTimeEstimate(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nUnits) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0.0;
	}

	// do we need to recalc?
	if (pTDI->AttribNeedsRecalc(TDIR_TIMEESTIMATE))
	{
		double dTime = 0;
		BOOL bIsParent = pTDS->HasSubTasks();

		// task's own time
		if (!bIsParent || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			dTime = CTimeHelper().GetTime(pTDI->dTimeEstimate, pTDI->nTimeEstUnits, THU_HOURS);

			if (HasStyle(TDCS_USEPERCENTDONEINTIMEEST))
			{
				if (pTDI->IsDone())
				{
					dTime = 0;
				}
				else
				{
					dTime *= ((100 - pTDI->nPercentDone) / 100.0);   // estimating time left
				}
			}
		}

		if (bIsParent) // children's time
		{
			for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
			{
				const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
				const TODOITEM* pTDIChild = GetTask(pTDSChild);

				dTime += CalcTimeEstimate(pTDIChild, pTDSChild, THU_HOURS);
			}
		}

		// update calc'ed value
		pTDI->dCalcTimeEstimate = dTime;
		pTDI->SetAttribNeedsRecalc(TDIR_TIMEESTIMATE, FALSE);
	}

	// its in hours (always) so we need to convert it to nUnits
	return CTimeHelper().GetTime(pTDI->dCalcTimeEstimate, THU_HOURS, nUnits);
}

double CToDoCtrlData::CalcTimeSpent(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nUnits) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0.0;
	}

	// do we need to recalc?
	if (pTDI->AttribNeedsRecalc(TDIR_TIMESPENT))
	{
		double dTime = 0;
		BOOL bIsParent = pTDS->HasSubTasks();

		// task's own time
		if (!bIsParent || HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
		{
			dTime = CTimeHelper().GetTime(pTDI->dTimeSpent, pTDI->nTimeSpentUnits, THU_HOURS);
		}

		if (bIsParent) // children's time
		{
			for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
			{
				const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
				const TODOITEM* pTDIChild = GetTask(pTDSChild);

				dTime += CalcTimeSpent(pTDIChild, pTDSChild, THU_HOURS);
			}
		}

		// update calc'ed value
		pTDI->dCalcTimeSpent = dTime;
		pTDI->SetAttribNeedsRecalc(TDIR_TIMESPENT, FALSE);
	}

	// convert it back from hours to nUnits
	return CTimeHelper().GetTime(pTDI->dCalcTimeSpent, THU_HOURS, nUnits);
}

BOOL CToDoCtrlData::IsTaskDue(DWORD dwTaskID, BOOL bToday) const
{
	if (!dwTaskID)
	{
		return FALSE;
	}

	const TODOITEM* pTDI = GetTask(dwTaskID);
	ASSERT(pTDI);

	TODOSTRUCTURE* pTDS = LocateTask(dwTaskID);
	ASSERT(pTDS);

	if (!pTDS)
	{
		return 0;
	}

	return IsTaskDue(pTDI, pTDS, bToday);
}

BOOL CToDoCtrlData::IsTaskOverDue(DWORD dwTaskID) const
{
	return IsTaskDue(dwTaskID, FALSE);
}

BOOL CToDoCtrlData::IsTaskOverDue(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const
{
	return IsTaskDue(pTDI, pTDS, FALSE);
}

double CToDoCtrlData::GetEarliestDueDate() const
{
	double dEarliest = 1e307;

	// traverse top level items
	for (int nSubTask = 0; nSubTask < m_struct.GetSubTaskCount(); nSubTask++)
	{
		const TODOSTRUCTURE* pTDS = m_struct.GetSubTask(nSubTask);
		const TODOITEM* pTDI = GetTask(pTDS);

		double dTaskEarliest = GetEarliestDueDate(pTDI, pTDS, TRUE);

		if (dTaskEarliest > 0.0)
		{
			dEarliest = min(dTaskEarliest, dEarliest);
		}
	}

	// we need to reset the cached calculation in the case where
	// the user is *not* including child due dates in the parents'
	// due date because the call to GetEarliestDueDate with TRUE
	// will have overwritten it
	if (!HasStyle(TDCS_USEEARLIESTDUEDATE))
	{
		ResetCachedCalculations();
	}

	return (dEarliest == 1e307) ? 0.0 : dEarliest;
}

BOOL CToDoCtrlData::GetTask(DWORD dwTaskID, const TODOITEM*& pTDI, const TODOSTRUCTURE*& pTDS) const
{
	// get item and structure
	pTDI = GetTask(dwTaskID);
	pTDS = m_struct.FindTask(dwTaskID);

	// we only need to assert if one could be found but not the other
	ASSERT((pTDI && pTDS) || (!pTDI && !pTDS));

	if (!pTDI || !pTDI)
	{
		pTDI = NULL;
		pTDS = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CToDoCtrlData::GetTask(DWORD dwTaskID, TODOITEM*& pTDI, TODOSTRUCTURE*& pTDS)
{
	// get item and structure
	pTDI = GetTask(dwTaskID);
	pTDS = m_struct.FindTask(dwTaskID);

	// we only need to assert if one could be found but not the other
	ASSERT((pTDI && pTDS) || (!pTDI && !pTDS));

	if (!pTDI || !pTDI)
	{
		pTDI = NULL;
		pTDS = NULL;
		return FALSE;
	}

	return TRUE;
}

double CToDoCtrlData::GetEarliestDueDate(DWORD dwTaskID, BOOL bCheckChildren) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0.0;
	}

	return GetEarliestDueDate(pTDI, pTDS, bCheckChildren);
}

int CToDoCtrlData::GetHighestPriority(DWORD dwTaskID, BOOL bIncludeDue) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0;
	}

	return GetHighestPriority(pTDI, pTDS, bIncludeDue);
}

int CToDoCtrlData::GetHighestRisk(DWORD dwTaskID) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0;
	}

	return GetHighestRisk(pTDI, pTDS);
}

double CToDoCtrlData::CalcCost(DWORD dwTaskID) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0.0;
	}

	return CalcCost(pTDI, pTDS);
}

int CToDoCtrlData::CalcPercentDone(DWORD dwTaskID) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0;
	}

	return CalcPercentDone(pTDI, pTDS);
}

double CToDoCtrlData::CalcTimeEstimate(DWORD dwTaskID, int nUnits) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0.0;
	}

	return CalcTimeEstimate(pTDI, pTDS, nUnits);
}

double CToDoCtrlData::CalcTimeSpent(DWORD dwTaskID, int nUnits) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return 0.0;
	}

	return CalcTimeSpent(pTDI, pTDS, nUnits);
}

BOOL CToDoCtrlData::GetSubtaskTotals(DWORD dwTaskID, int& nSubtasksTotal, int& nSubtasksDone) const
{
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return FALSE;
	}

	return GetSubtaskTotals(pTDI, pTDS, nSubtasksTotal, nSubtasksDone);
}

BOOL CToDoCtrlData::IsTaskDone(DWORD dwTaskID, DWORD dwExtraCheck) const
{
	const TODOITEM* pTDI = GetTask(dwTaskID);
	ASSERT(!dwTaskID || pTDI);

	if (!pTDI)
	{
		return FALSE;
	}

	TODOSTRUCTURE* pTDS = LocateTask(dwTaskID);
	ASSERT(pTDS);

	if (!pTDS)
	{
		return FALSE;
	}

	BOOL bDone = IsTaskDone(pTDI, pTDS, dwExtraCheck);

	// update cached value
	if (dwExtraCheck == TDCCHECKALL)
	{
		pTDI->bGoodAsDone = bDone;
		pTDI->SetAttribNeedsRecalc(TDIR_GOODASDONE, FALSE);
	}

	return bDone;
}

BOOL CToDoCtrlData::IsTaskDue(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bToday) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return FALSE;
	}

	// some optimizations
	if (!pTDI->AttribNeedsRecalc(TDIR_DUE) && !bToday)
	{
		return pTDI->bDue;
	}

	double dDue = floor(GetEarliestDueDate(pTDI, pTDS, HasStyle(TDCS_USEEARLIESTDUEDATE)));
	BOOL bDue = FALSE;

	if (bToday)
	{
		double dToday = floor(COleDateTime::GetCurrentTime()); // 12 midnight
		bDue = (dDue >= dToday && dDue < dToday + 1);
	}
	else
	{
		bDue = (dDue > 0 && dDue < COleDateTime::GetCurrentTime());
	}

	// update calc'ed value
	if (!bToday)
	{
		pTDI->bDue = bDue;
		pTDI->SetAttribNeedsRecalc(TDIR_DUE, FALSE);
	}

	return bDue;
}

double CToDoCtrlData::GetEarliestDueDate(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bCheckChildren) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return 0.0;
	}

	// some optimizations
	if (!pTDI->AttribNeedsRecalc(TDIR_EARLIESTDUE))
	{
		return pTDI->dateEarliestDue;
	}

	double dEarliest = pTDI->dateDue;
	BOOL bDone = IsTaskDone(pTDI, pTDS, TDCCHECKCHILDREN);

	if (bDone)
	{
		dEarliest = 0;
	}

	else if (bCheckChildren && pTDS->HasSubTasks())
	{
		// check children
		dEarliest = pTDI->HasDue() ? pTDI->dateDue : DBL_MAX;

		for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
		{
			const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
			const TODOITEM* pTDIChild = GetTask(pTDSChild);

			ASSERT(pTDSChild && pTDIChild);

			double dChildDue = GetEarliestDueDate(pTDIChild, pTDSChild, TRUE);

			if (dChildDue > 0 && dChildDue < dEarliest)
			{
				dEarliest = dChildDue;
			}
		}

		if (dEarliest == DBL_MAX)
		{
			dEarliest = 0;
		}
	}

	// finally if no date set then use today
	if (dEarliest == 0 && !bDone && HasStyle(TDCS_NODUEDATEISDUETODAY))
	{
		dEarliest = CDateHelper::GetDateOnly(COleDateTime::GetCurrentTime());
	}

	// update calc'ed value
	pTDI->dateEarliestDue = dEarliest;
	pTDI->SetAttribNeedsRecalc(TDIR_EARLIESTDUE, FALSE);

	return pTDI->dateEarliestDue;
}

int CToDoCtrlData::GetHighestPriority(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bIncludeDue) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return -1;
	}

	// some optimizations
	// try pre-calculated value first
	if (bIncludeDue)
	{
		if (!pTDI->AttribNeedsRecalc(TDIR_PRIORITYINCDUE))
		{
			return pTDI->nCalcPriorityIncDue;
		}
	}
	else if (!pTDI->AttribNeedsRecalc(TDIR_PRIORITY))
	{
		return pTDI->nCalcPriority;
	}

	// else need to recalc
	int nHighest = pTDI->nPriority;

	if (pTDI->IsDone() && !HasStyle(TDCS_INCLUDEDONEINPRIORITYCALC) && HasStyle(TDCS_DONEHAVELOWESTPRIORITY))
	{
		nHighest = min(nHighest, MIN_TDPRIORITY);
	}

	else if (nHighest < MAX_TDPRIORITY)
	{
		if (bIncludeDue && HasStyle(TDCS_DUEHAVEHIGHESTPRIORITY) && IsTaskDue(pTDI, pTDS))
		{
			nHighest = MAX_TDPRIORITY;
		}

		else if (HasStyle(TDCS_USEHIGHESTPRIORITY) && pTDS->HasSubTasks())
		{
			// check children
			for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
			{
				const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
				const TODOITEM* pTDIChild = GetTask(pTDSChild);

				if (HasStyle(TDCS_INCLUDEDONEINPRIORITYCALC) || !IsTaskDone(pTDIChild, pTDSChild, TDCCHECKALL))
				{
					int nChildHighest = GetHighestPriority(pTDIChild, pTDSChild, bIncludeDue);

					// optimization
					if (nChildHighest == MAX_TDPRIORITY)
					{
						nHighest = MAX_TDPRIORITY;
						break;
					}
					else
					{
						nHighest = max(nChildHighest, nHighest);
					}
				}
			}
		}
	}

	// update calc'ed value
	if (bIncludeDue)
	{
		pTDI->nCalcPriorityIncDue = nHighest;
		pTDI->SetAttribNeedsRecalc(TDIR_PRIORITYINCDUE, FALSE);
	}
	else
	{
		pTDI->nCalcPriority = nHighest;
		pTDI->SetAttribNeedsRecalc(TDIR_PRIORITY, FALSE);
	}

	return nHighest;
}

int CToDoCtrlData::GetHighestRisk(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDI)
	{
		return -1;
	}

	// some optimizations
	// try pre-calculated value first
	if (!pTDI->AttribNeedsRecalc(TDIR_RISK))
	{
		return pTDI->nCalcRisk;
	}

	int nHighest = pTDI->nRisk;

	if (pTDI->IsDone() && !HasStyle(TDCS_INCLUDEDONEINRISKCALC) && HasStyle(TDCS_DONEHAVELOWESTRISK))
	{
		nHighest = min(nHighest, MIN_TDRISK);
	}

	else if (nHighest < MAX_TDRISK)
	{
		if (HasStyle(TDCS_USEHIGHESTRISK) && pTDS->HasSubTasks())
		{
			// check children
			nHighest = max(nHighest, FT_NORISK);

			for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
			{
				const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
				const TODOITEM* pTDIChild = GetTask(pTDSChild);

				if (HasStyle(TDCS_INCLUDEDONEINRISKCALC) || !IsTaskDone(pTDIChild, pTDSChild, TDCCHECKALL))
				{
					int nChildHighest = GetHighestRisk(pTDIChild, pTDSChild);

					// optimization
					if (nChildHighest == MAX_TDRISK)
					{
						nHighest = MAX_TDRISK;
						break;
					}
					else
					{
						nHighest = max(nChildHighest, nHighest);
					}
				}
			}
		}
	}

	// update calc'ed value
	pTDI->nCalcRisk = nHighest;
	pTDI->SetAttribNeedsRecalc(TDIR_RISK, FALSE);

	return pTDI->nCalcRisk;
}

TODOITEM* CToDoCtrlData::GetTask(const TODOSTRUCTURE* pTDS) const
{
	if (!pTDS || !pTDS->GetTaskID())
	{
		return NULL;
	}

	// else
	return GetTask(pTDS->GetTaskID());
}


BOOL CToDoCtrlData::IsTaskDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, DWORD dwExtraCheck) const
{
	ASSERT(pTDS && pTDI);

	if (!pTDS || !pTDS->GetTaskID() || !pTDI)
	{
		return FALSE;
	}

	// check cached value
	if (dwExtraCheck == TDCCHECKALL && !pTDI->AttribNeedsRecalc(TDIR_GOODASDONE))
	{
		return pTDI->bGoodAsDone;
	}

	if (pTDI->IsDone())
	{
		return TRUE;
	}

	BOOL bDone = FALSE;

	if (dwExtraCheck & TDCCHECKPARENT)
	{
		bDone = IsParentTaskDone(pTDS);
	}

	// else check children for 'good-as-done'
	if (!bDone)
	{
		BOOL bTreatSubCompleteDone = HasStyle(TDCS_TREATSUBCOMPLETEDASDONE);

		if ((dwExtraCheck & TDCCHECKCHILDREN) && bTreatSubCompleteDone)
		{
			if (AreChildTasksDone(pTDS) > 0)
			{
				bDone = TRUE;
			}
		}
	}

	// update cached value
	if (dwExtraCheck == TDCCHECKALL)
	{
		pTDI->bGoodAsDone = bDone;
		pTDI->SetAttribNeedsRecalc(TDIR_GOODASDONE, FALSE);
	}

	// else return as is
	return bDone;
}

BOOL CToDoCtrlData::IsTaskTimeTrackable(DWORD dwTaskID) const
{
	if (!dwTaskID)
	{
		return FALSE;
	}

	// not trackable if a container or complete
	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		return FALSE;
	}

	if (!HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
	{
		return (!pTDS->HasSubTasks());
	}

	return (!pTDI->IsDone());
}

int CALLBACK CToDoCtrlData::CompareFuncMulti(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TDSORTPARAMS* pSS = (TDSORTPARAMS*)lParamSort;
	ASSERT(pSS->sort.nBy1 != TDC_UNSORTED);

	int nCompare = pSS->pData->CompareTasks(lParam1, lParam2, pSS->sort.nBy1, pSS->sort.bAscending1, pSS->bSortDueTodayHigh);

	if (nCompare == 0 && pSS->sort.nBy2 != TDC_UNSORTED)
	{
		nCompare = pSS->pData->CompareTasks(lParam1, lParam2, pSS->sort.nBy2, pSS->sort.bAscending2, pSS->bSortDueTodayHigh);

		if (nCompare == 0 && pSS->sort.nBy3 != TDC_UNSORTED)
		{
			nCompare = pSS->pData->CompareTasks(lParam1, lParam2, pSS->sort.nBy3, pSS->sort.bAscending3, pSS->bSortDueTodayHigh);
		}
	}

	return nCompare;
}

int CALLBACK CToDoCtrlData::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TDSORTPARAMS* pSS = (TDSORTPARAMS*)lParamSort;

	// 'unsorted' is a special case
	if (pSS->sort.nBy1 == TDC_UNSORTED)
	{
		TODOSTRUCTURE* pTDSParent = NULL;
		int nPos1, nPos2;

		VERIFY(pSS->pData->m_struct.FindTask(lParam1, pTDSParent, nPos1));
		nPos2 = pTDSParent->GetSubTaskPosition(lParam2);
		ASSERT(nPos2 != -1); // must be a sibling

		return pSS->pData->Compare(nPos1, nPos2);
	}

	return pSS->pData->CompareTasks(lParam1, lParam2, pSS->sort.nBy1, pSS->sort.bAscending1, pSS->bSortDueTodayHigh);
}

int CToDoCtrlData::CompareTasks(DWORD dwTask1ID, DWORD dwTask2ID, TDC_SORTBY nSortBy, BOOL bAscending, BOOL bSortDueTodayHigh)
{
	// special case: sort by ID can be optimized
	if (nSortBy == TDC_SORTBYID)
	{
		return (bAscending ? (dwTask1ID - dwTask2ID) : (dwTask2ID - dwTask1ID));
	}

	int nCompare = 0;

	// figure out if either or both tasks are completed
	// but only if the user has specified to sort these differently
	BOOL bHideDone = HasStyle(TDCS_HIDESTARTDUEFORDONETASKS);
	BOOL bSortDoneBelow = HasStyle(TDCS_SORTDONETASKSATBOTTOM);
	BOOL bDone1 = FALSE, bDone2 = FALSE;

	if (bSortDoneBelow ||
		nSortBy == TDC_SORTBYDONE || nSortBy == TDC_SORTBYDONEDATE ||
		(bHideDone && (nSortBy == TDC_SORTBYSTARTDATE || nSortBy == TDC_SORTBYDUEDATE)))
	{
		bDone1 = IsTaskDone(dwTask1ID, TDCCHECKALL);
		bDone2 = IsTaskDone(dwTask2ID, TDCCHECKALL);

		// can also do a partial optimization
		if (bSortDoneBelow && (nSortBy != TDC_SORTBYDONE && nSortBy != TDC_SORTBYDONEDATE))
		{
			if (bDone1 != bDone2)
			{
				return bDone1 ? 1 : -1;
			}
		}
	}

	TODOITEM* pTDI1 = NULL, *pTDI2 = NULL;
	TODOSTRUCTURE* pTDS1 = NULL, *pTDS2 = NULL;

	// if sorting by icon then we need to be able to pick parent tasks
	if (nSortBy == TDC_SORTBYICON)
	{
		GetTask(dwTask1ID, pTDI1, pTDS1);
		GetTask(dwTask2ID, pTDI2, pTDS2);
	}
	else
	{
		pTDI1 = GetTask(dwTask1ID);
		pTDI2 = GetTask(dwTask2ID);
	}

	switch (nSortBy)
	{

	case TDC_SORTBYNAME:
		nCompare = Compare(pTDI1->sTitle, pTDI2->sTitle);
		break;

	case TDC_SORTBYDONE:
		nCompare = Compare(bDone1, bDone2);
		break;

	case TDC_SORTBYFLAG:
		nCompare = Compare(pTDI1->bFlagged, pTDI2->bFlagged);
		break;

	case TDC_SORTBYRECURRENCE:
		nCompare = Compare(pTDI1->trRecurrence.nRegularity, pTDI2->trRecurrence.nRegularity);
		break;

	case TDC_SORTBYVERSION:
		nCompare = Misc::CompareVersions(pTDI1->sVersion, pTDI2->sVersion);
		break;

	case TDC_SORTBYCREATIONDATE:
		nCompare = Compare(pTDI1->dateCreated, pTDI2->dateCreated);
		break;

	case TDC_SORTBYLASTMOD:
		{
			BOOL bHasModify1 = pTDI1->HasLastMod() && !(bHideDone && bDone1);
			BOOL bHasModify2 = pTDI2->HasLastMod() && !(bHideDone && bDone2);

			if (bHasModify1 != bHasModify2)
			{
				return bHasModify1 ? -1 : 1;
			}

			else if (!bHasModify1) //  and !bHasStart2
			{
				return 0;
			}

			nCompare = Compare(pTDI1->tLastMod, pTDI2->tLastMod);
		}
		break;


	case TDC_SORTBYDONEDATE:
		{
			COleDateTime date1 = pTDI1->dateDone; // default
			COleDateTime date2 = pTDI2->dateDone; // default

			// sort tasks 'good as done' between done and not-done
			if (date1 <= 0 && bDone1)
			{
				date1 = 0.1;
			}

			if (date2 <= 0 && bDone2)
			{
				date2 = 0.1;
			}

			nCompare = Compare(date1, date2);
		}
		break;

	case TDC_SORTBYDUEDATE:
	case TDC_SORTBYREMAINING: // mostly the same
		{
			COleDateTime date1, date2;

			BOOL bUseEarliestDueDate = HasStyle(TDCS_USEEARLIESTDUEDATE);

			if (!bHideDone || !bDone1)
			{
				date1 = GetEarliestDueDate(dwTask1ID, bUseEarliestDueDate);
			}

			if (!bHideDone || !bDone2)
			{
				date2 = GetEarliestDueDate(dwTask2ID, bUseEarliestDueDate);
			}

			// Sort undated options below others
			BOOL bHasDue1 = (date1.m_dt > 0) ? 1 : 0;
			BOOL bHasDue2 = (date2.m_dt > 0) ? 1 : 0;

			if (bHasDue1 != bHasDue2)
			{
				return bHasDue1 ? -1 : 1;
			}

			else if (!bHasDue1 && !bHasDue2)
			{
				return 0;
			}

			// compare
			if (nSortBy == TDC_SORTBYREMAINING)
			{
				// calc remaining time
				COleDateTime dtCur = COleDateTime::GetCurrentTime();

				double dRemain1 = date1 - dtCur;
				double dRemain2 = date2 - dtCur;

				// compare
				nCompare = Compare(dRemain1, dRemain2);
			}
			else
			{
				nCompare = Compare(date1, date2);
			}
		}
		break;

	case TDC_SORTBYSTARTDATE:
		{
			BOOL bHasStart1 = pTDI1->HasStart() && !(bHideDone && bDone1);
			BOOL bHasStart2 = pTDI2->HasStart() && !(bHideDone && bDone2);

			if (bHasStart1 != bHasStart2)
			{
				return bHasStart1 ? -1 : 1;
			}

			else if (!bHasStart1 && !bHasStart2)
			{
				return 0;
			}

			nCompare = Compare(pTDI1->dateStart, pTDI2->dateStart);
		}
		break;

	case TDC_SORTBYPRIORITY:
		{
			// done items have even less than zero priority!
			// and due items have greater than the highest priority
			int nPriority1 = pTDI1->nPriority; // default
			int nPriority2 = pTDI2->nPriority; // default

			BOOL bUseHighestPriority = HasStyle(TDCS_USEHIGHESTPRIORITY);

			// item1
			if (bDone1)
			{
				nPriority1 = -1;
			}
			else if (IsTaskDue(dwTask1ID) && HasStyle(TDCS_DUEHAVEHIGHESTPRIORITY) &&
				(bSortDueTodayHigh || !IsTaskDue(dwTask1ID, TRUE)))
			{
				nPriority1 = pTDI1->nPriority + 11;
			}
			else if (bUseHighestPriority)
			{
				nPriority1 = GetHighestPriority(dwTask1ID);
			}

			// item2
			if (bDone2)
			{
				nPriority2 = -1;
			}
			else if (IsTaskDue(dwTask2ID) && HasStyle(TDCS_DUEHAVEHIGHESTPRIORITY) &&
				(bSortDueTodayHigh || !IsTaskDue(dwTask2ID, TRUE)))
			{
				nPriority2 = pTDI2->nPriority + 11;
			}
			else if (bUseHighestPriority)
			{
				nPriority2 = GetHighestPriority(dwTask2ID);
			}

			nCompare = Compare(nPriority1, nPriority2);
		}
		break;

	case TDC_SORTBYRISK:
		{
			// done items have even less than zero priority!
			// and due items have greater than the highest priority
			int nRisk1 = pTDI1->nRisk; // default
			int nRisk2 = pTDI2->nRisk; // default

			BOOL bUseHighestRisk = HasStyle(TDCS_USEHIGHESTRISK);

			// item1
			if (bDone1)
			{
				nRisk1 = -1;
			}
			else if (bUseHighestRisk)
			{
				nRisk1 = GetHighestRisk(dwTask1ID);
			}

			// item2
			if (bDone2)
			{
				nRisk2 = -1;
			}
			else if (bUseHighestRisk)
			{
				nRisk2 = GetHighestRisk(dwTask2ID);
			}

			nCompare = Compare(nRisk1, nRisk2);
		}
		break;

	case TDC_SORTBYCOLOR:
		nCompare = Compare((int)pTDI1->color, (int)pTDI2->color);
		break;

	case TDC_SORTBYALLOCTO:
		nCompare = Compare(Misc::FormatArray(pTDI1->aAllocTo),
			Misc::FormatArray(pTDI2->aAllocTo), TRUE);
		break;

	case TDC_SORTBYALLOCBY:
		nCompare = Compare(pTDI1->sAllocBy, pTDI2->sAllocBy, TRUE);
		break;

	case TDC_SORTBYCREATEDBY:
		nCompare = Compare(pTDI1->sCreatedBy, pTDI2->sCreatedBy, TRUE);
		break;

	case TDC_SORTBYSTATUS:
		nCompare = Compare(pTDI1->sStatus, pTDI2->sStatus, TRUE);
		break;

	case TDC_SORTBYEXTERNALID:
		nCompare = Compare(pTDI1->sExternalID, pTDI2->sExternalID, TRUE);
		break;

	case TDC_SORTBYCATEGORY:
		nCompare = Compare(Misc::FormatArray(pTDI1->aCategories),
			Misc::FormatArray(pTDI2->aCategories), TRUE);
		break;

	case TDC_SORTBYPERCENT:
		{
			int nPercent1 = CalcPercentDone(dwTask1ID);
			int nPercent2 = CalcPercentDone(dwTask2ID);

			nCompare = Compare(nPercent1, nPercent2);
		}
		break;

	case TDC_SORTBYICON:
		{
			int nIcon1 = pTDI1->nIconIndex;

			ASSERT(pTDS1);
			ASSERT(pTDS2);

			if (nIcon1 == -1 && pTDS1->HasSubTasks())
			{
				nIcon1 = 0;   // parent icon
			}

			int nIcon2 = pTDI2->nIconIndex;

			if (nIcon2 == -1 && pTDS2->HasSubTasks())
			{
				nIcon2 = 0;   // parent icon
			}

			nCompare = Compare(nIcon1, nIcon2);
		}
		break;

	case TDC_SORTBYCOST:
		{
			double dCost1 = CalcCost(dwTask1ID);
			double dCost2 = CalcCost(dwTask2ID);

			nCompare = Compare(dCost1, dCost2);
		}
		break;

	case TDC_SORTBYTIMEEST:
		{
			double dTime1 = CalcTimeEstimate(dwTask1ID, TDITU_HOURS);
			double dTime2 = CalcTimeEstimate(dwTask2ID, TDITU_HOURS);

			nCompare = Compare(dTime1, dTime2);
		}
		break;

	case TDC_SORTBYTIMESPENT:
		{
			double dTime1 = CalcTimeSpent(dwTask1ID, TDITU_HOURS);
			double dTime2 = CalcTimeSpent(dwTask2ID, TDITU_HOURS);

			nCompare = Compare(dTime1, dTime2);
		}
		break;

	case TDC_SORTBYRECENTEDIT:
		{
			BOOL bRecent1 = pTDI1->IsRecentlyEdited();
			BOOL bRecent2 = pTDI2->IsRecentlyEdited();

			nCompare = Compare(bRecent1, bRecent2);
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	return bAscending ? nCompare : -nCompare;
}

int CToDoCtrlData::Compare(const COleDateTime& date1, const COleDateTime& date2)
{
	return (date1 < date2) ? -1 : (date1 > date2) ? 1 : 0;
}

int CToDoCtrlData::Compare(const CString& sText1, const CString& sText2, BOOL bCheckEmpty)
{
	if (bCheckEmpty)
	{
		BOOL bEmpty1 = sText1.IsEmpty();
		BOOL bEmpty2 = sText2.IsEmpty();

		if (bEmpty1 != bEmpty2)
		{
			return bEmpty1 ? 1 : -1;
		}
	}

	return sText1.CompareNoCase(sText2);
}

int CToDoCtrlData::Compare(int nNum1, int nNum2)
{
	return (nNum1 - nNum2);
}

int CToDoCtrlData::Compare(double dNum1, double dNum2)
{
	return (dNum1 < dNum2) ? -1 : (dNum1 > dNum2) ? 1 : 0;
}

int CToDoCtrlData::FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const
{
	if (!GetTaskCount())
	{
		return 0;
	}

	for (int nSubTask = 0; nSubTask < m_struct.GetSubTaskCount(); nSubTask++)
	{
		const TODOSTRUCTURE* pTDS = m_struct.GetSubTask(nSubTask);
		const TODOITEM* pTDI = GetTask(pTDS);
		ASSERT(pTDI);

		FindTasks(pTDI, pTDS, params, aResults);
	}

	return aResults.GetSize();
}

int CToDoCtrlData::FindTasks(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, const SEARCHPARAMS& params, CResultArray& aResults) const
{
	SEARCHRESULT result;
	int nResults = aResults.GetSize();

	// if the item is done and we're ignoring completed tasks
	// (and by corollary their children) then we can stop right-away
	if (params.bIgnoreDone && IsTaskDone(pTDI, pTDS, TDCCHECKALL))
	{
		return 0;
	}

	if (TaskMatches(pTDI, pTDS, params, result))
	{
		aResults.Add(result);
	}

	// process children
	for (int nSubTask = 0; nSubTask < pTDS->GetSubTaskCount(); nSubTask++)
	{
		const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubTask);
		const TODOITEM* pTDIChild = GetTask(pTDSChild);
		ASSERT(pTDIChild);

		FindTasks(pTDIChild, pTDSChild, params, aResults);
	}

	return (aResults.GetSize() - nResults);
}

BOOL CToDoCtrlData::TaskMatches(DWORD dwTaskID, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	if (!dwTaskID)
	{
		return FALSE;
	}

	const TODOITEM* pTDI = NULL;
	const TODOSTRUCTURE* pTDS = NULL;

	if (!GetTask(dwTaskID, pTDI, pTDS))
	{
		ASSERT(0);
		return FALSE;
	}

	return TaskMatches(pTDI, pTDS, params, result);
}

BOOL CToDoCtrlData::TaskMatches(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS,
	const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	ASSERT(pTDI && pTDS);

	if (!pTDI || !pTDS)
	{
		return 0;
	}

	BOOL bIsDone = IsTaskDone(pTDI, pTDS, TDCCHECKALL);

	// if the item is done and we're ignoring completed tasks
	// (and by corollary their children) then we can stop right-away
	if (params.bIgnoreDone && bIsDone)
	{
		return 0;
	}

	BOOL bMatches = TRUE;

	int nNumRules = params.rules.GetSize();

	for (int nRule = 0; nRule < nNumRules; nRule++)
	{
		SEARCHRESULT resTask;
		const SEARCHPARAM& sp = params.rules[nRule];
		BOOL bMatch = FALSE, bLastRule = (nRule == nNumRules - 1);

		switch (sp.attrib)
		{
		case TDCA_TASKNAME:
			bMatch = TaskMatches(pTDI->sTitle, sp, resTask);
			break;

		case TDCA_TASKNAMEORCOMMENTS:
			bMatch = (TaskMatches(pTDI->sTitle, sp, resTask) ||
				TaskMatches(pTDI->sComments, sp, resTask));
			break;

		case TDCA_COMMENTS:
			bMatch = TaskMatches(pTDI->sComments, sp, resTask);
			break;

		case TDCA_ALLOCTO:
			bMatch = TaskMatches(pTDI->aAllocTo, sp, resTask);
			break;

		case TDCA_ALLOCBY:
			// because the search string can contain multiple
			// comma delimited items we must treat it differently
			if (pTDI->sAllocBy.IsEmpty() && sp.op == FO_NOT_SET)
			{
				bMatch = TRUE;
			}
			else
			{
				CStringArray aItems;
				aItems.Add(pTDI->sAllocBy);

				bMatch = TaskMatches(aItems, sp, resTask);
			}
			break;

		case TDCA_CREATEDBY:
			bMatch = TaskMatches(pTDI->sCreatedBy, sp, resTask);
			break;

		case TDCA_STATUS:
			// because the search string can contain multiple
			// comma delimited items we must treat it differently
			if (pTDI->sStatus.IsEmpty() && sp.op == FO_NOT_SET)
			{
				bMatch = TRUE;
			}
			else
			{
				CStringArray aItems;
				aItems.Add(pTDI->sStatus);

				bMatch = TaskMatches(aItems, sp, resTask);
			}
			break;

		case TDCA_CATEGORY:
			bMatch = TaskMatches(pTDI->aCategories, sp, resTask);
			break;

		case TDCA_EXTERNALID:
			bMatch = TaskMatches(pTDI->sExternalID, sp, resTask);
			break;

		case TDCA_CREATIONDATE:
			bMatch = TaskMatches(pTDI->dateCreated, sp, resTask);
			break;

		case TDCA_STARTDATE:
			bMatch = TaskMatches(pTDI->dateStart, sp, resTask);
			break;

		case TDCA_DUEDATE:
			{
				COleDateTime dtDue = GetEarliestDueDate(pTDI, pTDS, HasStyle(TDCS_USEEARLIESTDUEDATE));
				bMatch = TaskMatches(dtDue, sp, resTask);

				// handle overdue tasks
				if (bMatch && params.bIgnoreOverDue && IsTaskOverDue(pTDI, pTDS))
				{
					bMatch = FALSE;
				}
			}
			break;

		case TDCA_DONEDATE:
			// there's a special case here where if the parent
			// is completed then the task is also treated as completed
			if (sp.op == FO_SET && bIsDone)
			{
				bMatch = TRUE;
			}
			else if (sp.op == FO_NOT_SET && bIsDone)
			{
				bMatch = FALSE;
			}
			else
			{
				bMatch = TaskMatches(pTDI->dateDone, sp, resTask);
			}
			break;

		case TDCA_LASTMOD:
			bMatch = TaskMatches(pTDI->tLastMod, sp, resTask);
			break;

		case TDCA_PRIORITY:
			{
				int nPriority = GetHighestPriority(pTDI, pTDS);
				bMatch = TaskMatches(nPriority, sp, resTask);
			}
			break;

		case TDCA_RISK:
			{
				int nRisk = GetHighestRisk(pTDI, pTDS);
				bMatch = TaskMatches(nRisk, sp, resTask);
			}
			break;

		case TDCA_ID:
			bMatch = TaskMatches((int)pTDS->GetTaskID(), sp, resTask);
			break;

		case TDCA_PERCENT:
			{
				int nPercent = CalcPercentDone(pTDI, pTDS);
				bMatch = TaskMatches(nPercent, sp, resTask);
			}
			break;

		case TDCA_TIMEEST:
			{
				double dTime = CalcTimeEstimate(pTDI, pTDS, THU_HOURS);
				bMatch = TaskMatches(dTime, sp, resTask);
			}
			break;

		case TDCA_TIMESPENT:
			{
				double dTime = CalcTimeSpent(pTDI, pTDS, THU_HOURS);
				bMatch = TaskMatches(dTime, sp, resTask);
			}
			break;

		case TDCA_COST:
			{
				double dCost = CalcCost(pTDI, pTDS);
				bMatch = TaskMatches(dCost, sp, resTask);
			}
			break;

		case TDCA_FLAG:
			{
				bMatch = (sp.op == FO_SET ? pTDI->bFlagged : !pTDI->bFlagged);

				if (bMatch)
				{
					resTask.aMatched.Add(CEnString(sp.op == FO_SET ? IDS_FLAGGED : IDS_UNFLAGGED));
				}
			}
			break;

		case TDCA_VERSION:
			// because the search string can contain multiple
			// comma delimited items we must treat it differently
			if (pTDI->sVersion.IsEmpty() && sp.op == FO_NOT_SET)
			{
				bMatch = TRUE;
			}
			else
			{
				CStringArray aItems;
				aItems.Add(pTDI->sVersion);

				bMatch = TaskMatches(aItems, sp, resTask);
			}
			break;

		case TDCA_ANYTEXTATTRIBUTE:
			bMatch = (TaskMatches(pTDI->sTitle, sp, resTask) ||
				TaskMatches(pTDI->sComments, sp, resTask) ||
				TaskMatches(pTDI->aAllocTo, sp, resTask) ||
				TaskMatches(pTDI->sAllocBy, sp, resTask) ||
				TaskMatches(pTDI->aCategories, sp, resTask) ||
				TaskMatches(pTDI->sStatus, sp, resTask) ||
				TaskMatches(pTDI->sVersion, sp, resTask) ||
				TaskMatches(pTDI->sExternalID, sp, resTask) ||
				TaskMatches(pTDI->sCreatedBy, sp, resTask));
			break;
		}

		// save off result
		if (bMatch)
		{
			result.aMatched.Append(resTask.aMatched);
		}

		// handle this result
		bMatches &= bMatch;

		// are we at the end of this group?
		if (sp.bAnd == FALSE || bLastRule) // == 'OR' or end of rules
		{
			// if the group result is a match then we're done because
			// whatever may come after this is 'ORed' and so cannot change
			// the result
			if (bMatches || bLastRule)
			{
				break;
			}

			else // we're not at the end so we reset bMatches and keep going
			{
				bMatches = TRUE;
			}
		}
	}

	if (bMatches)
	{
		if (bIsDone)
		{
			result.dwFlags |= RF_DONE;
		}

		if (pTDS->HasSubTasks())
		{
			result.dwFlags |= RF_PARENT;
		}

		result.dwID = pTDS->GetTaskID();
	}

	return bMatches;
}

BOOL CToDoCtrlData::TaskMatches(const COleDateTime& date, const SEARCHPARAM& sp, SEARCHRESULT& result) const
{
	double dDate = floor(date.m_dt), dSearch = floor(sp.dValue);
	BOOL bMatch = FALSE;

	switch (sp.op)
	{
	case FO_EQUALS:
		bMatch = (dDate != 0.0) && (dDate == dSearch);
		break;

	case FO_NOT_EQUALS:
		bMatch = (dDate != 0.0) && (dDate != dSearch);
		break;

	case FO_ON_OR_AFTER:
		bMatch = (dDate != 0.0) && (dDate >= dSearch);
		break;

	case FO_AFTER:
		bMatch = (dDate != 0.0) && (dDate > dSearch);
		break;

	case FO_ON_OR_BEFORE:
		bMatch = (dDate != 0.0) && (dDate <= dSearch);
		break;

	case FO_BEFORE:
		bMatch = (dDate != 0.0) && (dDate < dSearch);
		break;

	case FO_SET:
		bMatch = (dDate > 0.0);
		break;

	case FO_NOT_SET:
		bMatch = (dDate == 0.0);
		break;
	}

	if (bMatch)
	{
		DWORD dwFmt = 0;

		if (HasStyle(TDCS_SHOWWEEKDAYINDATES))
		{
			dwFmt |= DHFD_DOW;
		}

		if (HasStyle(TDCS_SHOWDATESINISO))
		{
			dwFmt |= DHFD_ISO;
		}

		if (sp.op != FO_NOT_SET)
		{
			CString sMatch = CDateHelper::FormatDate(date, dwFmt);
			result.aMatched.Add(sMatch);
		}
	}

	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(const CString& sText, const SEARCHPARAM& sp, SEARCHRESULT& result) const
{
	BOOL bMatch = FALSE;

	switch (sp.op)
	{
	case FO_EQUALS:
		bMatch = (sText.CompareNoCase(sp.sValue) == 0);
		break;

	case FO_NOT_EQUALS:
		bMatch = (sText.CompareNoCase(sp.sValue) != 0);
		break;

	case FO_INCLUDES:
	case FO_NOT_INCLUDES:
		{
			CStringArray aWords;

			if (!Misc::ParseSearchString(sp.sValue, aWords))
			{
				return FALSE;
			}

			// cycle all the words
			for (int nWord = 0; nWord < aWords.GetSize() && !bMatch; nWord++)
			{
				CString sWord = aWords.GetAt(nWord);
				bMatch = Misc::FindWord(sWord, sText, FALSE, FALSE);

				// handle !=
				if (sp.op == FO_NOT_INCLUDES)
				{
					bMatch = !bMatch;
				}
			}
		}
		break;

	case FO_SET:
		bMatch = !sText.IsEmpty();
		break;

	case FO_NOT_SET:
		bMatch = sText.IsEmpty();
		break;
	}

	if (bMatch)
	{
		result.aMatched.Add(sText);
	}

	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(const CStringArray& aItems, const SEARCHPARAM& sp, SEARCHRESULT& result) const
{
	// special cases
	if (sp.op == FO_SET && aItems.GetSize())
	{
		int nItem = aItems.GetSize();

		while (nItem--)
		{
			result.aMatched.Add(aItems[nItem]);
		}

		return TRUE;
	}
	else if (sp.op == FO_NOT_SET && !aItems.GetSize())
	{
		return TRUE;
	}

	// general case
	BOOL bMatch = FALSE;
	BOOL bMatchAll = (sp.op == FO_EQUALS || sp.op == FO_NOT_EQUALS);

	CStringArray aSearchItems;
	Misc::ParseIntoArray(sp.sValue, aSearchItems, TRUE);

	if (bMatchAll)
	{
		bMatch = Misc::ArraysMatch(aItems, aSearchItems);
	}
	else
	{
		if (aItems.GetSize())
		{
			bMatch = Misc::MatchAny(aItems, aSearchItems);
		}
		else
			// special case: task has no item and param.aItems
			// contains an empty item
		{
			bMatch = (Misc::Find(aSearchItems, _T("")) != -1);
		}
	}

	// handle !=
	if (sp.op == FO_NOT_EQUALS || sp.op == FO_NOT_INCLUDES)
	{
		bMatch = !bMatch;
	}

	if (bMatch)
	{
		result.aMatched.Add(Misc::FormatArray(aItems));
	}

	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(double dValue, const SEARCHPARAM& sp, SEARCHRESULT& result) const
{
	BOOL bMatch = FALSE;
	BOOL bTime = (sp.attrib == TDCA_TIMEEST || sp.attrib == TDCA_TIMESPENT);
	double dSearchVal = bTime ? CTimeHelper().GetTime(sp.dValue, sp.dwFlags, THU_HOURS) : dValue;

	switch (sp.op)
	{
	case FO_EQUALS:
		bMatch = (dValue == dSearchVal);
		break;

	case FO_NOT_EQUALS:
		bMatch = (dValue != dSearchVal);
		break;

	case FO_GREATER_OR_EQUAL:
		bMatch = (dValue >= dSearchVal);
		break;

	case FO_GREATER:
		bMatch = (dValue > dSearchVal);
		break;

	case FO_LESS_OR_EQUAL:
		bMatch = (dValue <= dSearchVal);
		break;

	case FO_LESS:
		bMatch = (dValue < dSearchVal);
		break;

	case FO_SET:
		bMatch = (dValue > 0.0);
		break;

	case FO_NOT_SET:
		bMatch = (dValue == 0.0);
		break;
	}

	if (bMatch)
	{
		CString sMatch;

		if (bTime)
		{
			sMatch.Format(_T("%.3f H"), dValue);
		}
		else
		{
			sMatch.Format(_T("%.3f"), dValue);
		}

		result.aMatched.Add(sMatch);
	}

	return bMatch;
}

BOOL CToDoCtrlData::TaskMatches(int nValue, const SEARCHPARAM& sp, SEARCHRESULT& result/*, BOOL bPriorityRisk*/) const
{
	BOOL bMatch = FALSE;
	BOOL bPriorityRisk = (sp.attrib == TDCA_PRIORITY || sp.attrib == TDCA_RISK);

	switch (sp.op)
	{
	case FO_EQUALS:
		bMatch = (nValue == sp.nValue);
		break;

	case FO_NOT_EQUALS:
		bMatch = (nValue != sp.nValue);
		break;

	case FO_GREATER_OR_EQUAL:
		bMatch = (nValue >= sp.nValue);
		break;

	case FO_GREATER:
		bMatch = (nValue > sp.nValue);
		break;

	case FO_LESS_OR_EQUAL:
		bMatch = (nValue <= sp.nValue);
		break;

	case FO_LESS:
		bMatch = (nValue < sp.nValue);
		break;

	case FO_SET:
		if (bPriorityRisk)
		{
			bMatch = (nValue != FT_NOPRIORITY);
		}
		else
		{
			bMatch = (nValue > 0);
		}
		break;

	case FO_NOT_SET:
		if (bPriorityRisk)
		{
			bMatch = (nValue == FT_NOPRIORITY);
		}
		else
		{
			bMatch = (nValue == 0);
		}
		break;
	}

	if (bMatch)
	{
		// we don't add a result for not set priority/risk because
		// this would appear as -2
		if (sp.op == FO_NOT_SET && bPriorityRisk)
		{
			CEnString sMatch(IDS_TDC_NONE);
			result.aMatched.Add(sMatch);
		}
		else
		{
			CEnString sMatch(_T("%d"), nValue);
			result.aMatched.Add(sMatch);
		}
	}

	return bMatch;
}
