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
//*****************************************************************************

// MergeToDoList.h: interface for the CMergeToDoList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MERGETODOLIST_H__7BFD737D_FF21_46B9_9378_D14BDA114022__INCLUDED_)
#define AFX_MERGETODOLIST_H__7BFD737D_FF21_46B9_9378_D14BDA114022__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

enum TDL_MERGEBY
{
	TDLM_BYTITLE,
	TDLM_BYID
};

enum TDL_MERGEHOW
{
	TDLM_LEAVE,
	TDLM_MOVE,
};

class CXmlItem;

class CMergeToDoList
{
public:
	CMergeToDoList(TDL_MERGEBY nBy, TDL_MERGEHOW nHow = TDLM_MOVE);
	virtual ~CMergeToDoList();

	int Merge(LPCTSTR szSrcPath, LPCTSTR szDestPath);
	int Merge(const CXmlItem* pXISrc, CXmlItem* pXIDest);
	int Merge(const CXmlItem* pXISrc, LPCTSTR szDestPath);
	int Merge(LPCTSTR szSrcPath, CXmlItem* pXIDest);

	DWORD GetNextID()
	{
		return m_dwNextID;
	}

protected:
	CXmlItem* m_pXIDestRoot;
	TDL_MERGEBY m_nMergeBy;
	TDL_MERGEHOW m_nMergeHow;
	DWORD m_dwNextID, m_dwOrgNextID;
	CMap<DWORD, DWORD, CXmlItem*, CXmlItem*&> m_mapID2Item;

protected:
	int MergeTasks(const CXmlItem* pXISrc, CXmlItem* pXIDest);
	BOOL MergeAttributes(const CXmlItem* pXISrc, CXmlItem* pXIDest);

	int MergeTasksByTitle(const CXmlItem* pXISrc, CXmlItem* pXIDest);
	int MergeTasksByID(const CXmlItem* pXISrc, CXmlItem* pXIDest);
	int MergeTaskByID(const CXmlItem* pXISrc, CXmlItem* pXIDestParent);

	CXmlItem* FindDestTask(DWORD dwID);
	void BuildLookupMap();
	void AddTaskToLookupMap(CXmlItem* pXIDest);
};

#endif // !defined(AFX_MERGETODOLIST_H__7BFD737D_FF21_46B9_9378_D14BDA114022__INCLUDED_)
