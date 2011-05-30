// XmlProfile.cpp  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XMLProfile implements a class to read and write XML profile files that 
//     is consistent with the MFC set of profile functions.  Conversion 
//     functions to allow reading/writing both INI and XML profiles are 
//     included.
//
// History
//     Version 1.0 - 2008 July 2
//     - Initial public release
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"  // use of MFC is optional, unless CXmlWinApp is being used
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "XMLProfile.h"
#include "XMLParser.h"

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

// if you want to see the TRACE output, 
// uncomment this line:
//#include "XTrace.h"

#pragma warning(disable : 4127)	// for _ASSERTE: conditional expression is constant
#pragma warning(disable : 4996)	// disable bogus deprecation warning

///////////////////////////////////////////////////////////////////////////////
//
// CXmlProfile()
//
// Purpose:     Constructor for CXmlProfile class
//
// Parameters:  none
//              
// Returns:     none
//
CXmlProfile::CXmlProfile() :
	m_strProfileTag(_T("profile")),
	m_strSectionTag(_T("section")),
	m_strKeyTag(_T("key")),
	m_strNameTag(_T("name")),
	m_strValueTag(_T("value")),
	m_strReadOnlyTag(_T("readonly")),
	m_nMaxValueLength(0),
	m_eEncoding(Default),
	m_bUniqueKeys(TRUE),
	m_bFileOpen(FALSE),
	m_bDirty(FALSE),
	m_dwBufLen(0),
	m_buf(NULL)
{
	TRACE(_T("in CXmlProfile::CXmlProfile\n"));
}

///////////////////////////////////////////////////////////////////////////////
//
// ~CXmlProfile()
//
// Purpose:     Destructor for CXmlProfile class
//
// Parameters:  none
//              
// Returns:     none
//
CXmlProfile::~CXmlProfile()
{
	delete [] m_buf;
	m_buf = NULL;
	DeleteContents();
}

///////////////////////////////////////////////////////////////////////////////
//
// DeleteContents()
//
// Purpose:     Empties data contents of section and key lists, frees memory
//
// Parameters:  none
//              
// Returns:     none
//
void CXmlProfile::DeleteContents()
{
	CXmlSectionListElement *pSection = m_Sections.GetFirst();
	while (pSection != NULL && !pSection->IsRoot())
	{
		CXmlSectionListElement *pNext = pSection->GetNext();
		pSection->DeleteRemove();
		pSection = pNext;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Load()
//
// Purpose:     Load XML profile from file
//
// Parameters:  lpszFileName - XML profile file path
//              eEncoding    - specifies how the contents are encoded:
//                                Default = contents read as Unicode if 
//                                          _UNICODE is defined; otherwise ANSI
//                                Ansi    = contents read as ANSI
//                                Unicode = contents read as Unicode
//
// Returns:     BOOL - TRUE - load successful
//
// Notes:       Load() sets the class variables m_bDuplicateSections and
//              m_nDuplicateKeys if duplicate sections/keys are found.
//              A duplicate section name will always result in a return code of
//              FALSE.  Duplicate key names will cause FALSE to be returned if
//              m_bUniqueKeys is TRUE;  otherwise duplicate keys names will
//              be permitted.  
//
//              If duplicate key names are used, you must use GetKeys() to get 
//              all the keys and values, since GetKey() will always return the 
//              first key found.
//
//              The class variable m_nBadKeys is set to the number of bad
//              (ignored) keys.  These are keys with either a blank name or a
//              blank value.
//
BOOL CXmlProfile::Load(LPCTSTR lpszFileName, ENCODING eEncoding /*= Default*/)
{
	TRACE(_T("in CXmlProfile::Load\n"));
	BOOL rc = FALSE;
	m_strProfileFile = _T("");

	DeleteContents();
	m_nDuplicateKeys = 0;
	m_nBadKeys = 0;
	m_bDuplicateSections = FALSE;
	m_eEncoding = Default;
	m_strProfileFile = lpszFileName;

	CXmlParser xml;
	if (xml.Load(lpszFileName, (CXmlParser::ENCODING)eEncoding))
	{
		TRACE(_T("----- xml profile loaded from '%s'\n"), lpszFileName);

		m_eEncoding = eEncoding;

		CXmlElement *pRoot = xml.GetRootElement();
		_ASSERTE(pRoot);

		if (pRoot)
		{
			TRACE(_T("pRoot=<%s>\n"), pRoot->m_strName);
			_ASSERTE(pRoot->m_strName == m_strProfileTag);
			if (pRoot->m_strName == m_strProfileTag)
			{
				CXmlElement *pSection = xml.GetFirstChild(pRoot);
				CXmlSectionListElement *pSectionElement = NULL;

				BOOL bFirstSection = TRUE;

				while (pSection)
				{
					_ASSERTE(pSection->m_strName == m_strSectionTag);
					if (pSection->m_strName == m_strSectionTag)
					{
						CString strSectionName = pSection->GetValue(m_strNameTag);
						if (!strSectionName.IsEmpty())
						{
							CString strSectionReadOnly = pSection->GetValue(m_strReadOnlyTag);
							BOOL bSectionReadOnly = _ttoi(strSectionReadOnly);
							TRACE(_T("section name=<%s>  readonly=%d\n"), strSectionName, bSectionReadOnly);

							if (FindSection(strSectionName))
							{
								TRACE(_T("ERROR: duplicate section <%s>\n"), strSectionName);
								m_bDuplicateSections = TRUE;
								break;
							}

							CXmlElement *pKey = xml.GetFirstChild(pSection);

							BOOL bFirstKey = TRUE;
							CXmlProfileSection *pProfileSection = NULL;

							while (pKey)
							{
								_ASSERTE(pKey->m_strName == m_strKeyTag);
								if (pKey->m_strName == m_strKeyTag)
								{
									CString strKeyName = pKey->GetValue(m_strNameTag);
									CString strKeyValue = pKey->GetValue(m_strValueTag);
									CString strKeyReadOnly = pKey->GetValue(m_strReadOnlyTag);
									BOOL bKeyReadOnly = _ttoi(strKeyReadOnly);

									if (!strKeyName.IsEmpty() && !strKeyValue.IsEmpty())
									{
										TRACE(_T("   key name=<%s>  value=<%s>  readonly=%d\n"), strKeyName, strKeyValue, bKeyReadOnly);

										if (bFirstSection)
										{

											bFirstSection = FALSE;
										}

										if (bFirstKey)
										{
											pProfileSection = new CXmlProfileSection;
											pProfileSection->m_strName = strSectionName;
											pProfileSection->m_bReadOnly = bSectionReadOnly;
											pSectionElement = m_Sections.AddTail(pProfileSection);
											bFirstKey = FALSE;
										}

										if (pProfileSection)
										{
											if (m_bUniqueKeys && FindKey(pSectionElement, strKeyName))
											{
												TRACE(_T("ERROR: ignoring duplicate key <%s>\n"), strKeyName);
												m_nDuplicateKeys++;
											}
											else
											{
												CXmlProfileKey *pKey = new CXmlProfileKey;
												pKey->m_strName = strKeyName;
												pKey->m_strValue = strKeyValue;
												pKey->m_bReadOnly = bKeyReadOnly;
												pProfileSection->m_Keys.AddTail(pKey);
											}
										}
									}
									else
									{
										TRACE(_T("ERROR: bad key <%s> - <%s>\n"), strSectionName, strKeyName);
										m_nBadKeys++;
									}
								}
								pKey = xml.GetNextSibling(pSection);
							}
						}
					}
					pSection = xml.GetNextSibling(pRoot);
				}
			}
		}

		Dump();

		m_bFileOpen = TRUE;

		if (!m_bDuplicateSections)
			rc = TRUE;
	}
	else
	{
		TRACE(_T("ERROR: failed to load xml profile '%s'\n"), lpszFileName);
	}

	m_bDirty = FALSE;

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetProfileFileSize()
//
// Purpose:     Returns size in bytes of profile file
//
// Parameters:  lpszFileName - profile file path
//
// Returns:     DWORD - file size in bytes if successful; otherwise 0
//
DWORD CXmlProfile::GetProfileFileSize(LPCTSTR lpszFileName)
{
	DWORD rc = 0;

	_ASSERTE(lpszFileName);

	if (lpszFileName)
	{
		HANDLE hFile = CreateFile(lpszFileName, READ_CONTROL, 0, 0,
			OPEN_EXISTING, 0, 0);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			rc = ::GetFileSize(hFile, 0);
			CloseHandle(hFile);
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// LoadIni()
//
// Purpose:     Load INI profile from file
//
// Parameters:  lpszFileName - INI profile file path
//              eEncoding    - specifies how the contents are encoded:
//                                Default = contents read as Unicode if 
//                                          _UNICODE is defined; otherwise ANSI
//                                Ansi    = contents read as ANSI
//                                Unicode = contents read as Unicode
//
// Returns:     BOOL - TRUE - load successful
//
BOOL CXmlProfile::LoadIni(LPCTSTR lpszFileName, ENCODING eEncoding /*= Default*/)
{
	TRACE(_T("in CXmlProfile::LoadIni\n"));
	BOOL rc = FALSE;

	m_strProfileFile = _T("");
	DeleteContents();
	m_nDuplicateKeys = 0;
	m_nBadKeys = 0;
	m_bDuplicateSections = FALSE;
	m_eEncoding = eEncoding;
	m_strProfileFile = lpszFileName;

	DWORD dwFileSize = GetProfileFileSize(lpszFileName);	// in bytes
	if (dwFileSize)
	{
		DWORD dwKeyBufSize = 100;		// key name
		DWORD dwValueBufSize = 1000;	// value string
		TCHAR *pszSectionNames = new TCHAR [dwFileSize+100];
		TCHAR *pszKeys         = new TCHAR [dwFileSize+100];
		TCHAR *pszKey          = new TCHAR [dwKeyBufSize+100];
		TCHAR *pszValue        = new TCHAR [dwValueBufSize+100];

		memset(pszSectionNames, 0, (dwFileSize+100)*sizeof(TCHAR));

		// read all section names
		DWORD dwSectionSize = ::GetPrivateProfileSectionNames(pszSectionNames, 
									dwFileSize, lpszFileName);

		if (dwSectionSize)
		{
			DWORD index = 0;
			while (index < dwSectionSize)
			{
				TCHAR *cp = &pszSectionNames[index];
				if (*cp)
				{
					TRACE(_T("section=<%s>\n"), cp);

					memset(pszKeys, 0, (dwFileSize+100)*sizeof(TCHAR));

					// read all keys in this section
					DWORD dwKeySize = ::GetPrivateProfileSection(cp, pszKeys,
											dwFileSize, lpszFileName);

					if (dwKeySize)
					{
						DWORD index2 = 0;
						while (index2 < dwKeySize)
						{
							TCHAR *cp2 = &pszKeys[index2];

							if (*cp2)
							{
								TRACE(_T("key=<%s>\n"), cp2);
								TCHAR *cp3 = _tcschr(cp2, _T('='));
								if (cp3)
								{
									size_t n = cp3 - cp2;
									if (n >= dwKeyBufSize)
									{
										delete [] pszKey;
										dwKeyBufSize = n + 100;
										pszKey = new TCHAR [dwKeyBufSize+100];
									}
									_tcsncpy(pszKey, cp2, n);
									pszKey[n] = 0;

									cp3++;
									n = _tcslen(cp3);
									if (n >= dwValueBufSize)
									{
										delete [] pszValue;
										dwValueBufSize = n + 100;
										pszValue = new TCHAR [dwValueBufSize+100];
									}
									_tcsncpy(pszValue, cp3, n);
									pszValue[n] = 0;
									TRACE(_T("szKey=<%s>  szValue=<%s>\n"), pszKey, pszValue);
									if (SetKey(cp, pszKey, pszValue) == NULL)
									{
										TRACE(_T("ERROR: SetKey failed\n"));
										_ASSERTE(FALSE);
									}
								}
							}
							index2 += _tcslen(cp2) + 1;
						}	// while (index2 < dwKeySize)
					}
				}
				index += _tcslen(cp) + 1;
			}	// while (index < dwSectionSize)
		}

		delete [] pszSectionNames;
		delete [] pszKeys;
		delete [] pszKey;
		delete [] pszValue;

		rc = TRUE;
	}
	else
	{
		TRACE(_T("ERROR: dwFileSize=0\n"));
	}

	m_bFileOpen = TRUE;

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// Close()
//
// Purpose:     Close profile file
//
// Parameters:  bSave - TRUE = save contents
//
// Returns:     none
//
void CXmlProfile::Close(BOOL bSave)
{
	if (m_bDirty && bSave)
		Save(m_eEncoding);
	m_bFileOpen = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// WriteString()
//
// Purpose:     WriteString writes the string pointed to by lpszBuf to the file
//              (already open) specified by hFile.
//
// Parameters:  hFile     - handle of output file
//              lpszBuf   - pointer to nul-terminated string
//              eEncoding - specifies how output is encoded:
//                             Default = string written as Unicode if 
//                                       _UNICODE is defined; otherwise ANSI
//                             Ansi    = string written as ANSI
//                             Unicode = string written as Unicode
//              bFirst    - TRUE = first string;  write BOM if Unicode
//
// Returns:     BOOL - TRUE - operation successful
//
BOOL CXmlProfile::WriteString(HANDLE hFile, 
							  LPCTSTR lpszBuf, 
							  ENCODING eEncoding, 
							  BOOL bFirst)
{
	BOOL rc = FALSE;

	if (hFile && 
		(hFile != INVALID_HANDLE_VALUE) &&
		lpszBuf &&
		(lpszBuf[0] != _T('\0')))
	{
		BOOL bFileIsUnicode = FALSE;

		if (eEncoding == Ansi)
		{
		}
		else if (eEncoding == Unicode)
		{
			bFileIsUnicode = TRUE;
		}
		else
		{
#ifdef _UNICODE
			bFileIsUnicode = TRUE;
#endif
		}

		TRACE(_T("bFileIsUnicode=%d\n"), bFileIsUnicode);

		DWORD dwBytesWritten = 0;

		if (bFirst && bFileIsUnicode)
		{
			BYTE bom[2] = { 0xFF, 0xFE };
			::WriteFile(hFile, bom, 2, &dwBytesWritten, NULL);
		}

		// calculate maximum possible buffer needed
		DWORD dwMaxBytes = _tcslen(lpszBuf) * sizeof(WCHAR);

		if ((dwMaxBytes > m_dwBufLen) || (m_buf == NULL))
		{
			delete [] m_buf;
			m_dwBufLen = max(dwMaxBytes, 4096);
			m_dwBufLen += 2;		// for nul
			m_buf = new BYTE [m_dwBufLen + 100];
		}

		DWORD dwBytes = 0;

#ifdef _UNICODE

		if (bFileIsUnicode)
		{
			// UNICODE ==> UNICODE - no conversion needed
			dwBytes = _tcslen(lpszBuf) * sizeof(TCHAR);
			_tcscpy((LPTSTR)m_buf, lpszBuf);
		}
		else
		{
			TRACE(_T("converting to ansi\n"));
			dwBytes = _tcslen(lpszBuf);
			WideCharToMultiByte(CP_ACP, 0, lpszBuf, -1, 
				(LPSTR)m_buf, m_dwBufLen, NULL, NULL);
		}

#else

		if (bFileIsUnicode)
		{
			TRACE(_T("converting to unicode\n"));
			dwBytes = _tcslen(lpszBuf) * sizeof(WCHAR);
			MultiByteToWideChar(CP_ACP, 0, lpszBuf, 
				-1, (LPWSTR)m_buf, m_dwBufLen/sizeof(WCHAR));
		}
		else
		{
			// ANSI ==> ANSI - no conversion needed
			dwBytes = _tcslen(lpszBuf);
			_tcscpy((LPTSTR)m_buf, lpszBuf);
		}

#endif

		rc = ::WriteFile(hFile, (LPCTSTR)m_buf, dwBytes, &dwBytesWritten, NULL);
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// Save()
//
// Purpose:     Save XML profile file
//
// Parameters:  eEncoding - specifies how output is encoded:
//                             Default = profile written as Unicode if 
//                                       _UNICODE is defined; otherwise ANSI
//                             Ansi    = profile written as ANSI
//                             Unicode = profile written as Unicode
//
// Returns:     BOOL - TRUE = operation successful
//
BOOL CXmlProfile::Save(ENCODING eEncoding /*= Default*/)
{
	TRACE(_T("in CXmlProfile::Save\n"));
	return Save(m_strProfileFile, eEncoding);
}

///////////////////////////////////////////////////////////////////////////////
//
// Save()
//
// Purpose:     Save XML profile file
//
// Parameters:  lpszFileName - pointer to file path string
//              eEncoding    - specifies how output is encoded:
//                                Default = profile written as Unicode if 
//                                          _UNICODE is defined; otherwise ANSI
//                                Ansi    = profile written as ANSI
//                                Unicode = profile written as Unicode
//
// Returns:     BOOL - TRUE = operation successful
//
BOOL CXmlProfile::Save(LPCTSTR lpszFileName, ENCODING eEncoding /*= Default*/)
{
	BOOL rc = FALSE;

	_ASSERTE(lpszFileName && (lpszFileName[0] != _T('\0')));

	if (lpszFileName && (lpszFileName[0] != _T('\0')))
	{
		TRACE(_T("~~~~~ writing xml profile to <%s>\n"), lpszFileName);
		DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

#ifdef _DEBUG
		TCHAR szCurDir[MAX_PATH*2];
		szCurDir[0] = _T('\0');
		::GetCurrentDirectory(sizeof(szCurDir)/sizeof(TCHAR)-2, szCurDir);
		TRACE(_T("----- cur dir = <%s>\n"), szCurDir);
#endif

		HANDLE hFile = ::CreateFile(lpszFileName, dwDesiredAccess, dwShareMode, 
			NULL, CREATE_ALWAYS, dwFlagsAndAttributes, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE(_T("ERROR: CreateFile failed - %s\n"), lpszFileName);
		}
		else
		{
			BOOL bFileIsUnicode = FALSE;

			if (eEncoding == Ansi)
			{
			}
			else if (eEncoding == Unicode)
			{
				bFileIsUnicode = TRUE;
			}
			else
			{
#ifdef _UNICODE
				bFileIsUnicode = TRUE;
#endif
			}

			CString str = _T("");
			str.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-%d\"?>\r\n"),
				bFileIsUnicode ? 16 : 8);
			rc = WriteString(hFile, str, eEncoding, TRUE);
			str.Format(_T("<%s>\r\n"), m_strProfileTag);
			rc = WriteString(hFile, str, eEncoding, FALSE);

			CString strSection = _T("");
			CString strKey = _T("");

			BOOL bFirstSection = TRUE;
			CXmlSectionListElement *pSection = m_Sections.GetFirst();
			while (pSection != NULL && !pSection->IsRoot())
			{
				BOOL bFirstKey = TRUE;
				TRACE(_T("   section=<%s>\n"), pSection->GetEntry()->m_strName);
				CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

				while (pKey != NULL && !pKey->IsRoot())
				{
					strSection = _T("");
					if (!pKey->GetEntry()->m_strName.IsEmpty() && !pKey->GetEntry()->m_strValue.IsEmpty())
					{
						if (bFirstKey)
						{
							// don't write readonly element unles it's readonly
							if (pSection->GetEntry()->m_bReadOnly)
							{
								strSection.Format(_T("\t<%s %s=\"%s\" %s=\"%s\">\r\n"), 
									m_strSectionTag,
									m_strNameTag,
									pSection->GetEntry()->m_strName,
									m_strReadOnlyTag,
									pSection->GetEntry()->m_bReadOnly ? _T("1") : _T("0"));
							}
							else
							{
								strSection.Format(_T("\t<%s %s=\"%s\">\r\n"), 
									m_strSectionTag,
									m_strNameTag,
									pSection->GetEntry()->m_strName);
							}
							bFirstSection = FALSE;
							bFirstKey = FALSE;
						}

						TRACE(_T("      key=<%s> - <%s> - <%u>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue, pKey->GetEntry()->m_bReadOnly);

						if (pKey->GetEntry()->m_bReadOnly)
						{
							strKey.Format(_T("\t\t<%s %s=\"%s\" %s=\"%s\" %s=\"%s\"/>\r\n"), 
									m_strKeyTag,
									m_strNameTag,
									pKey->GetEntry()->m_strName, 
									m_strValueTag,
									pKey->GetEntry()->m_strValue,
									m_strReadOnlyTag,
									pKey->GetEntry()->m_bReadOnly ? _T("1") : _T("0"));
						}
						else
						{
							strKey.Format(_T("\t\t<%s %s=\"%s\" %s=\"%s\"/>\r\n"), 
									m_strKeyTag,
									m_strNameTag,
									pKey->GetEntry()->m_strName, 
									m_strValueTag,
									pKey->GetEntry()->m_strValue);
						}

						strSection += strKey;

						rc = WriteString(hFile, strSection, eEncoding, FALSE);

						pKey = pKey->GetNext();
					}
				}

				strSection.Format(_T("\t</%s>\r\n"), m_strSectionTag);
				rc = WriteString(hFile, strSection, eEncoding, FALSE);

				pSection = pSection->GetNext();
			}
			str.Format(_T("</%s>\r\n"), m_strProfileTag);
			rc = WriteString(hFile, str, eEncoding, FALSE);
			CloseHandle(hFile);
			m_bDirty = FALSE;
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// SaveIni()
//
// Purpose:     Save profile as ini file
//
// Parameters:  lpszIniFile - ini file path
//              eEncoding   - specifies how output is encoded:
//                               Default = profile written as Unicode if 
//                                         _UNICODE is defined; otherwise ANSI
//                               Ansi    = profile written as ANSI
//                               Unicode = profile written as Unicode
//
// Returns:     BOOL - TRUE = operation successful
//
BOOL CXmlProfile::SaveIni(LPCTSTR lpszIniFile, 
							ENCODING eEncoding /*= Default*/)
{
	TRACE(_T("in CXmlProfile::SaveIni\n"));

	BOOL rc = FALSE;

	if (!lpszIniFile || (lpszIniFile[0] == _T('\0')))
		return FALSE;

	DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

#ifdef _DEBUG
	TCHAR szCurDir[MAX_PATH*2];
	szCurDir[0] = _T('\0');
	::GetCurrentDirectory(sizeof(szCurDir)/sizeof(TCHAR)-2, szCurDir);
	TRACE(_T("----- cur dir = <%s>\n"), szCurDir);
#endif

	HANDLE hFile = ::CreateFile(lpszIniFile, dwDesiredAccess, dwShareMode, 
		NULL, CREATE_ALWAYS, dwFlagsAndAttributes, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("ERROR: CreateFile failed - %s\n"), lpszIniFile);
	}
	else
	{
		CString strSection = _T("");
		CString strKey = _T("");

		BOOL bFirst = TRUE;
		BOOL bFirstSection = TRUE;
		CXmlSectionListElement *pSection = m_Sections.GetFirst();
		while (pSection != NULL && !pSection->IsRoot())
		{
			BOOL bFirstKey = TRUE;
			TRACE(_T("   section=<%s>\n"), pSection->GetEntry()->m_strName);
			CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

			while (pKey != NULL && !pKey->IsRoot())
			{
				strSection = _T("");
				if (!pKey->GetEntry()->m_strName.IsEmpty() && !pKey->GetEntry()->m_strValue.IsEmpty())
				{
					if (bFirstKey)
					{
						strSection.Format(_T("%s[%s]\r\n"), 
							bFirstSection ? _T("") : _T("\r\n"), 
							pSection->GetEntry()->m_strName);
						bFirstSection = FALSE;
						bFirstKey = FALSE;
					}

					TRACE(_T("      key=<%s> - <%s> - <%u>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue, pKey->GetEntry()->m_bReadOnly);
					strKey.Format(_T("%s=%s\r\n"), 
						pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue);
					strSection += strKey;

					rc = WriteString(hFile, strSection, eEncoding, bFirst);
					bFirst = FALSE;

					pKey = pKey->GetNext();
				}
			}

			pSection = pSection->GetNext();
		}
		CloseHandle(hFile);
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// Dump()
//
// Purpose:     Dumps contents of profile via OutputDebugString()
//
// Parameters:  none
//
// Returns:     none
//
void CXmlProfile::Dump()
{
#ifdef _DEBUG
	TRACE(_T("in CXmlProfile::Dump\n"));
	CXmlSectionListElement *pSection = m_Sections.GetFirst();
	while (pSection != NULL && !pSection->IsRoot())
	{
		TRACE(_T("   section=<%s>\n"), pSection->GetEntry()->m_strName);
		CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

		while (pKey != NULL && !pKey->IsRoot())
		{
			TRACE(_T("      key=<%s> - <%s> - <%u>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue, pKey->GetEntry()->m_bReadOnly);
			pKey = pKey->GetNext();
		}

		pSection = pSection->GetNext();
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// Validate()
//
// Purpose:     Validates contents of profile
//
// Parameters:  none
//
// Returns:     BOOL - TRUE = profile ok
//
BOOL CXmlProfile::Validate()
{
	TRACE(_T("in CXmlProfile::Validate\n"));
	BOOL rc = TRUE;
	CXmlSectionListElement *pSection = m_Sections.GetFirst();
	while (rc && pSection && !pSection->IsRoot())
	{
		CXmlProfileSection *pXmlSection = pSection->GetEntry();

		if (!pXmlSection || pXmlSection->m_strName.IsEmpty())
		{
			rc = FALSE;
			TRACE(_T("ERROR: invalid section\n"));
			_ASSERTE(FALSE);
		}
		else
		{
			CXmlKeyListElement *pKey = pXmlSection->m_Keys.GetFirst();

			while (pKey != NULL && !pKey->IsRoot())
			{
				CXmlProfileKey *pXmlKey = pKey->GetEntry();

				if (!pXmlKey || pXmlKey->m_strName.IsEmpty() || pXmlKey->m_strValue.IsEmpty())
				{
					rc = FALSE;
					TRACE(_T("ERROR: invalid key\n"));
					_ASSERTE(FALSE);
				}
				pKey = pKey->GetNext();
			}
		}
		pSection = pSection->GetNext();
	}
	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// FindSection()
//
// Purpose:     FindSection retrieves section named lpszName
//
// Parameters:  lpszName - name of section to find
//
// Returns:     CXmlSectionListElement * - pointer to section list element,
//                                         or NULL if not found
//
CXmlSectionListElement * CXmlProfile::FindSection(LPCTSTR lpszName)
{
	if (lpszName && (lpszName[0] != _T('\0')))
	{
		CXmlSectionListElement *pSection = m_Sections.GetFirst();
		while (pSection != NULL && !pSection->IsRoot())
		{
			TRACE(_T("FindSection: section=<%s>\n"), pSection->GetEntry()->m_strName);

			if (pSection->GetEntry()->m_strName.CompareNoCase(lpszName) == 0)
				return pSection;
			pSection = pSection->GetNext();
		}
	}
	else
	{
		TRACE(_T("ERROR: lpszName is NULL or empty\n"));
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// FindKey()
//
// Purpose:     FindKey retrieves key named lpszName in section named 
//              lpszSectionName
//
// Parameters:  lpszSectionName - section name
//              lpszName        - name of key to find
//
// Returns:     CXmlKeyListElement * - pointer to key list element,
//                                     or NULL if not found
//
CXmlKeyListElement * CXmlProfile::FindKey(LPCTSTR lpszSectionName, 
										  LPCTSTR lpszName)
{
	CXmlSectionListElement *pSection = FindSection(lpszSectionName);
	return FindKey(pSection, lpszName);
}

///////////////////////////////////////////////////////////////////////////////
//
// FindKey()
//
// Purpose:     FindKey retrieves key named lpszName in section specified by 
//              pSection
//
// Parameters:  pSection - pointer to section list entry
//              lpszName - name of key to find
//
// Returns:     CXmlKeyListElement * - pointer to key list element,
//                                     or NULL if not found
//
CXmlKeyListElement * CXmlProfile::FindKey(CXmlSectionListElement *pSection, 
										  LPCTSTR lpszName)
{
	if (pSection)
	{
		if (lpszName && (lpszName[0] != _T('\0')))
		{
			CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

			while (pKey != NULL && !pKey->IsRoot())
			{
				TRACE(_T("FindKey: key=<%s> - <%s>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue);
				if (pKey->GetEntry()->m_strName.CompareNoCase(lpszName) == 0)
					return pKey;
				pKey = pKey->GetNext();
			}
		}
		else
		{
			TRACE(_T("ERROR: lpszName is NULL or empty\n"));
		}
	}
	else
	{
		TRACE(_T("ERROR: pSection = NULL\n"));
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetKey()
//
// Purpose:     GetKey retrieves key named lpszName in section specified by 
//              pSection
//
// Parameters:  lpszSectionName - section name
//              lpszKeyName     - name of key
//              lpszDefault     - default value to return if key not found
//
// Returns:     CString      - string containing value
//
CString CXmlProfile::GetKey(LPCTSTR lpszSectionName, 
							   LPCTSTR lpszKeyName, 
							   LPCTSTR lpszDefault)
{
	CString strResult(lpszDefault);

	if (!m_bFileOpen)
	{
		TRACE(_T("ERROR: profile file not open\n"));
	}

	CXmlSectionListElement *pSection = FindSection(lpszSectionName);

	if (pSection)
	{
		CXmlKeyListElement *pKey = FindKey(pSection, lpszKeyName);

		if (pKey)
		{
			CXmlProfileKey *pXmlKey = pKey->GetEntry();

			if (pXmlKey)
				strResult = pXmlKey->m_strValue;
		}
	}

	return strResult;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetKey()
//
// Purpose:     SetKey sets the value lpszValue in the key named lpszKeyName in 
//              section named lpszSectionName
//
// Parameters:  lpszSectionName - section name
//              lpszKeyName     - key name
//              lpszValue       - value set
//              bReadOnly       - TRUE = key is read only (used only when 
//                                creating key)
//
// Returns:     CXmlKeyListElement * - pointer to key list entry, or NULL
//                                     if error
//
CXmlKeyListElement * CXmlProfile::SetKey(LPCTSTR lpszSectionName, 
										 LPCTSTR lpszKeyName, 
										 LPCTSTR lpszValue,
										 BOOL bReadOnly /*= FALSE*/)
{
	if (lpszSectionName && (lpszSectionName[0] != _T('\0')))
	{
		CXmlSectionListElement *pSection = FindSection(lpszSectionName);

		if (!pSection)
		{
			TRACE(_T("creating section <%s>\n"), lpszSectionName);
			CXmlProfileSection *pProfileSection = new CXmlProfileSection;
			pProfileSection->m_strName = lpszSectionName;
			pProfileSection->m_bReadOnly = FALSE;
			pSection = m_Sections.AddTail(pProfileSection);
		}

		return SetKey(pSection, lpszKeyName, lpszValue, bReadOnly);
	}
	else
	{
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// SetKey()
//
// Purpose:     SetKey sets the value lpszValue in the key named lpszKeyName in 
//              section specified by pSection
//
// Parameters:  pSection    - pointer to section list entry
//              lpszKeyName - key name
//              lpszValue   - value set
//              bReadOnly   - TRUE = key is read only (used only when creating key)
//
// Returns:     CXmlKeyListElement * - pointer to key list entry, or NULL
//                                     if error
//
CXmlKeyListElement * CXmlProfile::SetKey(CXmlSectionListElement *pSection, 
										 LPCTSTR lpszKeyName, 
										 LPCTSTR lpszValue,
										 BOOL bReadOnly /*= FALSE*/)
{
	if (!m_bFileOpen)
	{
		TRACE(_T("ERROR: profile file not open\n"));
	}

	CXmlKeyListElement *pKey = NULL;

	if (pSection)
	{
		if (!pSection->GetEntry()->m_bReadOnly)
		{
			if (lpszKeyName)
			{
				pKey = FindKey(pSection, lpszKeyName);

				if (pKey)
				{
					if (!pKey->GetEntry()->m_bReadOnly)
					{
						if (lpszValue)
						{
							TRACE(_T("updating key <%s>\n"), lpszKeyName);
							CString str = lpszValue;
							if (m_nMaxValueLength && (str.GetLength() > m_nMaxValueLength))
								str = str.Left(m_nMaxValueLength);
							pKey->GetEntry()->m_strValue = str;
							m_bDirty = TRUE;
						}
						else
						{
							// there may be multiple identical keys
							while (pKey)
							{
								TRACE(_T("deleting key <%s>\n"), lpszKeyName);
								pSection->GetEntry()->m_Keys.DeleteRemove(pKey);
								m_bDirty = TRUE;
								pKey = FindKey(pSection, lpszKeyName);;
							}
						}
					}
					else
					{
						TRACE(_T("ERROR:  cannot change read only key\n"));
						pKey = NULL;
					}
				}
				else
				{
					TRACE(_T("creating key <%s>\n"), lpszKeyName);
					CXmlProfileKey *pNewKey = new CXmlProfileKey;
					pNewKey->m_strName = lpszKeyName;
					pNewKey->m_strValue = lpszValue;
					pNewKey->m_bReadOnly = bReadOnly;
					pKey = pSection->GetEntry()->m_Keys.AddTail(pNewKey);
					m_bDirty = TRUE;
				}
			}
			else
			{
				// lpszKeyName is NULL, so delete section
				TRACE(_T("deleting section <%s>\n"), pSection->GetEntry()->m_strName);
				pSection->DeleteRemove();
				m_bDirty = TRUE;
			}
		}
		else
		{
			TRACE(_T("ERROR:  cannot change read only section\n"));
			pKey = NULL;
		}

		Dump();
	}
	else
	{
		TRACE(_T("ERROR: pSection = NULL\n"));
	}

	return pKey;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetNumSections()
//
// Purpose:     GetNumSections retrieves the number of sections in the profile
//
// Parameters:  none
//
// Returns:     UINT - number of sections
//
UINT CXmlProfile::GetNumSections()
{
	UINT rc = 0;

	CXmlSectionListElement *pSection = m_Sections.GetFirst();
	while (pSection != NULL && !pSection->IsRoot())
	{
		TRACE(_T("section=<%s>\n"), pSection->GetEntry()->m_strName);
		rc++;
		pSection = pSection->GetNext();
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetSections()
//
// Purpose:     GetSections retrieves all the sections in the profile
//
// Parameters:  ppSSectionArray - pointer to a variable that receives the
//              array pointer
//
// Returns:     UINT - number of sections
//
// Notes:       The array pointer returned via ppSSectionArray must be freed by
//              the caller using delete [].
//
UINT CXmlProfile::GetSections(CXmlProfileSectionEntry **ppSSectionArray)
{
	TRACE(_T("in CXmlProfile::GetSections\n"));
	UINT rc = 0;
	
	_ASSERTE(ppSSectionArray);

	if (ppSSectionArray)
	{
		rc = GetNumSections();

		if (rc)
		{
			*ppSSectionArray = new CXmlProfileSectionEntry [rc];
			CXmlSectionListElement *pSection = m_Sections.GetFirst();
			for (UINT i = 0; i < rc; i++)
			{
				if (pSection == NULL || pSection->IsRoot())
					break;
				TRACE(_T("section=<%s>\n"), pSection->GetEntry()->m_strName);
				(*ppSSectionArray)[i].m_strName = pSection->GetEntry()->m_strName;
				(*ppSSectionArray)[i].m_bReadOnly = pSection->GetEntry()->m_bReadOnly;
				pSection = pSection->GetNext();
			}
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetNumKeys()
//
// Purpose:     GetNumKeys retrieves the number of keys in a section
//
// Parameters:  lpszSectionName - name of section
//
// Returns:     UINT - number of keys
//
UINT CXmlProfile::GetNumKeys(LPCTSTR lpszSectionName)
{
	TRACE(_T("in CXmlProfile::GetNumKeys: <%s>\n"), lpszSectionName);
	UINT rc = 0;

	CXmlSectionListElement *pSection = FindSection(lpszSectionName);
	if (pSection)
	{
		CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

		while (pKey != NULL && !pKey->IsRoot())
		{
			TRACE(_T("key=<%s> - <%s>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue);
			rc++;
			pKey = pKey->GetNext();
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetKeys()
//
// Purpose:     GetKeys retrieves all the keys in a section
//
// Parameters:  lpszSectionName - name of section
//              ppKeyArray      - pointer to variable that receives the
//                                array pointer
//
// Returns:     UINT - number of keys
//
// Notes:       The array pointer returned via ppKeyArray must be freed by
//              the caller using delete [].
//
UINT CXmlProfile::GetKeys(LPCTSTR lpszSectionName, 
						  CXmlProfileKey **ppKeyArray)
{
	UINT rc = GetNumKeys(lpszSectionName);

	if (rc)
	{
		*ppKeyArray = new CXmlProfileKey [rc];

		CXmlSectionListElement *pSection = FindSection(lpszSectionName);
		if (pSection)
		{
			CXmlKeyListElement *pKey = pSection->GetEntry()->m_Keys.GetFirst();

			for (UINT i = 0; i < rc; i++)
			{
				if  (pKey == NULL || pKey->IsRoot())
					break;
				TRACE(_T("key=<%s> - <%s>\n"), pKey->GetEntry()->m_strName, pKey->GetEntry()->m_strValue);
				(*ppKeyArray)[i].m_strName   = pKey->GetEntry()->m_strName;
				(*ppKeyArray)[i].m_strValue  = pKey->GetEntry()->m_strValue;
				(*ppKeyArray)[i].m_bReadOnly = pKey->GetEntry()->m_bReadOnly;
				pKey = pKey->GetNext();
			}
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetKeys()
//
// Purpose:     SetKeys creates/updates keys in a section
//
// Parameters:  lpszSectionName - name of section
//              pKeyArray       - pointer to array of keys
//              nKeys           - number of keys in array
//
// Returns:     BOOL - TRUE = operation successful
//
BOOL CXmlProfile::SetKeys(LPCTSTR lpszSectionName, 
						  CXmlProfileKey *pKeyArray,
						  UINT nKeys)
{
	BOOL rc = FALSE;

	_ASSERTE(lpszSectionName && (lpszSectionName[0] != _T('\0')));
	_ASSERTE(pKeyArray);
	_ASSERTE(nKeys != 0);

	if (!lpszSectionName || 
		(lpszSectionName[0] == _T('\0')) ||
		!pKeyArray ||
		(nKeys == 0))
	{
		return FALSE;
	}

	for (UINT i = 0; i < nKeys; i++)
	{
		// section will be created if it doesn't exist
		CXmlKeyListElement *pKey = SetKey(lpszSectionName, 
										  pKeyArray[i].m_strName, 
										  pKeyArray[i].m_strValue,
										  pKeyArray[i].m_bReadOnly);
		
		if  (pKey == NULL)
		{
#ifdef _DEBUG
			CString s = lpszSectionName;
			s += _T(" - ");
			s += pKeyArray[i].m_strValue;
			TRACE(_T("ERROR: SetKey failed for %s\n"), s);
			_ASSERTE(FALSE);
#endif
		}
		else
		{
			rc = TRUE;
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetReadOnly()
//
// Purpose:     GetReadOnly retrieves the readonly status of a section
//
// Parameters:  pSection - pointer to section list element
//
// Returns:     BOOL - TRUE = section is readonly
//
BOOL CXmlProfile::GetReadOnly(CXmlSectionListElement *pSection)
{
	BOOL rc = FALSE;

	if (pSection)
	{
		rc = pSection->GetEntry()->m_bReadOnly;
	}
	else
	{
		TRACE(_T("ERROR: pSection = NULL\n"));
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetReadOnly()
//
// Purpose:     SetReadOnly sets the readonly status of a section
//
// Parameters:  pSection  - pointer to section list element
//              bReadOnly - readonly status to set
//
// Returns:     none
//
void CXmlProfile::SetReadOnly(CXmlSectionListElement *pSection, BOOL bReadOnly)
{
	if (pSection)
	{
		pSection->GetEntry()->m_bReadOnly = bReadOnly;
		m_bDirty = TRUE;
	}
	else
	{
		TRACE(_T("ERROR: pSection = NULL\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// GetReadOnly()
//
// Purpose:     GetReadOnly retrieves the readonly status of a key
//
// Parameters:  pKey - pointer to key list element
//
// Returns:     BOOL - TRUE = key is readonly
//
BOOL CXmlProfile::GetReadOnly(CXmlKeyListElement *pKey)
{
	BOOL rc = FALSE;

	if (pKey)
	{
		rc = pKey->GetEntry()->m_bReadOnly;
	}
	else
	{
		TRACE(_T("ERROR: pKey = NULL\n"));
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// SetReadOnly()
//
// Purpose:     SetReadOnly sets the readonly status of a key
//
// Parameters:  pKey      - pointer to key list element
//              bReadOnly - readonly status to set
//
// Returns:     none
//
void CXmlProfile::SetReadOnly(CXmlKeyListElement *pKey, BOOL bReadOnly)
{
	if (pKey)
	{
		pKey->GetEntry()->m_bReadOnly = bReadOnly;
		m_bDirty = TRUE;
	}
	else
	{
		TRACE(_T("ERROR: pKey = NULL\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// IsSection()
//
// Purpose:     IsSection checks if section exists
//
// Parameters:  lpszSection - section name
//
// Returns:     BOOL - TRUE = section exists
//
BOOL CXmlProfile::IsSection(LPCTSTR lpszSection)
{
	return (FindSection(lpszSection) != NULL);
}

///////////////////////////////////////////////////////////////////////////////
//
// IsKey()
//
// Purpose:     IsKey checks if key exists
//
// Parameters:  lpszSectionName - section name
//              lpszKeyName     - key name
//
// Returns:     BOOL - TRUE = key exists
//
BOOL CXmlProfile::IsKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName)
{
	return (FindKey(FindSection(lpszSectionName), lpszKeyName) != NULL);
}


//=============================================================================
//=============================================================================
//
// The following functions emulate the MFC CWinApp profile functions.
//
//=============================================================================
//=============================================================================


///////////////////////////////////////////////////////////////////////////////
//
// GetProfileBinary()
//
// Purpose:     GetProfileBinary retrieve binary data from an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              ppData      - pointer to a variable that receives the
//                            array pointer
//              pBytes      - pointer to a variable that receives the size of
//                            the data in bytes
//
// Returns:     BOOL - TRUE = operation successful
//
// Notes:       The array pointer returned via ppData must be freed by
//              the caller using delete [].
//
BOOL CXmlProfile::GetProfileBinary(LPCTSTR lpszSection, 
								   LPCTSTR lpszEntry,
								   BYTE** ppData, 
								   UINT* pBytes)
{
	_ASSERTE(lpszSection);
	_ASSERTE(lpszEntry);
	_ASSERTE(ppData);
	_ASSERTE(pBytes);
	if (!lpszSection || !lpszEntry || !ppData || !pBytes)
	{
		TRACE(_T("ERROR: bad args\n"));
		return FALSE;
	}

	*ppData = NULL;
	*pBytes = 0;

	CString str = GetProfileString(lpszSection, lpszEntry, NULL);
	if (str.IsEmpty())
		return FALSE;
	int nLen = str.GetLength();
	_ASSERTE(nLen%2 == 0);
	*pBytes = nLen/2;
	*ppData = new BYTE [*pBytes];
	TRACE(_T("buffer must be freed by caller using 'delete [] ...'\n"));
	for (int i = 0; i < nLen; i += 2)
	{
		(*ppData)[i/2] = (BYTE)	(((str[i+1] - 'A') << 4) + (str[i] - 'A'));
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// WriteProfileBinary()
//
// Purpose:     WriteProfileBinary writes binary data into an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              pData       - pointer to data to be written
//              nBytes      - number of bytes to be written
//
// Returns:     BOOL - TRUE = operation successful
//
// Notes:       Each four-bit nibble of a byte is replaced with an alpha 
//              character between A (for 0x0) and P (for 0xF).  Some
//              examples (note that the nibbles are flipped):
//                 0x00 ==> written to profile as ==> AA
//                 0x01 ==> written to profile as ==> BA
//                 0x7F ==> written to profile as ==> PH
//                 0x80 ==> written to profile as ==> AI
//                 0xFF ==> written to profile as ==> PP
//                 BYTE buf[6] = { 0, 2, 3, 0x7f, 0x80, 0xff } ==> AACADAPHAIPP
//
BOOL CXmlProfile::WriteProfileBinary(LPCTSTR lpszSection, 
									 LPCTSTR lpszEntry,
									 LPBYTE pData, 
									 UINT nBytes)
{
	_ASSERTE(lpszSection);
	_ASSERTE(lpszEntry);
	_ASSERTE(pData);
	_ASSERTE(nBytes != 0);
	if (!lpszSection || !lpszEntry || !pData || (nBytes == 0))
	{
		TRACE(_T("ERROR: bad args\n"));
		return FALSE;
	}

	TCHAR * lpsz = new TCHAR [nBytes*2+1];
	UINT i = 0;
	for (i = 0; i < nBytes; i++)
	{
		lpsz[i*2]   = (TCHAR)((pData[i] & 0x0F) + 'A');			// low nibble
		lpsz[i*2+1] = (TCHAR)(((pData[i] >> 4) & 0x0F) + 'A');	// high nibble
	}
	lpsz[i*2] = 0;

	BOOL rc = WriteProfileString(lpszSection, lpszEntry, lpsz);
	delete [] lpsz;

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetProfileInt()
//
// Purpose:     GetProfileInt retrieves an integer value from an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              nDefault    - specifies the default value to return if the 
//                            entry cannot be found
//
// Returns:     UINT - the integer value of the string that is stored in 
//                     lpszEntry, or nDefault if lpszEntry is not found
//
UINT CXmlProfile::GetProfileInt(LPCTSTR lpszSection, 
								LPCTSTR lpszEntry, 
								int nDefault)
{
	_ASSERTE(lpszSection);
	_ASSERTE(lpszEntry);
	if (!lpszSection || !lpszEntry)
	{
		TRACE(_T("ERROR: bad args\n"));
		return nDefault;
	}

	UINT rc = nDefault;

	CString str = GetProfileString(lpszSection, lpszEntry, NULL);
	if (!str.IsEmpty())
		rc = _ttoi(str);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// WriteProfileInt()
//
// Purpose:     WriteProfileInt writes an integer value into an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              nValue      - specifies the value to be written
//
// Returns:     BOOL - TRUE = operation successful
//
BOOL CXmlProfile::WriteProfileInt(LPCTSTR lpszSection,
								  LPCTSTR lpszEntry,
								  int nValue)
{
	_ASSERTE(lpszSection);
	_ASSERTE(lpszEntry);
	if (!lpszSection || !lpszEntry)
	{
		TRACE(_T("ERROR: bad args\n"));
		return FALSE;
	}

	TCHAR buf[100] = { 0 };
	_itot(nValue, buf, 10);

	return WriteProfileString(lpszSection, lpszEntry, buf);
}

///////////////////////////////////////////////////////////////////////////////
//
// GetProfileString()
//
// Purpose:     GetProfileString retrieves a string from an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              lpszDefault - specifies the default string to return if the 
//                            entry cannot be found
//
// Returns:     CString - the string that is stored in lpszEntry, or 
//                           lpszDefault if lpszEntry is not found
//
CString CXmlProfile::GetProfileString(LPCTSTR lpszSection, 
										 LPCTSTR lpszEntry,
										 LPCTSTR lpszDefault)
{
	_ASSERTE(lpszSection);
	_ASSERTE(lpszEntry);
	if (!lpszSection || !lpszEntry)
	{
		TRACE(_T("ERROR: bad args\n"));
		return lpszDefault;
	}
	return GetKey(lpszSection, lpszEntry, lpszDefault);
}

///////////////////////////////////////////////////////////////////////////////
//
// WriteProfileString()
//
// Purpose:     WriteProfileString writes a string into an entry within a 
//              specified section of the profile
//
// Parameters:  lpszSection - name of section
//              lpszEntry   - name of key
//              lpszValue   - the string to be written
//
// Returns:     BOOL - TRUE = operation successful
//
// Notes:       If lpszValue is NULL, the key specified by lpszEntry is deleted.
//              If lpszEntry is NULL, the section specified by lpszSection -
//              including all keys in the section - is deleted.
//
BOOL CXmlProfile::WriteProfileString(LPCTSTR lpszSection, 
									 LPCTSTR lpszEntry,
									 LPCTSTR lpszValue)
{
	_ASSERTE(lpszSection);
	if (!lpszSection)
	{
		TRACE(_T("ERROR: bad args\n"));
		return FALSE;
	}
	return (SetKey(lpszSection, lpszEntry, lpszValue) != NULL);
}
