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

#if !defined(_ITASKLIST_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define _ITASKLIST_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#pragma once

// ITaskList.h: interface and implementation of the ITaskList class.
//
/////////////////////////////////////////////////////////////////////////////////

#define ITASKLISTBASE ITaskList7 // latest interface

// extended interface IDs
static const GUID IID_TASKLIST2 = { 0x41d9fd9e, 0xaa1f, 0x4ee0, { 0x86, 0x05, 0xeb, 0x3f, 0x64, 0x7e, 0x70, 0xf6 } };
static const GUID IID_TASKLIST3 = { 0xb75b2120, 0x267d, 0x4a82, { 0xab, 0x58, 0x1a, 0x45, 0x40, 0x50, 0xa, 0x1a } };
static const GUID IID_TASKLIST4 = { 0xd063e3de, 0x83d1, 0x40d7, { 0xbc, 0x2d, 0xfa, 0x24, 0x4, 0x85, 0x57, 0xed } };
static const GUID IID_TASKLIST5 = { 0x5a1ac54b, 0x84f, 0x4299, { 0xb5, 0xaa, 0x2b, 0x93, 0x22, 0xf1, 0x3d, 0xc0 } };
static const GUID IID_TASKLIST6 = { 0xb782136e, 0x546f, 0x4184, { 0xab, 0xaf, 0xe9, 0x7, 0xdd, 0xf6, 0x91, 0x81 } };
static const GUID IID_TASKLIST7 = { 0x65781a9b, 0xced2, 0x490a, { 0xa9, 0x39, 0x71, 0x6d, 0xd9, 0x18, 0x53, 0x33 } };

typedef void* HTASKITEM;
class ITaskList;

// handy template functions for extracting interfaces
template <class TLInterface>
const TLInterface* GetITLInterface(const ITaskList* pTasks, const GUID& IID)
{
	TLInterface* pInterface = 0;
	ITaskList* pNCTasks = const_cast<ITaskList*>(pTasks);

	pNCTasks->QueryInterface(IID, reinterpret_cast<void**>(&pInterface));

	return reinterpret_cast<TLInterface*>(pInterface);
}

template <class TLInterface>
TLInterface* GetITLInterface(ITaskList* pTasks, const GUID& IID)
{
	TLInterface* pInterface = 0;
	pTasks->QueryInterface(IID, reinterpret_cast<void**>(&pInterface));

	return reinterpret_cast<TLInterface*>(pInterface);
}

class ITaskList : public IUnknown
{
public:
	// file level methods
	virtual bool IsArchive() const = 0;
	virtual bool IsCheckedOut() const = 0;
	virtual bool IsSourceControlled() const = 0;

	virtual const char* GetProjectName() const = 0;
	virtual const char* GetCheckOutTo() const = 0;

	virtual unsigned long GetFileFormat() const = 0;
	virtual unsigned long GetFileVersion() const = 0;

	virtual bool SetProjectName(const char* szName) = 0;
	virtual bool SetFileVersion(unsigned long nVersion) = 0;

	// task level methods
	virtual HTASKITEM NewTask(const char* szTitle, HTASKITEM hParent = NULL) = 0;

	virtual HTASKITEM GetFirstTask(HTASKITEM hParent = NULL) const = 0;
	virtual HTASKITEM GetNextTask(HTASKITEM hTask) const = 0;

	// get methods
	virtual const char* GetTaskTitle(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskComments(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskAllocatedTo(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskAllocatedBy(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskCategory(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskStatus(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskFileReferencePath(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskWebColor(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskPriorityWebColor(HTASKITEM hTask) const = 0;

	virtual unsigned long GetTaskID(HTASKITEM hTask) const = 0;
	virtual unsigned long GetTaskColor(HTASKITEM hTask) const = 0;
	virtual unsigned long GetTaskPriorityColor(HTASKITEM hTask) const = 0;

	virtual int GetTaskPriority(HTASKITEM hTask, BOOL bHighest) const = 0;
	virtual unsigned char GetTaskPercentDone(HTASKITEM hTask, BOOL bCalc) const = 0;

	virtual double GetTaskTimeEstimate(HTASKITEM hTask, char& cUnits, BOOL bCalc) const = 0;
	virtual double GetTaskTimeSpent(HTASKITEM hTask, char& cUnits, BOOL bCalc) const = 0;

	virtual time_t GetTaskLastModified(HTASKITEM hTask) const = 0;
	virtual time_t GetTaskDoneDate(HTASKITEM hTask) const = 0;
	virtual time_t GetTaskDueDate(HTASKITEM hTask) const = 0;
	virtual time_t GetTaskStartDate(HTASKITEM hTask) const = 0;

	virtual const char* GetTaskDoneDateString(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskDueDateString(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskStartDateString(HTASKITEM hTask) const = 0;

	virtual bool IsTaskDone(HTASKITEM hTask) const = 0;
	virtual bool IsTaskDue(HTASKITEM hTask) const = 0;

	virtual unsigned long GetTaskPosition(HTASKITEM hTask) const = 0;

	virtual bool TaskHasAttribute(HTASKITEM hTask, const char* szAttrib) const = 0;
	virtual const char* GetTaskAttribute(HTASKITEM hTask, const char* szAttrib) const = 0;
	virtual HTASKITEM GetTaskParent(HTASKITEM hTask) const = 0;

	// set methods
	virtual bool SetTaskTitle(HTASKITEM hTask, const char* szTitle) = 0;
	virtual bool SetTaskComments(HTASKITEM hTask, const char* szComments) = 0;
	virtual bool SetTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo) = 0;
	virtual bool SetTaskAllocatedBy(HTASKITEM hTask, const char* szAllocBy) = 0;
	virtual bool SetTaskCategory(HTASKITEM hTask, const char* szCategory) = 0;
	virtual bool SetTaskStatus(HTASKITEM hTask, const char* szStatus) = 0;
	virtual bool SetTaskFileReferencePath(HTASKITEM hTask, const char* szFileRefpath) = 0;

	virtual bool SetTaskColor(HTASKITEM hTask, unsigned long nColor) = 0;

	virtual bool SetTaskPriority(HTASKITEM hTask, unsigned char nPriority) = 0;
	virtual bool SetTaskPercentDone(HTASKITEM hTask, unsigned char nPercent) = 0;

	virtual bool SetTaskTimeEstimate(HTASKITEM hTask, double dTimeEst, char cUnits) = 0;
	virtual bool SetTaskTimeSpent(HTASKITEM hTask, double dTimeSpent, char cUnits) = 0;

	virtual bool SetTaskLastModified(HTASKITEM hTask, time_t tLastMod) = 0;
	virtual bool SetTaskDoneDate(HTASKITEM hTask, time_t tDoneDate) = 0;
	virtual bool SetTaskDueDate(HTASKITEM hTask, time_t tDueDate) = 0;
	virtual bool SetTaskStartDate(HTASKITEM hTask, time_t tStartDate) = 0;

	virtual bool SetTaskPosition(HTASKITEM hTask, unsigned long nPos) = 0;

	// new methods
	virtual bool SetTaskFlag(HTASKITEM hTask, bool bFlag) = 0;
	virtual bool IsTaskFlagged(HTASKITEM hTask) const = 0;

};

class ITaskList2 : public ITaskList
{
	// new methods
public:
	virtual const char* GetTaskCreatedBy(HTASKITEM hTask) const = 0;
	virtual time_t GetTaskCreationDate(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskCreationDateString(HTASKITEM hTask) const = 0;

	virtual bool SetTaskCreatedBy(HTASKITEM hTask, const char* szCreatedBy) = 0;
	virtual bool SetTaskCreationDate(HTASKITEM hTask, time_t tCreationDate) = 0;
};

class ITaskList3 : public ITaskList2
{
	// new methods
public:
	virtual time_t GetTaskDueDate(HTASKITEM hTask, BOOL bEarliest) const = 0;
	virtual const char* GetTaskDueDateString(HTASKITEM hTask, BOOL bEarliest) const = 0;
	virtual unsigned long GetTaskTextColor(HTASKITEM hTask) const = 0;
	virtual int GetTaskRisk(HTASKITEM hTask, BOOL bHighest) const = 0;
	virtual const char* GetTaskExternalID(HTASKITEM hTask) const = 0;

	virtual bool SetTaskRisk(HTASKITEM hTask, unsigned char nRisk) = 0;
	virtual bool SetTaskExternalID(HTASKITEM hTask, const char* szID) = 0;
};

class ITaskList4 : public ITaskList3
{
	// new methods
public:
	virtual const char* GetAttribute(const char* szAttrib) const = 0;

	virtual const char* GetHtmlCharSet() const = 0;
	virtual const char* GetReportTitle() const = 0;
	virtual const char* GetReportDate() const = 0;
	virtual double GetTaskCost(HTASKITEM hTask, BOOL bCalc) const = 0;
	virtual unsigned char GetTaskCategoryCount(HTASKITEM hTask) const = 0;
	virtual const char* GetTaskCategory(HTASKITEM hTask, int nIndex) const = 0;
	virtual const char* GetTaskDependency(HTASKITEM hTask) const = 0;

	virtual bool SetTaskCost(HTASKITEM hTask, double dCost) = 0;
	virtual bool SetTaskDependency(HTASKITEM hTask, const char* szDepends) = 0;
};

class ITaskList5 : public ITaskList4
{
	// new methods
public:
	virtual bool AddTaskCategory(HTASKITEM hTask, const char* szCategory) = 0;
};

class ITaskList6 : public ITaskList5
{
	// new methods
public:
	virtual bool SetTaskRecurrence(HTASKITEM hTask, int nRegularity, DWORD dwSpecific1,
		DWORD dwSpecific2, BOOL bRecalcFromDue, int nReuse) = 0;
	virtual bool GetTaskRecurrence(HTASKITEM hTask, int& nRegularity, DWORD& dwSpecific1,
		DWORD& dwSpecific2, BOOL& bRecalcFromDue, int& nReuse) const = 0;

	virtual bool SetTaskVersion(HTASKITEM hTask, const char* szVersion) = 0;
	virtual const char* GetTaskVersion(HTASKITEM hTask) const = 0;
};

class ITaskList7 : public ITaskList6
{
	// new methods
public:
	virtual unsigned char GetTaskDependencyCount(HTASKITEM hTask) const = 0;
	virtual bool AddTaskDependency(HTASKITEM hTask, const char* szDepends) = 0;
	virtual const char* GetTaskDependency(HTASKITEM hTask, int nIndex) const = 0;

	virtual unsigned char GetTaskAllocatedToCount(HTASKITEM hTask) const = 0;
	virtual bool AddTaskAllocatedTo(HTASKITEM hTask, const char* szAllocTo) = 0;
	virtual const char* GetTaskAllocatedTo(HTASKITEM hTask, int nIndex) const = 0;
};

#endif // _ITASKLIST_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
