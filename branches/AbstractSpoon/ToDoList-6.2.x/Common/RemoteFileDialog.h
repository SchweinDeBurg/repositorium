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
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
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

#if !defined(AFX_REMOTEFILEDIALOG_H__1EB7B1E7_62C6_4402_945E_D60A50455E39__INCLUDED_)
#define AFX_REMOTEFILEDIALOG_H__1EB7B1E7_62C6_4402_945E_D60A50455E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoteFileDialog.h : header file
//

#include "FileEdit.h"
#include "../../CodeProject/Source/RuntimeDlg.h"

#include <afxinet.h>
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CRemoteFileDialog dialog

enum // flags
{
	RFD_DOWNLOAD        = 0x0000,
	RFD_UPLOAD          = 0x0001,
	RFD_FILEMUSTEXIST   = 0x0002, // implies RFD_DOWNLOAD
	RFD_MULTISELECT     = 0x0004,
	RFD_FOLDERSELECT    = 0x0008, // upload only, excludes files
	RFD_NONAVIGATE      = 0x0010, // use is restricted to initial directory
};

struct FILERESULT
{
	FILERESULT(LPCTSTR szFilePath = NULL, DWORD size = 0)
	{
		dwSize = size;

		if (szFilePath)
		{
			sFilePath = szFilePath;
			sFilePath.Replace(_T('\\'), _T('/'));

			TCHAR szFilename[_MAX_FNAME], szExt[_MAX_EXT];
#if _MSC_VER >= 1400
			_tsplitpath_s(szFilePath, NULL, 0, NULL, 0, szFilename, _MAX_FNAME, szExt, _MAX_EXT);
#else
			_tsplitpath(szFilePath, NULL, NULL, szFilename, szExt);
#endif

			sFileName.Format(_T("%s%s"), szFilename, szExt);
		}
	}

	CString sFileName; // sans path
	CString sFilePath; // relative to root
	DWORD dwSize;
};

typedef CArray<FILERESULT, FILERESULT&> CFRArray;

class CRemoteFileDialog : public CRuntimeDlg
{
// Construction
public:
	CRemoteFileDialog(CFtpConnection* pConnection, LPCTSTR szServer, LPCTSTR szFilters = NULL, LPCTSTR szInitialFolder = NULL);   // standard constructor
	virtual ~CRemoteFileDialog();

	int DoModal(DWORD dwOptions = RFD_DOWNLOAD | RFD_FILEMUSTEXIST | RFD_MULTISELECT, LPCTSTR szFilename = NULL);

	BOOL IsRootFolder()
	{
		return m_bRoot;
	}
	CString GetFolder()
	{
		return m_bRoot ? _T("") : m_sCurFolder;
	}
	CString GetFirstPath();

	int GetPathCount()
	{
		return m_aFiles.GetSize();
	}
	void GetPaths(CFRArray& aFiles)
	{
		aFiles.Copy(m_aFiles);
	}

protected:
// Dialog Data
	//{{AFX_DATA(CRemoteFileDialog)
	CComboBox   m_cbFileTypes;
	CListCtrl   m_lcFiles;
	CString m_sCurFolder;
	CString m_sFilenames;
	CString m_sServer;
	//}}AFX_DATA
	CString m_sFilterExt;
	CFileEdit   m_eCurFolder;
	CFileEdit   m_eFilename;
	CFtpConnection* m_pConnection;
	DWORD m_dwFileSize; // selected file
	CToolBar m_toolbar;
	CFRArray m_aFiles;
	DWORD m_dwOptions;
	BOOL m_bFilling;

	struct FILEFILTER
	{
		FILEFILTER() {}
		FILEFILTER(LPCTSTR szName, LPCTSTR szExt) : sName(szName), sExt(szExt) {}

		CString sName;
		CString sExt;
	};

	CArray<FILEFILTER, FILEFILTER&> m_aFilters;

	// sorting
	struct FILEITEM
	{
		FILEITEM()
		{
			nType = 0;
			dwSize = 0;
			dLastMod = 0.0;
		}

		CString sFilename;
		int nType;
		DWORD dwSize;
		COleDateTime dLastMod;
	};

	typedef CArray<FILEITEM, FILEITEM&> CFIArray;
	typedef CMap<int, int, FILEITEM, FILEITEM&> CFileMap;
	CFileMap m_mapItems;
	BOOL m_bRoot;
	CSysImageList m_silLarge, m_silSmall;
	BOOL m_bInitReport;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteFileDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();
	virtual void PostCreate();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemoteFileDialog)
	afx_msg void OnSelchangeFiletypes();
	afx_msg void OnChangeFilename();
	afx_msg void OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewMenu();
	afx_msg void OnUpOneLevel();
	afx_msg void OnChangeView(UINT nCmdID);
	afx_msg BOOL OnToolbarNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	void SetCurrentFolder(LPCTSTR szFolder);
	int GetFirstSelectedItem(); // assumes single selection
	int AddFileItem(LPCTSTR szFileName, int nType, UINT nUniqueID, DWORD dwFileSize,
		const FILETIME* pLastMod = NULL, int nImage = -1);
	int GetItemType(int nItem);
	DWORD GetItemSize(int nItem);
	void InitFilterArray(LPCTSTR szFilters);
	void FillFileList(); // for whereever its at
	int FindMatch(LPCTSTR szFilename);
	BOOL FileMustExist()
	{
		return !(m_dwOptions & RFD_UPLOAD) && (m_dwOptions & RFD_FILEMUSTEXIST);
	}
	BOOL FolderSelect()
	{
		return (m_dwOptions & RFD_UPLOAD) && (m_dwOptions & RFD_FOLDERSELECT);
	}
	void UpdateOKButton(BOOL bRefreshResults = TRUE);
	void UpdateFileResults();

	static CString FormatSize(DWORD dwSize);
	static CString FormatDate(double dDate);
	static int CALLBACK SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEFILEDIALOG_H__1EB7B1E7_62C6_4402_945E_D60A50455E39__INCLUDED_)
