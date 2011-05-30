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
// - taken out from the original ToDoList package for better sharing
// - renamed to CAfxRegKey because of conflict with ATL/MFC shared CRegKey
//*****************************************************************************

#include "stdafx.h"
#include "AfxRegKey.h"
#include <winerror.h>

#include "RegUtil.h"

CAfxRegKey::CAfxRegKey()
{
	m_hKeyRoot = NULL;
	m_hKey = NULL;
	m_sPath.Empty();
}

CAfxRegKey::CAfxRegKey(CAfxRegKey& regKey)
{
	m_hKeyRoot = NULL;
	m_hKey = NULL;
	m_sPath.Empty();

	try
	{
		Open(regKey.m_hKeyRoot, regKey.m_sPath);
	}
	catch (...)
	{
	}
}

CAfxRegKey::~CAfxRegKey()
{
	Close();
}

CAfxRegKey& CAfxRegKey::operator=(CAfxRegKey regKey)
{
	m_hKey = regKey.m_hKey;
	m_sPath = regKey.m_sPath;

	return *this; 
}

LONG CAfxRegKey::Open(HKEY hKeyRoot, const TCHAR* pszPath)
{
	DWORD dwDisp;
	LONG lResult;

	// if the key has aleady been opened then close it first
	if (m_hKey)
	{
		ASSERT(0);
		return ERROR_ACCESS_DENIED;
	}

	m_sPath = pszPath;
	m_hKeyRoot = hKeyRoot;

	lResult = ::RegCreateKeyEx(hKeyRoot, pszPath, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
		&m_hKey, &dwDisp);

	return lResult;
}

BOOL CAfxRegKey::KeyExists(HKEY hKeyRoot, const TCHAR* pszPath)
{
	BOOL bExistingKey;
	DWORD dwDisp;
	HKEY hTemp;

	// open a temporary key
	::RegCreateKeyEx(hKeyRoot, pszPath, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTemp, &dwDisp);
	
	// make sure we close it
	::RegCloseKey(hTemp);

	// test for existing
	bExistingKey = (dwDisp == REG_OPENED_EXISTING_KEY);

	// and then delete the physical entry from the registry if new
	if (!bExistingKey)
		Delete(hKeyRoot, pszPath);

	return bExistingKey;
}

LONG CAfxRegKey::RecurseDeleteKey(HKEY key, LPCTSTR lpszKey)
{
	HKEY rslt;
	LONG lRes;
	FILETIME time;
	TCHAR szBuffer[256];
	DWORD dwSize = 256;

	lRes = ::RegOpenKeyEx(key, lpszKey, 0, KEY_ALL_ACCESS, &rslt);

	if (lRes != ERROR_SUCCESS)
		return lRes;

	while (::RegEnumKeyEx(rslt, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == ERROR_SUCCESS)
	{
		lRes = RecurseDeleteKey(rslt, szBuffer);

		if (lRes != ERROR_SUCCESS)
			return lRes;

		dwSize = 256;
	}

	::RegCloseKey(rslt);

	return ::RegDeleteKey(key, lpszKey);
}

LONG CAfxRegKey::Delete(HKEY hKeyRoot, const TCHAR* pszPath)
{
	return RecurseDeleteKey(hKeyRoot, pszPath);
}

void CAfxRegKey::Close()
{
	if (m_hKey)
	{
		::RegCloseKey(m_hKey);
	}
		
	m_hKey = NULL;
}

LONG CAfxRegKey::Write(const TCHAR* pszItem, DWORD dwVal) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	return ::RegSetValueEx(m_hKey, pszItem, 0L, REG_DWORD, (const BYTE*)&dwVal, sizeof(DWORD));
}

DWORD CAfxRegKey::GetValueType(const TCHAR* pszItem) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;

	LONG lRet = ::RegQueryValueEx(m_hKey, pszItem, NULL, &dwType, NULL, NULL);

	if (lRet == ERROR_SUCCESS)
		return dwType;

	return REG_NONE;
}

LONG CAfxRegKey::Write(const TCHAR* pszItem, const TCHAR* pszVal) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	ASSERT(pszVal);
	ASSERT(AfxIsValidAddress(pszVal, _tcslen(pszVal), FALSE));

	return ::RegSetValueEx(m_hKey, pszItem, 0L, REG_SZ, (const BYTE*)pszVal, (_tcslen(pszVal) + 1) * sizeof(TCHAR));
}

LONG CAfxRegKey::Write(const TCHAR* pszItem, const BYTE* pData, DWORD dwLength) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	ASSERT(pData && dwLength > 0);
	ASSERT(AfxIsValidAddress(pData, dwLength, FALSE));

	return ::RegSetValueEx(m_hKey, pszItem, 0L, REG_BINARY, pData, dwLength);
}

LONG CAfxRegKey::Read(const TCHAR* pszItem, DWORD& dwVal) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lRet = ::RegQueryValueEx(m_hKey, pszItem, NULL, &dwType, (BYTE*)&dwDest, &dwSize);

	if (lRet == ERROR_SUCCESS)
		dwVal = dwDest;

	return lRet;
}

LONG CAfxRegKey::Read(const TCHAR* pszItem, CString& sVal) const
{
	ASSERT(m_hKey);

	DWORD dwType;
	TCHAR string[1024];
	DWORD dwSize = sizeof(string);

	LONG lReturn = ::RegQueryValueEx(m_hKey, pszItem, NULL, &dwType, (BYTE*)string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		sVal = string;

	return lReturn;
}

LONG CAfxRegKey::Read(const TCHAR* pszItem, BYTE* pData, DWORD& dwLen) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;

	return ::RegQueryValueEx(m_hKey, pszItem, NULL, &dwType, pData, &dwLen);
}

int CAfxRegKey::GetSubkeyNames(CStringArray& aNames) const
{
	ASSERT(m_hKey);

	aNames.RemoveAll();

	DWORD nKey = 0;
	TCHAR szName[512];
	LONG lResult = ERROR_SUCCESS;
	DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

	while (lResult == ERROR_SUCCESS)
	{ 
		lResult = ::RegEnumKey(m_hKey, nKey, szName, nNameLen);

		// we have a valid key name
		if (lResult == ERROR_SUCCESS)
			aNames.Add(szName);

		nKey++; // next 
	}

	return aNames.GetSize();
}

int CAfxRegKey::GetValueNames(CStringArray& aNames) const
{
	ASSERT(m_hKey);

	DWORD nVal = 0;
	TCHAR szName[512];
	LONG lResult = ERROR_SUCCESS;

	while (lResult == ERROR_SUCCESS)
	{ 
		DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

		lResult = ::RegEnumValue(m_hKey, nVal, szName, &nNameLen,
			NULL, NULL, NULL, NULL);

		// we have a valid key name
		if (lResult == ERROR_SUCCESS)
			aNames.Add(szName);

		nVal++; // next 
	} 

	return aNames.GetSize();
}

BOOL CAfxRegKey::HasValues() const
{
	ASSERT(m_hKey);

	TCHAR szName[512];
	DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

	return (::RegEnumValue(m_hKey, 0, szName, &nNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS);
}

CString CAfxRegKey::GetAppRegPath(LPCTSTR szAppName)
{
	ASSERT(AfxGetApp()->m_pszRegistryKey);
	CString sRegPath, sAppName;

	if (szAppName && _tcslen(szAppName))
		sAppName = szAppName;
	else
		sAppName = AfxGetAppName();

	// construct reg path
	sRegPath = _T("Software\\");
	sRegPath += CString(AfxGetApp()->m_pszRegistryKey);
	sRegPath += _T('\\');
	sRegPath += sAppName;
	sRegPath += _T('\\');

	return sRegPath;
}

BOOL CAfxRegKey::ExportToIni(LPCTSTR szIniPath) const
{
	ASSERT (m_hKey);
	
	if (!m_hKey)
		return FALSE;
	
	CStdioFile file;
	
	if (!file.Open(szIniPath, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
	
	// process top level keys. ie we ignore any values in the root
	CStringArray aSubKeys;
	
	if (GetSubkeyNames(aSubKeys))
	{
		for (int nKey = 0; nKey < aSubKeys.GetSize(); nKey++)
		{
			CString sName = aSubKeys[nKey];
			
			if (!ExportKeyToIni(sName, file))
				return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CAfxRegKey::ExportKeyToIni(const TCHAR* pszKey, CStdioFile& file) const
{
	ASSERT (pszKey && *pszKey);
	CAfxRegKey reg;
	
	if (reg.Open(m_hKey, pszKey) == ERROR_SUCCESS)
	{
		BOOL bSectionHasValues = reg.HasValues();
		
		if (bSectionHasValues)
		{
			// write out section heading
			CString sSection;
			sSection.Format(_T("[%s]\n"), pszKey);
			file.WriteString(sSection);

			// write out values
			int nIndex = 0;
			
			while (reg.ExportValueToIni(nIndex, file))
				nIndex++;
		}
		
		// write out subkeys
		CStringArray aSubKeys;
		
		if (reg.GetSubkeyNames(aSubKeys))
		{
			for (int nKey = 0; nKey < aSubKeys.GetSize(); nKey++)
			{
				CString sName = aSubKeys[nKey];
				
				// process subkey
				CString sKeyName;
				sKeyName.Format(_T("%s\\%s"), pszKey, sName);
				
				if (!ExportKeyToIni(sKeyName, file))
					return FALSE;
			}
		}
		
		return TRUE;
	}
	
	// else
	return FALSE;
}

BOOL CAfxRegKey::ExportValueToIni(DWORD nIndex, CStdioFile& file) const
{
	TCHAR szName[512];
	DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);
	DWORD dwType;
	
	LONG lResult = ::RegEnumValue(m_hKey, nIndex, szName, &nNameLen,
		NULL, &dwType, NULL, NULL);
	
	// we have a valid key name
	if (lResult == ERROR_SUCCESS)
	{
		CString sValueLine;
		
		switch (dwType)
		{
		case REG_DWORD:
			{
				DWORD dwValue;
				Read(szName, dwValue);
				sValueLine.Format(_T("%s = %d\n"), szName, dwValue);
			}
			break;
			
		case REG_SZ:
			{
				CString sValue;
				Read(szName, sValue);
				sValueLine.Format(_T("%s = \"%s\"\n"), szName, sValue);
			}
		}
		
		if (!sValueLine.IsEmpty())
			file.WriteString(sValueLine);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CAfxRegKey::ImportFromIni(LPCTSTR szIniPath)
{
	ASSERT (m_hKey);
	
	if (!m_hKey)
		return FALSE;
	
	CStdioFile file;
	
	if (!file.Open(szIniPath, CFile::modeRead))
		return FALSE;

	CString sSection, sNextSection;
	BOOL bRes = TRUE;

	do 
	{
		// detect invalid ini files
		bRes &= ImportSectionFromIni(sSection, file, sNextSection);
		sSection = sNextSection;
	} 
	while(bRes && !sSection.IsEmpty());
	
	return bRes;
}

BOOL CAfxRegKey::ImportSectionFromIni(const CString& sSection, CStdioFile& file, CString& sNextSection)
{
	sNextSection.Empty();
	BOOL bRoot = (sSection.IsEmpty());
	CAfxRegKey reg;
	
	// open the reg key if not root
	if (!bRoot)
	{
		if (reg.Open(m_hKey, sSection) != ERROR_SUCCESS)
			return FALSE;
	}
	
	CString sLine;
	
	while (file.ReadString(sLine))
	{
		sLine.TrimLeft();
		sLine.TrimRight();
		
		if (sLine.IsEmpty())
			continue;

		else if (sLine[0] == _T('['))
		{
			// check for end tag
			if (sLine[sLine.GetLength() - 1] == ']')
			{
				sNextSection = sLine.Mid(1, sLine.GetLength() - 2);
				return TRUE;
			}
			
			// else
			return FALSE;
		}
		else if (!bRoot) // can't have values in the root
		{
			CString sName, sValue;
			int nEquals = sLine.Find('=');
			
			if (nEquals > 0)
			{
				sName = sLine.Left(nEquals);
				sValue = sLine.Mid(nEquals + 1);
				
				sName.TrimLeft();
				sName.TrimRight();
				sValue.TrimLeft();
				sValue.TrimRight();
				
				// name must not be empty
				if (!sName.IsEmpty())
				{
					// if value contains only digits optionally beginning 
					// with a minus sign then its a DWORD else a string
					BOOL bString = FALSE;

					if (sValue.IsEmpty())
						bString = TRUE;
					else
					{
						// traverse the chars
						for (int nChar = 0; nChar < sValue.GetLength() && !bString; nChar++)
						{
							switch (sValue[nChar])
							{
							case _T('-'):
								bString = (nChar > 0);
								break;

							case _T('0'):
							case _T('1'):
							case _T('2'):
							case _T('3'):
							case _T('4'):
							case _T('5'):
							case _T('6'):
							case _T('7'):
							case _T('8'):
							case _T('9'):
								break; // okay

							default:
								bString = TRUE; // everything else
								break;

							}
						}
					}

					if (bString)
					{
						// remove possible leading and trailing quotes
						if (sValue.GetLength() && sValue[0] == _T('\"') && sValue[sValue.GetLength() - 1] == _T('\"'))
							reg.Write(sName, sValue.Mid(1, sValue.GetLength() - 2));
						else
							reg.Write(sName, sValue);
					}
					else // DWORD
						reg.Write(sName, (DWORD)_ttoi(sValue));
				}
			}
		}
		else // invalid file
			return FALSE;
	}
	
	return TRUE;
}

BOOL CAfxRegKey::CopyKey(HKEY hkRootFrom, const CString& sFromPath, HKEY hkRootTo, const CString& sToPath)
{
	return CopyRegistryKey(hkRootFrom, sFromPath, hkRootTo, sToPath);
}
