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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ContentCtrl.h: interface for the CContentCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_)
#define AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IContentControl;
class ISpellCheck;
class IPreferences;
struct UITHEME;

class CONTENTFORMAT : public CString
{
public:
	CONTENTFORMAT(LPCTSTR szID = NULL) : CString(szID) {}
	CONTENTFORMAT(const CString& sID) : CString(sID) {}
	BOOL FormatIsText() const;
};

class CContentCtrl
{
public:
	CContentCtrl(IContentControl* pContentCtrl = NULL);
	virtual ~CContentCtrl();

	BOOL Attach(IContentControl* pContentCtrl);

	int GetContent(unsigned char* pContent) const;
	int GetContent(CString& sContent) const;
	int GetTextContent(CString& sContent) const;

	BOOL SetContent(unsigned char* pContent, int nLength, BOOL bResetSelection);
	BOOL SetContent(const CString& sContent, BOOL bResetSelection);
	BOOL SetTextContent(const TCHAR* szContent, BOOL bResetSelection);
	BOOL SettingContent()
	{
		return m_bSettingContent;
	}

	LPCTSTR GetTypeID() const;
	BOOL IsFormat(const CONTENTFORMAT& cf) const;
	CONTENTFORMAT GetContentFormat() const;

	BOOL SetReadOnly(BOOL bReadOnly);

	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
	BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	operator HWND() const;
	HWND GetSafeHwnd() const
	{
		return *this;
	}

	UINT GetDlgCtrlID()
	{
		return ::GetDlgCtrlID(*this);
	}

	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL HasFocus() const;
	void SetFocus();

	ISpellCheck* GetSpellCheckInterface();

	BOOL Undo();
	BOOL Redo();

	void SetUITheme(const UITHEME* pTheme);

	void SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const;
	void LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey);

protected:
	IContentControl* m_pContentCtrl;
	BOOL m_bSettingContent;
};

#endif // !defined(AFX_CONTENTCTRL_H__BE763781_9932_4582_B3D0_64DEE59B1A92__INCLUDED_)
