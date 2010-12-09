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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// TrayIcon.h : header file for CTrayIcon
//
//////////////////////////////////////////////////////////////////////////

#if !defined ( TRAYICON_H )
#define TRAYICON_H

#include "Subclass.h"
#include <afxtempl.h>

#ifndef NIIF_NONE
#define NIIF_NONE    0x00
#define NIIF_INFO    0x01
#define NIIF_WARNING 0x02
#define NIIF_ERROR   0x03
#endif

struct NM_TRAYICON
{
	NMHDR hdr;
	POINT ptAction;
};

class CTrayIcon : public CWnd, protected CSubclasser
{
	// Construction
public:
	CTrayIcon();
	virtual ~CTrayIcon();
	BOOL Create(DWORD dwStyle, CWnd* pParentWnd, UINT uID, UINT uIDIcon, UINT uIDTip = 0);
	BOOL Create(DWORD dwStyle, CWnd* pParentWnd, UINT uID, UINT uIDIcon, LPCTSTR sTip = _T(""));
	void ShowTrayIcon(BOOL bShow = TRUE);
	void StartAnimation();
	void StopAnimation();
	void SetAnimationIcons(UINT pIconIDs[], int nNumIcons);
	void SetAnimationDelay(int nDelay);
	BOOL SetTip(UINT uIDNewTip);
	BOOL SetTip(LPCTSTR sNewTip);
	BOOL SetIcon(UINT uIDNewIcon);

	BOOL ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,
			DWORD dwIcon = NIIF_NONE, UINT uTimeout = 5);

	// Attributes
private:
	CString m_sTip;
	HICON m_hIcon;
	BOOL m_bVisible;
	NM_TRAYICON m_nm;
	UINT m_nPrevMsg;
	CUIntArray m_aAnimationIconIDs;
	BOOL m_bAnimationOn;
	int m_nAnimationDelay;
	int m_nCurIcon;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrayIcon)
	afx_msg LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	BOOL AddToTray();
	BOOL DeleteFromTray();
	BOOL ModifyTip(UINT uIDNewTip);
	BOOL ModifyTip(LPCTSTR sNewTip);
	BOOL ModifyIcon(UINT uIDNewIcon);

};

#endif
