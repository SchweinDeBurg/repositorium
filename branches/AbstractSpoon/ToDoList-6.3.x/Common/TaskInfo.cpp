// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
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
//*****************************************************************************

#include "StdAfx.h"
#include "TaskInfo.h"

CTaskInfo::CTaskInfo(int _nTaskID, const CString& _strTaskTitle, BOOL _bIsStart, BOOL _bIsDue, BOOL _bIsComplete):
m_nTaskID(_nTaskID),
m_strTaskTitle(_strTaskTitle),
m_bIsStart(_bIsStart),
m_bIsDue(_bIsDue),
m_bIsComplete(_bIsComplete)
{
}

CTaskInfo::~CTaskInfo()
{
}

//copy constructor
CTaskInfo::CTaskInfo(const CTaskInfo& _that)
{
	*this = _that;
}

//assignment operator
CTaskInfo& CTaskInfo::operator=(const CTaskInfo& _that)
{
	m_nTaskID = _that.m_nTaskID;
	m_strTaskTitle = _that.m_strTaskTitle;
	m_bIsStart = _that.m_bIsStart;
	m_bIsDue = _that.m_bIsDue;
	m_bIsComplete = _that.m_bIsComplete;

	return *this;
}

int CTaskInfo::GetTaskID() const
{
	return m_nTaskID;
}

CString CTaskInfo::GetTaskTitle() const
{
	return m_strTaskTitle;
}

BOOL CTaskInfo::IsStart() const
{
	return m_bIsStart;
}

BOOL CTaskInfo::IsDue() const
{
	return m_bIsDue;
}

BOOL CTaskInfo::IsComplete() const
{
	return m_bIsComplete;
}
