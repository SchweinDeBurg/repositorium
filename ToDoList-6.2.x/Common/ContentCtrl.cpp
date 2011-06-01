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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ContentCtrl.cpp: implementation of the CContentCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ContentCtrl.h"
#include "../../CodeProject/Source/AutoFlag.h"
#include "../../CodeProject/Source/Misc.h"
#include "UITheme.h"

#include "IContentControl.h"
#include "ISpellCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

BOOL CONTENTFORMAT::FormatIsText() const
{
	GUID id;
	return (Misc::GuidFromString(*this, id) == FALSE);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentCtrl::CContentCtrl(IContentControl* pContentCtrl) :
m_pContentCtrl(pContentCtrl), m_bSettingContent(FALSE)
{
}

CContentCtrl::~CContentCtrl()
{
	if (m_pContentCtrl)
	{
		m_pContentCtrl->Release();
		m_pContentCtrl = NULL;
	}
}

CContentCtrl::operator HWND() const
{
	if (m_pContentCtrl)
	{
		return m_pContentCtrl->GetHwnd();
	}

	// else
	return NULL;
}

BOOL CContentCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_pContentCtrl)
	{
		// allow tooltip handling thru
		CWnd* pWnd = CWnd::FromHandle(GetSafeHwnd());
		pWnd->FilterToolTipMessage(pMsg);

		// only process if we have the focus
		if (HasFocus())
		{
			return m_pContentCtrl->ProcessMessage(pMsg);
		}
	}

	return FALSE;
}

BOOL CContentCtrl::Attach(IContentControl* pContentCtrl)
{
	ASSERT(pContentCtrl && pContentCtrl->GetHwnd());

	if (pContentCtrl && pContentCtrl->GetHwnd())
	{
		// release existing control
		if (m_pContentCtrl)
		{
			m_pContentCtrl->Release();
		}

		m_pContentCtrl = pContentCtrl;
		return TRUE;
	}

	// else
	return FALSE;
}

void CContentCtrl::SetUITheme(const UITHEME* pTheme)
{
	if (m_pContentCtrl)
	{
		m_pContentCtrl->SetUITheme(pTheme);
	}
}

void CContentCtrl::SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const
{
	if (m_pContentCtrl && szKey && *szKey)
	{
		m_pContentCtrl->SavePreferences(pPrefs, szKey);
	}
}

void CContentCtrl::LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey)
{
	if (m_pContentCtrl && szKey && *szKey)
	{
		m_pContentCtrl->LoadPreferences(pPrefs, szKey);
	}
}

BOOL CContentCtrl::HasFocus() const
{
	if (m_pContentCtrl)
	{
		HWND hFocus = ::GetFocus();
		HWND hThis = GetSafeHwnd();

		return (hFocus == hThis) || ::IsChild(hThis, hFocus);
	}

	// else
	return FALSE;
}

void CContentCtrl::SetFocus()
{
	HWND hwndThis = GetSafeHwnd();

	if (::IsWindowEnabled(hwndThis)/* && !HasFocus()*/)
	{
		::SetFocus(hwndThis);
	}
}

ISpellCheck* CContentCtrl::GetSpellCheckInterface()
{
	if (m_pContentCtrl)
	{
		return m_pContentCtrl->GetSpellCheckInterface();
	}

	// else
	return NULL;
}

BOOL CContentCtrl::Undo()
{
	if (m_pContentCtrl)
	{
		return m_pContentCtrl->Undo();
	}

	// else
	return FALSE;
}

BOOL CContentCtrl::Redo()
{
	if (m_pContentCtrl)
	{
		return m_pContentCtrl->Redo();
	}

	// else
	return FALSE;
}

BOOL CContentCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if (GetSafeHwnd())
	{
		return CWnd::ModifyStyle(*this, dwRemove, dwAdd, nFlags);
	}

	// else
	return FALSE;
}

BOOL CContentCtrl::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if (GetSafeHwnd())
	{
		return CWnd::ModifyStyleEx(*this, dwRemove, dwAdd, nFlags);
	}

	// else
	return FALSE;
}

LRESULT CContentCtrl::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pContentCtrl)
	{
		return ::SendMessage(*this, message, wParam, lParam);
	}

	return 0L;
}

BOOL CContentCtrl::SetReadOnly(BOOL bReadOnly)
{
	if (m_pContentCtrl)
	{
		m_pContentCtrl->SetReadOnly(bReadOnly != FALSE);
		return TRUE;
	}

	return FALSE;
}

BOOL CContentCtrl::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pContentCtrl)
	{
		return ::PostMessage(*this, message, wParam, lParam);
	}

	return FALSE;
}

int CContentCtrl::GetContent(unsigned char* pContent) const
{
	if (m_pContentCtrl)
	{
		return m_pContentCtrl->GetContent(pContent);
	}

	// else
	return 0;
}

BOOL CContentCtrl::SetContent(unsigned char* pContent, int nLength, BOOL bResetSelection)
{
	CAutoFlag af(m_bSettingContent, TRUE);

	if (m_pContentCtrl)
	{
		return m_pContentCtrl->SetContent(pContent, nLength, bResetSelection);
	}

	// else
	return false;
}

BOOL CContentCtrl::SetContent(const CString& sContent, BOOL bResetSelection)
{
	return SetContent((unsigned char*)(LPCTSTR)sContent, sContent.GetLength(), bResetSelection);
}

int CContentCtrl::GetContent(CString& sContent) const
{
	if (m_pContentCtrl)
	{
		int nLen = m_pContentCtrl->GetContent(NULL);

		if (nLen) // excludes NULL
		{
			unsigned char* szContent = (unsigned char*)sContent.GetBufferSetLength(nLen);
			nLen = m_pContentCtrl->GetContent(szContent);
			sContent.ReleaseBuffer(nLen);
			return nLen;
		}
	}

	// else
	sContent.Empty();
	return 0;
}

int CContentCtrl::GetTextContent(CString& sContent) const
{
	if (m_pContentCtrl)
	{
		int nLen = m_pContentCtrl->GetTextContent(NULL);

		if (nLen)
		{
			TCHAR* szContent = sContent.GetBufferSetLength(nLen);
			VERIFY(nLen + 1 == m_pContentCtrl->GetTextContent(szContent, nLen + 1));
			sContent.ReleaseBuffer(nLen);
			return nLen;
		}
	}

	// else
	sContent.Empty();
	return 0;
}

BOOL CContentCtrl::SetTextContent(const TCHAR* szContent, BOOL bResetSelection)
{
	CAutoFlag af(m_bSettingContent, TRUE);

	if (m_pContentCtrl)
	{
		return m_pContentCtrl->SetTextContent(szContent, bResetSelection);
	}

	// else
	return false;
}

LPCTSTR CContentCtrl::GetTypeID() const
{
	if (m_pContentCtrl)
	{
		return ATL::CA2T(m_pContentCtrl->GetTypeID());
	}

	// else
	return _T("");
}

BOOL CContentCtrl::IsFormat(const CONTENTFORMAT& cf) const
{
	return (!cf.IsEmpty() && cf == GetContentFormat());
}

CONTENTFORMAT CContentCtrl::GetContentFormat() const
{
	return GetTypeID();
}
