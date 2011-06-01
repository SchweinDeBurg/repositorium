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
//*****************************************************************************

// TaskListCsvImporter.h: interface for the CTaskListCsvImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTCSVIMPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
#define AFX_TASKLISTCSVIMPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tdcenum.h"
#include "TDLCsvImportExportDlg.h"

#include "..\shared\Itasklist.h"
#include "..\shared\IImportExport.h"

class CTaskListCsvImporter : public IImportTasklist
{
public:
	CTaskListCsvImporter();
	virtual ~CTaskListCsvImporter();

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

	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);
	void Release()
	{
		delete this;
	}

protected:
	LPCTSTR DELIM;
	CString INDENT;
	int ROOTDEPTH;

	CTDCCsvColumnMapping m_aColumnMapping;

protected:
	void ImportTask(ITaskList8* pTasks, const CString& sLine) const;

	void InitConsts();
	int GetDepth(const CString& sLine);

	int AttributeIndex(TDC_ATTRIBUTE attrib) const;
	BOOL WantAttribute(TDC_ATTRIBUTE attrib) const
	{
		return (AttributeIndex(attrib) != -1);
	}

	void PreProcessFileLines(CStringArray& aLines) const;

	void GetTaskAndParentIDs(const CStringArray& sValues, DWORD& dwTaskID, DWORD& dwParentID) const;
	CString GetTaskTitle(const CStringArray& sValues) const;

	static int SortProc(const void* item1, const void* item2);
	static time_t String2Date(const CString& sDate);

	void AddAttributeToTask(ITaskList8* pTasks, HTASKITEM hTask, TDC_ATTRIBUTE nAttrib, const CStringArray& aValues) const;

};

#endif // !defined(AFX_TASKLISTCSVImPORTER_H__ADF211CB_FBD2_42A2_AD51_DFF58E566753__INCLUDED_)
