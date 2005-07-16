// atlmfc71 library.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.
// General copyright (c) 1992-2002 Microsoft Corporation.

// FileDialogEx.h - interface of the CFileDialogEx class

#if !defined(__FileDialogEx_h)
#define __FileDialogEx_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if (_MSC_VER >= 1300)
#error use native <afxdlgs.h> instead of this header
#endif	// _MSC_VER

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500)
#error this code uses features only available under Windows 2000 or later
#endif	// _WIN32_WINNT

#if !defined(__AFXDLGS_H__)
#pragma message(__FILE__ " : put <afxdlgs.h> in your PCH to speed up compilation")
#include <afxdlgs.h>
#endif	// __AFXDLGS_H__

// extracted from <afxdlgs.h> v7.1

class CFileDialogEx : public CCommonDialog
{
	DECLARE_DYNAMIC(CFileDialogEx)

public:
// Attributes
	__declspec(property(get=GetOFN)) OPENFILENAME m_ofn;
	const OPENFILENAME& GetOFN() const;
	OPENFILENAME& GetOFN();
	LPOPENFILENAME m_pOFN;

// Constructors
	explicit CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);
	virtual ~CFileDialogEx();	

// Operations
	virtual INT_PTR DoModal();

	// Helpers for parsing file name after successful return
	// or during Overridable callbacks if OFN_EXPLORER is set
	CString GetPathName() const;  // return full path and filename
	CString GetFileName() const;  // return only filename
	CString GetFileExt() const;   // return only ext
	CString GetFileTitle() const; // return file title
	BOOL GetReadOnlyPref() const; // return TRUE if readonly checked

	// Enumerating multiple file selections
	POSITION GetStartPosition() const;
	CString GetNextPathName(POSITION& pos) const;

	// Helpers for custom templates
	void SetTemplate(UINT nWin3ID, UINT nWin4ID);
	void SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID);

	// Other operations available while the dialog is visible
	CString GetFolderPath() const; // return full path
	void SetControlText(int nID, LPCSTR lpsz);
	void HideControl(int nID);
	void SetDefExt(LPCSTR lpsz);

// Overridable callbacks
protected:
	friend UINT_PTR CALLBACK _AfxCommDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual UINT OnShareViolation(LPCTSTR lpszPathName);
	virtual BOOL OnFileNameOK();
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

	// only called back if OFN_EXPLORER is set
	virtual void OnInitDone();
	virtual void OnFileNameChange();
	virtual void OnFolderChange();
	virtual void OnTypeChange();

// Implementation
#ifdef _DEBUG
public:
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
	CString m_strFilter;          // filter string
						// separate fields with '|', terminate with '||\0'
	TCHAR m_szFileTitle[64];       // contains file title after return
	TCHAR m_szFileName[_MAX_PATH]; // contains full path name after return

	OPENFILENAME*  m_pofnTemp;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

// inlines
#if defined(_AFX_ENABLE_INLINES)
#define _FileDialogEx_INLINE AFX_INLINE
#include "FileDialogEx.inl"
#endif	// _AFX_ENABLE_INLINES

#endif	// __FileDialogEx_h

// end of file
