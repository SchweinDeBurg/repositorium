// atlmfc71 library.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.
// General copyright (c) 1992-2002 Microsoft Corporation.

// FileDialogEx.cpp - implementation of the CFileDialogEx class

#include "stdafx.h"
#include "FileDialogEx.h"

#if !defined(_INC_SHLWAPI)
#pragma message(__FILE__ " : put <shlwapi.h> in your PCH to speed up compilation")
#include <shlwapi.h>
#endif	// _INC_SHLWAPI

#include <../src/afximpl.h>

#if !defined(__AFXPRIV_H__)
#pragma message(__FILE__ " : put <afxpriv.h> in your PCH to speed up compilation")
#include <afxpriv.h>
#endif	// __AFXPRIV_H__

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #177: variable was declared but never referenced
#pragma warning(disable: 177)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// extracted from "dlgfile.cpp" v7.1

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog,
	LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
	LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize) : CCommonDialog(pParentWnd)
{
	// determine size of OPENFILENAME struct if dwSize is zero
	if (dwSize == 0)
	{
		OSVERSIONINFO vi;
		ZeroMemory(&vi, sizeof(OSVERSIONINFO));
		vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&vi);
		// if running under NT and version is >= 5
		if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion >= 5)
			dwSize = sizeof(OPENFILENAME);
		else
			dwSize = OPENFILENAME_SIZE_VERSION_400;
	}

	// size of OPENFILENAME must be at least version 4
	ASSERT(dwSize >= OPENFILENAME_SIZE_VERSION_400);
	// allocate memory for OPENFILENAME struct based on size passed in
	m_pOFN = static_cast<LPOPENFILENAME>(malloc(dwSize));
	ASSERT(m_pOFN != NULL);
	if (m_pOFN == NULL)
		AfxThrowMemoryException();

	memset(&m_ofn, 0, dwSize); // initialize structure to 0/NULL
	m_szFileName[0] = '\0';
	m_szFileTitle[0] = '\0';
	m_pofnTemp = NULL;

	m_bOpenFileDialog = bOpenFileDialog;
	m_nIDHelp = bOpenFileDialog ? AFX_IDD_FILEOPEN : AFX_IDD_FILESAVE;

	m_ofn.lStructSize = dwSize;
	m_ofn.lpstrFile = m_szFileName;
	m_ofn.nMaxFile = _countof(m_szFileName);
	m_ofn.lpstrDefExt = lpszDefExt;
	m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
	m_ofn.nMaxFileTitle = _countof(m_szFileTitle);
	m_ofn.Flags |= dwFlags | OFN_ENABLEHOOK | OFN_EXPLORER;
	if(dwFlags & OFN_ENABLETEMPLATE)
		m_ofn.Flags &= ~OFN_ENABLESIZING;
	m_ofn.hInstance = AfxGetResourceHandle();
	m_ofn.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

	// setup initial file name
	if (lpszFileName != NULL)
		lstrcpyn(m_szFileName, lpszFileName, _countof(m_szFileName));

	// Translate filter into commdlg format (lots of \0)
	if (lpszFilter != NULL)
	{
		m_strFilter = lpszFilter;
		LPTSTR pch = m_strFilter.GetBuffer(0); // modify the buffer in place
		// MFC delimits with '|' not '\0'
		while ((pch = _tcschr(pch, '|')) != NULL)
			*pch++ = '\0';
		m_ofn.lpstrFilter = m_strFilter;
		// do not call ReleaseBuffer() since the string contains '\0' characters
	}
}

CFileDialogEx::~CFileDialogEx()
{
	free(m_pOFN);
}

const OPENFILENAME& CFileDialogEx::GetOFN() const
{
	return *m_pOFN;
}

OPENFILENAME& CFileDialogEx::GetOFN()
{
	return *m_pOFN;
}

INT_PTR CFileDialogEx::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
	ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

	// zero out the file buffer for consistent parsing later
	ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
	DWORD nOffset = lstrlen(m_ofn.lpstrFile)+1;
	ASSERT(nOffset <= m_ofn.nMaxFile);
	memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));

	// WINBUG: This is a special case for the file open/save dialog,
	//  which sometimes pumps while it is coming up but before it has
	//  disabled the main window.
	HWND hWndFocus = ::GetFocus();
	BOOL bEnableParent = FALSE;
	m_ofn.hwndOwner = PreModal();
	AfxUnhookWindowCreate();
	if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
	{
		bEnableParent = TRUE;
		::EnableWindow(m_ofn.hwndOwner, FALSE);
	}

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	ASSERT(pThreadState->m_pAlternateWndInit == NULL);

	if (m_ofn.Flags & OFN_EXPLORER)
		pThreadState->m_pAlternateWndInit = this;
	else
		AfxHookWindowCreate(this);

	INT_PTR nResult;
	if (m_bOpenFileDialog)
		nResult = ::GetOpenFileName(&m_ofn);
	else
		nResult = ::GetSaveFileName(&m_ofn);

	if (nResult)
		ASSERT(pThreadState->m_pAlternateWndInit == NULL);
	pThreadState->m_pAlternateWndInit = NULL;

	// WINBUG: Second part of special case for file open/save dialog.
	if (bEnableParent)
		::EnableWindow(m_ofn.hwndOwner, TRUE);
	if (::IsWindow(hWndFocus))
		::SetFocus(hWndFocus);

	PostModal();
	return nResult ? nResult : IDCANCEL;
}

CString CFileDialogEx::GetPathName() const
{
	if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		CString strResult;
		if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
			(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
		{
			strResult.Empty();
		}
		else
		{
			strResult.ReleaseBuffer();
		}

		if (!strResult.IsEmpty())
		{
			if (GetParent()->SendMessage(CDM_GETFILEPATH, (WPARAM)MAX_PATH,
				(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
				strResult.Empty();
			else
			{
				strResult.ReleaseBuffer();
				return strResult;
			}
		}
	}
	return m_ofn.lpstrFile;
}

CString CFileDialogEx::GetFileName() const
{
	if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		CString strResult;
		if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
			(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
		{
			strResult.Empty();
		}
		else
		{
			strResult.ReleaseBuffer();
			return strResult;
		}
	}
	return m_ofn.lpstrFileTitle;
}

CString CFileDialogEx::GetFileExt() const
{
	if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		CString strResult;
		LPTSTR pszResult = strResult.GetBuffer(MAX_PATH);
		LRESULT nResult = GetParent()->SendMessage(CDM_GETSPEC, MAX_PATH,
			reinterpret_cast<LPARAM>(pszResult));
		strResult.ReleaseBuffer();
		if (nResult >= 0)
		{
			LPTSTR pszExtension = ::PathFindExtension(strResult);
			if (pszExtension != NULL)
				return pszExtension+1;
		}

		strResult.Empty();
		return strResult;
	}

	if (m_pofnTemp != NULL)
		if (m_pofnTemp->nFileExtension == 0)
			return _T("");
		else
			return m_pofnTemp->lpstrFile + m_pofnTemp->nFileExtension;

	if (m_ofn.nFileExtension == 0)
		return _T("");
	else
		return m_ofn.lpstrFile + m_ofn.nFileExtension;
}

CString CFileDialogEx::GetFileTitle() const
{
	CString strResult = GetFileName();
	LPTSTR pszBuffer = strResult.GetBuffer(_MAX_PATH);
	::PathRemoveExtension(pszBuffer);
	strResult.ReleaseBuffer();
	return strResult;
}

CString CFileDialogEx::GetNextPathName(POSITION& pos) const
{
	BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
	TCHAR chDelimiter;
	if (bExplorer)
		chDelimiter = '\0';
	else
		chDelimiter = ' ';

	LPTSTR lpsz = (LPTSTR)pos;
	if (lpsz == m_ofn.lpstrFile) // first time
	{
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}

		// find char pos after first Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		lpsz = _tcsinc(lpsz);

		// if single selection then return only selection
		if (*lpsz == 0)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}
	}

	CString strPath = m_ofn.lpstrFile;
	if (!bExplorer)
	{
		LPTSTR lpszPath = m_ofn.lpstrFile;
		while(*lpszPath != chDelimiter)
			lpszPath = _tcsinc(lpszPath);
		strPath = strPath.Left(int(lpszPath - m_ofn.lpstrFile));
	}

	LPTSTR lpszFileName = lpsz;
	CString strFileName = lpsz;

	// find char pos at next Delimiter
	while(*lpsz != chDelimiter && *lpsz != '\0')
		lpsz = _tcsinc(lpsz);

	if (!bExplorer && *lpsz == '\0')
		pos = NULL;
	else
	{
		if (!bExplorer)
			strFileName = strFileName.Left(int(lpsz - lpszFileName));

		lpsz = _tcsinc(lpsz);
		if (*lpsz == '\0') // if double terminated then done
			pos = NULL;
		else
			pos = (POSITION)lpsz;
	}

	// only add '\\' if it is needed
	if (!strPath.IsEmpty())
	{
		// check for last back-slash or forward slash (handles DBCS)
		LPCTSTR lpsz = _tcsrchr(strPath, '\\');
		if (lpsz == NULL)
			lpsz = _tcsrchr(strPath, '/');
		// if it is also the last character, then we don't need an extra
		if (lpsz != NULL &&
			(lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
		{
			ASSERT(*lpsz == '\\' || *lpsz == '/');
			return strPath + strFileName;
		}
	}
	return strPath + '\\' + strFileName;
}

void CFileDialogEx::SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID)
{
	if (m_ofn.Flags & OFN_EXPLORER)
		m_ofn.lpTemplateName = lpWin4ID;
	else
		m_ofn.lpTemplateName = lpWin3ID;
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
}

CString CFileDialogEx::GetFolderPath() const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(m_ofn.Flags & OFN_EXPLORER);

	CString strResult;
	if (GetParent()->SendMessage(CDM_GETFOLDERPATH, (WPARAM)MAX_PATH, (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
		strResult.Empty();
	else
		strResult.ReleaseBuffer();
	return strResult;
}

void CFileDialogEx::SetControlText(int nID, LPCSTR lpsz)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(m_ofn.Flags & OFN_EXPLORER);
	GetParent()->SendMessage(CDM_SETCONTROLTEXT, (WPARAM)nID, (LPARAM)lpsz);
}

void CFileDialogEx::HideControl(int nID)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(m_ofn.Flags & OFN_EXPLORER);
	GetParent()->SendMessage(CDM_HIDECONTROL, (WPARAM)nID, 0);
}

void CFileDialogEx::SetDefExt(LPCSTR lpsz)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(m_ofn.Flags & OFN_EXPLORER);
	GetParent()->SendMessage(CDM_SETDEFEXT, 0, (LPARAM)lpsz);
}

UINT CFileDialogEx::OnShareViolation(LPCTSTR)
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	return OFN_SHAREWARN; // default
}

BOOL CFileDialogEx::OnFileNameOK()
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	return FALSE;
}

void CFileDialogEx::OnLBSelChangedNotify(UINT, UINT, UINT)
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	// no default processing needed
}

void CFileDialogEx::OnInitDone()
{
	ASSERT_VALID(this);
	GetParent()->CenterWindow();

	// Do not call Default() if you override
	// no default processing needed
}

void CFileDialogEx::OnFileNameChange()
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	// no default processing needed
}

void CFileDialogEx::OnFolderChange()
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	// no default processing needed
}

void CFileDialogEx::OnTypeChange()
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	// no default processing needed
}

BOOL CFileDialogEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);

	// allow message map to override
	if (CCommonDialog::OnNotify(wParam, lParam, pResult))
		return TRUE;

	OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
	switch(pNotify->hdr.code)
	{
	case CDN_INITDONE:
		OnInitDone();
		return TRUE;
	case CDN_SELCHANGE:
		OnFileNameChange();
		return TRUE;
	case CDN_FOLDERCHANGE:
		OnFolderChange();
		return TRUE;
	case CDN_SHAREVIOLATION:
		*pResult = OnShareViolation(pNotify->pszFile);
		return TRUE;
	case CDN_HELP:
		if (!SendMessage(WM_COMMAND, ID_HELP))
			SendMessage(WM_COMMANDHELP, 0, 0);
		return TRUE;
	case CDN_FILEOK:
		*pResult = OnFileNameOK();
		return TRUE;
	case CDN_TYPECHANGE:
		OnTypeChange();
		return TRUE;
	}

	return FALSE;   // not handled
}

////////////////////////////////////////////////////////////////////////////
// CFileDialogEx diagnostics

#ifdef _DEBUG
void CFileDialogEx::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);

	if (m_bOpenFileDialog)
		dc << "File open dialog";
	else
		dc << "File save dialog";
	dc << "\nm_ofn.hwndOwner = " << (void*)m_ofn.hwndOwner;
	dc << "\nm_ofn.nFilterIndex = " << m_ofn.nFilterIndex;
	dc << "\nm_ofn.lpstrFile = " << m_ofn.lpstrFile;
	dc << "\nm_ofn.nMaxFile = " << m_ofn.nMaxFile;
	dc << "\nm_ofn.lpstrFileTitle = " << m_ofn.lpstrFileTitle;
	dc << "\nm_ofn.nMaxFileTitle = " << m_ofn.nMaxFileTitle;
	dc << "\nm_ofn.lpstrTitle = " << m_ofn.lpstrTitle;
	dc << "\nm_ofn.Flags = " << (LPVOID)m_ofn.Flags;
	dc << "\nm_ofn.lpstrDefExt = " << m_ofn.lpstrDefExt;
	dc << "\nm_ofn.nFileOffset = " << m_ofn.nFileOffset;
	dc << "\nm_ofn.nFileExtension = " << m_ofn.nFileExtension;

	dc << "\nm_ofn.lpstrFilter = ";
	LPCTSTR lpstrItem = m_ofn.lpstrFilter;
	LPTSTR lpszBreak = _T("|");

	while (lpstrItem != NULL && *lpstrItem != '\0')
	{
		dc << lpstrItem << lpszBreak;
		lpstrItem += lstrlen(lpstrItem) + 1;
	}
	if (lpstrItem != NULL)
		dc << lpszBreak;

	dc << "\nm_ofn.lpstrCustomFilter = ";
	lpstrItem = m_ofn.lpstrCustomFilter;
	while (lpstrItem != NULL && *lpstrItem != '\0')
	{
		dc << lpstrItem << lpszBreak;
		lpstrItem += lstrlen(lpstrItem) + 1;
	}
	if (lpstrItem != NULL)
		dc << lpszBreak;

	if (m_ofn.lpfnHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";

	dc << "\n";
}
#endif //_DEBUG

IMPLEMENT_DYNAMIC(CFileDialogEx, CDialog)

#if !defined(_AFX_ENABLE_INLINES)
// inline functions expanded out-of-line

static char _szFileDialogEx_inl[] = "FileDialogEx.inl";
#undef THIS_FILE
#define THIS_FILE _szFileDialogEx_inl
#define _FileDialogEx_INLINE

#include "FileDialogEx.inl"

#endif	// _AFX_ENABLE_INLINES

// import libraries
#pragma comment(lib, "shlwapi.lib")

// end of file
