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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// OutlookImportDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "OutlookImpExpApp.h"
#include "OutlookImportDlg.h"
#include "msoutl.h"

#include "../../Common/ITaskList.h"
#include "../../../CodeProject/Source/Misc.h"

#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int olFolderDeletedItems  = 3;
const int olFolderOutbox        = 4;
const int olFolderSentMail      = 5;
const int olFolderInbox         = 6;
const int olFolderCalendar      = 9;
const int olFolderContacts      = 10;
const int olFolderJournal       = 11;
const int olFolderNotes         = 12;
const int olFolderTasks         = 13;

LPCTSTR PATHDELIM = _T(" \\ ");

/////////////////////////////////////////////////////////////////////////////
// COutlookImportDlg dialog


COutlookImportDlg::COutlookImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutlookImportDlg::IDD, pParent), m_pDestTaskFile(NULL), m_pOutlook(NULL)
{
	//{{AFX_DATA_INIT(COutlookImportDlg)
	//}}AFX_DATA_INIT
	m_bRemoveOutlookTasks = AfxGetApp()->GetProfileInt(_T("Importers\\Outlook"), _T("RemoveOutlookTasks"), FALSE);
}


void COutlookImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutlookImportDlg)
	DDX_Control(pDX, IDC_TASKLIST, m_lbTasks);
	DDX_Check(pDX, IDC_REMOVEOUTLOOKTASKS, m_bRemoveOutlookTasks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutlookImportDlg, CDialog)
	//{{AFX_MSG_MAP(COutlookImportDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHOOSEFOLDER, OnChoosefolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutlookImportDlg message handlers

BOOL COutlookImportDlg::ImportTasks(ITaskList* pDestTaskFile)
{
	m_pDestTaskFile = GetITLInterface<ITaskList7>(pDestTaskFile, IID_TASKLIST7);
	ASSERT(m_pDestTaskFile);

	if (!m_pDestTaskFile)
	{
		return FALSE;
	}

	return (DoModal() == IDOK);
}

BOOL COutlookImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT(m_pOutlook == NULL);
	m_pOutlook = new _Application;

	if (m_pOutlook->CreateDispatch(_T("Outlook.Application")))
	{
		_NameSpace nmspc(m_pOutlook->GetNamespace(_T("MAPI")));
		nmspc.m_lpDispatch->AddRef(); // to keep it alive

		m_pFolder = new MAPIFolder(nmspc.GetDefaultFolder(olFolderTasks));
		m_pFolder->m_lpDispatch->AddRef(); // to keep it alive

		AddFolderItemsToList(m_pFolder);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COutlookImportDlg::AddFolderItemsToList(MAPIFolder* pFolder, const CString& sPath)
{
	_Items items(pFolder->GetItems());
	items.m_lpDispatch->AddRef(); // to keep it alive

	int nItem, nCount = items.GetCount();

	for (nItem = 1; nItem <= nCount; nItem++)
	{
		LPDISPATCH lpd = items.Item(COleVariant((short)nItem));
		lpd->AddRef(); // to keep it alive
		_TaskItem task(lpd);

		CString sItem(task.GetSubject());

		if (!sPath.IsEmpty())
		{
			sItem.Format(_T("%s%s%s"), sPath, PATHDELIM, task.GetSubject());
		}

		int nIndex = m_lbTasks.AddString(sItem);

		m_lbTasks.SetItemData(nIndex, (DWORD)lpd);
		m_lbTasks.SetCheck(nIndex, TRUE);
	}

	// likewise for subfolders
	_Items folders(pFolder->GetFolders());
	nCount = folders.GetCount();


	for (nItem = 1; nItem <= nCount; nItem++)
	{
		MAPIFolder folder(folders.Item(COleVariant((short)nItem)));
		folder.m_lpDispatch->AddRef();
		CString sSubPath(folder.GetName());

		if (!sPath.IsEmpty())
		{
			sSubPath.Format(_T("%s%s%s"), sPath, PATHDELIM, folder.GetName());
		}

		AddFolderItemsToList(&folder, sSubPath);
	}
}

void COutlookImportDlg::OnOK()
{
	CDialog::OnOK();

	ASSERT(m_pOutlook && m_pDestTaskFile);

	// make sure nothing has changed
	_NameSpace nmspc(m_pOutlook->GetNamespace(_T("MAPI")));
	nmspc.m_lpDispatch->AddRef(); // to keep it alive
	MAPIFolder mapi(nmspc.GetDefaultFolder(olFolderTasks));
	mapi.m_lpDispatch->AddRef(); // to keep it alive
	_Items items(mapi.GetItems());
	items.m_lpDispatch->AddRef(); // to keep it alive

	int nTaskCount = items.GetCount();
	(nTaskCount);
	int nLBCount = m_lbTasks.GetCount();

	// iterate the listbox items looking for checked items
	for (int nItem = 0; nItem <= nLBCount; nItem++)
	{
		if (m_lbTasks.GetCheck(nItem))
		{
			// get the task index that this item points to
			LPDISPATCH lpd = (LPDISPATCH)m_lbTasks.GetItemData(nItem);
			_TaskItem task(lpd);

			// create a new task
			CString sTitle;
			m_lbTasks.GetText(nItem, sTitle); // gets the full path

			HTASKITEM hTask = m_pDestTaskFile->NewTask(ATL::CT2A(sTitle));
			ASSERT(hTask);

			SetTaskAttributes(hTask, &task);

			// delete the item as we go if required
			if (m_bRemoveOutlookTasks)
			{
				DeleteTaskFromFolder(&task, &mapi);
			}
		}
	}

	AfxGetApp()->WriteProfileInt(_T("Importers\\Outlook"), _T("RemoveOutlookTasks"), m_bRemoveOutlookTasks);
}

BOOL COutlookImportDlg::DeleteTaskFromFolder(_TaskItem* pTask, MAPIFolder* pFolder)
{
	// look through this folders tasks first
	_Items items(pFolder->GetItems());
	items.m_lpDispatch->AddRef(); // to keep it alive
	int nItem, nTaskCount = items.GetCount();

	for (nItem = 1; nItem <= nTaskCount; nItem++)
	{
		LPDISPATCH lpd = items.Item(COleVariant((short)nItem));
		_TaskItem taskTest(lpd);

		if (TaskPathsMatch(pTask, &taskTest))
		{
			items.Remove(nItem);
			return TRUE;
		}
	}

	// then for subfolders
	_Items folders(pFolder->GetFolders());
	int nCount = folders.GetCount();

	for (nItem = 1; nItem <= nCount; nItem++)
	{
		MAPIFolder folder(folders.Item(COleVariant((short)nItem)));

		if (DeleteTaskFromFolder(pTask, &folder))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL COutlookImportDlg::TaskPathsMatch(_TaskItem* pTask1, _TaskItem* pTask2)
{
	CString sPath1 = GetFullPath(pTask1);
	CString sPath2 = GetFullPath(pTask2);

	return (sPath1 == sPath2);
}

CString COutlookImportDlg::GetFullPath(_TaskItem* pTask)
{
	CString sPath(pTask->GetSubject()), sFolder;
	LPDISPATCH lpd = pTask->GetParent();

	do
	{
		try
		{
			MAPIFolder folder(lpd);
			sFolder = folder.GetName(); // will throw when we hit the highest level
			sPath = sFolder + PATHDELIM + sPath;

			lpd = folder.GetParent();
		}
		catch (...)
		{
			break;
		}
	}
	while (true);

	return sPath;
}

void COutlookImportDlg::SetTaskAttributes(HTASKITEM hTask, _TaskItem* pTask)
{
	// set it's attributes
	m_pDestTaskFile->SetTaskComments(hTask, ATL::CT2A(pTask->GetBody()));

	// can have multiple categories
	CStringArray aCats;
	Misc::Split(pTask->GetCategories(), aCats);

	for (int nCat = 0; nCat < aCats.GetSize(); nCat++)
	{
		m_pDestTaskFile->AddTaskCategory(hTask, ATL::CT2A(aCats[nCat]));
	}
	
	if (pTask->GetComplete())
	{
		m_pDestTaskFile->SetTaskDoneDate(hTask, ConvertDate(pTask->GetDateCompleted()));
	}

	m_pDestTaskFile->SetTaskDueDate(hTask, ConvertDate(pTask->GetDueDate()));
	m_pDestTaskFile->SetTaskStartDate(hTask, ConvertDate(pTask->GetStartDate()));
	m_pDestTaskFile->SetTaskCreationDate(hTask, ConvertDate(pTask->GetCreationTime()));
	m_pDestTaskFile->SetTaskLastModified(hTask, ConvertDate(pTask->GetLastModificationTime()));
	m_pDestTaskFile->SetTaskExternalID(hTask, ATL::CT2A(pTask->GetEntryID()));
	m_pDestTaskFile->SetTaskAllocatedBy(hTask, ATL::CT2A(pTask->GetDelegator()));
	m_pDestTaskFile->SetTaskAllocatedTo(hTask, ATL::CT2A(pTask->GetOwner()));
	m_pDestTaskFile->SetTaskPriority(hTask, (unsigned char)(pTask->GetImportance() * 5));
}

void COutlookImportDlg::OnDestroy()
{
	CDialog::OnDestroy();

	delete m_pOutlook;
	delete m_pFolder;

	m_pOutlook = NULL;
	m_pDestTaskFile = NULL;
	m_pFolder = NULL;
}

time_t COutlookImportDlg::ConvertDate(DATE date)
{
	if (date <= 0.0)
	{
		return 0;
	}

	SYSTEMTIME st;
	COleDateTime dt(date);

	dt.GetAsSystemTime(st);

	tm t = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, 0 };
	return mktime(&t);
}

void COutlookImportDlg::OnChoosefolder()
{
	_NameSpace nmspc(m_pOutlook->GetNamespace(_T("MAPI")));
	nmspc.m_lpDispatch->AddRef(); // to keep it alive

	delete m_pFolder;
	m_pFolder = new MAPIFolder(nmspc.PickFolder());

	m_lbTasks.ResetContent();
	AddFolderItemsToList(m_pFolder);
}
