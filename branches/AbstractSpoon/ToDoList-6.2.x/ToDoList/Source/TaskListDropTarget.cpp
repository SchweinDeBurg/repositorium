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

// TaskListDropTarget.cpp: implementation of the CTaskListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskListDropTarget.h"

#include "../../../CodeProject/Source/WClassDefines.h"
#include "../../../CodeProject/Source/WinClasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IS_WND_TYPE(pWnd, cls, type) (pWnd->IsKindOf(RUNTIME_CLASS(cls)) || CWinClasses::IsClass(*pWnd, type))

CTaskListDropTarget::CTaskListDropTarget() : m_pParent(NULL), m_nLVPrevHilite(-1)
{
}

CTaskListDropTarget::~CTaskListDropTarget()
{
}

BOOL CTaskListDropTarget::Register(CWnd* pTarget, CWnd* pParent)
{
	m_pParent = pParent;

	return COleDropTarget::Register(pTarget);
}

DROPEFFECT CTaskListDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* /*pObject*/, DWORD /*dwKeyState*/, CPoint /*point*/)
{
	if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
	{
		TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
	}

	else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
	{
		if (m_nLVPrevHilite != -1) // shouldn't happen
		{
			ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED);   // all items
		}

		m_nLVPrevHilite = -1;
	}

	return DROPEFFECT_NONE;
}

void CTaskListDropTarget::OnDragLeave(CWnd* pWnd)
{
	if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
	{
		TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
	}

	else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
	{
		if (m_nLVPrevHilite != -1) // shouldn't happen
		{
			ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED);   // all items
		}

		m_nLVPrevHilite = -1;
	}
}

DROPEFFECT CTaskListDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pObject, DWORD /*dwKeyState*/, CPoint point)
{
	if (!pWnd->IsWindowEnabled())
	{
		return DROPEFFECT_NONE;
	}

	BOOL bFilename = pObject->IsDataAvailable((CLIPFORMAT)::RegisterClipboardFormat(_T("FileName")));
	BOOL bFileDrop = pObject->IsDataAvailable(CF_HDROP);

	if (bFilename || bFileDrop)
	{
		if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
		{
			TVHITTESTINFO tvhti = { { point.x, point.y }, 0, 0 };

			TreeView_HitTest(pWnd->GetSafeHwnd(), &tvhti);
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), tvhti.hItem);

			if (tvhti.hItem)
			{
				return bFilename ? DROPEFFECT_LINK : DROPEFFECT_COPY;
			}
		}
		else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
		{
			LVHITTESTINFO lvhti = { { point.x, point.y }, 0 };
			ListView_HitTest(pWnd->GetSafeHwnd(), &lvhti);

			// remove previous highlighting
			if (m_nLVPrevHilite != -1 && m_nLVPrevHilite != lvhti.iItem)
			{
				ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED);
			}

			if (lvhti.iItem != -1)
			{
				ListView_SetItemState(pWnd->GetSafeHwnd(), lvhti.iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
				m_nLVPrevHilite = lvhti.iItem;

				return bFilename ? DROPEFFECT_LINK : DROPEFFECT_COPY;
			}
		}
		else if (IS_WND_TYPE(pWnd, CEdit, WC_EDIT))
		{
			if (!(pWnd->GetStyle() & ES_READONLY))
			{
				return /*bFilename ? DROPEFFECT_LINK : */DROPEFFECT_COPY;
			}
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)) ||
			pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			// allow dropping only on titlebar
			if ((pWnd->GetStyle() & WS_CAPTION) && point.y < 0)
			{
				return DROPEFFECT_COPY;
			}
		}
	}
	else
	{
		if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
		{
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
		}

		else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
		{
			if (m_nLVPrevHilite != -1)
			{
				ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED);   // all items
			}

			m_nLVPrevHilite = -1;
		}
	}

	// else
	return DROPEFFECT_NONE;
}

BOOL CTaskListDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pObject, DROPEFFECT /*dropEffect*/, CPoint point)
{
	CString sFilePath;

	if (GetDroppedFilePath(pObject, sFilePath))
	{
		CString sClass = CWinClasses::GetClass(*pWnd);

		m_pParent->SetForegroundWindow();

		if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
		{
			TVHITTESTINFO tvhti = { { point.x, point.y }, 0, 0 };

			TreeView_HitTest(pWnd->GetSafeHwnd(), &tvhti);
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);

			if (tvhti.hItem)
			{
				m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)tvhti.hItem, (LPARAM)(LPCTSTR)sFilePath);
			}
		}
		else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
		{
			LVHITTESTINFO lvhti = { { point.x, point.y }, 0 };

			ListView_HitTest(pWnd->GetSafeHwnd(), &lvhti);

			if (m_nLVPrevHilite != -1)
			{
				ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED); // all items
				m_nLVPrevHilite = -1;
			}

			if (lvhti.iItem != -1)
			{
				m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)lvhti.iItem, (LPARAM)(LPCTSTR)sFilePath);
			}
		}
		else if (IS_WND_TYPE(pWnd, CEdit, WC_EDIT))
		{
			if (!(pWnd->GetStyle() & ES_READONLY))
			{
				m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)0, (LPARAM)(LPCTSTR)sFilePath);
			}
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)) ||
			pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			// allow dropping only on titlebar
			if ((pWnd->GetStyle() & WS_CAPTION) && point.y < 0)
			{
				m_pParent->SendMessage(WM_TLDT_DROPFILE, (WPARAM)0, (LPARAM)(LPCTSTR)sFilePath);
			}
		}
	}
	else // cleanup
	{
		if (IS_WND_TYPE(pWnd, CTreeCtrl, WC_TREEVIEW))
		{
			TreeView_SelectDropTarget(pWnd->GetSafeHwnd(), NULL);
		}

		else if (IS_WND_TYPE(pWnd, CListCtrl, WC_LISTVIEW))
		{
			if (m_nLVPrevHilite != -1)
			{
				ListView_SetItemState(pWnd->GetSafeHwnd(), m_nLVPrevHilite, 0, LVIS_DROPHILITED);   // all items
			}

			m_nLVPrevHilite = -1;
		}
	}

	return FALSE; // because we handle it
}

typedef DWORD (WINAPI* FNGETLONGPATHNAME)(LPCTSTR, LPTSTR, DWORD);

CString CTaskListDropTarget::GetLongPathName(LPCTSTR szShortPath)
{
	CString sLongPath(szShortPath);

	// must link dynamically to kernel32 else problem with win95/NT4
	HMODULE hLib = LoadLibrary(_T("kernel32.dll"));

	if (hLib)
	{
		FNGETLONGPATHNAME pFN = (FNGETLONGPATHNAME)GetProcAddress(hLib, STRINGIZE(GetLongPathName));

		if (pFN)
		{
			TCHAR szLongPath[MAX_PATH + 1];
			pFN(szShortPath, szLongPath, MAX_PATH);

			sLongPath = szLongPath;
		}
	}

	return sLongPath;
}

BOOL CTaskListDropTarget::GetDroppedFilePath(COleDataObject* pObject, CString& sFilename)
{
	HGLOBAL hGlobal = NULL;
	sFilename.Empty();

	// first try
	CLIPFORMAT cf = (CLIPFORMAT)::RegisterClipboardFormat(_T("FileName"));

	if (pObject->IsDataAvailable(cf))
	{
		hGlobal = pObject->GetGlobalData(cf);

		if (hGlobal)
		{
			sFilename = (LPCTSTR)GlobalLock(hGlobal);   // copy
		}
	}

	// second try
	if (!hGlobal && pObject->IsDataAvailable(CF_HDROP))
	{
		hGlobal = pObject->GetGlobalData(CF_HDROP);

		if (hGlobal)
		{
			HDROP hDrop = (HDROP)GlobalLock(hGlobal);

			TCHAR szFileName[_MAX_PATH];
			::DragQueryFile(hDrop, 0, szFileName, _MAX_PATH);

			sFilename = szFileName;
		}
	}

	// cleanup
	if (hGlobal)
	{
		GlobalUnlock(hGlobal);
	}

	// convert to long pathname
	if (!sFilename.IsEmpty())
	{
		sFilename = GetLongPathName(sFilename);
	}

	return !sFilename.IsEmpty();
}
