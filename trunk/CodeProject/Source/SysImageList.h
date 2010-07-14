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

// SysImageList.h: interface for the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)
#define AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSysImageList
{
public:
	CSysImageList(BOOL bLargeIcons = FALSE);
	virtual ~CSysImageList();

	const CImageList* GetImageList() const; // temporary. should not be stored
	CImageList* GetImageList(); // temporary. should not be stored
	HIMAGELIST GetHImageList();

	BOOL Initialize();
	int GetFileImageIndex(LPCTSTR szFilePath, BOOL bFailUnKnown = FALSE); // will call Initialize if nec.
	int GetFolderImageIndex(); // will call Initialize if nec.

	// caller is responsible for clean up
	HICON ExtractAppIcon();
	HICON ExtractFileIcon(LPCTSTR szFilePath);
	HICON ExtractFolderIcon();

	static BOOL IsWebAddress(LPCTSTR szFilePath);

protected:
	BOOL m_bLargeIcons;
	int m_nFolderImage, m_nHtmlImage, m_nRemoteFolderImage;
	HIMAGELIST m_hImageList;

protected:
	// raw version that resolves directly to SHGetFileInfo
	int GetImageIndex(LPCTSTR szFile); // will call Initialize if nec.

};

#endif // !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)
