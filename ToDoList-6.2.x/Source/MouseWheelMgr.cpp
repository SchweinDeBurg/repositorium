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
//*****************************************************************************

// MouseWheelMgr.cpp: implementation of the CMouseWheelMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MouseWheelMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseWheelMgr::CMouseWheelMgr()
{
}

CMouseWheelMgr::~CMouseWheelMgr()
{
}

BOOL CMouseWheelMgr::Initialize()
{
	return GetInstance().InitHooks(HM_MOUSE);
}

BOOL CMouseWheelMgr::OnMouseEx(UINT uMouseMsg, const MOUSEHOOKSTRUCTEX& info)
{
	if (uMouseMsg == WM_MOUSEWHEEL)
	{
		//fabio_2005
#if _MSC_VER >= 1300
		HWND hwndPt = ::WindowFromPoint(info.pt);

		if (info.hwnd != hwndPt)  // does the window under the mouse have the focus.
		{
			::PostMessage(hwndPt, WM_MOUSEWHEEL, (info.mouseData & 0xffff0000),
				MAKELPARAM(info.pt.x, info.pt.y));

			return TRUE; // eat
		}
	}
#else
		HWND hwndPt = ::WindowFromPoint(info.MOUSEHOOKSTRUCT.pt);

		if (info.MOUSEHOOKSTRUCT.hwnd != hwndPt)  // does the window under the mouse have the focus.
		{
			::PostMessage(hwndPt, WM_MOUSEWHEEL, (info.mouseData & 0xffff0000),
				MAKELPARAM(info.MOUSEHOOKSTRUCT.pt.x, info.MOUSEHOOKSTRUCT.pt.y));

			return TRUE; // eat
		}
	}
#endif

	// all else
	return FALSE;
}
