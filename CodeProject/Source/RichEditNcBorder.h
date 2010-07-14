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

// RichEditNcBorder.h: interface for the CRichEditNcBorder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITNCBORDER_H__AB98DDF6_FE73_4518_B805_DCF65A954860__INCLUDED_)
#define AFX_RICHEDITNCBORDER_H__AB98DDF6_FE73_4518_B805_DCF65A954860__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"

class CRichEditNcBorder : public CSubclassWnd  
{
public:
	CRichEditNcBorder();
	virtual ~CRichEditNcBorder();

	BOOL Initialize(HWND hwndRichEdit);

protected:
	BOOL m_bThemedBorder;

protected:
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

	void NcDrawBorder();
	void VerifyThemedBorderState(/*BOOL bRecheck*/);

};

#endif // !defined(AFX_RICHEDITNCBORDER_H__AB98DDF6_FE73_4518_B805_DCF65A954860__INCLUDED_)
