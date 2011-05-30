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
// - adjusted #include's paths
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_)
#define AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPreferences.h"
#include <afxtempl.h>

struct INIENTRY
{
	INIENTRY(LPCTSTR szName = NULL, LPCTSTR szValue = NULL) : sName(szName), sValue(szValue) {}

	CString sName;
	CString sValue;
};

typedef CMap<CString, LPCTSTR, INIENTRY, INIENTRY&> CIniEntryMap;

struct INISECTION
{
	INISECTION(LPCTSTR szName = NULL) : sSection(szName) {}

	CString sSection;
	CIniEntryMap aEntries;
};

typedef CArray<INISECTION*, INISECTION*> CIniSectionArray;

class CPreferences : public IPreferences
{
public:
	CPreferences();
	~CPreferences();

	// directly from CWinApp
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0) const;
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL) const;
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	// note: Binary not supported by ini file

	// extras
	double GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault = 0) const;
	BOOL WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);

	int GetArrayItems(LPCTSTR lpszSection, CStringArray& aItems) const;
	void WriteArrayItems(const CStringArray& aItems, LPCTSTR lpszSection);

	static CString KeyFromFile(LPCTSTR szFilePath, BOOL bFilenameOnly = FALSE);

protected:
	static CIniSectionArray s_aIni;
	static BOOL s_bDirty;
	static BOOL s_bIni;
	static int s_nRef;

	static POSITION GetEntry(INISECTION& section, LPCTSTR lpszEntry);
	static INISECTION* GetSection(LPCTSTR lpszSection, BOOL bCreateNotExist);

	static CString GetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	static void SetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR szValue);

	static CString ToString(int nValue);
	static CString ToString(double dValue);
};

#endif // !defined(AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_)
