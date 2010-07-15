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

	const char* GetMenuText() { return "Spreadsheet"; }
	const char* GetFileFilter() { return "Spreadsheet Files (*.csv)|*.csv||"; }
	const char* GetFileExtension() { return "csv"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);
    void Release() { delete this; }

protected:
	BOOL ROUNDTIMEFRACTIONS;
	LPCTSTR DELIM;
	CString INDENT;
	CStringArray m_aAttribs;

protected:
	CString& ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, 
						int nPos, const CString& sParentPos, CString& sOutput) const;
	CString ColumnHeadings() const;

	void AppendAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
						LPCTSTR szAttribName, LPCTSTR szAltAttribName, 
                        CString& sOutput, LPCTSTR szPrefix = NULL) const;
	void AppendAttribute(LPCTSTR szAttrib, CString& sOutput) const;
	void AppendAttribute(double dAttrib, LPCTSTR szFormat, 
								CString& sOutput) const;
	void AppendAttribute(int nAttrib, LPCTSTR szFormat, CString& sOutput) const;

	int BuildAttribList(const ITaskList6* pTasks, HTASKITEM hTask);
	BOOL WantAttribute(LPCTSTR szAttribName) const;
	void CheckAddAttribtoList(const ITaskList6* pTasks, HTASKITEM hTask, LPCTSTR szAttribName);

	CString CheckGetColumnHeading(LPCTSTR szAttribName, UINT nIDHeading) const;

	void AppendAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                          CString& sOutput) const;
	CString CalcIndent(int nDepth) const;

};

#endif // !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
