// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// UIExtensionMgr.cpp: implementation of the CUIExtensionMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIExtensionMgr.h"
#include "ITaskList.h"
#include "IUIExtension.h"
#include "../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIExtension::CUIExtension(IUIExtension* pExt):
m_pExtension(pExt),
m_pMapWindows(NULL)
{
}

CUIExtension::~CUIExtension()
{
	// cleanup
	Release();
}

CString CUIExtension::GetMenuText()
{
	if (m_pExtension)
	{
		return CString(ATL::CA2T(m_pExtension->GetMenuText()));
	}

	return _T("");
}

HICON CUIExtension::GetIcon()
{
	if (m_pExtension)
	{
		return m_pExtension->GetIcon();
	}

	return NULL;
}

void CUIExtension::SetUITheme(const UITHEME& theme)
{
	if (m_pExtension && m_pMapWindows)
	{
		POSITION pos = m_pMapWindows->GetStartPosition();

		while (pos)
		{
			UIEXTENSIONWINDOW* pUIWnd = NULL;
			DWORD dwDummy;

			m_pMapWindows->GetNextAssoc(pos, dwDummy, pUIWnd);

			if (pUIWnd && pUIWnd->pWindow)
			{
				pUIWnd->pWindow->SetUITheme(theme);
			}
		}
	}
}

void CUIExtension::Release()
{
	if (m_pExtension)
	{
		if (m_pMapWindows)
		{
			POSITION pos = m_pMapWindows->GetStartPosition();

			while (pos)
			{
				UIEXTENSIONWINDOW* pUIWnd = NULL;
				DWORD dwDummy;

				m_pMapWindows->GetNextAssoc(pos, dwDummy, pUIWnd);

				if (pUIWnd)
				{
					if (pUIWnd->pWindow)
					{
						pUIWnd->pWindow->Release();
					}

					delete pUIWnd;
				}
			}

			m_pMapWindows->RemoveAll();
			delete m_pMapWindows;
			m_pMapWindows = NULL;
		}
		m_pExtension->Release();
	}

	m_pExtension = NULL;
}

IUIExtensionWindow* CUIExtension::GetWindow(DWORD dwItemData, HWND hParent, BOOL bAutoCreate)
{
	UIEXTENSIONWINDOW* pUIWnd = FindWindow(dwItemData);

	if (!pUIWnd && bAutoCreate)
	{
		SIZE size = { 300, 200 };
		IUIExtensionWindow* pWindow = m_pExtension->CreateExtensionWindow(hParent, FALSE, &size);

		pUIWnd = new UIEXTENSIONWINDOW(pWindow);
		WindowMap()->SetAt(dwItemData, pUIWnd);
	}

	return pUIWnd ? pUIWnd->pWindow : NULL;
}

BOOL CUIExtension::HasWindow(DWORD dwItemData, BOOL bVisibleOnly) const
{
	UIEXTENSIONWINDOW* pUIWnd = FindWindow(dwItemData);

	if (bVisibleOnly)
	{
		return (pUIWnd && pUIWnd->pWindow->IsShowing());
	}

	// else
	return (pUIWnd != NULL);
}

void CUIExtension::RemoveWindow(DWORD dwItemData)
{
	UIEXTENSIONWINDOW* pUIWnd = FindWindow(dwItemData);

	if (pUIWnd)
	{
		pUIWnd->pWindow->Release();
		delete pUIWnd;
		WindowMap()->RemoveKey(dwItemData);
	}
}

BOOL CUIExtension::ShowWindow(DWORD dwItemData, UI_SHOW nShow)
{
	UIEXTENSIONWINDOW* pUIWnd = FindWindow(dwItemData);

	if (pUIWnd)
	{
		switch (nShow)
		{
		case UIS_HIDE:
			// save current state
			pUIWnd->bVisible = pUIWnd->pWindow->IsShowing();
			return pUIWnd->pWindow->Show(FALSE);

		case UIS_SHOW:
			return pUIWnd->pWindow->Show(TRUE);

		case UIS_RESTORE:
			return pUIWnd->pWindow->Show(pUIWnd->bVisible);
		}
	}

	// else
	return FALSE;
}

void CUIExtension::UpdateWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags)
{
	UIEXTENSIONWINDOW* pUIWnd = FindWindow(dwItemData);

	if (pUIWnd)
	{
		pUIWnd->pWindow->Update(pTasks, dwFlags);
	}
}

CUIExtensionWndMap* CUIExtension::WindowMap()
{
	if (!m_pMapWindows)
	{
		m_pMapWindows = new CUIExtensionWndMap;
	}

	return m_pMapWindows;
};

UIEXTENSIONWINDOW* CUIExtension::FindWindow(DWORD dwItemData) const
{
	UIEXTENSIONWINDOW* pUIWnd = NULL;

	if (m_pMapWindows)
	{
		m_pMapWindows->Lookup(dwItemData, pUIWnd);
	}

	ASSERT(!pUIWnd || pUIWnd->pWindow);

	return (pUIWnd && pUIWnd->pWindow) ? pUIWnd : NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIExtensionMgr::CUIExtensionMgr():
m_bInitialized(FALSE)
{
}

CUIExtensionMgr::~CUIExtensionMgr()
{
	// cleanup
	int nInterface = m_aUIExtensions.GetSize();

	while (nInterface--)
	{
		delete m_aUIExtensions[nInterface];
	}

	m_aUIExtensions.RemoveAll();
}

void CUIExtensionMgr::Initialize()
{
	if (m_bInitialized)
	{
		return;
	}

	// look at every dll from wherever we are now
	CFileFind ff;
	CString sSearchPath = FileMisc::GetAppFileName(), sFolder, sDrive;

	FileMisc::SplitPath(sSearchPath, &sDrive, &sFolder);
	FileMisc::MakePath(sSearchPath, sDrive, sFolder, _T("*"), _T(".dll"));

	BOOL bContinue = ff.FindFile(sSearchPath);

	while (bContinue)
	{
		bContinue = ff.FindNextFile();

		if (!ff.IsDots() && !ff.IsDirectory())
		{
			CString sDllPath = ff.GetFilePath();

			if (IsUIExtemsionDll(sDllPath))
			{
				IUIExtension* m_pExtension = CreateUIExtensionInterface(sDllPath);

				if (m_pExtension)
				{
					m_aUIExtensions.Add(new CUIExtension(m_pExtension));
				}

			}
		}
	}

	m_bInitialized = TRUE;
}

int CUIExtensionMgr::GetNumUIExtensions() const
{
	if (!m_bInitialized)
	{
		return 0;
	}

	return m_aUIExtensions.GetSize();
}

CString CUIExtensionMgr::GetUIExtensionMenuText(int nExtension) const
{
	CString sText;

	if (m_bInitialized)
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
		{
			sText = m_aUIExtensions[nExtension]->GetMenuText();
		}
	}

	return Clean(sText);
}

HICON CUIExtensionMgr::GetUIExtensionIcon(int nExtension) const
{
	if (m_bInitialized)
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
		{
			return m_aUIExtensions[nExtension]->GetIcon();
		}
	}

	return NULL;
}

CString& CUIExtensionMgr::Clean(CString& sText)
{
	sText.TrimLeft();
	sText.TrimRight();

	return sText;
}

IUIExtensionWindow* CUIExtensionMgr::GetExtensionWindow(int nExtension, DWORD dwItemData, HWND hParent,
	BOOL bAutoCreate)
{
	IUIExtensionWindow* pWindow = NULL;

	if (m_bInitialized)
	{
		if (nExtension >= 0 && nExtension < m_aUIExtensions.GetSize())
		{
			pWindow = m_aUIExtensions[nExtension]->GetWindow(dwItemData, hParent, bAutoCreate);
		}
	}

	return pWindow;
}

BOOL CUIExtensionMgr::HasAnyExtensionWindows(DWORD dwItemData, BOOL bVisibleOnly) const
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		if (m_aUIExtensions[nExtension]->HasWindow(dwItemData, bVisibleOnly))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CUIExtensionMgr::ShowAllExtensionsWindows(DWORD dwItemData, UI_SHOW nShow)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		m_aUIExtensions[nExtension]->ShowWindow(dwItemData, nShow);
	}
}

void CUIExtensionMgr::RemoveAllExtensionsWindows(DWORD dwItemData)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		m_aUIExtensions[nExtension]->RemoveWindow(dwItemData);
	}
}

void CUIExtensionMgr::UpdateAllExtensionsWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		m_aUIExtensions[nExtension]->UpdateWindow(dwItemData, pTasks, dwFlags);
	}
}

void CUIExtensionMgr::SetUITheme(const UITHEME& theme)
{
	int nExtension = GetNumUIExtensions();

	while (nExtension--)
	{
		m_aUIExtensions[nExtension]->SetUITheme(theme);
	}
}
