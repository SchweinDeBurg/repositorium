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
// - adjusted #include's paths
//*****************************************************************************

// GroupLine.h: interface for the CGroupLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPLINE_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_)
#define AFX_GROUPLINE_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CodeProject/Source/Subclass.h"
#include <afxtempl.h>

class CGroupLine : public CSubclassWnd
{
public:
	CGroupLine();
	virtual ~CGroupLine();

	BOOL Initialize(HWND hwndStatic);

protected:
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
};

class CGroupLineManager
{
public:
	CGroupLineManager();
	virtual ~CGroupLineManager();

	BOOL AddGroupLine(UINT nIDStatic, HWND hwndParent);

protected:
	CMap<HWND, HWND, CGroupLine*, CGroupLine*&> m_mapGroupLines;
};

#endif // !defined(AFX_GROUPLINE_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_)
