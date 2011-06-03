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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

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

bool CPlainTextImporter::Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile)
{
	COptionsDlg dialog(TRUE);

	if (dialog.DoModal() != IDOK)
	{
		return false;
	}

	INDENT = dialog.GetIndent();

	CStdioFile file;

	if (!file.Open(szSrcFilePath, CFile::modeRead))
	{
		return false;
	}

	// else
	ITaskList4* pITL4 = GetITLInterface<ITaskList4>(pDestTaskFile, IID_TASKLIST4);

	// the first line can be the project name
	if (dialog.GetWantProject())
	{
		CString sProjectName;
		file.ReadString(sProjectName);
		sProjectName.TrimRight();
		sProjectName.TrimLeft();

		pITL4->SetProjectName(ATL::CT2A(sProjectName));
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
		{
			continue;
		}

		// find the appropriate parent fro this task
		HTASKITEM hParent = NULL;
		int nDepth = GetDepth(sLine);

		if (nDepth == nLastDepth) // sibling
		{
			hParent = hLastTask ? pITL4->GetTaskParent(hLastTask) : NULL;
		}

		else if (nDepth > nLastDepth) // child
		{
			hParent = hLastTask;
		}

		else if (hLastTask) // we need to work up the tree
		{
			hParent = pITL4->GetTaskParent(hLastTask);

			while (hParent && nDepth < nLastDepth)
			{
				hParent = pITL4->GetTaskParent(hParent);
				nLastDepth--;
			}
		}

		HTASKITEM hTask = pITL4->NewTask(ATL::CT2A(sTitle), hParent);

		if (!sComments.IsEmpty())
		{
			pITL4->SetTaskComments(hTask, ATL::CT2A(sComments));
		}

		// update state
		hLastTask = hTask;
		nLastDepth = nDepth;
	}

	return true;
}

int CPlainTextImporter::GetDepth(const CString& sLine)
{
	if (INDENT.IsEmpty() || sLine.IsEmpty())
	{
		return 0;
	}

	// else
	int nDepth = 0;

	if (INDENT == _T("\t"))
	{
		while (nDepth < sLine.GetLength())
		{
			if (sLine[nDepth] == _T('\t'))
			{
				nDepth++;
			}
			else
			{
				break;
			}
		}
	}
	else // one or more spaces
	{
		int nPos = 0;

		while (nPos < sLine.GetLength())
		{
			if (sLine.Find(INDENT, nPos) == nPos)
			{
				nDepth++;
			}
			else
			{
				break;
			}

			// next
			nPos = nDepth * INDENT.GetLength();
		}
	}

	// set root depth if not set
	if (ROOTDEPTH == -1)
	{
		ROOTDEPTH = nDepth;
	}

	// and take allowance for it
	nDepth -= ROOTDEPTH;

	return nDepth;
}

BOOL CPlainTextImporter::GetTitleComments(const CString& sLine, CString& sTitle, CString& sComments)
{
	int nDelim = sLine.Find(_T("|"));

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
