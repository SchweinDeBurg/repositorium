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

#if !defined(AFX_URLRICHEDITCTRL_H__B5421D69_41F2_4FCF_AC58_13D2B3D3D3C8__INCLUDED_)
#define AFX_URLRICHEDITCTRL_H__B5421D69_41F2_4FCF_AC58_13D2B3D3D3C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// UrlRichEditCtrl.h : header file
//

#include "../../CodeProject/Source/RichEditBaseCtrl.h"
#include "../../CodeProject/Source/RichEditNcBorder.h"

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl window

const UINT WM_UREN_CUSTOMURL = ::RegisterWindowMessage(_T("WM_UREN_CUSTOMURL")); // lParam == full url

struct URLITEM
{
	CHARRANGE cr;
	CString sUrl;
	BOOL bWantNotify;
};

struct PROTOCOL
{
	PROTOCOL(LPCTSTR szProtocol = NULL, BOOL bNotify = FALSE) : sProtocol(szProtocol), bWantNotify(bNotify) {}

	CString sProtocol;
	BOOL bWantNotify;
};

#include <afxtempl.h>

typedef CArray<URLITEM, URLITEM&> CUrlArray;
typedef CArray<PROTOCOL, PROTOCOL&> CProtocolArray;

class CUrlRichEditCtrl : public CRichEditBaseCtrl//COleRichEditCtrl
{
	// Construction
public:
	CUrlRichEditCtrl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	int GetUrlCount() const
	{
		return m_aUrls.GetSize();
	}
	CString GetUrl(int nURL, BOOL bAsFile = FALSE) const;
	void PathReplaceSel(LPCTSTR lpszPath, BOOL bFile);
	BOOL GoToUrl(const CString& sUrl) const; // must exist
	BOOL GoToUrl(int nUrl) const;
	CPoint GetContextMenuPos()
	{
		return m_ptContextMenu;
	}
	int AddProtocol(LPCTSTR szProtocol, BOOL bWantNotify = TRUE);
	void ParseAndFormatText(BOOL bForceReformat = FALSE);
	int GetContextUrl()
	{
		return m_nContextUrl;
	}

	static void SetGotoErrorMsg(LPCTSTR szErrMsg)
	{
		s_sGotoErrMsg = szErrMsg;
	}
	static void SetCtrlClickMsg(LPCTSTR szMsg)
	{
		s_sCtrlClickMsg = szMsg;
	}

	// Attributes
protected:
	CUrlArray m_aUrls;
	CProtocolArray m_aProtocols;
	CRichEditNcBorder m_ncBorder;

	CPoint m_ptContextMenu;
	int m_nContextUrl;
	static CString s_sGotoErrMsg, s_sCtrlClickMsg;
	CHARRANGE m_crDropSel;
	int m_nFileProtocol;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrlRichEditCtrl)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual int OnToolHitTest(CPoint pt, TOOLINFO* pTI) const;
	virtual LRESULT SendNotifyCustomUrl(LPCTSTR szUrl) const;

	// Implementation
public:
	virtual ~CUrlRichEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUrlRichEditCtrl)
	afx_msg BOOL OnChangeText();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSetText(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDropFiles(WPARAM wp, LPARAM lp);
	afx_msg BOOL OnNotifyLink(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNeedTooltip(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

protected:
	virtual HRESULT QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR* lpcfFormat,
		DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
	virtual HRESULT GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
	virtual HRESULT GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR* lpchrg,
		HMENU FAR* lphmenu);
	virtual CLIPFORMAT GetAcceptableClipFormat(LPDATAOBJECT lpDataOb, CLIPFORMAT format);

protected:
	int GetLineHeight() const;
	int CharFromPoint(const CPoint& point);
	void SetFirstVisibleLine(int nLine);
	CPoint GetCaretPos();
	int FindUrl(const CHARRANGE& cr);
	int FindUrl(const CPoint& point);
	int FindUrlEx(const CPoint& point);
	BOOL UrlsMatch(const CUrlArray& aUrls);
	void TrackDragCursor();
	int MatchProtocol(LPCTSTR szText) const;

	static BOOL IsDelim(LPCTSTR szText);
	static void InsertInOrder(URLITEM& urli, CUrlArray& aUrls);
	static CString CreateFileLink(LPCTSTR szFile);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URLRICHEDITCTRL_H__B5421D69_41F2_4FCF_AC58_13D2B3D3D3C8__INCLUDED_)
