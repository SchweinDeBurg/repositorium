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
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
		return TRUE;

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
	if (!m_hImageList && !Initialize())
		return -1;

	SHFILEINFO sfi;

	UINT nFlags = SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES  | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szFile, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), nFlags);

	if (hIL)
		return sfi.iIcon;

	return -1;
}

int CSysImageList::GetFileImageIndex(LPCTSTR szFilePath, BOOL bFailUnKnown)
{
	if (!m_hImageList && !Initialize() || !szFilePath || !(*szFilePath))
		return -1;

	// test for web protocol
	if (IsWebAddress(szFilePath))
	{
		if (m_nHtmlImage == -1)
			m_nHtmlImage = GetImageIndex(_T("test.html"));

		return m_nHtmlImage;
	}

	// get the file's extension
	CString sExt;
	FileMisc::SplitPath(szFilePath, NULL, NULL, NULL, &sExt);

	// check if its a folder first if it has no extension
	if (sExt.IsEmpty())
	{
		// if its a remote path then simply assume it is a folder for now
		if (CDriveInfo::IsRemotePath(szFilePath, FALSE))
		{
			if (m_nRemoteFolderImage == -1)
				m_nRemoteFolderImage = GetImageIndex(_T("\\\\dummy\\."));

			return m_nRemoteFolderImage;
		}

		// else
		if (FileMisc::FolderExists(szFilePath))
			return m_nFolderImage;
	}

	// fail if no extension
	if (bFailUnKnown && sExt.IsEmpty())
		return -1;

	// use the entire path if <= MAX_PATH in length else just the extension
	if (lstrlen(szFilePath) > MAX_PATH)
		szFilePath = sExt;

	// else
	SHFILEINFO sfi;

	UINT nFlags = SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szFilePath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), nFlags);

	ASSERT (!hIL || hIL == m_hImageList);

	m_hImageList = hIL;

	if (!hIL)
		return -1;

	return sfi.iIcon;
}

int CSysImageList::GetFolderImageIndex()
{
	if (!m_hImageList && !Initialize())
		return -1;

	return m_nFolderImage;
}

HICON CSysImageList::ExtractAppIcon()
{
	TCHAR szAppPath[MAX_PATH + 1];
	GetModuleFileName(NULL, szAppPath, MAX_PATH);

	return ExtractFileIcon(szAppPath);
}

HICON CSysImageList::ExtractFileIcon(LPCTSTR szFilePath)
{
	if (!m_hImageList && !Initialize())
		return NULL;

	int nIndex = GetFileImageIndex(szFilePath);

	if (nIndex != -1)
		return ImageList_GetIcon(m_hImageList, nIndex, 0);

	return NULL;
}

HICON CSysImageList::ExtractFolderIcon()
{
	if (!m_hImageList && !Initialize())
		return NULL;

	return ImageList_GetIcon(m_hImageList, m_nFolderImage, 0);
}

const CImageList* CSysImageList::GetImageList() const
{
	CImageList* pIL = CImageList::FromHandle(m_hImageList);

	if (pIL)
		return pIL;

	// else
	static CImageList il;
	return &il;
}

CImageList* CSysImageList::GetImageList()
{
	if (!m_hImageList)
		Initialize();

	CImageList* pIL = CImageList::FromHandle(m_hImageList);

	if (pIL)
		return pIL;

	// else backup plan
	static CImageList il;
	return &il;
}

HIMAGELIST CSysImageList::GetHImageList()
{
	if (!m_hImageList)
		Initialize();

	return m_hImageList;
}

BOOL CSysImageList::IsWebAddress(LPCTSTR szFilePath)
{
	if (::PathIsURL(szFilePath))
		return TRUE;

	// check for www.
	return (CString(szFilePath).Find(_T("www.")) == 0);
}
