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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// ContentMgr.cpp: implementation of the CContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ContentMgr.h"
#include "IContentControl.h"
#include "../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContentMgr::CContentMgr() : m_bInitialized(FALSE), m_bSomeBadVersions(FALSE)
{
}

CContentMgr::~CContentMgr()
{
	// cleanup
	int nInterface = m_aContent.GetSize();

	while (nInterface--)
	{
		m_aContent[nInterface]->Release();
	}

	m_aContent.RemoveAll();
}

BOOL CContentMgr::Initialize()
{
	if (m_bInitialized)
	{
		return TRUE;
	}

	// look at every dll from whereever we are now
	CFileFind ff;
	CString sSearchPath = FileMisc::GetModuleFileName(), sFolder, sDrive;

	FileMisc::SplitPath(sSearchPath, &sDrive, &sFolder);
	FileMisc::MakePath(sSearchPath, sDrive, sFolder, _T("*"), _T(".dll"));

	BOOL bContinue = ff.FindFile(sSearchPath);

	while (bContinue)
	{
		bContinue = ff.FindNextFile();

		if (!ff.IsDots() && !ff.IsDirectory())
		{
			CString sDllPath = ff.GetFilePath(), sLog;

			// logging
			sLog.Format(_T("CContentMgr::Initialize(%s)"), static_cast<LPCTSTR>(sDllPath));
			FileMisc::LogText(sLog);

			if (IsContentDll(sDllPath))
			{
				sLog.Format(_T("\'%s\' is a content plugin"), static_cast<LPCTSTR>(sDllPath));
				FileMisc::LogText(sLog);

				int nDllVer = 0;
				IContent* pContent = CreateContentInterface(sDllPath, &nDllVer);

				if (pContent)
				{
					// save
					m_aContent.Add(pContent);
				}
				else
				{
					if (nDllVer < ICONTENTCTRL_VERSION)
					{
						m_bSomeBadVersions = TRUE;
					}

					sLog.Format(_T("*** \'%s\' could NOT be created"), static_cast<LPCTSTR>(sDllPath));
					FileMisc::LogText(sLog);
				}
			}
			else
			{
				sLog.Format(_T("\'%s\' is NOT a content plugin"), static_cast<LPCTSTR>(sDllPath));
				FileMisc::LogText(sLog);
			}
		}
	}

	m_bInitialized = TRUE;
	return TRUE;
}

int CContentMgr::GetNumContent() const
{
	if (!m_bInitialized)
	{
		return 0;
	}

	return m_aContent.GetSize();
}

CString CContentMgr::GetContentTypeID(int nContent) const
{
	if (!m_bInitialized)
	{
		return _T("");
	}

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT(m_aContent[nContent] != NULL);
		return CString(ATL::CA2T(m_aContent[nContent]->GetTypeID()));
	}

	// else
	return _T("");
}

CString CContentMgr::GetContentTypeDescription(int nContent) const
{
	if (!m_bInitialized)
	{
		return _T("");
	}

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT(m_aContent[nContent] != NULL);
		return CString(ATL::CA2T(m_aContent[nContent]->GetTypeDescription()));
	}

	// else
	return _T("");
}

BOOL CContentMgr::ContentFormatIsText(int nContent) const
{
	return GetContentFormat(nContent).FormatIsText();
}

BOOL CContentMgr::ContentFormatIsText(const CString& sTypeID) const
{
	return ContentFormatIsText(FindContent(sTypeID));
}

BOOL CContentMgr::CreateContentControl(int nContent, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle,
	DWORD dwExStyle, const CRect& rect, HWND hwndParent)
{
	if (!m_bInitialized)
	{
		return FALSE;
	}

	if (nContent >= 0 && nContent < m_aContent.GetSize())
	{
		ASSERT(m_aContent[nContent] != NULL);

		IContentControl* pControl = m_aContent[nContent]->CreateCtrl((WORD)nCtrlID, nStyle, rect.left,
			rect.top, rect.Width(), rect.Height(), hwndParent);

		if (pControl && pControl->GetHwnd())
		{
			if (dwExStyle)
			{
				CWnd::ModifyStyleEx(pControl->GetHwnd(), 0, dwExStyle, 0);
			}

			return ctrl.Attach(pControl);
		}

		if (pControl)
		{
			pControl->Release();
		}
	}

	// else
	return FALSE;
}

BOOL CContentMgr::CreateContentControl(const CONTENTFORMAT& cf, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle,
	DWORD dwExStyle, const CRect& rect, HWND hwndParent)
{
	// check if the CContentCtrl already has a valid control
	if (ctrl.GetSafeHwnd() && ctrl.IsFormat(cf))
	{
		return TRUE;
	}

	return CreateContentControl(FindContent(cf), ctrl, nCtrlID, nStyle, dwExStyle, rect, hwndParent);
}

int CContentMgr::FindContent(LPCTSTR szID) const
{
	int nContent = m_aContent.GetSize();

	while (nContent--)
	{
		ASSERT(m_aContent[nContent] != NULL);

		if (GetContentTypeID(nContent).Compare(szID) == 0)
		{
			return nContent;
		}
	}

	// else not found
	return -1;
}

CONTENTFORMAT CContentMgr::GetContentFormat(int nContent) const
{
	return GetContentTypeID(nContent);
}

BOOL CContentMgr::ConvertContentToHtml(const CString& sContent, CString& sHtml, LPCTSTR szID, LPCTSTR szCharSet)
{
	int nContent = FindContent(szID);

	if (nContent == -1)
	{
		return FALSE;
	}

	char* szHtml = NULL;

	ASSERT(m_aContent[nContent] != NULL);

	int nHtmlLen = m_aContent[nContent]->ConvertToHtml((const unsigned char*)(LPCTSTR)sContent, sContent.GetLength(), szCharSet, szHtml);

	if (nHtmlLen)
	{
		sHtml = szHtml;
	}

	delete [] szHtml; // cleanup

	return (nHtmlLen > 0);
}
