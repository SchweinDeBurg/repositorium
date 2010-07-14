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

// DockManager.h: interface for the CDockManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKMANAGER_H__E0312E72_217B_4520_88F2_6708F4148F9D__INCLUDED_)
#define AFX_DOCKMANAGER_H__E0312E72_217B_4520_88F2_6708F4148F9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"

enum DM_POS
{
	DMP_UNDOCKED = -1,
	DMP_LEFT,
	DMP_RIGHT,
	DMP_BELOW,
};

class CDockManager : public CSubclassWnd, public CSubclasser  
{
public:
	CDockManager();
	virtual ~CDockManager();

	BOOL Initialize(CWnd* pMainWnd, CWnd* pDockWnd, 
					DM_POS nPos = DMP_UNDOCKED,	DM_POS nLastPos = DMP_RIGHT,
					int nWidthDocked = -1, int nWidthDockedMax = -1, 
					int nHeightDocked = -1, int nHeightDockedMax = -1);
	BOOL Initialized() { return IsHooked(); }
	BOOL Dock(DM_POS nPos);
	BOOL UnDock();

	inline DM_POS GetDockPos() { return m_nDockPos; }
	inline DM_POS GetLastDockPos() { return IsDocked() ? m_nDockPos : m_nLastDockPos; }
	inline BOOL IsDocked() { return m_nDockPos != DMP_UNDOCKED; }
	CRect GetUnDockedRect();
	int GetDockedWidth(BOOL bMaximized) { return bMaximized ? m_nWidthDockedMax : m_nWidthDocked; }
	int GetDockedHeight(BOOL bMaximized) { return bMaximized ? m_nHeightDockedMax : m_nHeightDocked; }

	inline BOOL Maximized() { return ::IsZoomed(GetHwnd()); }

protected:
	DM_POS m_nDockPos, m_nLastDockPos;
	CRect m_rUndocked;
	int m_nWidthDocked, m_nWidthDockedMax;
	int m_nHeightDocked, m_nHeightDockedMax;
	CSize m_sizeMainMin, m_sizeDockMin;
	BOOL m_bResizeUpdate;
	BOOL m_bSizeUpdate;

protected:
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp); // main wnd
	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp); // dock wnd
	virtual CSubclasser* GetTopSubclasser() { return this; }

	void UpdateDockWindowPos();
	void UpdateMainWindowPos();
	
	CRect GetWorkArea(BOOL bMaximized = TRUE);
	void MoveWindow(CWnd* pWnd, CRect rect);
	CSize GetMinMaximizedSize();
	void FitDockWindowToWorkArea();
	
	// pseudo message handlers
	void OnMaximize();
	void OnRestore();
	void OnMinMaxInfo(LPMINMAXINFO pMMI, BOOL bMain);

};

#endif // !defined(AFX_DOCKMANAGER_H__E0312E72_217B_4520_88F2_6708F4148F9D__INCLUDED_)
