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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// EnRecentFileList.cpp: implementation of the CEnRecentFileList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "EnRecentFileList.h"
#include "../../CodeProject/Source/FileMisc.h"

#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnRecentFileList::CEnRecentFileList(UINT nStart, LPCTSTR lpszSection,
	LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen, LPCTSTR szOriginal):
CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{
	m_strOriginal = szOriginal;
}

CEnRecentFileList::~CEnRecentFileList()
{
}

void CEnRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
	if (m_arrNames[0].IsEmpty())
	{
		if (pCmdUI->m_pMenu == NULL)
		{
			return;
		}

		// delete all but the first item
		for (int iMRU = 1; iMRU < m_nSize; iMRU++)
		{
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
		}
	}

	CRecentFileList::UpdateMenu(pCmdUI);
}

void CEnRecentFileList::RemoveAll(BOOL bClearProfile)
{
	int nItem = GetSize();

	while (nItem--)
	{
		Remove(nItem);
	}

	if (bClearProfile)
	{
		CRecentFileList::WriteList();
	}
}

void CEnRecentFileList::WriteList(CPreferences& prefs, BOOL bRelativeToExe) const
{
	for (int nFile = 0; nFile < GetSize(); nFile++)
	{
		CString sFile(m_arrNames[nFile]);

		if (sFile.IsEmpty())
		{
			break;
		}

		CString sItem;
		sItem.Format(m_strEntryFormat, nFile + 1);

		if (bRelativeToExe)
		{
			FileMisc::MakeRelativePath(sFile, FileMisc::GetAppFolder(), FALSE);
		}

		prefs.WriteProfileString(_T("MRU"), sItem, sFile);
	}
}

void CEnRecentFileList::ReadList(const CPreferences& prefs)
{
	for (int nFile = 0; nFile < GetSize(); nFile++)
	{
		CString sItem, sFile;
		sItem.Format(m_strEntryFormat, nFile + 1);
		sFile = prefs.GetProfileString(_T("MRU"), sItem);

		if (!sFile.IsEmpty())
		{
			m_arrNames[nFile] = FileMisc::GetFullPath(sFile, FileMisc::GetAppFolder());
		}
	}
}
