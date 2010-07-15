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

// TaskFile.h: interface for the CTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKFILE_H__BA5D71E7_2770_45FD_A693_A2344B589DF4__INCLUDED_)
#define AFX_TASKFILE_H__BA5D71E7_2770_45FD_A693_A2344B589DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/ITaskList.h"

#include <afxtempl.h>

#ifdef NO_TL_ENCRYPTDECRYPT
#	include "../../Common/XmlFile.h"
#	define XMLBASE CXmlFile
#else
#	include "../../Common/XmlFileEx.h"
#	define XMLBASE CXmlFileEx
#endif

struct TDIRECURRENCE; // predeclaration

class CTaskFile : public ITASKLISTBASE, public XMLBASE
{
public:
	CTaskFile(LPCTSTR szPassword = NULL);
	virtual ~CTaskFile();

	BOOL Load(LPCTSTR szFilePath, IXmlParse* pCallback = NULL, BOOL bDecrypt = TRUE);

	virtual BOOL LoadEx(IXmlParse* pCallback = NULL);
	virtual BOOL SaveEx();
	virtual BOOL LoadHeader(LPCTSTR szFilePath);

#ifndef NO_TL_ENCRYPTDECRYPT
	virtual BOOL Decrypt(LPCTSTR szPassword = NULL); 
#endif

	BOOL Copy(const CTaskFile& tasks);
	BOOL Copy(const ITaskList* pTasks);

	int GetTaskCount() const { return m_mapHandles.GetCount(); }

#ifndef NO_TL_MERGE
	int Merge(const CTaskFile& tasks, BOOL bByID, BOOL bMoveExist);
	int Merge(LPCTSTR szTaskFilePath, BOOL bByID, BOOL bMoveExist);
#endif

	HTASKITEM NewTask(const char* szTitle, HTASKITEM hParent, DWORD dwID);

	DWORD GetNextUniqueID() const; 
	BOOL SetNextUniqueID(DWORD dwNextID); 

	BOOL SetArchive(BOOL bArchive = TRUE);
	BOOL SetCheckedOutTo(const CString& sCheckedOutTo);
	BOOL SetFileFormat(unsigned long lFormat);
	BOOL SetLastModified(const CString& sLastMod);
//	void SortTasksByID();
	void SortTasksByPos();
	BOOL SetCharSet(LPCTSTR szCharSet);
	BOOL SetFileName(LPCTSTR szFilename);
	
	BOOL SetCategoryNames(const CStringArray& aCategories);
	BOOL SetStatusNames(const CStringArray& aStatuses);
	BOOL SetAllocToNames(const CStringArray& aAllocTo);
	BOOL SetAllocByNames(const CStringArray& aAllocBy);
	BOOL SetVersionNames(const CStringArray& aVersions);
	int GetCategoryNames(CStringArray& aCategories) const;
	int GetStatusNames(CStringArray& aStatuses) const;
	int GetAllocToNames(CStringArray& aAllocTo) const;
	int GetAllocByNames(CStringArray& aAllocBy) const;
	int GetVersionNames(CStringArray& aVersions) const;

	BOOL SetEarliestDueDate(const COleDateTime& date);
	BOOL GetEarliestDueDate(COleDateTime& date) const;

// 	BOOL SetCommentsType(LPCTSTR szID); 
	CString GetCommentsType() const; 

	COleDateTime GetTaskLastModifiedOle(HTASKITEM hTask) const;
	COleDateTime GetTaskDoneDateOle(HTASKITEM hTask) const;
	COleDateTime GetTaskDueDateOle(HTASKITEM hTask) const;
	COleDateTime GetTaskStartDateOle(HTASKITEM hTask) const;
	COleDateTime GetTaskCreationDateOle(HTASKITEM hTask) const;

	BOOL SetTaskID(HTASKITEM hTask, unsigned long nID, BOOL bVisible = TRUE);

	BOOL SetTaskLastModified(HTASKITEM hTask, const COleDateTime& tLastMod);
	BOOL SetTaskDoneDate(HTASKITEM hTask, const COleDateTime& date);
	BOOL SetTaskDueDate(HTASKITEM hTask, const COleDateTime& date);
	BOOL SetTaskStartDate(HTASKITEM hTask, const COleDateTime& date);
	BOOL SetTaskCreationDate(HTASKITEM hTask, const COleDateTime& date);

	BOOL SetTaskRecurrence(HTASKITEM hTask, const TDIRECURRENCE& tr, LPCTSTR szRegularity);
	BOOL GetTaskRecurrence(HTASKITEM hTask, TDIRECURRENCE& tr) const;

	BOOL SetTaskTextColor(HTASKITEM hTask, COLORREF color);
	BOOL SetTaskPriorityColor(HTASKITEM hTask, COLORREF color);
	BOOL SetTaskCalcTimeEstimate(HTASKITEM hTask, double dTime);
	BOOL SetTaskCalcTimeSpent(HTASKITEM hTask, double dTime);
	BOOL SetTaskEarliestDueDate(HTASKITEM hTask,  const COleDateTime& date);
	BOOL SetTaskCalcCompletion(HTASKITEM hTask, int nPercent);
	BOOL SetTaskHighestPriority(HTASKITEM hTask, int nPriority);
	BOOL SetTaskHighestRisk(HTASKITEM hTask, int nRisk);
	BOOL SetTaskCalcCost(HTASKITEM hTask, double dCost);

	BOOL SetTaskIconIndex(HTASKITEM hTask, int nIndex);
	int  GetTaskIconIndex(HTASKITEM hTask) const;

	BOOL SetTaskCategories(HTASKITEM hTask, const CStringArray& aCategories);
	int  GetTaskCategories(HTASKITEM hTask, CStringArray& aCategories) const;

	BOOL SetTaskDependencies(HTASKITEM hTask, const CStringArray& aDepends);
	int  GetTaskDependencies(HTASKITEM hTask, CStringArray& aDepends) const;

	BOOL SetTaskAllocatedTo(HTASKITEM hTask, const CStringArray& aAllocTo);
	int  GetTaskAllocatedTo(HTASKITEM hTask, CStringArray& aAllocTo) const;

	BOOL SetTaskCustomComments(HTASKITEM hTask, const CString& sContent, const CString& sType);
	BOOL GetTaskCustomComments(HTASKITEM hTask, CString& sContent, CString& sType) const;
	BOOL SetTaskHtmlComments(HTASKITEM hTask, const CString& sContent, BOOL bForTransform);
	
	BOOL DeleteTaskAttributes(HTASKITEM hTask);// deletes all but child tasks
	BOOL DeleteTask(HTASKITEM hTask);
	HTASKITEM FindTask(DWORD dwTaskID) const;

	BOOL CheckIn();
	BOOL CheckOut();
	BOOL CheckOut(CString& sCheckedOutTo);

	BOOL SetReportAttributes(LPCTSTR szTitle, const COleDateTime& date = 0.0);
	BOOL HideAttribute(HTASKITEM hTask, const char* szAttrib, BOOL bHide = TRUE);

	bool SetTaskPriority(HTASKITEM hTask, int nPriority);
	bool SetTaskRisk(HTASKITEM hTask, int nRisk);

	//////////////////////////////////////////////////////////////
	// ITaskList7 implementation 
	unsigned char GetTaskDependencyCount(HTASKITEM hTask) const;
	bool AddTaskDependency(HTASKITEM hTask, const char* szDepends);
	const char* GetTaskDependency(HTASKITEM hTask, int nIndex) const;

	unsigned char GetTaskAllocatedToCount(HTASKITEM hTask) const;
	bool AddTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo);
	const char* GetTaskAllocatedTo(HTASKITEM hTask, int nIndex) const;

	//////////////////////////////////////////////////////////////
	// ITaskList6 implementation 
	bool SetTaskRecurrence(HTASKITEM hTask, int nRegularity, DWORD dwSpecific1, 
									DWORD dwSpecific2, BOOL bRecalcFromDue, int nReuse);
	bool GetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1, 
									DWORD& dwSpecific2, BOOL& bRecalcFromDue, int& nReuse) const;

	bool SetTaskVersion(HTASKITEM hTask, const char* szVersion);
	const char* GetTaskVersion(HTASKITEM hTask) const;

	//////////////////////////////////////////////////////////////
	// ITaskList5 implementation 
	bool AddTaskCategory(HTASKITEM hTask, const char* szCategory);

	//////////////////////////////////////////////////////////////
	// ITaskList4 implementation 
	const char* GetAttribute(const char* szAttrib) const;

	const char* GetHtmlCharSet() const;
	const char* GetReportTitle() const;
	const char* GetReportDate() const;
	double GetTaskCost(HTASKITEM hTask, BOOL bCalc) const;
	unsigned char GetTaskCategoryCount(HTASKITEM hTask) const;
	const char* GetTaskCategory(HTASKITEM hTask, int nIndex) const;
	const char* GetTaskDependency(HTASKITEM hTask) const;

	bool SetTaskCost(HTASKITEM hTask, double dCost);
	bool SetTaskDependency(HTASKITEM hTask, const char* szDepends);

	//////////////////////////////////////////////////////////////
	// ITaskList3 implementation 
	time_t GetTaskDueDate(HTASKITEM hTask, BOOL bEarliest) const;
	const char* GetTaskDueDateString(HTASKITEM hTask, BOOL bEarliest) const;
	unsigned long GetTaskTextColor(HTASKITEM hTask) const;
	int GetTaskRisk(HTASKITEM hTask, BOOL bHighest) const;
	const char* GetTaskExternalID(HTASKITEM hTask) const;

	bool SetTaskRisk(HTASKITEM hTask, unsigned char nRisk);
	bool SetTaskExternalID(HTASKITEM hTask, const char* szID);

	//////////////////////////////////////////////////////////////
	// ITaskList2 implementation 
	
	const char* GetTaskCreatedBy(HTASKITEM hTask) const;
	time_t GetTaskCreationDate(HTASKITEM hTask) const;
	const char* GetTaskCreationDateString(HTASKITEM hTask) const;

	bool SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy);
	bool SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate);

	//////////////////////////////////////////////////////////////
	// ITaskList implementation 

	bool IsArchive() const;
	bool IsCheckedOut() const;
	bool IsSourceControlled() const;
	
	const char* GetProjectName() const;
	const char* GetCheckOutTo() const;
	
	unsigned long GetFileFormat() const;
	unsigned long GetFileVersion() const;
	
	time_t GetLastModified() const;

	bool SetProjectName(const char* szName);
	bool SetFileVersion(unsigned long nVersion);

	//////////////////////////////////////////////////////////////

	HTASKITEM NewTask(const char* szTitle, HTASKITEM hParent = NULL);

	HTASKITEM GetFirstTask(HTASKITEM hParent = NULL) const;
	HTASKITEM GetNextTask(HTASKITEM hTask) const;

	const char* GetTaskTitle(HTASKITEM hTask) const;
	const char* GetTaskComments(HTASKITEM hTask) const;
	const char* GetTaskAllocatedTo(HTASKITEM hTask) const;
	const char* GetTaskAllocatedBy(HTASKITEM hTask) const;
	const char* GetTaskCategory(HTASKITEM hTask) const;
	const char* GetTaskStatus(HTASKITEM hTask) const;
	const char* GetTaskFileReferencePath(HTASKITEM hTask) const;
	const char* GetTaskWebColor(HTASKITEM hTask) const;
	const char* GetTaskPriorityWebColor(HTASKITEM hTask) const;

	unsigned long GetTaskID(HTASKITEM hTask) const;
	unsigned long GetTaskColor(HTASKITEM hTask) const;
	unsigned long GetTaskPriorityColor(HTASKITEM hTask) const;

	int GetTaskPriority(HTASKITEM hTask, BOOL bHighest) const;
	unsigned char GetTaskPercentDone(HTASKITEM hTask, BOOL bCalc) const;

	double GetTaskTimeEstimate(HTASKITEM hTask, char& cUnits, BOOL bCalc) const;
	double GetTaskTimeSpent(HTASKITEM hTask, char& cUnits, BOOL bCalc) const;

	time_t GetTaskLastModified(HTASKITEM hTask) const;
	time_t GetTaskDoneDate(HTASKITEM hTask) const;
	time_t GetTaskDueDate(HTASKITEM hTask) const;
	time_t GetTaskStartDate(HTASKITEM hTask) const;

	const char* GetTaskDoneDateString(HTASKITEM hTask) const;
	const char* GetTaskDueDateString(HTASKITEM hTask) const;
	const char* GetTaskStartDateString(HTASKITEM hTask) const;
	
	bool IsTaskDone(HTASKITEM hTask) const;
	bool IsTaskDue(HTASKITEM hTask) const;

	unsigned long GetTaskPosition(HTASKITEM hTask) const;

	bool IsTaskFlagged(HTASKITEM hTask) const;

	bool TaskHasAttribute(HTASKITEM hTask, const char* szAttrib) const;
	const char* GetTaskAttribute(HTASKITEM hTask, const char* szAttrib) const;
	HTASKITEM GetTaskParent(HTASKITEM hTask) const;

	/////////////////////////////////////////////////////

	bool SetTaskTitle(HTASKITEM hTask, const char* szTitle);
	bool SetTaskComments(HTASKITEM hTask, const char* szComments);
	bool SetTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo);
	bool SetTaskAllocatedBy(HTASKITEM hTask, const char* szAllocBy);
	bool SetTaskCategory(HTASKITEM hTask, const char* szCategory);
	bool SetTaskStatus(HTASKITEM hTask, const char* szStatus);
	bool SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath);

	bool SetTaskColor(HTASKITEM hTask, unsigned long nColor);
	bool SetTaskWebColor(HTASKITEM hTask, unsigned long nColor);

	bool SetTaskPriority(HTASKITEM hTask, unsigned char nPriority);
	bool SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent);

	bool SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits);
	bool SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits);

	bool SetTaskLastModified(HTASKITEM hTask, time_t tLastMod);
	bool SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate);
	bool SetTaskDueDate(HTASKITEM hTask, time_t tDueDate);
	bool SetTaskStartDate(HTASKITEM hTask, time_t tStartDate);

	bool SetTaskPosition(HTASKITEM hTask, unsigned long nPos);

	bool SetTaskFlag(HTASKITEM hTask, bool bFlag);

	/////////////////////////////////////////////////////
	// IUnknown implementation
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void) { return 1; } // do nothing
    ULONG STDMETHODCALLTYPE Release( void) { return 1; } // do nothing

protected:
	CMap <HTASKITEM, HTASKITEM, CXmlItem*, CXmlItem*&> m_mapHandles;
	DWORD m_dwNextUniqueID;

protected:
	void BuildHandleMap();
	void AddTaskToMap(CXmlItem* pXITask, BOOL bRecurse);
	void RemoveTaskFromMap(CXmlItem* pXITask);
	CXmlItem* TaskFromHandle(HTASKITEM hTask) const;
	
	double GetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem) const;
	char GetTaskTimeUnits(HTASKITEM hTask, LPCTSTR szUnitsItem) const;
	time_t GetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const;
	COleDateTime GetTaskDateOle(HTASKITEM hTask, LPCTSTR szDateItem, BOOL bIncTime) const;
	unsigned char GetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem) const;
	unsigned long GetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem) const;
	int GetTaskInt(HTASKITEM hTask, LPCTSTR szIntItem) const;
	const char* GetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem) const;
	double GetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem) const;

	bool SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, time_t tVal, BOOL bIncTime);
	bool SetTaskDate(HTASKITEM hTask, LPCTSTR szDateItem, const COleDateTime& tVal, BOOL bIncTime);
	bool SetTaskUChar(HTASKITEM hTask, LPCTSTR szUCharItem, unsigned char cVal);
	bool SetTaskULong(HTASKITEM hTask, LPCTSTR szULongItem, unsigned long lVal);
	bool SetTaskInt(HTASKITEM hTask, LPCTSTR szIntItem, int iVal);
	bool SetTaskCChar(HTASKITEM hTask, LPCTSTR szCCharItem, const char* szVal, XI_TYPE nType = XIT_ATTRIB);
	bool SetTaskDouble(HTASKITEM hTask, LPCTSTR szDoubleItem, double dVal);
	bool SetTaskTime(HTASKITEM hTask, LPCTSTR szTimeItem, double dTime,
					 LPCTSTR szUnitsItem, char cUnits);

	// for handling arrays at *task* level
	bool AddTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, 
						  const char* szItemTag, const char* szItem);
	const char* GetTaskArrayItem(HTASKITEM hTask, const char* szNumItemTag, 
				  				 const char* szItemTag, int nIndex) const;
	BOOL SetTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
				  	 const char* szItemTag, const CStringArray& aItems);
	int GetTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
				  	 const char* szItemTag, CStringArray& aItems) const;
	bool DeleteTaskArray(HTASKITEM hTask, const char* szNumItemTag, 
						 const char* szItemTag);

	// for handling arrays at *tasklist* level
	BOOL SetArray(const char* szItemTag, const CStringArray& aItems);
	int GetArray(const char* szItemTag, CStringArray& aItems) const;

	virtual CXmlItem* NewItem(LPCTSTR szName = NULL);
	BOOL CopyTask(HTASKITEM hSrcTask, const ITaskList* pSrcTasks, HTASKITEM hDestParent);

	static CString GetWebColor(COLORREF color);
};

#endif // !defined(AFX_TASKFILE_H__BA5D71E7_2770_45FD_A693_A2344B589DF4__INCLUDED_)
