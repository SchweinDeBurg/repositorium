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
//*****************************************************************************

// TDLImportExportMgr.h: interface for the CTDLImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_)
#define AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/ImportExportMgr.h"

enum { EXP2HTML, EXP2TXT, EXP2CSV };

class CTDLImportExportMgr : public CImportExportMgr  
{
public:
	CTDLImportExportMgr();
	virtual ~CTDLImportExportMgr();

	BOOL ExportTaskListToHtml(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToHtml(const ITaskList* pSrcTasks) const;

	BOOL ExportTaskListToText(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToText(const ITaskList* pSrcTasks) const;

	BOOL ExportTaskListToCsv(const ITaskList* pSrcTasks, LPCTSTR szDestFile) const;
	CString ExportTaskListToCsv(const ITaskList* pSrcTasks) const;

	void Initialize();
};

#endif // !defined(AFX_TDIMPORTEXPORTMGR_H__0810C7FB_A895_4035_BEFA_B7097ABC2A7B__INCLUDED_)
