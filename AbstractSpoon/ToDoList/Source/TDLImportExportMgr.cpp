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

// TDLImportExportMgr.cpp: implementation of the CTDLImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLImportExportMgr.h"
#include "TasklistHtmlExporter.h"
#include "TaskListTxtExporter.h"
#include "TaskListCsvExporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
		m_aExporters.InsertAt(EXP2HTML, new CTaskListHtmlExporter);
		m_aExporters.InsertAt(EXP2TXT, new CTaskListTxtExporter);
		m_aExporters.InsertAt(EXP2CSV, new CTaskListCsvExporter);
	}

	CImportExportMgr::Initialize();
}

BOOL CTDLImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2HTML, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToHtml(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToHtml(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine;
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2TXT, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToText(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToText(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine + "\n";
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}

BOOL CTDLImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const
{
	return ExportTaskList(pSrcTasks, szDestFile, EXP2CSV, FALSE);
}

CString CTDLImportExportMgr::ExportTaskListToCsv(const ITaskList* pSrcTasks) const
{
	char szTempFile[MAX_PATH], szTempPath[MAX_PATH];
		
	if (!::GetTempPath(MAX_PATH, szTempPath) || 
		!::GetTempFileName(szTempPath, "tdl", 0, szTempFile))
		return "";
		
	CString sFile;

	if (ExportTaskListToCsv(pSrcTasks, szTempFile))
	{
		CStdioFile file;

		if (file.Open(szTempFile, CFile::modeRead))
		{
			CString sLine;

			while (file.ReadString(sLine))
				sFile += sLine + "\n";
		}
	}

	::DeleteFile(szTempFile);
	return sFile;
}
