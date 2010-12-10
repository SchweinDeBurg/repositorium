// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#ifndef _TASKINFO_H_
#define _TASKINFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CTaskInfo
{
public:
	CTaskInfo(int _nTaskID = -1, const CString& _strTaskTitle = _T(""), BOOL _bIsStart = FALSE, BOOL _bIsDue = FALSE, BOOL _bIsComplete = FALSE);
	~CTaskInfo();

	CTaskInfo(const CTaskInfo& _that);              //copy constructor
	CTaskInfo& operator=(const CTaskInfo& _that);   //assignment operator

	int     GetTaskID() const;
	CString GetTaskTitle() const;
	BOOL    IsStart() const;
	BOOL    IsDue() const;
	BOOL    IsComplete() const;

protected:
	int     m_nTaskID;
	CString m_strTaskTitle;
	BOOL    m_bIsStart;
	BOOL    m_bIsDue;
	BOOL    m_bIsComplete;
};

typedef CList <CTaskInfo, CTaskInfo&> CTaskInfoList;

#endif//_TASKINFO_H_
