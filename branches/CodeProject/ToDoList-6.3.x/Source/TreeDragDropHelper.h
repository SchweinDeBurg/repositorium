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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// TreeDragDropHelper.h: interface for the CTreeDragDropHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEDRAGDROPHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_)
#define AFX_TREEDRAGDROPHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DragDrop.h"
#include "TreeSelectionHelper.h"

enum DDWHERE
{
	DD_ABOVE = -1,
	DD_ON = 0,
	DD_BELOW = 1
};

enum
{
	DD_USESTEXTCALLBACK = 0x01,
	DD_USESIMAGECALLBACK = 0x02,
};

struct TDDHCOPY; // private helper

class CTreeDragDropHelper
{
public:
	CTreeDragDropHelper(CTreeSelectionHelper& selection, CTreeCtrl& tree, DWORD dwCallbackFlags = 0);
	virtual ~CTreeDragDropHelper();

	BOOL Initialize(CWnd* pOwner, BOOL bEnabled = TRUE, BOOL bAllowNcDrag = TRUE);
	void EnableDragDrop(BOOL bEnable)
	{
		m_bEnabled = bEnable;
	}
	UINT ProcessMessage(const MSG* pMsg);

	BOOL AddTargetWnd(CWnd* pWnd);

	BOOL GetDropTarget(HTREEITEM& htiDrop, HTREEITEM& htiAfter);
	BOOL IsDragging()
	{
		return m_ddMgr.IsDragging();
	}
	void CancelDrag()
	{
		m_ddMgr.CancelDrag();
	}

	// helpers
	static HTREEITEM CopyTree(CTreeCtrl& tree, HTREEITEM hDest, HTREEITEM hSrc, DDWHERE nWhere, DWORD dwCallbackFlags = 0);
	static HTREEITEM MoveTree(CTreeCtrl& tree, HTREEITEM hDest, HTREEITEM hSrc, DDWHERE nWhere, DWORD dwCallbackFlags = 0);

protected:
	CTreeSelectionHelper& m_selection;
	CTreeCtrl& m_tree;
	CDragDropMgr m_ddMgr;
	BOOL m_bEnabled;
	HTREEITEM m_htiDropTarget, m_htiDropAfter;
	UINT m_nScrollTimer, m_nExpandTimer;
	BOOL m_bAllowNcDrag;
	DWORD m_dwCallbackFlags;

	static CTreeDragDropHelper* s_pTDDH;

	struct DROPPOSITION
	{
		HTREEITEM htiDrop;
		DDWHERE nWhere;
	};

	DROPPOSITION m_dropPos;

protected:
	static void BuildCopy(CTreeCtrl& tree, const HTREEITEM hti, TDDHCOPY* pCopy);
	static HTREEITEM CopyTree(CTreeCtrl& tree, HTREEITEM hDest, const TDDHCOPY* pSrc, DDWHERE nWhere,
		DWORD dwCallbackFlags = 0);

	HTREEITEM HitTest(CPoint pt) const
	{
		DDWHERE nWhere;
		return HitTest(pt, nWhere);
	}
	HTREEITEM HitTest(CPoint pt, DDWHERE& nWhere) const;
	HTREEITEM HighlightDropTarget(CPoint point);
	HTREEITEM HighlightDropTarget(); // get current cursor pos

	enum
	{
		TIMER_SCROLL,
		TIMER_EXPAND
	};
	void SetTimer(int nTimer, UINT nPeriod);
	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

	// pseudo message handlers
	void OnTimer(UINT nIDEvent);
	BOOL OnDragEnter(DRAGDROPINFO* pDDI); // not const arg because we set pData
	BOOL OnDragPreMove(const DRAGDROPINFO* pDDI);
	UINT OnDragOver(const DRAGDROPINFO* pDDI);
	BOOL OnDragDrop(const DRAGDROPINFO* pDDI);
	BOOL OnDragAbort();
};

#endif // !defined(AFX_TREEDRAGDROPHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_)
