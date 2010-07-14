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

// DeferWndMove.h: interface for the CDeferWndMove class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFERWNDMOVE_H__4D6EF0E2_B8BF_11D3_ADD8_0000E8425C3E__INCLUDED_)
#define AFX_DEFERWNDMOVE_H__4D6EF0E2_B8BF_11D3_ADD8_0000E8425C3E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDeferWndMove
{
public:
	CDeferWndMove(int nNumWindows = 10);
	virtual ~CDeferWndMove();

	BOOL BeginMove(int nNumWindows);
	BOOL EndMove();

	BOOL MoveWindow(CWnd* pWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	BOOL MoveWindow(CWnd* pWnd, LPCRECT lpRect, BOOL bRepaint = TRUE);
	CRect OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x = 0, int y = 0);
	CRect ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx = 0, int cy = 0);
	CRect MoveCtrl(CWnd* pParent, UINT nCtrlID, int x, int y);

protected:
	HDWP m_hdwp;
};

#endif // !defined(AFX_MULTIWNDMOVE_H__4D6EF0E2_B8BF_11D3_ADD8_0000E8425C3E__INCLUDED_)
