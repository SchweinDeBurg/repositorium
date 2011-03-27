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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ToolsHelper.h: interface for the CToolsHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_)
#define AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolsCmdlineParser.h"
#include "../../../CodeProject/Source/MenuIconMgr.h"

struct USERTOOL
{
	CString sToolName;
	CString sToolPath;
	CString sCmdline;
	BOOL bRunMinimized;
	CString sIconPath;
};

struct USERTOOLARGS
{
	CString sTasklist;
	DWORD dwTaskID;
	CString sTaskTitle;
	CString sTaskExtID;
	CString sTaskComments;
	CString sTaskFileLink;
	CString sTaskAllocBy;
	CString sTaskAllocTo;
};

class CSysImageList;
class CPreferencesDlg;

typedef CArray<USERTOOL, USERTOOL&> CUserToolArray;

class CToolsHelper
{
public:
	CToolsHelper(BOOL bTDLEnabled, UINT nStart, int nSize = 16);
	virtual ~CToolsHelper();

	void UpdateMenu(CCmdUI* pCmdUI, const CUserToolArray& tools, CMenuIconMgr& iconMgr);
	BOOL RunTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd = NULL);
	void TestTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd = NULL);
	void AppendToolsToToolbar(const CUserToolArray& aTools, CToolBar& toolbar, UINT nCmdAfter);
	void RemoveToolsFromToolbar(CToolBar& toolbar, UINT nCmdAfter);

protected:
	UINT m_nStartID;
	int m_nSize;
	BOOL m_bTDLEnabled;

protected:
	BOOL PrepareCmdline(const USERTOOL& tool, CString& sCmdline, const USERTOOLARGS& args);
	LPCTSTR GetFileFilter();
	LPCTSTR GetDefaultFileExt();

	static HICON GetToolIcon(CSysImageList& sil, const USERTOOL& ut);
};

#endif // !defined(AFX_TOOLSHELPER_H__6BAD432D_0189_46A9_95ED_EF869CFC6CE1__INCLUDED_)
