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

// MouseWheelMgr.h: interface for the CMouseWheelMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSEWHEELMGR_H__6738593F_D10B_45D9_ACA0_335DA7C0F630__INCLUDED_)
#define AFX_MOUSEWHEELMGR_H__6738593F_D10B_45D9_ACA0_335DA7C0F630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HookMgr.h"

class CMouseWheelMgr : public CHookMgr<CMouseWheelMgr>  
{
   friend class CHookMgr<CMouseWheelMgr>;

public:
	virtual ~CMouseWheelMgr();
 	static BOOL Initialize();

protected:
	CMouseWheelMgr();
 	static CMouseWheelMgr& Instance() { return CHookMgr<CMouseWheelMgr>::GetInstance(); }

  	virtual BOOL OnMouseEx(UINT uMouseMsg, const MOUSEHOOKSTRUCTEX& info);
};

#endif // !defined(AFX_MOUSEWHEELMGR_H__6738593F_D10B_45D9_ACA0_335DA7C0F630__INCLUDED_)
