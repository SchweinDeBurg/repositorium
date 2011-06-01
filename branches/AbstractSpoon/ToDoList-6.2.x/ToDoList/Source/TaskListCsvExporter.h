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
// - merged with ToDoList versions 6.1.2-6.1.7 sources
//*****************************************************************************

// TaskListCsvExporter.h: interface for the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
#define AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/ITaskList.h"
#include "../../Common/IImportExport.h"

class CTaskListCsvExporter : public IExportTasklist
{
public:
	CTaskListCsvExporter();
	virtual ~CTaskListCsvExporter();

	const char* GetMenuText()
	{
		return "Spreadsheet";
	}
	const char* GetFileFilter()
	{
		return "Spreadsheet Files (*.csv)|*.csv||";
	}
	const char* GetFileExtension()
	{
		return "csv";
	}

	bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	void Release()
	{
		delete this;
	}

protected:
	BOOL ROUNDTIMEFRACTIONS;
	LPCTSTR DELIM;
	CString INDENT;
	CStringArray m_aAttribs;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth,
		int nPos, const CString& sParentPos, CString& sOutput) const;
	CString ColumnHeadings() const;

	enum ATTRIBTYPE { AT_TEXT, AT_COST, AT_TIME };

	void AppendAttribute(const ITaskList6* pTasks, HTASKITEM hTask,
		LPCTSTR szAttribName, LPCTSTR szAltAttribName,
		CString& sOutput, ATTRIBTYPE at = AT_TEXT, LPCTSTR szPrefix = NULL) const;

	void AppendAttribute(LPCTSTR szAttrib, CString& sOutput, ATTRIBTYPE at = AT_TEXT, BOOL bForceQuoted = FALSE) const;
	void AppendComments(const ITaskList6* pTasks, HTASKITEM hTask, CString& sOutput) const;

	int BuildAttribList(const ITaskList6* pTasks, HTASKITEM hTask);
	BOOL WantAttribute(LPCTSTR szAttribName) const;
	void CheckAddAttribtoList(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName);

	CString CheckGetColumnHeading(LPCTSTR szAttribName, UINT nIDHeading) const;

	void AppendAttributeList(const ITaskList6* pTasks, HTASKITEM hTask,
		LPCTSTR szNumAttribName, LPCTSTR szAttribName,
		CString& sOutput) const;
	CString CalcIndent(int nDepth) const;
	void RemoveTrailingDelimiter(CString& sOutput) const;
};

#endif // !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
