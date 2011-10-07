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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// TDLImportExportMgr.cpp: implementation of the CTDLImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLImportExportMgr.h"
#include "TasklistHtmlExporter.h"
#include "TaskListTxtExporter.h"
#include "TaskListCsvExporter.h"
#include "TaskListCsvImporter.h"

#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////

#define ExportTaskListTo(FUNCTION)                                  \
                                                                    \
    TCHAR szTempFile[MAX_PATH], szTempPath[MAX_PATH];               \
                                                                    \
    if (!::GetTempPath(MAX_PATH, szTempPath) ||                     \
        !::GetTempFileName(szTempPath, _T("tdl"), 0, szTempFile))   \
        return _T("");                                              \
                                                                    \
    CString sFile;                                                  \
                                                                    \
    if (FUNCTION(pSrcTasks, szTempFile))                            \
        FileMisc::LoadFile(szTempFile, sFile);                      \
                                                                    \
    ::DeleteFile(szTempFile);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDLImportExportMgr::CTDLImportExportMgr()
{
}

CTDLImportExportMgr::~CTDLImportExportMgr()
{
}

void CTDLImportExportMgr::Initialize()
{
	// add html and text exporters first
	if (!m_bInitialized)
	{
		m_aExporters.InsertAt(EXPTOHTML, new CTaskListHtmlExporter);
		m_aExporters.InsertAt(EXPTOTXT, new CTaskListTxtExporter);
		m_aExporters.InsertAt(EXPTOCSV, new CTaskListCsvExporter);

		m_aImporters.InsertAt(IMPFROMCSV, new CTaskListCsvImporter);
	}

	CImportExportMgr::Initialize();
}

BOOL CTDLImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXPTOHTML, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListToHtml)
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListsToHtml(const IMultiTaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskLists(pSrcTasks, szDestFile, EXPTOHTML, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListsToHtml(const IMultiTaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListsToHtml)
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXPTOTXT, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListToText)
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListsToText(const IMultiTaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskLists(pSrcTasks, szDestFile, EXPTOTXT, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListsToText(const IMultiTaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListsToText)
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXPTOCSV, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListToCsv)
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListsToCsv(const IMultiTaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskLists(pSrcTasks, szDestFile, EXPTOCSV, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListsToCsv(const IMultiTaskList* pSrcTasks) const
{
	ExportTaskListTo(ExportTaskListsToCsv)
	return sFile;
}

BOOL CTDLImportExportMgr::ImportTaskListFromCsv(LPCTSTR szSrcFile, ITaskList* pDestTasks) const
{
	return ImportTaskList(szSrcFile, pDestTasks, IMPFROMCSV);
}
