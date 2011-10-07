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
//*****************************************************************************

// FileRegister.h: interface for the CFileRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_)
#define AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileRegister
{
public:
	CFileRegister(LPCTSTR sExt, LPCTSTR szFileType = NULL);
	~CFileRegister();

	BOOL IsRegisteredAppInstalled();
	CString GetRegisteredAppPath(BOOL bFilenameOnly = FALSE);
	BOOL RegisterFileType(LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen = TRUE, LPCTSTR szParams = NULL,
		BOOL bAskUser = TRUE);
	BOOL UnRegisterFileType();
	BOOL IsRegisteredApp();

	static BOOL IsRegisteredAppInstalled(LPCTSTR szExt);
	static CString GetRegisteredAppPath(LPCTSTR szExt, BOOL bFilenameOnly = FALSE);
	static BOOL RegisterFileType(LPCTSTR szExt, LPCTSTR szFileType, LPCTSTR szFileDesc, int nIcon,
		BOOL bAllowShellOpen = TRUE, LPCTSTR szParams = NULL, BOOL bAskUser = TRUE);
	static BOOL UnRegisterFileType(LPCTSTR szExt, LPCTSTR szFileType);
	static BOOL IsRegisteredApp(LPCTSTR szExt, LPCTSTR szFileType = NULL);

protected:
	CString m_sExt;
	CString m_sFileType;
	CString m_sAppPath;
};

#endif // !defined(AFX_FILEREGISTER_H__67F5AB1F_4CE5_405C_A6F5_DE7B3802790A__INCLUDED_)
