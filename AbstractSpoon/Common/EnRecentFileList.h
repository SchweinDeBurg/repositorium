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

// EnRecentFileList.h: interface for the CEnRecentFileList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENRECENTFILELIST_H__AA4B1746_135C_4E81_96E1_DE6CE758EFDC__INCLUDED_)
#define AFX_ENRECENTFILELIST_H__AA4B1746_135C_4E81_96E1_DE6CE758EFDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Preferences.h"

#include <afxadv.h> // for CRecentFileList


class CEnRecentFileList : public CRecentFileList
{
public:
	CEnRecentFileList(UINT nStart, LPCTSTR lpszSection, LPCTSTR lpszEntryFormat,
		int nSize, int nMaxDispLen = AFX_ABBREV_FILENAME_LEN,
		LPCTSTR szOriginal = NULL);
	virtual ~CEnRecentFileList();

	void RemoveAll(BOOL bClearProfile = TRUE);
	virtual void UpdateMenu(CCmdUI* pCmdUI);

	void ReadList(const CPreferences& prefs);
	void WriteList(CPreferences& prefs) const;

};

#endif // !defined(AFX_ENRECENTFILELIST_H__AA4B1746_135C_4E81_96E1_DE6CE758EFDC__INCLUDED_)
