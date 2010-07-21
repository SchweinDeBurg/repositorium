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

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutlookImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	int DoModal() { return CDialog::DoModal(); }

	// Generated message map functions
	//{{AFX_MSG(COutlookImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static time_t ConvertDate(DATE date);

	void AddFolderItemsToList(MAPIFolder* pFolder, const CString& sPath = "");
	void SetTaskAttributes(HTASKITEM hTask, _TaskItem* pTask);
	BOOL DeleteTaskFromFolder(_TaskItem* pTask, MAPIFolder* pFolder);

	static BOOL TaskPathsMatch(_TaskItem* pTask1, _TaskItem* pTask2);
	static CString GetFullPath(_TaskItem* pTask);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTLOOKIMPORTDLG_H__E9D6C9F1_A424_4CB8_9AEF_0CE8641F1C53__INCLUDED_)
