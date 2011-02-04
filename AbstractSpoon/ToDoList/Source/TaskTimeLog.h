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
// - merged with ToDoList version 6.1.6 sources
//*****************************************************************************

// TaskTimeLog.h: interface for the CTaskTimeLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_)
#define AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTaskTimeLog
{
public:
	CTaskTimeLog(LPCTSTR szRefPath);
	virtual ~CTaskTimeLog();

	BOOL LogTime(DWORD dwTaskID, LPCTSTR szTaskTitle, double dTime, BOOL bLogSeparately); // time must be in hours
	BOOL LogTime(DWORD dwTaskID, LPCTSTR szTaskTitle, double dTime, COleDateTime dtWhen, BOOL bLogSeparately); // time must be in hours
	double CalcAccumulatedTime(DWORD dwTaskID, BOOL bLogSeparately); // returns time in hours
	CString GetLogPath(DWORD dwTaskID, BOOL bLogSeparately);

protected:
	CString m_sRefPath;

protected:
	static CString BuildColumnHeader();
	static CString BuildRowFormat();
};

#endif // !defined(AFX_TASKTIMELOG_H__6C9F21CD_509E_4890_9B28_F8C6E52FF54B__INCLUDED_)
