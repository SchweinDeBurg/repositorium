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

// TaskFileHtmlExporter.h: interface for the CTaskListHtmlExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_)
#define AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/ITaskList.h"
#include "../../Common/IImportExport.h"

class CTaskListHtmlExporter : public IExportTasklist
{
public:
	CTaskListHtmlExporter();
	virtual ~CTaskListHtmlExporter();

	const char* GetMenuText()
	{
		return "Web Page";
	}
	const char* GetFileFilter()
	{
		return "Web Pages (*.html)|*.html||";
	}
	const char* GetFileExtension()
	{
		return "html";
	}

	bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	void Release()
	{
		delete this;
	}

protected:
	CString DEFAULTFONT, HTMLNOTES;
	BOOL STRIKETHRUDONE, ROUNDTIMEFRACTIONS;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, const CString& sParentPos, CString& sOutput) const;
	CString FormatCharSet(const ITaskList6* pTasks) const;

	static BOOL FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName,
		LPCTSTR szFormat, CString& sAttribText);
	static BOOL FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask,
		LPCTSTR szNumAttribName, LPCTSTR szAttribName,
		LPCTSTR szFormat, CString& sAttribText);
};

#endif // !defined(AFX_TASKFILEHTMLEXPORTER_H__E4FD92AB_2BF2_40E3_9C8E_5018A72AEA89__INCLUDED_)
