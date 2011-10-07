// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// TaskListTxtExporter.h: interface for the CTaskListTxtExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_)
#define AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/ITaskList.h"
#include "../../Common/IImportExport.h"

class CTaskListTxtExporter : public IExportTasklist
{
public:
	CTaskListTxtExporter();
	virtual ~CTaskListTxtExporter();

	const char* GetMenuText()
	{
		return "Plain Text";
	}
	const char* GetFileFilter()
	{
		return "Text Files (*.txt)|*.txt||";
	}
	const char* GetFileExtension()
	{
		return "txt";
	}

	bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	bool Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	void Release()
	{
		delete this;
	}

protected:
	CString TEXTNOTES, INDENT;
	BOOL ROUNDTIMEFRACTIONS;

protected:
	bool ExportOutput(const TCHAR* szDestFilePath, const CString& sOutput);
	void InitConsts();
	CString& ExportTask(const ITaskList8* pTasks, HTASKITEM hTask, int nDepth, int nPos, const CString& sParentPos,
		CString& sOutput) const;

	static BOOL FormatAttributeList(const ITaskList8* pTasks, HTASKITEM hTask, LPCTSTR szNumAttribName,
		LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText);
	static BOOL FormatAttribute(const ITaskList8* pTasks, HTASKITEM hTask, LPCTSTR szAttribName, LPCTSTR szFormat,
		CString& sAttribText);
};

#endif // !defined(AFX_TASKLISTTXTEXPORTER_H__CF68988D_FBBD_431D_BB56_464E8737D993__INCLUDED_)
