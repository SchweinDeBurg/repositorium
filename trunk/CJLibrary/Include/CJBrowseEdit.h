// CJCJBrowseEdit.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:35p $
 * $Revision: 9 $
 * $Archive: /CodeJock/Include/CJBrowseEdit.h $
 *
 * $History: CJBrowseEdit.h $
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:35p
 * Updated in $/CodeJock/Include
 * Modified resource include for static builds.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/25/99    Time: 11:03a
 * Updated in $/CodeJockey/Include
 * Chaned image list color mask and export macros.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:41p
 * Updated in $/CodeJockey/Include
 * Cleaned up control and pointer initialization, creation and activation.
 * Added support for CCJMenu popup.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:54p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 10:53p
 * Created in $/CodeJockey/Include
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJBROWSEEDIT_H__
#define __CJBROWSEEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// holds the current search style for the control.
typedef enum BROWSE_TYPE {
	BT_DIRECTORY = 1,
	BT_FILE		 = 2,
	BT_POPUPMENU = 3
};

// CCJBrowseButton is a CButton derived class which is used by
// the CCJBrowseEdit class.
class _CJX_EXT_CLASS CCJBrowseButton : public CButton
{

public:

	// Default constructor
	//
	CCJBrowseButton();

	// Virtual destructor
	//
	virtual ~CCJBrowseButton();

public:

	BOOL			m_bOpenFileDialog;	// TRUE for open file dialog, FALSE for 
										// save as see SetDlgOpenFile(...)
	CString			m_strDefExt;
	CString			m_strFileName;
	DWORD			m_dwFlags;
	CString			m_strFilter;
	CString			m_strTitle;
	CWnd*			m_pParentWnd;	// parent of edit control.

protected:
	
	UINT			m_nID;			// resource id for push button
	CWnd*			m_pEditWnd;		// edit control.
	CFont			m_Font;			// button font
	BROWSE_TYPE		m_eType;		// search type
	UINT			m_nMenu;		// popup menu id
	UINT			m_nSubMenuIndx;	// index of popup submenu.
	HICON			m_hIcon;		// handle to button icon.

public:
	virtual void UpdateFont();

	// returns an unused resource id for use by the control
	//
	virtual UINT GetNextID(
		// window to search for availiable resource ids
		CWnd* pWnd) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJBrowseButton)
	public:
	virtual BOOL Create(CWnd* pWnd, BROWSE_TYPE eType, UINT nMenu, UINT nSubMenuIndx);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:

	//{{AFX_MSG(CCJBrowseButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

// CCJBrowseEdit is a CEdit derived class which has a push button associated
// with it. It can be used to search for directories, files or activate a
// popup menu
class _CJX_EXT_CLASS CCJBrowseEdit : public CEdit
{
public:

	// Default constructor
	//
	CCJBrowseEdit();

	// Virtual destructor
	//
	virtual ~CCJBrowseEdit();

protected:

	CCJBrowseButton		m_btnBrowse;	// push button
	BROWSE_TYPE			m_eType;		// search type
	UINT				m_nMenu;		// popup menu id
	UINT				m_nSubMenuIndx;	// index of popup submenu.

public:
	
	// this member function will initialize the control to
	// default values.
	//
	virtual void InitializeControl();

	// Set to TRUE to construct a File Open dialog box or 
	// FALSE to construct a File Save As dialog box
	//
	virtual void SetDlgOpenFile(BOOL bOpenFileDialog);

	// this member function sets the default extension for the file
	// open dialog.
	//
	virtual void SetDlgDefExt(LPCTSTR strDefExt);

	// this member funciton sets the default file name for the file
	// open dialog.
	//
	virtual void SetDlgFileName(LPCTSTR strFileName);

	// this member function sets the style flags for the file
	// open dialog.
	//
	virtual void SetDlgStyle(DWORD dwFlags);

	// this member function sets the file filter for the file
	// open dialog.
	//
	virtual void SetDlgFilter(LPCTSTR strFilter);

	// this member function sets the owner for the file
	// open dialog
	//
	virtual void SetDlgOwner(CWnd* pParentWnd);

	// this member function sets the title for the directory
	// dialog
	//
	virtual void SetDlgTitle(LPCTSTR strTitle);

	// this member function sets the current search type
	// for the control
	//
	virtual void SetSearchType(
		// type of search to perform - BT_FILE, BT_DIRECTORY, BT_POPUPMENU
		BROWSE_TYPE eType,
		// if eType is a popup menu, resource id of menu.
		UINT nMenu = 0,
		// index of submenu to display.
		int nSubMenuIndx = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJBrowseEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJBrowseEdit)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE UINT CCJBrowseButton::GetNextID(CWnd* pWnd) const
	{ for(UINT nID=32767; nID!=1; --nID){if(!pWnd->GetDlgItem(nID)){return nID;}}return 0; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgOpenFile(BOOL bOpenFileDialog=TRUE)
	{ m_btnBrowse.m_bOpenFileDialog = bOpenFileDialog; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgDefExt(LPCTSTR strDefExt=NULL)
	{ m_btnBrowse.m_strDefExt = strDefExt; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgFileName(LPCTSTR strFileName=NULL)
	{ m_btnBrowse.m_strFileName = strFileName; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgStyle(DWORD dwFlags=OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT)
	{ m_btnBrowse.m_dwFlags = dwFlags; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgFilter(LPCTSTR strFilter=NULL)
	{ m_btnBrowse.m_strFilter = strFilter; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgOwner(CWnd* pParentWnd=NULL)
	{ m_btnBrowse.m_pParentWnd = pParentWnd; }

_CJXLIB_INLINE void CCJBrowseEdit::SetDlgTitle(LPCTSTR strTitle=NULL)
	{ m_btnBrowse.m_strTitle = strTitle; }

_CJXLIB_INLINE void CCJBrowseEdit::SetSearchType(BROWSE_TYPE eType, UINT nMenu, int nSubMenuIndx)
	{ m_eType = eType; m_nMenu = nMenu; m_nSubMenuIndx = nSubMenuIndx; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJBROWSEEDIT_H__
