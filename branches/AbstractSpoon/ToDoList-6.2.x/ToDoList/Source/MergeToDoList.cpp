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

// MergeToDoList.cpp: implementation of the CMergeToDoList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MergeToDoList.h"
#include "TDLSchemaDef.h"
#include "TaskFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMergeToDoList::CMergeToDoList(TDL_MERGEBY nBy, TDL_MERGEHOW nHow):
m_pXIDestRoot(NULL),
m_nMergeBy(nBy),
m_nMergeHow(nHow),
m_dwNextID(0)
{
}

CMergeToDoList::~CMergeToDoList()
{
}

int CMergeToDoList::Merge(LPCTSTR szSrcPath, LPCTSTR szDestPath)
{
	CTaskFile fileSrc(TDL_ROOT);

	if (fileSrc.Load(szSrcPath))
	{
		return Merge(fileSrc.Root(), szDestPath);
	}

	return 0;
}

int CMergeToDoList::Merge(const CXmlItem* pXISrc, CXmlItem* pXIDest)
{
	// all the external methods end up here
	m_pXIDestRoot = pXIDest;
	m_dwNextID = pXIDest->GetItemValueI(TDL_NEXTUNIQUEID);

	int nRes = MergeTasks(pXISrc, pXIDest);

	m_pXIDestRoot = NULL;

	return nRes;
}

int CMergeToDoList::Merge(const CXmlItem* pXISrc, LPCTSTR szDestPath)
{
	CTaskFile fileDest(TDL_ROOT);

	if (fileDest.Load(szDestPath))
	{
		return Merge(pXISrc, fileDest.Root());
	}

	return 0;
}

int CMergeToDoList::Merge(LPCTSTR szSrcPath, CXmlItem* pXIDest)
{
	CTaskFile fileSrc(TDL_ROOT);

	if (fileSrc.Load(szSrcPath))
	{
		return Merge(fileSrc.Root(), pXIDest);
	}

	return 0;
}

int CMergeToDoList::MergeTasks(const CXmlItem* pXISrc, CXmlItem* pXIDest)
{
	// simple checks first
	if (!pXISrc || !pXIDest)
	{
		return 0;
	}

	ASSERT(!(m_nMergeBy == TDLM_BYTITLE && m_dwNextID == 0));

	if (m_nMergeBy == TDLM_BYTITLE && m_dwNextID == 0)
	{
		return 0;
	}

	BuildLookupMap();

	switch (m_nMergeBy)
	{
	case TDLM_BYID:
		return MergeTasksByID(pXISrc, pXIDest);

	case TDLM_BYTITLE:
		return MergeTasksByTitle(pXISrc, pXIDest);
	}

	// else
	return 0;
}

int CMergeToDoList::MergeTasksByID(const CXmlItem* pXISrc, CXmlItem* pXIDest)
{
	// task ID must be same for both
	DWORD dwSrcID = pXISrc->GetItemValueI(TDL_TASKID);
	DWORD dwDestID = pXIDest->GetItemValueI(TDL_TASKID);

	if (dwSrcID != dwDestID)
	{
		return 0;
	}

	// if the item is a task (could be the root) merge task attributes
	int nMerged = 0;

	if (dwSrcID)
	{
		// keep track of max ID
		m_dwNextID = max(m_dwNextID, dwSrcID + 1);

		if (MergeAttributes(pXISrc, pXIDest))
		{
			nMerged++;
		}
	}

	// now merge child items individually.
	const CXmlItem* pXISrcChild = pXISrc->GetItem(TDL_TASK);

	if (pXISrcChild)
	{
		nMerged += MergeTaskByID(pXISrcChild, pXIDest);
	}

	// then merge next sibling similarly (if we are a task)
	if (dwSrcID)
	{
		const CXmlItem* pXISrcNext = pXISrc->GetSibling();

		if (pXISrcNext)
		{
			nMerged += MergeTaskByID(pXISrcNext, pXIDest->GetParent());
		}
	}

	return nMerged;
}

int CMergeToDoList::MergeTaskByID(const CXmlItem* pXISrc, CXmlItem* pXIDestParent)
{
	ASSERT(pXISrc);
	ASSERT(pXIDestParent);

	DWORD dwID = pXISrc->GetItemValueI(TDL_TASKID);
	ASSERT(dwID);

	int nMerged = 0;

	// look up this item in the dest by ID
	CXmlItem* pXIDest = FindDestTask(dwID);

	// if no such item exists then simply add child to dest
	if (!pXIDest)
	{
		// create placeholder
		pXIDest = pXIDestParent->AddItem(TDL_TASK);

		// set the id
		pXIDest->AddItem(TDL_TASKID, (int)dwID);
	}
	else
	{
		// else if the dest items parent is not pXIDest and the user wants to
		// move existing items then first move the item before merging
		CXmlItem* pXICurDestParent = pXIDest->GetParent();

		if (pXICurDestParent != pXIDestParent && m_nMergeHow == TDLM_MOVE)
		{
			if (pXICurDestParent->RemoveItem(pXIDest))
			{
				pXIDest = pXIDestParent->AddItem(pXIDest);
			}
		}
	}

	// then merge attributes
	nMerged = MergeTasksByID(pXISrc, pXIDest);

	return nMerged;
}

int CMergeToDoList::MergeTasksByTitle(const CXmlItem* pXISrc, CXmlItem* pXIDest)
{
	// merge our attributes first
	int nMerged = 0;

	// if both these items are root items (have no parents) then we just
	// merge their children else if the task name does not match then we don't merge
	if (pXISrc->GetParent())
	{
		ASSERT(pXISrc->NameMatches(TDL_TASK));
		ASSERT(pXISrc->NameMatches(pXIDest));
		ASSERT(pXIDest->GetParent());
		ASSERT(pXISrc->ItemValueMatches(pXIDest, TDL_TASKTITLE));

		if (!pXISrc->ItemValueMatches(pXIDest, TDL_TASKTITLE))
		{
			return 0;
		}
	}
	else
	{
		ASSERT(!pXIDest->GetParent());
	}

	if (MergeAttributes(pXISrc, pXIDest))
	{
		nMerged++;
	}

	// merge children
	// essentially we simply add whatever tasks exist in pXISrc
	// which do not exist in pXIDest and merge duplicates
	const CXmlItem* pXISrcChild = pXISrc->GetItem(TDL_TASK);

	while (pXISrcChild)
	{
		// cycle pXIDest children looking for a match
		CXmlItem* pXIDestChild = pXIDest->GetItem(TDL_TASK);

		while (pXIDestChild)
		{
			if (pXIDestChild->ItemValueMatches(pXISrcChild, TDL_TASKTITLE))
			{
				break;
			}

			// next child
			pXIDestChild = pXIDestChild->GetSibling();
		}

		// if no match found then create new placeholder
		if (!pXIDestChild)
		{
			pXIDestChild = pXIDest->AddItem(TDL_TASK);
			pXIDestChild->AddItem(TDL_TASKTITLE, pXISrcChild->GetItemValue(TDL_TASKTITLE));
			pXIDestChild->AddItem(TDL_TASKID, (int)m_dwNextID++);

			nMerged++;
		}

		// finally merge the child items
		nMerged += MergeTasksByTitle(pXISrcChild, pXIDestChild);

		// then do next source child
		pXISrcChild = pXISrcChild->GetSibling();
	}

	return nMerged;
}

BOOL CMergeToDoList::MergeAttributes(const CXmlItem* pXISrc, CXmlItem* pXIDest)
{
	// don't merge if the source is older than the dest
	const CXmlItem* pXISrcMod = pXISrc->GetItem(TDL_TASKLASTMOD);
	const CXmlItem* pXIDestMod = pXIDest->GetItem(TDL_TASKLASTMOD);

	if (pXISrcMod && pXIDestMod)
	{
		if (pXISrcMod->GetValueF() <= pXIDestMod->GetValueF())
		{
			return FALSE;
		}
	}
	else if (pXIDestMod)
	{
		return FALSE;
	}

	BOOL bChange = FALSE;
	POSITION pos = pXISrc->GetFirstItemPos();

	while (pos)
	{
		const CXmlItem* pXISrcAttrib = pXISrc->GetNextItem(pos);

		// don't merge task ID or subtasks
		if (pXISrcAttrib->NameMatches(TDL_TASK) || pXISrcAttrib->NameMatches(TDL_TASKID))
		{
			continue;
		}

		// don't merge last mod unless last mod does not exist in dest task
		if (pXISrcAttrib->NameMatches(TDL_TASKLASTMOD) && pXIDestMod)
		{
			continue;
		}

		// does it already exist?
		LPCTSTR szAttrib = pXISrcAttrib->GetName();
		CXmlItem* pXIDestAttrib = pXIDest->GetItem(szAttrib);

		// if not then add it
		if (!pXIDestAttrib)
		{
			pXIDest->AddItem(*pXISrcAttrib, FALSE);
			bChange = TRUE;
		}
		// else update it
		else
		{
			// update value
			if (!pXIDestAttrib->ValueMatches(pXISrcAttrib, FALSE))
			{
				pXIDestAttrib->SetValue(pXISrcAttrib->GetValue());
				bChange = TRUE;
			}

			// and any children
			if (pXISrcAttrib->GetItemCount())
			{
				bChange |= MergeAttributes(pXISrcAttrib, pXIDestAttrib);
			}
		}
	}

	return bChange;
}

CXmlItem* CMergeToDoList::FindDestTask(DWORD dwID)
{
	if (!dwID)
	{
		return NULL;
	}

	CXmlItem* pXITask = NULL;
	m_mapID2Item.Lookup(dwID, pXITask);

	return pXITask;
}

void CMergeToDoList::BuildLookupMap()
{
	m_mapID2Item.RemoveAll();
	AddTaskToLookupMap(m_pXIDestRoot);
}

void CMergeToDoList::AddTaskToLookupMap(CXmlItem* pXIDest)
{
	if (!pXIDest)
	{
		return;
	}

	DWORD dwID = pXIDest->GetItemValueI(TDL_TASKID);

	if (dwID)
	{
		m_mapID2Item[dwID] = pXIDest;
	}

	// first child
	AddTaskToLookupMap(pXIDest->GetItem(TDL_TASK));

	// next sibling
	AddTaskToLookupMap(pXIDest->GetSibling());
}
