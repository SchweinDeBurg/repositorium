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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ToDoCtrlFind.h: interface for the CToDoCtrlData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODOCTRLTREEDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
#define AFX_TODOCTRLTREEDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDCStruct.h"

class TODOITEM;
class TODOSTRUCTURE;
class CToDoCtrlData;

class CToDoCtrlFind
{
public:
	CToDoCtrlFind(CTreeCtrl& tree, const CToDoCtrlData& data);
	virtual ~CToDoCtrlFind();

	HTREEITEM GetItem(DWORD dwID) const;
	DWORD GetTaskID(HTREEITEM hti) const;
	TODOITEM* GetTask(HTREEITEM hti) const;

	// Gets
	CString GetLongestVisibleExternalID() const;
	CString GetLongestVisibleCategory() const;
	CString GetLongestVisibleAllocTo() const;
	CString GetLongestVisibleTimeEstimate(int nDefUnits) const;
	CString GetLongestVisibleTimeSpent(int nDefUnits) const;
	CString GetLongestVisibleRecurrence() const;

	BOOL FindVisibleTaskWithDueTime() const;
	BOOL FindVisibleTaskWithStartTime() const;
	BOOL FindVisibleTaskWithDoneTime() const;

	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	DWORD FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	DWORD FindNextTask(DWORD dwStart, const SEARCHPARAMS& params, SEARCHRESULT& result, BOOL bNext = TRUE) const;

protected:
	CTreeCtrl& m_tree;
	const CToDoCtrlData& m_data;

protected:
	void FindTasks(HTREEITEM hti, const SEARCHPARAMS& params, CResultArray& aResults) const;
	DWORD FindFirstTask(HTREEITEM htiStart, const SEARCHPARAMS& params, SEARCHRESULT& result) const;

	CString GetLongestVisibleExternalID(HTREEITEM hti) const;
	CString GetLongestVisibleCategory(HTREEITEM hti) const;
	CString GetLongestVisibleAllocTo(HTREEITEM hti) const;
	CString GetLongestVisibleTime(HTREEITEM hti, int nDefUnits, BOOL bTimeEst) const;
	CString GetLongestVisibleRecurrence(HTREEITEM hti) const;

	BOOL FindVisibleTaskWithDueTime(HTREEITEM hti) const;
	BOOL FindVisibleTaskWithStartTime(HTREEITEM hti) const;
	BOOL FindVisibleTaskWithDoneTime(HTREEITEM hti) const;

	CString GetLongestVisibleExternalID(HTREEITEM hti, const TODOITEM* pTDI) const;
	CString GetLongestVisibleCategory(HTREEITEM hti, const TODOITEM* pTDI) const;
	CString GetLongestVisibleAllocTo(HTREEITEM hti, const TODOITEM* pTDI) const;
	CString GetLongestVisibleTime(HTREEITEM hti, const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nDefUnits, BOOL bTimeEst) const;
	CString GetLongestVisibleRecurrence(HTREEITEM hti, const TODOITEM* pTDI) const;
};

#endif // !defined(AFX_TODOCTRLTREEDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
