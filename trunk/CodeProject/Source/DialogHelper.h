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

// DialogHelper.h: interface for the CDialogHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_)
#define AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

enum RT_CTRLSTATE // for SetCtrlState
{
	RTCS_ENABLED,
	RTCS_DISABLED,
	RTCS_READONLY,
};

class CDialogHelper
{
public:
	// safe versions if the window text is empty
	static void DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, short& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, int& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, UINT& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, long& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, CString& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, float& value);
	static void DDX_Text(CDataExchange* pDX, int nIDC, double& value);

	static UINT MessageBoxEx(CWnd* pWnd, UINT nIDText, UINT nIDCaption, UINT nType = MB_OK);
	static UINT MessageBoxEx(CWnd* pWnd, LPCTSTR szText, UINT nIDCaption, UINT nType = MB_OK);

	static BOOL IsChildOrSame(HWND hWnd, HWND hwndChild);
	static BOOL ControlWantsEnter(HWND hwnd);

	// font helper
	static void SetFont(CWnd* pWnd, HFONT hFont, BOOL bRedraw = TRUE);
	static HFONT GetFont(CWnd* pWnd);
	static HFONT GetFont(HWND hWnd);
	
	// comboboxes
	static int SetComboBoxItems(CComboBox& combo, const CStringArray& aItems);
	static int RefreshMaxDropWidth(CComboBox& combo, CDC* pDCRef = NULL);
	static int CalcMaxTextWidth(CComboBox& combo, int nMinWidth = 0, BOOL bDropped = FALSE, CDC* pDCRef = NULL);
	static BOOL SelectItemByValue(CComboBox& combo, int nValue);
	static int GetSelectedItemAsValue(const CComboBox& combo);
	
	// better dialog control shortcut handling
	static BOOL ProcessDialogControlShortcut(const MSG* pMsg);
	static UINT GetShortcut(const CString& sText);

	// child clipping
	void ClipChild(CDC* pDC, CWnd* pWnd, UINT nChildID);
	void ClipChild(CDC* pDC, CWnd* pChild);

	// static helpers
	static CRect OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x = 0, int y = 0);
	static CRect OffsetCtrl(CWnd* pParent, CWnd* pChild, int x = 0, int y = 0);
	static CRect ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx = 0, int cy = 0);
	static CRect MoveCtrl(CWnd* pParent, UINT nCtrlID, int x = 0, int y = 0);

	static void SetControlState(CWnd* pParent, UINT nCtrlID, RT_CTRLSTATE nState);
	static void SetControlState(HWND hCtrl, RT_CTRLSTATE nState);
	static void SetControlsState(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, RT_CTRLSTATE nState);

	static void ShowControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow = TRUE);
	static void ShowControl(CWnd* pParent, UINT nCtrlID, BOOL bShow = TRUE);
	static void ShowControl(CWnd* pCtrl, BOOL bShow = TRUE);

	static void ExcludeControls(CWnd* pParent, CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo);
	static void ExcludeChildren(CWnd* pParent, CDC* pDC);
	static void ExcludeChild(CWnd* pParent, CDC* pDC, CWnd* pChild);

	static BOOL IsEdit(CWnd* pParent, UINT nCtrlID);
	static BOOL IsEdit(HWND hCtrl);

	static CString GetClassName(CWnd* pWnd); // returns whatever ::GetClassName() returns

protected:
	CDialogHelper() : m_bInUpdateEx(FALSE) {}

	// helpers for updating only a single item
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, BYTE& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, short& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, int& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, UINT& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, long& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, DWORD& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, float& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, double& value, BOOL bSaveAndValidate = TRUE);
	BOOL UpdateDataEx(CWnd* pWnd, int nIDC, CWnd& ctrl, BOOL bSaveAndValidate = TRUE);
	BOOL InUpdateEx() { return m_bInUpdateEx; }

	// for combo and listboxes only
	BOOL UpdateDataExact(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate = TRUE);

	static CWnd* FindNextMatch(CWnd* pCurrent, UINT nShortcut);

private:
	BOOL m_bInUpdateEx;

};

#endif // !defined(AFX_DIALOGHELPER_H__8543A453_171B_11D4_AE08_0000E8425C3E__INCLUDED_)
