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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// FileRegister.cpp: implementation of the CFileRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileRegister.h"
#include "AfxRegKey.h"
#include "FileMisc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileRegister::CFileRegister(LPCTSTR szExt, LPCTSTR szFileType)
{
	m_sExt = szExt;
	m_sFileType = szFileType;

	m_sExt.TrimRight();
	m_sExt.TrimLeft();
	m_sFileType.TrimRight();
	m_sFileType.TrimLeft();

	if (!m_sExt.IsEmpty() && m_sExt[0] != _T('.'))
	{
		m_sExt = _T(".") + m_sExt;
	}

	// get the app path
	m_sAppPath = FileMisc::GetAppFileName();

	ASSERT(!m_sAppPath.IsEmpty());
}

CFileRegister::~CFileRegister()
{
}

BOOL CFileRegister::RegisterFileType(LPCTSTR szFileDesc, int nIcon, BOOL bAllowShellOpen, LPCTSTR szParams, BOOL bAskUser)
{
	CAfxRegKey reg;
	CString sKey;
	CString sEntry;
	int nRet = IDYES;
	CString sMessage;
	BOOL bSuccess = TRUE, bChange = FALSE;

	ASSERT(!m_sExt.IsEmpty());
	ASSERT(!m_sFileType.IsEmpty());

	if (m_sExt.IsEmpty() || m_sFileType.IsEmpty())
	{
		return FALSE;
	}

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read(_T(""), sEntry);

		// if the current filetype is not correct query the use to reset it
		if (!sEntry.IsEmpty() && sEntry.CompareNoCase(m_sFileType) != 0)
		{
			if (bAskUser)
			{
				sMessage.Format(_T("The file extension %s is used by %s for its %s.\n\nWould you like %s to be the default application for this file type."),
					m_sExt, AfxGetAppName(), szFileDesc, AfxGetAppName());

				nRet = AfxMessageBox(sMessage, MB_YESNO | MB_ICONQUESTION);
			}

			bChange = TRUE;
		}
		else
		{
			bChange = sEntry.IsEmpty();
		}

		// if not no then set
		if (nRet != IDNO)
		{
			bSuccess = (reg.Write(_T(""), m_sFileType) == ERROR_SUCCESS);

			reg.Close();

			if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType) == ERROR_SUCCESS)
			{
				bSuccess &= (reg.Write(_T(""), szFileDesc) == ERROR_SUCCESS);
				reg.Close();

				// app to open file
				if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType + _T("\\shell\\open\\command")) == ERROR_SUCCESS)
				{
					CString sShellOpen;

					if (bAllowShellOpen)
					{
						if (szParams)
						{
							sShellOpen = _T("\"") + m_sAppPath + _T("\" \"%1\" ") + CString(szParams);
						}
						else
						{
							sShellOpen = _T("\"") + m_sAppPath + _T("\" \"%1\"");
						}
					}

					bSuccess &= (reg.Write(_T(""), sShellOpen) == ERROR_SUCCESS);
				}
				else
				{
					bSuccess = FALSE;
				}

				// icons
				reg.Close();

				if (reg.Open(HKEY_CLASSES_ROOT, m_sFileType + _T("\\DefaultIcon")) == ERROR_SUCCESS)
				{
					CString sIconPath;
					sIconPath.Format(_T("%s,%d"), m_sAppPath, nIcon);
					bSuccess &= (reg.Write(_T(""), sIconPath) == ERROR_SUCCESS);
				}
				else
				{
					bSuccess = FALSE;
				}
			}
			else
			{
				bSuccess = FALSE;
			}
		}
		else
		{
			bSuccess = FALSE;
		}
	}
	else
	{
		bSuccess = FALSE;
	}

	if (bSuccess && bChange)
	{
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
	}

	return bSuccess;
}

BOOL CFileRegister::UnRegisterFileType()
{
	CAfxRegKey reg;
	CString sKey;
	CString sEntry;
	BOOL bSuccess = FALSE;

	ASSERT(!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
	{
		return FALSE;
	}

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read(_T(""), sEntry);

		if (sEntry.IsEmpty())
		{
			return TRUE;   // we werent the register app so all's well
		}

		ASSERT(!m_sFileType.IsEmpty());

		if (m_sFileType.IsEmpty())
		{
			return FALSE;
		}

		if (sEntry.CompareNoCase(m_sFileType) != 0)
		{
			return TRUE;   // we werent the register app so all's well
		}

		// else delete the keys
		reg.Close();
		CAfxRegKey::Delete(HKEY_CLASSES_ROOT, m_sExt);
		CAfxRegKey::Delete(HKEY_CLASSES_ROOT, m_sFileType);
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CFileRegister::IsRegisteredAppInstalled()
{
	ASSERT(!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
	{
		return FALSE;
	}

	CString sRegAppPath = GetRegisteredAppPath();

	if (!sRegAppPath.IsEmpty())
	{
		CFileStatus fs;

		return CFile::GetStatus(sRegAppPath, fs);
	}

	return FALSE;
}

BOOL CFileRegister::IsRegisteredApp()
{
	CAfxRegKey reg;
	CString sEntry;

	ASSERT(!m_sExt.IsEmpty());

	if (m_sExt.IsEmpty())
	{
		return FALSE;
	}

	// if the file type is not empty we check the file type first
	if (!m_sFileType.IsEmpty())
	{
		if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
		{
			reg.Read(_T(""), sEntry);

			if (sEntry.IsEmpty() || sEntry.CompareNoCase(m_sFileType) != 0)
			{
				return FALSE;
			}
		}
	}

	// then we check the app itself
	CString sRegAppFileName = GetRegisteredAppPath(TRUE);

	if (!sRegAppFileName.IsEmpty())
	{
		CString sFName, sExt;
		FileMisc::SplitPath(m_sAppPath, NULL, NULL, &sFName, &sExt);

		if (sRegAppFileName.CompareNoCase(sFName + sExt) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

CString CFileRegister::GetRegisteredAppPath(BOOL bFilenameOnly)
{
	CAfxRegKey reg;
	CString sEntry, sAppPath;

	ASSERT(!m_sExt.IsEmpty());

	if (reg.Open(HKEY_CLASSES_ROOT, m_sExt) == ERROR_SUCCESS)
	{
		reg.Read(_T(""), sEntry);

		if (!sEntry.IsEmpty())
		{
			reg.Close();

			if (reg.Open(HKEY_CLASSES_ROOT, sEntry) == ERROR_SUCCESS)
			{
				reg.Close();

				// app to open file
				if (reg.Open(HKEY_CLASSES_ROOT, sEntry + CString(_T("\\shell\\open\\command"))) == ERROR_SUCCESS)
				{
					reg.Read(_T(""), sAppPath);
				}
			}
		}
	}

	if (!sAppPath.IsEmpty())
	{
		// note: apps often have parameters after so we do this
		CString sDrive, sDir, sFName;
		FileMisc::SplitPath(sAppPath, &sDrive, &sDir, &sFName);

		sFName += _T(".exe");

		if (bFilenameOnly)
		{
			sAppPath = sFName;
		}
		else
		{
			FileMisc::MakePath(sAppPath, sDrive, sDir, sFName);
		}
	}

	return sAppPath;
}

// static versions
BOOL CFileRegister::IsRegisteredAppInstalled(LPCTSTR szExt)
{
	return CFileRegister(szExt).IsRegisteredAppInstalled();
}

CString CFileRegister::GetRegisteredAppPath(LPCTSTR szExt, BOOL bFilenameOnly)
{
	return CFileRegister(szExt).GetRegisteredAppPath(bFilenameOnly);
}

BOOL CFileRegister::RegisterFileType(LPCTSTR szExt, LPCTSTR szFileType, LPCTSTR szFileDesc, int nIcon,
	BOOL bAllowShellOpen, LPCTSTR szParams, BOOL bAskUser)
{
	return CFileRegister(szExt, szFileType).RegisterFileType(szFileDesc, nIcon, bAllowShellOpen, szParams, bAskUser);
}

BOOL CFileRegister::UnRegisterFileType(LPCTSTR szExt, LPCTSTR szFileType)
{
	return CFileRegister(szExt, szFileType).UnRegisterFileType();
}

BOOL CFileRegister::IsRegisteredApp(LPCTSTR szExt, LPCTSTR szFileType)
{
	return CFileRegister(szExt, szFileType).IsRegisteredApp();
}
