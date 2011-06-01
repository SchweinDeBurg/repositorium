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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ToDoCtrlUndo.h: interface for the CToDoCtrlUndo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODOCTRLUNDO_H__5299C326_E181_47B2_A971_1563E5F6CEEC__INCLUDED_)
#define AFX_TODOCTRLUNDO_H__5299C326_E181_47B2_A971_1563E5F6CEEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToDoItem.h"
#include "../../../CodeProject/Source/Misc.h"
#include <afxtempl.h>

struct TDCUNDOELEMENT
{
	TDCUNDOELEMENT(TDCUNDOELMOP op = TDCUEO_EDIT, DWORD taskID = 0, DWORD parentID = 0,
		DWORD prevSiblingID = 0, WORD flags = 0, const TODOITEM* pTDI = NULL) :
	nOp(op),
	dwTaskID(taskID),
	dwParentID(parentID),
	dwPrevSiblingID(prevSiblingID),
	wFlags(flags),
	tdi(pTDI)
	{
	}

	const TDCUNDOELEMENT& operator=(const TDCUNDOELEMENT& elm)
	{
		nOp = elm.nOp;
		dwTaskID = elm.dwTaskID;
		dwParentID = elm.dwParentID;
		dwPrevSiblingID = elm.dwPrevSiblingID;
		tdi = elm.tdi;
		wFlags = elm.wFlags;

		return *this;
	}

	BOOL operator==(const TDCUNDOELEMENT& elm) const
	{
		return (nOp == elm.nOp &&
			dwTaskID == elm.dwTaskID &&
			dwParentID == elm.dwParentID &&
			dwPrevSiblingID == elm.dwPrevSiblingID &&
			wFlags == elm.wFlags);
	}

	BOOL operator!=(const TDCUNDOELEMENT& elm) const
	{
		return !(*this == elm);
	}

	TDCUNDOELMOP nOp;
	DWORD dwTaskID;
	DWORD dwParentID;
	DWORD dwPrevSiblingID;
	WORD wFlags;

	TODOITEM tdi;
};

typedef CArray<TDCUNDOELEMENT, TDCUNDOELEMENT&> CArrayUndoElements;

struct TDCUNDOACTION
{
	TDCUNDOACTION(TDCUNDOACTIONTYPE type = TDCUAT_NONE) : nType(type)
	{
	}

	const TDCUNDOACTION& operator=(const TDCUNDOACTION& action)
	{
		nType = action.nType;
		aElements.Copy(action.aElements);

		return *this;
	}

	BOOL operator==(const TDCUNDOACTION& action) const
	{
		return (nType == action.nType && Misc::ArraysMatch(aElements, action.aElements));
	}

	int GetTaskIDs(CDWordArray& aIDs) const
	{
		aIDs.RemoveAll();

		// get around constness
		TDCUNDOACTION* pThis = const_cast<TDCUNDOACTION*>(this);

		for (int nElm = 0; nElm < aElements.GetSize(); nElm++)
		{
			const TDCUNDOELEMENT& elm = pThis->aElements.ElementAt(nElm);
			aIDs.Add(elm.dwTaskID);
		}

		return aIDs.GetSize();
	}

	TDCUNDOACTIONTYPE nType;
	CArrayUndoElements aElements;
};

class CToDoCtrlUndo
{
public:
	CToDoCtrlUndo();
	virtual ~CToDoCtrlUndo();

	void ResetAll();

	BOOL BeginNewAction(TDCUNDOACTIONTYPE nType);
	TDCUNDOACTIONTYPE CurrentAction() const
	{
		return m_nActiveAction;
	}
	BOOL IsActive() const
	{
		return (CurrentAction() != TDCUAT_NONE);
	}
	BOOL EndCurrentAction();

	BOOL SaveElement(TDCUNDOELMOP nOp, DWORD dwTaskID, DWORD dwParentID, DWORD dwPrevSiblingID,
		WORD wFlags, const TODOITEM* pTDI);
	BOOL IsValidElementOperation(TDCUNDOELMOP nOp) const;

	int GetLastUndoActionTaskIDs(CDWordArray& aIDs) const;
	int GetLastRedoActionTaskIDs(CDWordArray& aIDs) const;

	BOOL DeleteLastUndoAction(); // only if no redos exist

	TDCUNDOACTIONTYPE GetLastUndoType() const;
	TDCUNDOACTIONTYPE GetLastRedoType() const;

	TDCUNDOACTION* UndoLastAction();
	TDCUNDOACTION* RedoLastAction();

	BOOL CanUndo() const
	{
		return m_aUndo.GetSize();
	}
	BOOL CanRedo() const
	{
		return m_aRedo.GetSize();
	}

protected:
	CArray<TDCUNDOACTION, TDCUNDOACTION&> m_aUndo;
	CArray<TDCUNDOACTION, TDCUNDOACTION&> m_aRedo;

	TDCUNDOACTIONTYPE m_nActiveAction;

protected:
	TDCUNDOACTION& LastUndoAction();
	TDCUNDOACTION& LastRedoAction();
	const TDCUNDOACTION& LastUndoAction() const;
	const TDCUNDOACTION& LastRedoAction() const;
	int LastUndoIndex() const
	{
		return m_aUndo.GetSize() - 1;
	}
	int LastRedoIndex() const
	{
		return m_aRedo.GetSize() - 1;
	}
};

#endif // !defined(AFX_TODOCTRLUNDO_H__5299C326_E181_47B2_A971_1563E5F6CEEC__INCLUDED_)
