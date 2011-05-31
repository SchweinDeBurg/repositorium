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

#if !defined(AFX_OUTLOOKIMPORTDLG_H__E9D6C9F1_A424_4CB8_9AEF_0CE8641F1C53__INCLUDED_)
#define AFX_OUTLOOKIMPORTDLG_H__E9D6C9F1_A424_4CB8_9AEF_0CE8641F1C53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutlookImportDlg.h : header file
//

#include "../../../CodeProject/Source/CheckListBoxEx.h"

/////////////////////////////////////////////////////////////////////////////
// COutlookImportDlg dialog

class _Application;
class ITaskList7;
class ITaskList;
class MAPIFolder;
class _TaskItem;
typedef void* HTASKITEM;

class COutlookImportDlg : public CDialog
{
// Construction
public:
	COutlookImportDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL ImportTasks(ITaskList* pDestTaskFile);

// Dialog Data
	//{{AFX_DATA(COutlookImportDlg)
	enum { IDD = IDD_IMPORT_DIALOG };
	BOOL	m_bRemoveOutlookTasks;
	//}}AFX_DATA
	CCheckListBoxEx	m_lbTasks;
	ITaskList7* m_pDestTaskFile;
	_Application* m_pOutlook;
	MAPIFolder* m_pFolder;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutlookImportDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	int DoModal()
	{
		return CDialog::DoModal();
	}

	// Generated message map functions
	//{{AFX_MSG(COutlookImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnChoosefolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static time_t ConvertDate(DATE date);

	void AddFolderItemsToList(MAPIFolder* pFolder, const CString& sPath = _T(""));
	void SetTaskAttributes(HTASKITEM hTask, _TaskItem* pTask);
	BOOL DeleteTaskFromFolder(_TaskItem* pTask, MAPIFolder* pFolder);

	static BOOL TaskPathsMatch(_TaskItem* pTask1, _TaskItem* pTask2);
	static CString GetFullPath(_TaskItem* pTask);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTLOOKIMPORTDLG_H__E9D6C9F1_A424_4CB8_9AEF_0CE8641F1C53__INCLUDED_)
