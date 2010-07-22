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

// PlainTextExporter.cpp: implementation of the CPlainTextExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PlainTextImportApp.h"
#include "PlainTextExporter.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = "\n";

CPlainTextExporter::CPlainTextExporter()
{
	INDENT = "\x20\x20";
}

CPlainTextExporter::~CPlainTextExporter()
{

}

bool CPlainTextExporter::Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent)
{
	BOOL bWantProject = FALSE;

	if (!bSilent)
	{
		COptionsDlg dialog(FALSE);

		if (dialog.DoModal() != IDOK)
			return false;
		
		INDENT = dialog.GetIndent();
		bWantProject = dialog.GetWantProject();
	}

	CStdioFile fileOut;

	if (!fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
		return false;

	// else
	int nDepth = 0;
	const ITaskList4* pITL4 = static_cast<const ITaskList4*>(pSrcTaskFile);
	
	// export report title as dummy task
	if (bWantProject)
	{
		CString sTitle = pITL4->GetReportTitle();
		
		if (sTitle.IsEmpty())
			sTitle = pITL4->GetProjectName();
		
//		if (!sTitle.IsEmpty())
//			nDepth++;
	
		// note: we export the title even if it's empty
		// to maintain consistency with the importer that the first line
		// is always the outline name
		sTitle += ENDL;
		fileOut.WriteString(sTitle);
	}
	
	// export first task
	ExportTask(pSrcTaskFile, pSrcTaskFile->GetFirstTask(), fileOut, nDepth);
	
	return true;
}

void CPlainTextExporter::ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, 
									CStdioFile& fileOut, int nDepth)
{
	if (!hTask)
		return;

	// export each task as '[indent]title|comments' on a single line
	CString sTask;

	// indent
	for (int nTab = 0; nTab < nDepth; nTab++)
		sTask += INDENT;

	// title
	sTask += pSrcTaskFile->GetTaskTitle(hTask);

	// comments
	CString sComments = pSrcTaskFile->GetTaskComments(hTask);

	if (!sComments.IsEmpty())
	{
		// remove all carriage returns
		sComments.Replace("\r\n", "");
		sComments.Replace("\n", "");

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
