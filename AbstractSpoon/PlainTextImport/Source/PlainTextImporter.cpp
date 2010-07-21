// PlainTextImporter.cpp: implementation of the CPlainTextImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PlainTextImporter.h"
#include "OptionsDlg.h"

#include <time.h>
#include <unknwn.h>

#include "../../Common/ITaskList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlainTextImporter::CPlainTextImporter()
{

}

CPlainTextImporter::~CPlainTextImporter()
{

}

bool CPlainTextImporter::Import(const char* szSrcFilePath, ITaskList* pDestTaskFile)
{
	COptionsDlg dialog(TRUE);

	if (dialog.DoModal() != IDOK)
		return false;

	INDENT = dialog.GetIndent();

	CStdioFile file;

	if (!file.Open(szSrcFilePath, CFile::modeRead))
		return false;

	// else
	ITaskList4* pITL4 = static_cast<ITaskList4*>(pDestTaskFile);

	// the first line can be the project name
	if (dialog.GetWantProject())
	{
		CString sProjectName;
		file.ReadString(sProjectName);
		sProjectName.TrimRight();
		sProjectName.TrimLeft();

		pITL4->SetProjectName(sProjectName);
	}

	// what follows are the tasks, indented to express subtasks
	int nLastDepth = 0;
	HTASKITEM hLastTask = NULL;

	ROOTDEPTH = -1; // gets set to the first task's depth

	CString sLine;
	
	while (file.ReadString(sLine)) 
	{
		CString sTitle, sComments;

		if (!GetTitleComments(sLine, sTitle, sComments))
			continue;

		// find the appropriate parent fro this task
		HTASKITEM hParent = NULL;
		int nDepth = GetDepth(sLine);

		if (nDepth == nLastDepth) // sibling
			hParent = hLastTask ? pITL4->GetTaskParent(hLastTask) : NULL;

		else if (nDepth > nLastDepth) // child
			hParent = hLastTask;

		else if (hLastTask) // we need to work up the tree
		{
			hParent = pITL4->GetTaskParent(hLastTask);

			while (hParent && nDepth < nLastDepth)
			{
				hParent = pITL4->GetTaskParent(hParent);
				nLastDepth--;
			}
		}
		
		HTASKITEM hTask = pITL4->NewTask(sTitle, hParent);

		if (!sComments.IsEmpty())
			pITL4->SetTaskComments(hTask, sComments);

		// update state
		hLastTask = hTask;
		nLastDepth = nDepth;
	}

	return true;
}

int CPlainTextImporter::GetDepth(const CString& sLine)
{
	if (INDENT.IsEmpty() || sLine.IsEmpty())
		return 0;

	// else
	int nDepth = 0;
	
	if (INDENT == "\t")
	{
		while (nDepth < sLine.GetLength())
		{
			if (sLine[nDepth] == '\t')
				nDepth++;
			else
				break;
		}
	}
	else // one or more spaces
	{
		int nPos = 0;

		while (nPos < sLine.GetLength())
		{
			if (sLine.Find(INDENT, nPos) == nPos)
				nDepth++;
			else
				break;

			// next
			nPos = nDepth * INDENT.GetLength();
		}
	}

	// set root depth if not set 
	if (ROOTDEPTH == -1)
		ROOTDEPTH = nDepth;

	// and take allowance for it
	nDepth -= ROOTDEPTH;

	return nDepth;
}

BOOL CPlainTextImporter::GetTitleComments(const CString& sLine, 
										  CString& sTitle, CString& sComments)
{
	int nDelim = sLine.Find("|"); 
	
	if (nDelim != -1)
	{
		sTitle = sLine.Left(nDelim);
		sComments = sLine.Mid(nDelim + 1);
	}
	else
	{
		sTitle = sLine;
		sComments.Empty();
	}

	// cleanup
	sTitle.TrimLeft();
	sTitle.TrimRight();
	sComments.TrimLeft();
	sComments.TrimRight();

	return sTitle.GetLength();
}
