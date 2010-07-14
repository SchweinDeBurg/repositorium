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

// WinClasses.h: interface for the CWinClasses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_)
#define AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// global helper function for everyone to use
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd);

class CWinClasses  
{
public:
	static CString GetClass(HWND hWnd);
	static CString GetClassEx(HWND hWnd); // will also determine the base type of mfc (Afx) classes
	static BOOL IsControlClass(HWND hWnd);
	static BOOL IsControlClass(LPCTSTR szClass);
	static BOOL IsClass(HWND hWnd, LPCTSTR szClass);
	static BOOL IsClassEx(HWND hWnd, LPCTSTR szClass);
	static BOOL IsClass(LPCTSTR szClass, LPCTSTR szWndClass) { return (lstrcmpi(szClass, szWndClass) == 0); }
	static BOOL IsClassEx(LPCTSTR szClass, LPCTSTR szWndClass);

	static BOOL IsDialog(HWND hWnd);
	static BOOL IsEditControl(HWND hWnd);
	static BOOL IsRichEditControl(LPCTSTR szClass);
	static BOOL IsRichEditControl(HWND hWnd);

protected:
	static CMapStringToPtr s_mapCtrlClasses;
};

#endif // !defined(AFX_WINCLASSES_H__4A958ACC_A399_4B69_A988_FA70F16C3EAC__INCLUDED_)
