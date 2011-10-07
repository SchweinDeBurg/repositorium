// XmlProfile.h  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
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

#ifndef XMLPROFILE_H
#define XMLPROFILE_H

#include "CLinkedList.h"

class CXmlProfileKey;
typedef CLinkedList<CXmlProfileKey> CXmlKeyList;
typedef CLinkedListElement<CXmlProfileKey> CXmlKeyListElement;

class CXmlProfileSection;
typedef CLinkedList<CXmlProfileSection> CXmlSectionList;
typedef CLinkedListElement<CXmlProfileSection> CXmlSectionListElement;

//=============================================================================
class CXmlProfileKey
//=============================================================================
{
public:
	CXmlProfileKey(LPCTSTR lpszName = NULL, 
				   LPCTSTR lpszValue = NULL, 
				   BOOL bReadOnly = FALSE) :
		m_strName(lpszName),
		m_strValue(lpszValue),
		m_bReadOnly(bReadOnly)
	{
	}
	void Init(LPCTSTR lpszName, 
			  LPCTSTR lpszValue, 
			  BOOL bReadOnly)
	{
		m_strName = lpszName;
		m_strValue = lpszValue;
		m_bReadOnly = bReadOnly;
	}
	CString	m_strName;
	CString	m_strValue;
	BOOL		m_bReadOnly;
};

//=============================================================================
class CXmlProfileSectionEntry
//=============================================================================
{
public:
	CXmlProfileSectionEntry() :
		m_strName(_T("")),
		m_bReadOnly(FALSE)
	{
	}
	CString	m_strName;
	BOOL		m_bReadOnly;
};

//=============================================================================
class CXmlProfileSection
//=============================================================================
{
public:
	CXmlProfileSection() :
		m_strName(_T("")),
		m_bReadOnly(FALSE)
	{
	}
	~CXmlProfileSection() 
	{
		CXmlKeyListElement *pKey = m_Keys.GetFirst();
		while (pKey != NULL && !pKey->IsRoot())
		{
			CXmlKeyListElement *pNext = pKey->GetNext();
			pKey->DeleteRemove();
			pKey = pNext;
		}
	}

	CString	m_strName;
	BOOL		m_bReadOnly;
	CXmlKeyList	m_Keys;
};

//=============================================================================
class CXmlProfile
//=============================================================================
{
//=============================================================================
// Construction
public:
	CXmlProfile();
	virtual ~CXmlProfile();

//=============================================================================
// MFC CWinApp-compatibile functions
public:

	BOOL GetProfileBinary(LPCTSTR lpszSection, 
						  LPCTSTR lpszEntry,
						  BYTE** ppData, 
						  UINT* pBytes);
	BOOL WriteProfileBinary(LPCTSTR lpszSection, 
							LPCTSTR lpszEntry,
							LPBYTE pData, 
							UINT nBytes);

	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	CString GetProfileString(LPCTSTR lpszSection, 
								LPCTSTR lpszEntry,
								LPCTSTR lpszDefault);
	BOOL WriteProfileString(LPCTSTR lpszSection, 
							LPCTSTR lpszEntry,
							LPCTSTR lpszValue);

//=============================================================================
// Attributes
public:

	BOOL GetReadOnly(CXmlSectionListElement *pSection);
	void SetReadOnly(CXmlSectionListElement *pSection, BOOL bReadOnly);
	BOOL GetReadOnly(CXmlKeyListElement *pKey);
	void SetReadOnly(CXmlKeyListElement *pKey, BOOL bReadOnly);

	BOOL IsFileOpen() { return m_bFileOpen; };
	BOOL IsKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);
	BOOL IsSection(LPCTSTR lpszSection);

//=============================================================================
// Operations
public:

	// must be the same as CXmlParser
	enum ENCODING { Default = 0, Ansi, Unicode };

	void Close(BOOL bSave);
	void DeleteContents();
	void Dump();
	CXmlKeyListElement * FindKey(LPCTSTR lpszSectionName, LPCTSTR lpszName);
	CXmlKeyListElement * FindKey(CXmlSectionListElement *pSection, LPCTSTR lpszName);
	CXmlSectionListElement * FindSection(LPCTSTR lpszName);
	CString GetKey(LPCTSTR lpszSectionName, 
					  LPCTSTR lpszKeyName, 
					  LPCTSTR lpszDefault);
	UINT GetKeys(LPCTSTR lpszSectionName, CXmlProfileKey **ppKeyBuffer);
	UINT GetNumKeys(LPCTSTR lpszSectionName);
	UINT GetNumSections();
	UINT GetSections(CXmlProfileSectionEntry **ppSSectionArray);
	BOOL Load(LPCTSTR lpszFileName, ENCODING eEncoding = Default);
	BOOL LoadIni(LPCTSTR lpszFileName, ENCODING eEncoding = Default);
	BOOL Save(ENCODING eEncoding = Default);
	BOOL Save(LPCTSTR lpszFileName, ENCODING eEncoding = Default);
	BOOL SaveIni(LPCTSTR lpszIniFile, ENCODING eEncoding = Default);
	CXmlKeyListElement * SetKey(LPCTSTR lpszSectionName, 
								LPCTSTR lpszKeyName, 
								LPCTSTR lpszValue,
								BOOL bReadOnly = FALSE);
	CXmlKeyListElement * SetKey(CXmlSectionListElement *pSection, 
								LPCTSTR lpszKeyName, 
								LPCTSTR lpszValue,
								BOOL bReadOnly = FALSE);
	BOOL SetKeys(LPCTSTR lpszSectionName, CXmlProfileKey *pKeyBuffer, UINT nKeys);
	BOOL Validate();

//=========================================================================
// class variables
public:

	CXmlSectionList	m_Sections;			// list of sections in profile
	CString		m_strProfileFile;	// profile file path
	ENCODING		m_eEncoding;		// encoding of profile file
	int				m_nMaxValueLength;	// max length of value string in TCHARs -
										// 0 = no limit
	int				m_nDuplicateKeys;	// number of duplicate keys found by Load()
	int				m_nBadKeys;			// number of bad keys found by Load()
	int				m_bDuplicateSections;	// number of duplicate sections found by Load()
	BOOL			m_bUniqueKeys;		// TRUE = keys must be unique
	BOOL			m_bFileOpen;		// TRUE = file is open
	BOOL			m_bDirty;			// TRUE = profile has been modifies

	// following are the tags used in xml file
	CString		m_strProfileTag;
	CString		m_strSectionTag;
	CString		m_strKeyTag;
	CString		m_strNameTag;		// used for both sections and keys
	CString		m_strValueTag;
	CString		m_strReadOnlyTag;

protected:
	DWORD GetProfileFileSize(LPCTSTR lpszFileName);
	BOOL WriteString(HANDLE hFile, LPCTSTR lpszBuf, ENCODING eEncoding, BOOL bFirst);

	DWORD	m_dwBufLen;
	BYTE *	m_buf;
};

#endif //XMLPROFILE_H
