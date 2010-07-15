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

// TaskListDropTarget.h: interface for the CTaskListDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_)
#define AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxole.h>

const UINT WM_TLDT_DROPFILE = ::RegisterWindowMessage("WM_TLDT_DROPFILE"); // wparam = HTREEITEM or 0, lParam == (LPCTSTR)filepath

class CTaskListDropTarget : public COleDropTarget  
{
public:
	CTaskListDropTarget();
	virtual ~CTaskListDropTarget();

	BOOL Register(CWnd* pTarget, CWnd* pParent);

protected:
	CWnd* m_pParent;
	int m_nLVPrevHilite;

protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pObject, DROPEFFECT dropEffect, CPoint point);

protected:
	static CString GetLongPathName(LPCTSTR szShortPath);
	static BOOL GetDroppedFilePath(COleDataObject* pObject, CString& sFilename);
};

#endif // !defined(AFX_TASKLISTDROPTARGET_H__56519FB1_2923_45BB_97A2_08B8B1DC7C97__INCLUDED_)
