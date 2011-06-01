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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ToDoItem.h: interface for the CToDoItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODOITEM_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
#define AFX_TODOITEM_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDCStruct.h"
#include <afxtempl.h>

class TODOITEM
{
public:
	TODOITEM(LPCTSTR szTitle, LPCTSTR szComments = NULL);
	TODOITEM();
	TODOITEM(const TODOITEM& tdi);
	TODOITEM(const TODOITEM* pTDI);

	const TODOITEM& operator=(const TODOITEM& tdi);

	BOOL HasLastMod() const;
	BOOL HasCreation() const;
	BOOL HasStart() const;
	BOOL HasStartTime() const;
	BOOL HasDue() const;
	BOOL HasDueTime() const;
	BOOL IsDone() const;
	BOOL HasDoneTime() const;

	void ClearStart();
	void ClearDue();
	void ClearDone();

	BOOL IsDue() const;
	BOOL IsDue(const COleDateTime& dateDueBy) const;

	void SetModified();
	void ResetCalcs() const;
	BOOL AttribNeedsRecalc(DWORD dwAttrib) const;
	void SetAttribNeedsRecalc(DWORD dwAttrib, BOOL bSet = TRUE) const;

	CString GetFirstCategory() const;
	CString GetFirstAllocTo() const;
	CString GetFirstDependency() const;

	BOOL GetNextOccurence(COleDateTime& dtNext) const;
	BOOL IsRecentlyEdited(const COleDateTimeSpan& dtSpan = 1.0 / 24.0) const; // 1 hour default

	COleDateTimeSpan GetRemainingDueTime() const; // in days

	static COleDateTimeSpan GetRemainingDueTime(const COleDateTime& date); // in days
	static BOOL HasTime(const COleDateTime& date);
	static void ParseTaskLink(const CString& sLink, DWORD& dwTaskID, CString& sFile);
	static CString MakeTaskLink(DWORD dwTaskID, const CString& sFile = _T(""));

	//  ------------------------------------------

	CString sTitle;
	CString sComments, sCustomComments;
	CString sCommentsTypeID;
	COLORREF color;
	COleDateTime dateStart, dateDue, dateDone, dateCreated;
	int nPriority;
	double dCost;
	CStringArray aAllocTo;
	CString sAllocBy;
	CString sStatus;
	CStringArray aCategories;
	CString sCreatedBy;
	int nPercentDone;
	CString sFileRefPath;
	double dTimeEstimate, dTimeSpent;
	int nTimeEstUnits, nTimeSpentUnits;
	COleDateTime tLastMod;
	BOOL bFlagged;
	int nRisk;
	CString sExternalID;
	CStringArray aDependencies;
	TDIRECURRENCE trRecurrence;
	CString sVersion;
	int nIconIndex;

	// cached calculations for drawing optimization
	// mutable so that they can be updated in const methods
	mutable DWORD dwCalculated;
	mutable int nCalcPriority;
	mutable int nCalcPriorityIncDue;
	mutable int nCalcPercent;
	mutable int nCalcRisk;
	mutable double dCalcTimeEstimate, dCalcTimeSpent;
	mutable double dCalcCost;
	mutable COleDateTime dateEarliestDue;
	mutable BOOL bGoodAsDone, bDue;
	mutable int nSubtasksCount, nSubtasksDone;

	// C4995: name was marked as #pragma deprecated
#pragma warning(disable: 4995)
	DECLARE_FIXED_ALLOC(TODOITEM);
#pragma warning(default: 4995)
};

class TODOSTRUCTURE
{
	friend class CToDoCtrlStructure;

public:
	TODOSTRUCTURE() : m_dwID(0), m_pTDSParent(NULL) {}
	TODOSTRUCTURE(DWORD dwID);
	~TODOSTRUCTURE();

	DWORD GetTaskID() const
	{
		return m_dwID;
	}
	DWORD GetSubTaskID(int nPos) const;

	int GetSubTaskPosition(DWORD dwID) const;
	int GetPosition() const;

	TODOSTRUCTURE* GetParentTask() const;
	DWORD GetParentTaskID() const;

	BOOL ParentIsRoot() const
	{
		return (GetParentTaskID() == 0);
	}
	BOOL IsRoot() const
	{
		return (GetTaskID() == 0);
	}

	DWORD GetPreviousSubTaskID(int nPos);

	int GetSubTaskCount() const
	{
		return m_aSubTasks.GetSize();
	}
	BOOL HasSubTasks() const
	{
		return GetSubTaskCount() > 0;
	}

	TODOSTRUCTURE* GetSubTask(int nPos) const;
	int GetSubTaskPos(TODOSTRUCTURE* pTDS) const;

	void DeleteAll()
	{
		CleanUp();
	}

	int MoveSubTask(int nPos, TODOSTRUCTURE* pTDSDestParent, int nDestPos);

protected:
	DWORD m_dwID;
	TODOSTRUCTURE* m_pTDSParent;
	CArray<TODOSTRUCTURE*, TODOSTRUCTURE*&> m_aSubTasks;

protected:
	TODOSTRUCTURE(const TODOSTRUCTURE& tds);
	const TODOSTRUCTURE& operator=(const TODOSTRUCTURE& tds);

	void CleanUp();
	BOOL DeleteSubTask(int nPos);
	BOOL InsertSubTask(TODOSTRUCTURE* pTDS, int nPos);
};

class CToDoCtrlStructure : public TODOSTRUCTURE
{
public:
	CToDoCtrlStructure() : TODOSTRUCTURE() {}
	CToDoCtrlStructure(const CToDoCtrlStructure& tds);
	~CToDoCtrlStructure();

	TODOSTRUCTURE* GetParentTask(DWORD dwID) const;
	DWORD GetParentTaskID(DWORD dwID) const;

	DWORD GetPreviousTaskID(DWORD dwID) const;

	BOOL DeleteTask(DWORD dwID);

	BOOL AddTask(DWORD dwID, TODOSTRUCTURE* pTDSParent);
	BOOL InsertTask(const TODOSTRUCTURE& tds, TODOSTRUCTURE* pTDSParent, int nPos);
	BOOL InsertTask(DWORD dwID, TODOSTRUCTURE* pTDSParent, int nPos);
	BOOL InsertTask(TODOSTRUCTURE* pTDS, TODOSTRUCTURE* pTDSParent, int nPos);

	TODOSTRUCTURE* FindTask(DWORD dwID) const;
	BOOL FindTask(DWORD dwID, TODOSTRUCTURE*& pTDSParent, int& nPos) const;

protected:
	CMap<DWORD, DWORD, TODOSTRUCTURE*, TODOSTRUCTURE*&> m_mapStructure;

protected:
	void BuildMap();
	void AddToMap(const TODOSTRUCTURE* pTDS);
	void RemoveFromMap(const TODOSTRUCTURE* pTDS);
	const CToDoCtrlStructure& operator=(const CToDoCtrlStructure& tds);
};


#endif // !defined(AFX_TODOITEM_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
