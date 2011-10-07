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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// PlainTextExporter.cpp: implementation of the CPlainTextExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PlainTextImportApp.h"
#include "PlainTextExporter.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = _T("\n");

CPlainTextExporter::CPlainTextExporter():
INDENT(CString(_T(' '), 2)),
WANTPROJECT(FALSE)
{
}

CPlainTextExporter::~CPlainTextExporter()
{
}

bool CPlainTextExporter::InitConsts(BOOL bSilent)
{
	WANTPROJECT = FALSE;
	INDENT = CString(_T(' '), 2);

	if (!bSilent)
	{
		COptionsDlg dialog(FALSE);

		if (dialog.DoModal() != IDOK)
		{
			return false;
		}

		INDENT = dialog.GetIndent();
		WANTPROJECT = dialog.GetWantProject();
	}

	return true;
}

bool CPlainTextExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent)
{
	if (!InitConsts(bSilent))
	{
		return false;
	}

	CStdioFile fileOut;

	if (!fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		return false;
	}

	// else
	const ITaskList4* pITL4 = GetITLInterface<ITaskList4>(pSrcTaskFile, IID_TASKLIST4);

	// export report title as dummy task
	if (WANTPROJECT)
	{
		CString sTitle = pITL4->GetReportTitle();

		if (sTitle.IsEmpty())
		{
			sTitle = pITL4->GetProjectName();
		}

		// note: we export the title even if it's empty
		// to maintain consistency with the importer that the first line
		// is always the outline name
		sTitle += ENDL;
		fileOut.WriteString(sTitle);
	}

	// export first task
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), fileOut, 0);

	return true;
}

bool CPlainTextExporter::Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent)
{
	if (!InitConsts(bSilent))
	{
		return false;
	}

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		for (int nTaskList = 0; nTaskList < pSrcTaskFile->GetTaskListCount(); nTaskList++)
		{
			const ITaskList4* pITL4 = GetITLInterface<ITaskList4>(pSrcTaskFile->GetTaskList(nTaskList), IID_TASKLIST4);

			if (pITL4)
			{
				// export report title as dummy task
				if (WANTPROJECT)
				{
					CString sTitle = pITL4->GetReportTitle();

					if (sTitle.IsEmpty())
					{
						sTitle = pITL4->GetProjectName();
					}

					// note: we export the title even if it's empty
					// to maintain consistency with the importer that the first line
					// is always the outline name
					sTitle += ENDL;
					fileOut.WriteString(sTitle);
				}

				// export first task
				ExportTask(pITL4, pITL4->GetFirstTask(), fileOut, 0);
			}
		}

		return true;
	}

	// else
	return false;
}

void CPlainTextExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, CStdioFile& fileOut, int nDepth)
{
	if (!hTask)
	{
		return;
	}

	// export each task as '[indent]title|comments' on a single line
	CString sTask;

	// indent
	for (int nTab = 0; nTab < nDepth; nTab++)
	{
		sTask += INDENT;
	}

	// title
	sTask += pSrcTaskFile->GetTaskTitle(hTask);

	// comments
	CString sComments = pSrcTaskFile->GetTaskComments(hTask);

	if (!sComments.IsEmpty())
	{
		// remove all carriage returns
		sComments.Replace(_T("\r\n"), _T(""));
		sComments.Replace(_T("\n"), _T(""));

		sTask += '|';
		sTask += sComments;
	}

	// add carriage return
	sTask += ENDL;

	// save write to file
	fileOut.WriteString(sTask);

	// copy across first child
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(hTask), fileOut, nDepth + 1);

	// copy across first sibling
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetNextTask(hTask), fileOut, nDepth);
}
