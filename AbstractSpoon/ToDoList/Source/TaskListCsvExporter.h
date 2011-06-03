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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// TaskListCsvExporter.h: interface for the CTaskListCsvExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
#define AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDCEnum.h"
#include "TDLCsvImportExportDlg.h"

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
	bool Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	void Release()
	{
		delete this;
	}

protected:
	BOOL ROUNDTIMEFRACTIONS;
	LPCTSTR DELIM;
	CString INDENT;

	CTDCCsvColumnMapping m_aColumnMapping;

protected:
	CString& ExportTask(const ITaskList8* pTasks, HTASKITEM hTask, int nDepth, int nPos, const CString& sParentPos,
		CString& sOutput) const;
	void InitConsts();

	bool ExportOutput(const TCHAR* szDestFilePath, const CString& sOutput);

	CString ColumnHeadings() const;
	CString CheckGetColumnHeading(TDC_ATTRIBUTE attrib) const;

	void AppendAttribute(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib, LPCTSTR szAttribName,
		LPCTSTR szAltAttribName, CString& sOutput, LPCTSTR szPrefix = NULL) const;
	void AppendAttribute(LPCTSTR szAttrib, CString& sOutput, BOOL bForceQuoted = FALSE) const;
	void AppendAttribute(double dAttrib, LPCTSTR szFormat, CString& sOutput) const;
	void AppendAttribute(int nAttrib, LPCTSTR szFormat, CString& sOutput) const;
	void AppendComments(const ITaskList8* pTasks, HTASKITEM hTask, CString& sOutput) const;
	void AppendAttributeList(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib, LPCTSTR szNumAttribName,
		LPCTSTR szAttribName, CString& sOutput) const;

	int AttributeIndex(TDC_ATTRIBUTE attrib) const;
	BOOL WantAttribute(TDC_ATTRIBUTE attrib) const
	{
		return (AttributeIndex(attrib) != -1);
	}

	static int BuildAttribList(const ITaskList8* pTasks, HTASKITEM hTask, CTDCAttributeArray& aAttributes);
	static void CheckAddAttribtoList(const ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE attrib,
			LPCTSTR szAttribName, CTDCAttributeArray& aAttrib);

	CString CalcIndent(int nDepth) const;

};

#endif // !defined(AFX_TASKLISTCSVEXPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
