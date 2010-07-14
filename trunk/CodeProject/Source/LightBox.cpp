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
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// LightBox.cpp: implementation of the CLightBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifndef WS_EX_LAYERED
#  define WS_EX_LAYERED 0x00080000
#  define LWA_COLORKEY 1 
#  define LWA_ALPHA 2 
#endif

#ifndef SM_REMOTESESSION
#	define SM_REMOTESESSION 0x1000
#endif

CLightBoxMgr::CLightBoxMgr() : m_crBkgnd(GetSysColor(COLOR_3DHILIGHT)), m_nOpaquePercent(50)
{
	
}

CLightBoxMgr::~CLightBoxMgr()
{
	POSITION pos = m_mapCtrls.GetStartPosition();
	
	while (pos)
	{
		HWND hwnd = NULL;
		CLightBox* pCtrl = NULL;
		
		m_mapCtrls.GetNextAssoc(pos, hwnd, pCtrl);
		delete pCtrl;
	}
	
	m_mapCtrls.RemoveAll();
}

BOOL CLightBoxMgr::Initialize(CWnd* pMainWnd, COLORREF crBkgnd, int nOpaquePercent)
{
	// don't lightbox over remote desktop
	if (GetSystemMetrics(SM_REMOTESESSION))
		return TRUE;

	Instance().m_crBkgnd = crBkgnd;
	Instance().m_nOpaquePercent = nOpaquePercent;
	
	if (Instance().CHookMgr<CLightBoxMgr>::InitHooks(HM_CALLWNDPROC))
	{
		Instance().AttachLightBox(pMainWnd);
		return TRUE;
	}
	
	// else
	return FALSE;
}

BOOL CLightBoxMgr::OnCallWndProc(const MSG& msg)
{   
	ASSERT (m_hCallWndHook);

	switch (msg.message)
	{
	case WM_CREATE:
		{
			CWnd* pWnd = CWnd::FromHandle(msg.hwnd);
			AttachLightBox(pWnd);
		}
		break;
		
		
	case WM_NCDESTROY:
		{
			CWnd* pWnd = CWnd::FromHandle(msg.hwnd);
			
			if ((pWnd->GetStyle() & WS_CAPTION) == WS_CAPTION)
			{
				
				CLightBox* pCtrl = NULL;
				
				if (m_mapCtrls.Lookup(msg.hwnd, pCtrl))
				{
					//TRACE ("CLightBoxMgr::DetachLightBox(%s)\n", pWnd->GetRuntimeClass()->m_lpszClassName);
					
					delete pCtrl;
					m_mapCtrls.RemoveKey(msg.hwnd);
				}
			}
		}
		break;
	}
	
	return TRUE;
}

BOOL CLightBoxMgr::AttachLightBox(CWnd* pWnd)
{
	ASSERT(pWnd);
	
	// only captioned top-level windows
	if (((pWnd->GetStyle() & WS_CAPTION) != WS_CAPTION))
		return TRUE;
	
	// don't hook CLightBoxes
	if (pWnd->IsKindOf(RUNTIME_CLASS(CLightBox)))
		return TRUE;
	
	// don't hook temporary windows
	if (CWnd::FromHandlePermanent(*pWnd) == NULL)
		return TRUE;
	
	CLightBox* pCtrl = NULL;
	
	if (!m_mapCtrls.Lookup(*pWnd, pCtrl))
	{
		//TRACE ("CLightBoxMgr::AttachLightBox(%s)\n", pWnd->GetRuntimeClass()->m_lpszClassName);
		
		pCtrl = new CLightBox;
		
		if (!pCtrl->Initialize(pWnd, m_crBkgnd, m_nOpaquePercent))
		{
			delete pCtrl;
			return FALSE;
		}
		
		// else
		m_mapCtrls[pWnd->m_hWnd] = pCtrl;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CLightBox, CRuntimeDlg)

CLightBox::CLightBox() : m_pSetLayeredWindowAttributes(NULL)
{
	
}

CLightBox::~CLightBox()
{
	
}

BEGIN_MESSAGE_MAP(CLightBox, CRuntimeDlg)
//{{AFX_MSG_MAP(CLightBox)
ON_WM_ERASEBKGND()
//ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLightBox::Initialize(CWnd* pWnd, COLORREF crBkgnd, int nOpaquePercent)
{
	if ((pWnd->GetStyle() & WS_CAPTION) != WS_CAPTION)
		return TRUE; // nothing to do
	
	m_crBkgnd = crBkgnd;
	m_nOpaquePercent = min(max(0, nOpaquePercent), 100);
	
	// make opacity multiple of 4
	m_nOpaquePercent = (m_nOpaquePercent / 4) * 4;
	
	if (HookWindow(*pWnd))
	{
		// create ourselves hidden
		if (!CRuntimeDlg::Create(_T(""), WS_POPUP | WS_CAPTION, RTD_DEFEXSTYLE, rectAuto, GetCWnd()))
			return FALSE;
		
		ModifyStyleEx(0, WS_EX_LAYERED);
	}
	
	return TRUE;
}

LRESULT CLightBox::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_ENABLE:
		{
			bool bShow = !wp;

			// only visible windows
			if (!bShow || CSubclassWnd::IsWindowVisible())
				ShowTransparentWnd(bShow);
		}
		break;
	}
	
	return CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
}

BOOL CLightBox::ShowTransparentWnd(BOOL bShow)
{
	ASSERT(GetSafeHwnd());
	
	if (!GetSafeHwnd())
		return FALSE;
	
	if (bShow && !m_pSetLayeredWindowAttributes)
	{
		HMODULE hDLL = LoadLibrary (_T("user32.dll"));
		m_pSetLayeredWindowAttributes = (PSLWA) GetProcAddress(hDLL,"SetLayeredWindowAttributes");
		
		if (m_pSetLayeredWindowAttributes == NULL) 
		{
			DestroyWindow();
			return FALSE;
		}
	}
	
	if (bShow)
	{
		CRect rect;
		GetCWnd()->GetWindowRect(rect);
		MoveWindow(rect);
		
		m_pSetLayeredWindowAttributes(*this, 0, (unsigned char)((255 * m_nOpaquePercent) / 100), LWA_ALPHA);
		
		ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

BOOL CLightBox::OnEraseBkgnd(CDC* pDC)
{
	if (m_crBkgnd != (COLORREF)-1)
	{
		CRect rClient;
		CRuntimeDlg::GetClientRect(rClient);
		
		pDC->FillSolidRect(rClient, m_crBkgnd);
		return FALSE;
	}
	
	// else
	return CRuntimeDlg::OnEraseBkgnd(pDC);
}
