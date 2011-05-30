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

// ToolsHelper.cpp: implementation of the CToolsHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Resource.h"
#include "ToolsHelper.h"
#include "ToolsUserInputDlg.h"

#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/SysImageList.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/FileMisc.h"

#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsHelper::CToolsHelper(BOOL bTDLEnabled, UINT nStart, int nSize) : m_nStartID(nStart), m_nSize(nSize), m_bTDLEnabled(bTDLEnabled)
{
}

CToolsHelper::~CToolsHelper()
{
}

BOOL CToolsHelper::RunTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd)
{
	CString sCmdline;

	if (PrepareCmdline(tool, sCmdline, args))
	{
		HWND hWnd = pWnd ? pWnd->GetSafeHwnd() : NULL;

		int nRes = (int)ShellExecute(hWnd, NULL, tool.sToolPath, sCmdline, NULL, tool.bRunMinimized ? SW_MINIMIZE : SW_SHOWNORMAL);
		return (nRes > 32);
	}

	return FALSE;
}

void CToolsHelper::TestTool(const USERTOOL& tool, const USERTOOLARGS& args, CWnd* pWnd)
{
	CString sCmdline;

	if (PrepareCmdline(tool, sCmdline, args))
	{
		CString sMessage;
		sMessage.Format(IDS_TH_TESTTOOL, tool.sToolPath, sCmdline);

		HWND hWnd = pWnd ? pWnd->GetSafeHwnd() : NULL;

		if (::MessageBox(hWnd, sMessage, CEnString(IDS_TH_TESTTOOL_TITLE), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			int nRes = (int)ShellExecute(hWnd, NULL, tool.sToolPath, sCmdline, NULL, tool.bRunMinimized ? SW_MINIMIZE : SW_SHOWNORMAL);

			if (nRes <= 32)
			{
			}
		}
	}
}

HICON CToolsHelper::GetToolIcon(CSysImageList& sil, const USERTOOL& ut)
{
	HICON hIcon = NULL;

	// check valid tool path
	// note: could also be url
	if (FileMisc::FileExists(ut.sToolPath) || FileMisc::FileExists(ut.sIconPath) || ::PathIsURL(ut.sToolPath))
	{
		if (ut.sIconPath.IsEmpty())
		{
			hIcon = sil.ExtractFileIcon(ut.sToolPath);
		}
		else
		{
			hIcon = sil.ExtractFileIcon(ut.sIconPath);
		}
	}

	return hIcon;
}

void CToolsHelper::UpdateMenu(CCmdUI* pCmdUI, const CUserToolArray& tools, CMenuIconMgr& iconMgr)
{
	if (pCmdUI->m_pMenu)
	{
		CUserToolArray aTools;
		aTools.Copy(tools);

		// delete existing tool entries and their icons first
		for (int nTool = 0; nTool < m_nSize; nTool++)
		{
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + nTool, MF_BYCOMMAND);
			iconMgr.DeleteImage(m_nStartID + nTool);
		}

		// if we have any tools to add we do it here
		if (aTools.GetSize())
		{
			// add valid tools only by first removing invalid items
			int nTool = aTools.GetSize();

			while (nTool--)
			{
				if (aTools[nTool].sToolName.IsEmpty() || aTools[nTool].sToolPath.IsEmpty())
				{
					aTools.RemoveAt(nTool);
				}
			}

			if (aTools.GetSize())
			{
				CSysImageList sil; // for menu icons
				VERIFY(sil.Initialize());

				int nPos = 0;

				for (nTool = 0; nTool < aTools.GetSize(); nTool++)
				{
					const USERTOOL& tool = aTools[nTool];
					CString sMenuItem;

					if (nPos < 9)
					{
						sMenuItem.Format(_T("&%d %s"), nPos + 1, tool.sToolName);
					}
					else
					{
						sMenuItem = tool.sToolName;
					}

					pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++, MF_BYPOSITION | MF_STRING,
						m_nStartID + nTool, sMenuItem);

					iconMgr.SetImage(m_nStartID + nTool, GetToolIcon(sil, tool));

					nPos++;
				}

				// update end menu count
				pCmdUI->m_nIndex--; // point to last menu added
				pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

				pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
			}
		}

		// if nothing to add just re-add placeholder
		if (!aTools.GetSize())
		{
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex, MF_BYPOSITION | MF_STRING | MF_GRAYED,
				m_nStartID, CEnString(IDS_USERDEFINEDTOOLS));
		}
	}
}

BOOL CToolsHelper::PrepareCmdline(const USERTOOL& tool, CString& sCmdline, const USERTOOLARGS& args)
{
	CToolsCmdlineParser tcp(tool.sCmdline);

	// do necessary substitutions
	CString sTasklist(args.sTasklist);

	if (tcp.HasTasklistArgument())
	{
		// allow for no loaded file
		if (!FileMisc::FileExists(sTasklist))
		{
			CEnString sTitle(IDS_SELECTTASKLIST_TITLE);
			CFileDialog dialog(TRUE, GetDefaultFileExt(), NULL, OFN_OVERWRITEPROMPT, GetFileFilter());

			dialog.m_ofn.lpstrTitle = sTitle;

			if (dialog.DoModal() != IDOK)
			{
				return FALSE;
			}

			sTasklist = dialog.GetPathName();
		}

		CString sDrive, sPath, sFName, sExt;

		FileMisc::SplitPath(sTasklist, &sDrive, &sPath, &sFName, &sExt);

		tcp.ReplaceArgument(CLAT_PATHNAME, sTasklist);
		tcp.ReplaceArgument(CLAT_FOLDER, sDrive + sPath);
		tcp.ReplaceArgument(CLAT_FILENAME, sFName + sExt);
		tcp.ReplaceArgument(CLAT_FILETITLE, sFName);
	}

	if (tcp.HasArgument(CLAT_TODOLIST))
	{
		CString sToDoList = FileMisc::GetModuleFileName();
		tcp.ReplaceArgument(CLAT_TODOLIST, sToDoList);
	}

	tcp.ReplaceArgument(CLAT_SELTASKID, args.dwTaskID);
	tcp.ReplaceArgument(CLAT_SELTASKTITLE, args.sTaskTitle);
	tcp.ReplaceArgument(CLAT_SELTASKEXTID, args.sTaskExtID);
	tcp.ReplaceArgument(CLAT_SELTASKCOMMENTS, args.sTaskComments);
	tcp.ReplaceArgument(CLAT_SELTASKFILELINK, args.sTaskFileLink);
	tcp.ReplaceArgument(CLAT_SELTASKALLOCBY, args.sTaskAllocBy);
	tcp.ReplaceArgument(CLAT_SELTASKALLOCTO, args.sTaskAllocTo);

	if (tcp.IsUserInputRequired())
	{
		CToolsUserInputDlg dialog(tcp);

		if (dialog.DoModal(tool.sToolName) != IDOK)
		{
			return FALSE;
		}

		// process the results
		CCLArgArray aArgs;
		int nArg = tcp.GetUserArguments(aArgs);

		while (nArg--)
		{
			CString sResult(dialog.GetResult(aArgs[nArg].sName));
			tcp.ReplaceArgument(aArgs[nArg].sName, sResult);
		}
	}

	sCmdline = tcp.GetCmdLine();
	return TRUE;
}

void CToolsHelper::RemoveToolsFromToolbar(CToolBar& toolbar, UINT nCmdAfter)
{
	int nRemoved = 0;

	TBBUTTON tbb;
	CImageList* pIL = toolbar.GetToolBarCtrl().GetImageList();

	for (UINT nToolID = m_nStartID; nToolID <= m_nStartID + m_nSize; nToolID++)
	{
		int nBtn = toolbar.CommandToIndex(nToolID);

		if (nBtn != -1)
		{
			VERIFY(toolbar.GetToolBarCtrl().GetButton(nBtn, &tbb));

			if (toolbar.GetToolBarCtrl().DeleteButton(nBtn))
			{
				// delete the image too
				pIL->Remove(tbb.iBitmap);

				nRemoved++;
			}
		}
	}

	// remove separator
	if (nRemoved)
	{
		int nSep = toolbar.CommandToIndex(nCmdAfter) + 1;
		toolbar.GetToolBarCtrl().DeleteButton(nSep);
	}
}

void CToolsHelper::AppendToolsToToolbar(const CUserToolArray& aTools, CToolBar& toolbar, UINT nCmdAfter)
{
	// remove tools first
	RemoveToolsFromToolbar(toolbar, nCmdAfter);

	// then re-add
	if (aTools.GetSize())
	{
		// figure out if we want the large or small images
		CSize sizeBtn(toolbar.GetToolBarCtrl().GetButtonSize());
		sizeBtn -= CSize(7, 7); // btn borders from BarTool.cpp

		CSysImageList sil((sizeBtn.cx > 16));
		VERIFY(sil.Initialize());

		// start adding after the pref button
		int nStartPos = toolbar.CommandToIndex(nCmdAfter) + 1;
		int nAdded = 0;

		for (int nTool = 0; nTool < aTools.GetSize(); nTool++)
		{
			const USERTOOL& tool = aTools[nTool];
			HICON hIcon = GetToolIcon(sil, tool);

			if (hIcon)
			{
				CImageList* pIL = toolbar.GetToolBarCtrl().GetImageList();
				int nImage = pIL->Add(hIcon);

				TBBUTTON tbb = { nImage, nTool + m_nStartID, 0, TBSTYLE_BUTTON, 0, 0, (UINT) - 1 };

				if (toolbar.GetToolBarCtrl().InsertButton(nStartPos + nAdded, &tbb))
				{
					nAdded++;
				}
				else // remove image
				{
					pIL->Remove(nImage);
				}

				// cleanup
				::DestroyIcon(hIcon);
			}
		}

		// add a separator if any buttons added
		if (nAdded)
		{
			TBBUTTON tbb = { -1, 0, 0, TBSTYLE_SEP, 0, 0, (UINT) - 1 };
			toolbar.GetToolBarCtrl().InsertButton(nStartPos, &tbb);
		}
	}
}

LPCTSTR CToolsHelper::GetFileFilter()
{
	static CEnString TDLFILEFILTER(IDS_TDLFILEFILTER);
	static CEnString XMLFILEFILTER(IDS_XMLFILEFILTER);

	return m_bTDLEnabled ? TDLFILEFILTER : XMLFILEFILTER;
}

LPCTSTR CToolsHelper::GetDefaultFileExt()
{
	static LPCTSTR TDLEXT = _T("tdl");
	static LPCTSTR XMLEXT = _T("xml");

	return m_bTDLEnabled ? TDLEXT : XMLEXT;
}
