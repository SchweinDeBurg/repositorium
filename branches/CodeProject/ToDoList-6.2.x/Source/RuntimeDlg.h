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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_)
#define AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuntimeDlg.h : header file
//

#include "RCCtrlParser.h"
#include "DialogHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg window

enum { RD_TITLE = 0 };

class CRuntimeDlg : public CDialog, public CDialogHelper
{
	DECLARE_DYNAMIC(CRuntimeDlg)

// Construction
public:
	CRuntimeDlg(LPCTSTR szSettingsKey = NULL);

	// do not use
	int DoModal()
	{
		ASSERT(0);
		return -1;
	}
	BOOL Create(LPCTSTR /*lpszTemplateName*/, CWnd* /*pParentWnd*/ = NULL)
	{
		ASSERT(0);
		return FALSE;
	}
	BOOL Create(UINT /*nIDTemplate*/, CWnd* /*pParentWnd*/ = NULL)
	{
		ASSERT(0);
		return FALSE;
	}

	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	void SetFont(HFONT hFont, BOOL bRedraw = TRUE);

// Operations
protected: // intended to be used only from a derived class

#define RTD_DEFSTYLE (WS_VISIBLE | WS_POPUPWINDOW  | WS_CAPTION | DS_CENTER)
#define RTD_DEFEXSTYLE (WS_EX_DLGMODALFRAME)

	virtual int DoModal(LPCTSTR szCaption,
		DWORD dwStyle = RTD_DEFSTYLE,
		DWORD dwExStyle = RTD_DEFEXSTYLE,
		const CRect& rect = rectAuto,
		CWnd* pParentWnd = NULL,
		UINT nID = -1);

	virtual BOOL Create(LPCTSTR szCaption,
		DWORD dwStyle = RTD_DEFSTYLE,
		DWORD dwExStyle = RTD_DEFEXSTYLE,
		const CRect& rect = rectAuto,
		CWnd* pParentWnd = NULL,
		UINT nID = -1);

	virtual void PostCreate() {}

	// default borders are 7 DLU
	void SetBorders(int nAll)
	{
		SetBorders(nAll, nAll, nAll, nAll);
	}
	void SetBordersDLU(int nAll)
	{
		SetBordersDLU(nAll, nAll, nAll, nAll);
	}
	void SetBorders(int nLeft, int nTop, int nRight, int nBottom);
	void SetBordersDLU(int nLeft, int nTop, int nRight, int nBottom);

	// use these when you do _not_ want to subclass the control
	virtual BOOL AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		const CRect& rect, UINT nID); // pos in pixels
	virtual BOOL AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		int x, int y, int cx, int cy, UINT nID); // pos in pixels

	// use these when you _do_ want to subclass via MFC
	virtual BOOL AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		const CRect& rect, UINT nID); // pos in pixels
	virtual BOOL AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		int x, int y, int cx, int cy, UINT nID); // pos in pixels

	// individual RC controls
	virtual BOOL AddRCControl(LPCTSTR szRCType, LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle,
		DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT uIconID = 0); // pos in DLUs

	virtual BOOL AddRCControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle,
		DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT uIconID = 0); // pos in DLUs

	// 'doctored' text straight from an RC file
	virtual int AddRCControls(const CString& sRCControls);

	// call to size the dialog to fit the controls
	// will be called automatically during the create process if 'rectAuto' specified
	void AutoFit();

// Attributes
public:
	static const CRect rectAuto;

protected:
	CRTCtrlList m_lstControls;
	CRect m_rBorders, m_rBordersDLU;
	HICON m_hILarge, m_hISmall;
	CString m_sSettingsKey;

	static CMapStringToString s_mapClasses;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuntimeDlg)
	//}}AFX_VIRTUAL
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CRuntimeDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRuntimeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual HWND CreateControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID = 0);

	// use this when you _do_ want to subclass
	virtual BOOL CreateControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID = 0);

	BOOL CreateControl(const RTCONTROL& ctrl);
	void CreateControls();
	BOOL AddRCControl(const CString& sRCCtrl);
	CSize GetCtrlSize(UINT nCtrlID)
	{
		return OffsetCtrl(nCtrlID).Size();
	}
	CPoint GetCtrlPos(UINT nCtrlID)
	{
		return OffsetCtrl(nCtrlID).TopLeft();
	}
	CRect OffsetCtrl(UINT nCtrlID, int x = 0, int y = 0);
	CRect ResizeCtrl(UINT nCtrlID, int cx = 0, int cy = 0);
	CRect MoveCtrl(UINT nCtrlID, int x, int y);
	void EnableControls(UINT uCtrlIDFrom, UINT nCtrlIDTo, BOOL bEnable = TRUE);
	void ShowControls(UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow = TRUE);
	void ShowControl(UINT nCtrlID, BOOL bShow = TRUE);
	void ExcludeControls(CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo);

	void SetInitialPos(LPCRECT pRect, DWORD dwStyle);
	void SaveCurrentPos();

	virtual CPoint GetInitialPos() const
	{
		return CPoint(-1, -1);
	}
	virtual CString OverrideItemText(UINT /*nIDItem*/)
	{
		return _T("");
	}

	inline BOOL IsEdit(UINT nCtrlID)
	{
		return CDialogHelper::IsEdit(this, nCtrlID);
	}

	static void BuildClassMap();
	static CString GetControlClassName(CWnd* pWnd); // works up the derivation chain till it finds a 'standard' windows control class
	static HWND CreateRichEdit(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
		int x, int y, int cx, int cy, HWND hwndParent);
	static int CalcLinesRequired(const CString& sText, int nWidthDLU);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_)
