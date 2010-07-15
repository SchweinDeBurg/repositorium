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

// ToDoCtrlData.h: interface for the CToDoCtrlData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
#define AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDCStruct.h"
#include "TDCEnum.h"
#include "ToDoCtrlUndo.h"
#include "ToDoItem.h"
#include <afxtempl.h>

class CToDoCtrlData;

typedef CMap<DWORD, DWORD, TODOITEM*, TODOITEM*&> CTDIMap;

// class to help start and end undo actions
class CUndoAction
{
public:
	CUndoAction(CToDoCtrlData& data, TDCUNDOACTIONTYPE nType = TDCUAT_EDIT);
	~CUndoAction();

protected:
	CToDoCtrlData& m_data;
	BOOL m_bSuccess;
};

class CToDoCtrlData  
{
public:
	CToDoCtrlData(const CWordArray& aStyles);
	virtual ~CToDoCtrlData();
	
	inline UINT GetTaskCount() const { return m_mapID2TDI.GetCount(); }
	
	TODOITEM* NewTask(const TODOITEM* pTDIRef = NULL);
	void AddTask(DWORD dwID, TODOITEM* pTDI, DWORD dwParentID, DWORD dwPrevSiblingID);
	TODOITEM* GetTask(DWORD dwID) const;
	BOOL DeleteTask(DWORD dwID);
	void DeleteAllTasks();

	BOOL LocateTask(DWORD dwID, TODOSTRUCTURE*& pTDSParent, int& nPos) const;
	TODOSTRUCTURE* LocateTask(DWORD dwID) const;

	const TODOSTRUCTURE* GetStructure() const { return &m_struct; }
	BOOL GetTask(DWORD dwID, const TODOITEM*& pTDI, const TODOSTRUCTURE*& pTDS) const;
	BOOL GetTask(DWORD dwID, TODOITEM*& pTDI, TODOSTRUCTURE*& pTDS);
	
	BOOL MoveTask(DWORD dwID, DWORD dwDestParentID, DWORD dwDestPrevSiblingID);
	BOOL MoveTasks(const CDWordArray& aTaskIDs, DWORD dwDestParentID, DWORD dwDestPrevSiblingID);

	// undo/redo
	BOOL BeginNewUndoAction(TDCUNDOACTIONTYPE nType);
	BOOL EndCurrentUndoAction();
	BOOL UndoLastAction(BOOL bUndo, CArrayUndoElements& aElms);
	BOOL CanUndoLastAction(BOOL bUndo) const;
	int GetLastUndoActionTaskIDs(BOOL bUndo, CDWordArray& aIDs) const;
	TDCUNDOACTIONTYPE GetLastUndoActionType(BOOL bUndo) const;
	BOOL DeleteLastUndoAction();

	// use only when this task does not provide a Set method eg. moving tasks
	void ClearUndo() { m_undo.ResetAll(); }

	double GetEarliestDueDate() const;
	
	// Gets
	CString GetTaskTitle(DWORD dwID) const;
	int GetTaskIconIndex(DWORD dwID) const;
	COleDateTime GetTaskDate(DWORD dwID, TDC_DATE nDate) const;
	COLORREF GetTaskColor(DWORD dwID) const; // -1 on no item selected
	CString GetTaskComments(DWORD dwID) const;
	CString GetTaskCustomComments(DWORD dwID, CString& sCommentsTypeID) const;
	int GetTaskPercent(DWORD dwID, BOOL bCheckIfDone) const;
	double GetTaskTimeEstimate(DWORD dwID, int& nUnits) const;
	double GetTaskTimeSpent(DWORD dwID, int& nUnits) const;
	double GetTaskCost(DWORD dwID) const;
	int GetTaskAllocTo(DWORD dwID, CStringArray& aAllocTo) const;
	CString GetTaskAllocBy(DWORD dwID) const;
	CString GetTaskCreatedBy(DWORD dwID) const;
	CString GetTaskStatus(DWORD dwID) const;
	int GetTaskCategories(DWORD dwID, CStringArray& aCategories) const;
	int GetTaskDependencies(DWORD dwID, CStringArray& aDepends) const;
	CString GetTaskFileRef(DWORD dwID) const;
	CString GetTaskExtID(DWORD dwID) const;
	int GetTaskPriority(DWORD dwID) const;
	int GetTaskRisk(DWORD dwID) const;
	BOOL IsTaskFlagged(DWORD dwID) const;
	BOOL GetTaskRecurrence(DWORD dwID, TDIRECURRENCE& tr) const;
	BOOL GetTaskNextOccurrence(DWORD dwID, COleDateTime& dtNext) const;
	CString GetTaskVersion(DWORD dwID) const;
	CString GetTaskPath(DWORD, int nMaxLen = -1) const; 

	BOOL IsTaskDone(DWORD dwID, DWORD dwExtraCheck = TDCCHECKNONE) const;

	int GetTaskDependents(DWORD dwTaskID, CDWordArray& aDependents) const;
	int GetTaskDependencies(DWORD dwTaskID, CDWordArray& aDepends) const;
	BOOL TaskHasCircularDependencies(DWORD dwTaskID) const;

	BOOL IsTaskDue(DWORD dwID, BOOL bToday = FALSE) const;
	BOOL IsTaskOverDue(DWORD dwID) const;
	double GetEarliestDueDate(DWORD dwID, BOOL bCheckChildren) const;
	int GetHighestPriority(DWORD dwID, BOOL bIncludeDue = TRUE) const;
	int GetHighestRisk(DWORD dwID) const;
	int CalcPercentDone(DWORD dwID) const;
	double CalcCost(DWORD dwID) const;
	double CalcTimeEstimate(DWORD dwID, int nUnits) const;
	double CalcTimeSpent(DWORD dwID, int nUnits) const;
	BOOL GetSubtaskTotals(DWORD dwID, int& nSubtasksTotal, int& nSubtasksDone) const;

	BOOL IsTaskDue(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bToday = FALSE) const;
	BOOL IsTaskOverDue(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const;
	double GetEarliestDueDate(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bCheckChildren) const;
	int GetHighestPriority(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, BOOL bIncludeDue = TRUE) const;
	int GetHighestRisk(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const;
	double CalcCost(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const;
	double CalcTimeEstimate(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nUnits) const;
	double CalcTimeSpent(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nUnits) const;
	int CalcPercentDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const;
	int CalcPercentFromTime(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS) const; // spent / estimate
	BOOL GetSubtaskTotals(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, 
							int& nSubtasksTotal, int& nSubtasksDone) const;
	BOOL IsTaskDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, DWORD dwExtraCheck) const;
	
	// Sets. 0 = failed, 1 = success, -1 = success (no change)
	int SetTaskDate(DWORD dwID, TDC_DATE nDate, const COleDateTime& date);
	int SetTaskColor(DWORD dwID, COLORREF color);
	int SetTaskIconIndex(DWORD dwID, int nIndex);
	int ClearTaskColor(DWORD dwID) { SetTaskColor(dwID, (COLORREF)-1); }
	int SetTaskComments(DWORD dwID, LPCTSTR szComments, const CString& sCustomComments = "", LPCTSTR szCommentsTypeID = NULL);
	int SetTaskCommentsType(DWORD dwID, LPCTSTR szCommentsTypeID);
	int SetTaskPercent(DWORD dwID, int nPercent);
	int SetTaskTimeEstimate(DWORD dwID, double dTime, int nUnits = TDITU_HOURS);
	int SetTaskTimeSpent(DWORD dwID, double dTime, int nUnits = TDITU_HOURS);
	int ResetTaskTimeSpent(DWORD dwID, BOOL bAndChildren = FALSE);
	int SetTaskCost(DWORD dwID, double dCost);
	int SetTaskAllocTo(DWORD dwID, const CStringArray& aAllocTo);
	int SetTaskAllocBy(DWORD dwID, LPCTSTR szAllocBy);
	int SetTaskStatus(DWORD dwID, LPCTSTR szStatus);
	int SetTaskCategories(DWORD dwID, const CStringArray& aCategories);
	int SetTaskDependencies(DWORD dwID, const CStringArray& aDepends);
	int SetTaskExtID(DWORD dwID, LPCTSTR szID);
	int SetTaskFileRef(DWORD dwID, LPCTSTR szFilePath);
	int SetTaskPriority(DWORD dwID, int nPriority); // 0-10 (10 is highest)
	int SetTaskRisk(DWORD dwID, int nRisk); // 0-10 (10 is highest)
	int SetTaskTitle(DWORD dwID, LPCTSTR szTitle);
	int SetTaskFlag(DWORD dwID, BOOL bFlagged);
	int SetTaskRecurrence(DWORD dwID, const TDIRECURRENCE& tr);
	int SetTaskVersion(DWORD dwID, LPCTSTR szVersion);
	int OffsetTaskDate(DWORD dwID, TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks);

	int CopyTaskAttributes(DWORD dwToTaskID, DWORD dwFromTaskID, const CTDCAttributeArray& aAttribs);
	int CopyTaskAttributes(TODOITEM* pToTDI, DWORD dwFromTaskID, const CTDCAttributeArray& aAttribs);
	
	BOOL TaskMatches(DWORD dwID, const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	BOOL TaskMatches(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	
	BOOL IsTaskTimeTrackable(DWORD dwID) const;
	BOOL IsParentTaskDone(DWORD dwID) const;
	BOOL IsParentTaskDone(const TODOSTRUCTURE* pTDS) const;

	int AreChildTasksDone(DWORD dwID) const;
	int AreChildTasksDone(const TODOSTRUCTURE* pTDS) const;

	BOOL ApplyLastChangeToSubtasks(DWORD dwID, TDC_ATTRIBUTE nAttrib);
	void ResetCachedCalculations() const;
	
	static int MapTimeUnits(const CString& sUnits);
	static CString MapTimeUnits(int nUnits);

	int CompareTasks(DWORD dwTask1ID, DWORD dwTask2ID, TDC_SORTBY nSortBy, BOOL bAscending, BOOL bSortDueTodayHigh);
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
	static int CALLBACK CompareFuncMulti(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	int FindTasks(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, const SEARCHPARAMS& params, CResultArray& aResults) const;
	inline BOOL HasStyle(int nStyle) const { return m_aStyles[nStyle] ? TRUE : FALSE; }

protected:
	CTDIMap m_mapID2TDI; // the real data
	const CWordArray& m_aStyles; // CToDoCtrl styles
	CToDoCtrlUndo m_undo;
	CToDoCtrlStructure m_struct;

protected:
	BOOL DeleteTask(TODOSTRUCTURE* pTDSParent, int nPos);


	BOOL FindDependency(DWORD dwTaskID, DWORD dwDependsID) const;

	BOOL AddUndoElement(TDCUNDOELMOP nOp, DWORD dwTaskID, DWORD dwParentID = 0, 
						DWORD dwPrevSiblingID = 0, WORD wFlags = 0);
	
	BOOL TaskMatches(const COleDateTime& date, const SEARCHPARAM& sp, SEARCHRESULT& result) const;
	BOOL TaskMatches(const CString& sText, const SEARCHPARAM& sp, SEARCHRESULT& result) const;
	BOOL TaskMatches(double dValue, const SEARCHPARAM& sp, SEARCHRESULT& result) const;
	BOOL TaskMatches(int nValue, const SEARCHPARAM& sp, SEARCHRESULT& result) const;
	BOOL TaskMatches(const CStringArray& aItems, const SEARCHPARAM& sp, SEARCHRESULT& result) const;

	void SumPercentDone(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS,
								   double& dTotalPercent, double& dTotalWeighting) const;
	

	TODOITEM* GetTask(const TODOSTRUCTURE* pTDS) const;

	BOOL ApplyLastChangeToSubtasks(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, TDC_ATTRIBUTE nAttrib);

	BOOL Locate(DWORD dwParentID, DWORD dwPrevSiblingID, TODOSTRUCTURE*& pTDSParent, int& nPos) const;
	int MoveTask(TODOSTRUCTURE* pTDSSrcParent, int nSrcPos, DWORD dwSrcPrevSiblingID,
							 TODOSTRUCTURE* pTDSDestParent, int nDestPos);

	
	static TDC_ATTRIBUTE MapDateToAttribute(TDC_DATE nDate);

	static int Compare(const COleDateTime& date1, const COleDateTime& date2);
	static int Compare(const CString& sText1, const CString& sText2, BOOL bCheckEmpty = FALSE);
	static int Compare(int nNum1, int nNum2);
	static int Compare(double dNum1, double dNum2);
};

#endif // !defined(AFX_TODOCTRLDATA_H__02C3C360_45AB_45DC_B1BF_BCBEA472F0C7__INCLUDED_)
