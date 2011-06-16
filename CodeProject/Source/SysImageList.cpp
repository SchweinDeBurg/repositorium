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
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2-6.2.4 sources
//*****************************************************************************

// SysImageList.cpp: implementation of the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysImageList.h"
#include "DriveInfo.h"
#include "FileMisc.h"

#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMap<CString, LPCTSTR, int, int&> CSysImageList::s_mapIndexCache;

CSysImageList::CSysImageList(BOOL bLargeIcons) :
m_bLargeIcons(bLargeIcons),
m_nFolderImage(-1),
m_nHtmlImage(-1),
m_hImageList(NULL),
m_nRemoteFolderImage(-1)
{
}

CSysImageList::~CSysImageList()
{
}

BOOL CSysImageList::Initialize()
{
	if (m_hImageList)
	{
		return TRUE;
	}

	// set up system image list
	TCHAR szWindows[MAX_PATH];
	GetWindowsDirectory(szWindows, MAX_PATH);

	SHFILEINFO sfi;

	UINT nFlags = SHGFI_SYSICONINDEX | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szWindows, 0, &sfi, sizeof(sfi), nFlags);

	if (hIL)
	{
		m_hImageList = hIL;

		// intialize the stock icons
		m_nFolderImage = sfi.iIcon;

		// intialize html and remote folder images on demand
	}

	// else
	return (m_hImageList != NULL);
}

int CSysImageList::GetImageIndex(LPCTSTR szFile)
{
	if (!m_hImageList && !Initialize() || !szFile || !(*szFile))
	{
		return -1;
	}

	SHFILEINFO sfi;

	UINT nFlags = SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES  | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szFile, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), nFlags);

	if (hIL)
	{
		return sfi.iIcon;
	}

	return -1;
}

int CSysImageList::GetStoreImageIndex(int& nIndex, LPCTSTR szFile)
{
	if (nIndex == -1)
	{
		nIndex = GetImageIndex(szFile);
	}

	return nIndex;
}

int CSysImageList::GetFileImageIndex(LPCTSTR szFilePath, BOOL bFailUnKnown)
{
	if (!m_hImageList && !Initialize() || !szFilePath || !(*szFilePath))
	{
		return -1;
	}

	// check index cache
	int nIndex = -1;

	if (s_mapIndexCache.Lookup(szFilePath, nIndex))
	{
		return nIndex;
	}

	// test for web protocol
	if (IsWebAddress(szFilePath))
	{
		nIndex = GetStoreImageIndex(m_nHtmlImage, _T(".html"));
	}
	else
	{
		// get the file's extension
		CString sExt;
		FileMisc::SplitPath(szFilePath, NULL, NULL, NULL, &sExt);

		// check if its a folder first if it has no extension
		BOOL bRemotePath = CDriveInfo::IsRemotePath(szFilePath, FALSE);

		if (sExt.IsEmpty())
		{
			// if its a remote path then simply assume it is a folder for now
			if (bRemotePath)
			{
				nIndex = GetStoreImageIndex(m_nRemoteFolderImage, _T("\\\\dummy\\."));
			}
			else if (IsPath(szFilePath))
			{
				nIndex = m_nFolderImage;
			}
		}

		// fail if no extension
		if (bFailUnKnown && nIndex == -1)
		{
			return -1;
		}

		// use only extension if not an exe
		if (sExt != _T(".exe"))
		{
			nIndex = GetImageIndex(sExt);
		}
		// and only then if a local file
		else if (bRemotePath)
		{
			nIndex = GetStoreImageIndex(m_nRemoteFolderImage, _T("\\\\dummy\\."));
		}
		else // local exe file
		{
			nIndex = GetImageIndex(szFilePath);
		}
	}

	// record for posterity
	if (nIndex != -1)
	{
		s_mapIndexCache[szFilePath] = nIndex;
	}

	return nIndex;
}

BOOL CSysImageList::IsPath(LPCTSTR szText)
{
	// check for back slashes
	return ((_tcsstr(szText, _T("\\\\")) != NULL) ||
		(_tcsstr(szText, _T(":\\")) != NULL) ||
		(_tcsstr(szText, _T("\\")) != NULL) ||
		(_tcsstr(szText, _T(".")) != NULL));
}

int CSysImageList::GetFolderImageIndex()
{
	if (!m_hImageList && !Initialize())
	{
		return -1;
	}

	return m_nFolderImage;
}

HICON CSysImageList::ExtractAppIcon()
{
	return ExtractFileIcon(FileMisc::GetAppFileName());
}

HICON CSysImageList::ExtractFileIcon(LPCTSTR szFilePath)
{
	if (!m_hImageList && !Initialize())
	{
		return NULL;
	}

	int nIndex = GetFileImageIndex(szFilePath);

	if (nIndex != -1)
	{
		return ImageList_GetIcon(m_hImageList, nIndex, 0);
	}

	return NULL;
}

HICON CSysImageList::ExtractFolderIcon()
{
	if (!m_hImageList && !Initialize())
	{
		return NULL;
	}

	return ImageList_GetIcon(m_hImageList, m_nFolderImage, 0);
}

const CImageList* CSysImageList::GetImageList() const
{
	CImageList* pIL = CImageList::FromHandle(m_hImageList);

	if (pIL)
	{
		return pIL;
	}

	// else
	static CImageList il;
	return &il;
}

CImageList* CSysImageList::GetImageList()
{
	if (!m_hImageList)
	{
		Initialize();
	}

	CImageList* pIL = CImageList::FromHandle(m_hImageList);

	if (pIL)
	{
		return pIL;
	}

	// else backup plan
	static CImageList il;
	return &il;
}

HIMAGELIST CSysImageList::GetHImageList()
{
	if (!m_hImageList)
	{
		Initialize();
	}

	return m_hImageList;
}

BOOL CSysImageList::IsWebAddress(LPCTSTR szFilePath)
{
	if (::PathIsURL(szFilePath))
	{
		return TRUE;
	}

	// check for www.
	return (CString(szFilePath).Find(_T("www.")) == 0);
}
