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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ToDoCtrlFind.cpp: implementation of the CToDoCtrlFind class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoCtrlFind.h"
#include "ToDoCtrlData.h"
#include "ToDoItem.h"

#include "../../../CodeProject/Source/TimeHelper.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/TreeCtrlHelper.h"

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

CToDoCtrlFind::CToDoCtrlFind(CTreeCtrl& tree, const CToDoCtrlData& data) :
m_tree(tree),
m_data(data)
{
}

CToDoCtrlFind::~CToDoCtrlFind()
{
}

HTREEITEM CToDoCtrlFind::GetItem(DWORD dwID) const
{
	if (dwID == 0)
	{
		return NULL;
	}

	return CTreeCtrlHelper(m_tree).FindItem(dwID, NULL);
}

DWORD CToDoCtrlFind::GetTaskID(HTREEITEM hti) const
{
	if (!hti || hti == TVI_ROOT || hti == TVI_FIRST || hti == TVI_LAST)
	{
		return 0;
	}

	return m_tree.GetItemData(hti);
}

TODOITEM* CToDoCtrlFind::GetTask(HTREEITEM hti) const
{
	return m_data.GetTask(GetTaskID(hti));
}

CString CToDoCtrlFind::GetLongestVisibleRecurrence(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);

	if (!pTDI)
	{
		return _T("");
	}

	return GetLongestVisibleRecurrence(hti, pTDI);
}

CString CToDoCtrlFind::GetLongestVisibleExternalID(HTREEITEM hti) const
{
	TODOITEM* pTDI = GetTask(hti);

	if (!pTDI)
	{
		return _T("");
	}

	return GetLongestVisibleExternalID(hti, pTDI);
}

CString CToDoCtrlFind::GetLongestVisibleAllocTo() const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleAllocTo(hti);

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleAllocTo(HTREEITEM hti) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);

	if (!pTDI)
	{
		return _T("");
	}

	return GetLongestVisibleAllocTo(hti, pTDI);
}

CString CToDoCtrlFind::GetLongestVisibleAllocTo(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = Misc::FormatArray(pTDI->aAllocTo);

	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestVisibleAllocTo(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
			{
				sLongest = sChildLongest;
			}

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleCategory() const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleCategory(hti);

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleTimeEstimate(int nDefUnits) const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleTime(hti, nDefUnits, TRUE); // TRUE == Estimate

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleTimeSpent(int nDefUnits) const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleTime(hti, nDefUnits, FALSE); // TRUE == Spent

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleRecurrence() const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleRecurrence(hti);

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleExternalID() const
{
	CString sLongest;
	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		CString sItemLongest = GetLongestVisibleExternalID(hti);

		if (sItemLongest.GetLength() > sLongest.GetLength())
		{
			sLongest = sItemLongest;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleCategory(HTREEITEM hti) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);

	if (!pTDI)
	{
		return _T("");
	}

	return GetLongestVisibleCategory(hti, pTDI);
}

CString CToDoCtrlFind::GetLongestVisibleTime(HTREEITEM hti, int nDefUnits, BOOL bTimeEst) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);
	TODOSTRUCTURE* pTDS = m_data.LocateTask(GetTaskID(hti));

	if (!pTDI || !pTDS)
	{
		return _T("");
	}

	return GetLongestVisibleTime(hti, pTDI, pTDS, nDefUnits, bTimeEst);
}

CString CToDoCtrlFind::GetLongestVisibleCategory(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = Misc::FormatArray(pTDI->aCategories);

	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestVisibleCategory(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
			{
				sLongest = sChildLongest;
			}

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleExternalID(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = pTDI->sExternalID;

	// children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestVisibleExternalID(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
			{
				sLongest = sChildLongest;
			}

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleRecurrence(HTREEITEM hti, const TODOITEM* pTDI) const
{
	CString sLongest = pTDI->trRecurrence.sRegularity;

	// children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestVisibleExternalID(htiChild);

			if (sChildLongest.GetLength() > sLongest.GetLength())
			{
				sLongest = sChildLongest;
			}

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

CString CToDoCtrlFind::GetLongestVisibleTime(HTREEITEM hti, const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nDefUnits, BOOL bTimeEst) const
{
	int nUnits = nDefUnits;
	CString sLongest;

	// get actual task time units
	if (!pTDS->HasSubTasks() || m_data.HasStyle(TDCS_ALLOWPARENTTIMETRACKING))
	{
		nUnits = bTimeEst ? pTDI->nTimeEstUnits : pTDI->nTimeSpentUnits;
	}

	// get time
	double dTime = bTimeEst ? m_data.CalcTimeEstimate(pTDI, pTDS, nUnits) : m_data.CalcTimeSpent(pTDI, pTDS, nUnits);

	if (dTime > 0 || !m_data.HasStyle(TDCS_HIDEZEROTIMECOST))
	{
		int nDecPlaces = m_data.HasStyle(TDCS_ROUNDTIMEFRACTIONS) ? 0 : 2;

		if (m_data.HasStyle(TDCS_DISPLAYHMSTIMEFORMAT))
		{
			sLongest = CTimeHelper().FormatTimeHMS(dTime, nUnits, (BOOL)nDecPlaces);
		}
		else
		{
			sLongest = CTimeHelper().FormatTime(dTime, nUnits, nDecPlaces);
		}
	}

	// children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		// check children
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			CString sChildLongest = GetLongestVisibleTime(htiChild, nDefUnits, bTimeEst);

			if (sChildLongest.GetLength() > sLongest.GetLength())
			{
				sLongest = sChildLongest;
			}

			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return sLongest;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithDueTime() const
{
	if (m_data.HasStyle(TDCS_HIDEDUETIMEFIELD))
	{
		return FALSE;
	}

	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		if (FindVisibleTaskWithDueTime(hti))
		{
			return TRUE;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return FALSE;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithDueTime(HTREEITEM hti) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);

	if (pTDI && !pTDI->IsDone() && pTDI->HasDueTime())
	{
		return TRUE;
	}

	// check children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			if (FindVisibleTaskWithDueTime(htiChild))
			{
				return TRUE;
			}

			// next child
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return FALSE;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithStartTime() const
{
	if (m_data.HasStyle(TDCS_HIDESTARTTIMEFIELD))
	{
		return FALSE;
	}

	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		if (FindVisibleTaskWithStartTime(hti))
		{
			return TRUE;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return FALSE;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithStartTime(HTREEITEM hti) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);

	if (pTDI && !pTDI->IsDone() && pTDI->HasStartTime())
	{
		return TRUE;
	}

	// check children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			if (FindVisibleTaskWithStartTime(htiChild))
			{
				return TRUE;
			}

			// next child
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return FALSE;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithDoneTime() const
{
	if (m_data.HasStyle(TDCS_HIDEDONETIMEFIELD))
	{
		return FALSE;
	}

	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		if (FindVisibleTaskWithDoneTime(hti))
		{
			return TRUE;
		}

		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	return FALSE;
}

BOOL CToDoCtrlFind::FindVisibleTaskWithDoneTime(HTREEITEM hti) const
{
	ASSERT(hti);
	TODOITEM* pTDI = GetTask(hti);

	if (pTDI && pTDI->IsDone() && pTDI->HasDoneTime())
	{
		return TRUE;
	}

	// check children
	BOOL bExpanded = (m_tree.GetItemState(hti, TVIS_EXPANDED) & TVIS_EXPANDED);

	if (bExpanded)
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			if (FindVisibleTaskWithDoneTime(htiChild))
			{
				return TRUE;
			}

			// next child
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}

	return FALSE;
}

int CToDoCtrlFind::FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const
{
	if (!m_data.GetTaskCount())
	{
		return 0;
	}

	HTREEITEM hti = m_tree.GetChildItem(NULL);

	while (hti)
	{
		FindTasks(hti, params, aResults);
		hti = m_tree.GetNextItem(hti, TVGN_NEXT);
	}

	// else
	return aResults.GetSize();
}

void CToDoCtrlFind::FindTasks(HTREEITEM hti, const SEARCHPARAMS& params, CResultArray& aResults) const
{
	SEARCHRESULT result;
	DWORD dwID = GetTaskID(hti);

	// if the item is done and we're ignoring completed tasks
	// (and by corollary their children) then we can stop right-away
	if (params.bIgnoreDone && m_data.IsTaskDone(dwID, TDCCHECKALL))
	{
		return;
	}

	// also we can ignore parent tasks if required but we still need
	// to process it's children
	if (m_data.TaskMatches(GetTaskID(hti), params, result))
	{
		// check for overdue tasks
		if (!params.bIgnoreOverDue || !m_data.IsTaskOverDue(dwID))
		{
			aResults.Add(result);
		}
	}

	// process children
	HTREEITEM htiChild = m_tree.GetChildItem(hti);

	while (htiChild)
	{
		FindTasks(htiChild, params, aResults); // RECURSIVE call

		// next
		htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
	}
}

DWORD CToDoCtrlFind::FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	if (!m_data.GetTaskCount())
	{
		return 0;
	}

	return FindFirstTask(m_tree.GetChildItem(NULL), params, result);
}

DWORD CToDoCtrlFind::FindNextTask(DWORD dwStart, const SEARCHPARAMS& params, SEARCHRESULT& result, BOOL bNext) const
{
	HTREEITEM htiNext = GetItem(dwStart);
	CTreeCtrlHelper tch(m_tree);

	while (htiNext)
	{
		if (m_data.TaskMatches(GetTaskID(htiNext), params, result))
		{
			return result.dwID;
		}

		// next item
		htiNext = bNext ? tch.GetNextItem(htiNext) : tch.GetPrevItem(htiNext);
	}

	// else
	return 0; // not found
}

DWORD CToDoCtrlFind::FindFirstTask(HTREEITEM htiStart, const SEARCHPARAMS& params, SEARCHRESULT& result) const
{
	DWORD dwTaskID = GetTaskID(htiStart);

	if (!m_data.TaskMatches(dwTaskID, params, result))
	{
		// also check first child (other children are handled by sibling check below)
		HTREEITEM htiChild = m_tree.GetChildItem(htiStart);

		if (htiChild)
		{
			dwTaskID = FindFirstTask(htiChild, params, result);
		}
		else
		{
			dwTaskID = 0;
		}

		// and first sibling
		if (!dwTaskID)
		{
			HTREEITEM htiNext = m_tree.GetNextItem(htiStart, TVGN_NEXT);

			if (htiNext)
			{
				dwTaskID = FindFirstTask(htiNext, params, result);
			}
		}
	}

	return dwTaskID;
}
