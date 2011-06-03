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
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// TreeCtrlHelper.h: interface for the CTreeCtrlHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_)
#define AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

typedef CMap<HTREEITEM, HTREEITEM, int, int&> CMapIndices;
typedef CMap<DWORD, DWORD, HTREEITEM, HTREEITEM&> CHTIMap;

enum TCH_EDGE { TCHE_TOP, TCHE_BOTTOM };
enum TCH_CHECK { TCHC_UNCHECKED, TCHC_CHECKED, TCHC_MIXED };

class CTreeCtrlHelper
{
public:
	CTreeCtrlHelper(CTreeCtrl& tree);
	virtual ~CTreeCtrlHelper();

	const CTreeCtrl& TreeCtrl() const
	{
		return m_tree;
	}
	CTreeCtrl& TreeCtrl()
	{
		return m_tree;
	}

	BOOL HasFocus(BOOL bIncEditing = TRUE);
	int IsItemExpanded(HTREEITEM hti, BOOL bFully = FALSE) const; // TRUE, FALSE, -1 if no children
	void ExpandAll(BOOL bExpand = TRUE, HTREEITEM hti = NULL);
	void ExpandItem(HTREEITEM hti, BOOL bExpand = TRUE);
	BOOL IsParentItemExpanded(HTREEITEM hti, BOOL bRecursive = FALSE) const;

	void SetItemIntegral(HTREEITEM hti, int iIntegral);

	void SetItemChecked(HTREEITEM hti, BOOL bChecked); // 2 state
	void SetItemChecked(HTREEITEM hti, TCH_CHECK nChecked); // 3 state
	TCH_CHECK GetItemCheckState(HTREEITEM hti) const;

	BOOL SelectItem(HTREEITEM hti); // won't auto edit if item already selected
	inline void EndLabelEdit(BOOL bCancel)
	{
		SendMessage(m_tree, TVM_ENDEDITLABELNOW, bCancel, 0);
	}

	void InvalidateItem(HTREEITEM hti, BOOL bChildren = TRUE);
	void GetClientRect(LPRECT lpRect, HTREEITEM htiFrom);
	int GetItemHeight(HTREEITEM hti = NULL);

	int GetItemPos(HTREEITEM hti, HTREEITEM htiParent);
	int GetItemLevel(HTREEITEM hti);

	HTREEITEM FindItem(DWORD dwItemData, HTREEITEM htiStart = NULL) const;

	BOOL IsItemBold(HTREEITEM hti);
	void SetItemBold(HTREEITEM hti, BOOL bBold = TRUE);
	void SetTopLevelItemsBold(BOOL bBold = TRUE);

	HTREEITEM GetLastChildItem(HTREEITEM hti) const;
	HTREEITEM GetLastItem() const;

	void SetItemStateEx(HTREEITEM hti, UINT nState, UINT nMask, BOOL bChildren = FALSE);

	HTREEITEM GetTopLevelParentItem(HTREEITEM hti) const;
	HTREEITEM GetNextTopLevelItem(HTREEITEM hti, BOOL bNext = TRUE) const;

	// returns the top level item whose child is the first visible item (or itself)
	HTREEITEM GetFirstVisibleTopLevelItem(); // return 0 if no items
	HTREEITEM GetFirstVisibleTopLevelItem(int& nPos); // return 0 if no items
	HTREEITEM GetTopLevelParentItem(HTREEITEM hti);

	int BuildVisibleIndexMap()
	{
		return BuildVisibleIndexMap(VIMap());
	}
	BOOL ItemLineIsOdd(HTREEITEM hti)
	{
		return ItemLineIsOdd(VIMap(), hti);
	}

	int BuildVisibleIndexMap(CMapIndices& index);
	BOOL ItemLineIsOdd(CMapIndices& index, HTREEITEM hti);

	// return increments of item height
	void SetMinDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge, int nItems);
	int GetDistanceToEdge(HTREEITEM htiFrom, TCH_EDGE nToEdge);

	// get next/prev selectable items, NULL if none
	HTREEITEM GetNextPageVisibleItem(HTREEITEM hti) const;
	HTREEITEM GetPrevPageVisibleItem(HTREEITEM hti) const;
	HTREEITEM GetNextVisibleItem(HTREEITEM hti, BOOL bAllowChildren = TRUE) const;
	HTREEITEM GetPrevVisibleItem(HTREEITEM hti, BOOL bAllowChildren = TRUE) const;

	HTREEITEM GetNextItem(HTREEITEM hti) const; // next item as-if all items were expanded
	HTREEITEM GetPrevItem(HTREEITEM hti) const; // prev item as-if all items were expanded

	int FindItem(HTREEITEM htiFind, HTREEITEM htiStart); // return -1 for above, 1 for below, 0 if same
	BOOL IsFullyVisible(HTREEITEM hti) const;

	void EnsureVisibleEx(HTREEITEM hti, BOOL bVPartialOK = TRUE, BOOL bHPartialOK = TRUE);

	void BuildHTIMap(CHTIMap& mapHTI, HTREEITEM htiRoot = NULL) const;
	void UpdateHTIMapEntry(CHTIMap& mapHTI, HTREEITEM hti) const;


protected:
	CTreeCtrl& m_tree;
	mutable CMapIndices* m_pVisibleIndices;

protected:
	void AddVisibleItemToIndex(CMapIndices& index, HTREEITEM hti);

	CMapIndices& VIMap();
	const CMapIndices& VIMap() const;
};

#endif // !defined(AFX_TREECTRLHELPER_H__F6652DBE_3770_4E1C_A246_057AD6AD16B7__INCLUDED_)
