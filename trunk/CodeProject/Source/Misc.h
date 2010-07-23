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
//*****************************************************************************

// Misc.h: interface for the CMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_)
#define AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

enum { MKS_CTRL = 0x01, MKS_SHIFT = 0x02, MKS_ALT = 0x04 };
enum { MKC_LEFTRIGHT = 0x01, MKC_UPDOWN = 0x02, MKC_ANY = (MKC_LEFTRIGHT | MKC_UPDOWN) };

namespace Misc  
{
	void CopyTexttoClipboard(const CString& sText, HWND hwnd); 
	CString GetClipboardText(HWND hwnd); 
	
	BOOL IsMultibyteString(const CString& sText);
	CString WideToMultiByte(wchar_t nChar, UINT nCodePage = CP_ACP);
	
	BOOL GuidFromString(LPCTSTR szGuid, GUID& guid);
	BOOL IsGuid(LPCTSTR szGuid);
	BOOL GuidToString(const GUID& guid, CString& sGuid);
	BOOL GuidIsNull(const GUID& guid);
	void NullGuid(GUID& guid);
	BOOL SameGuids(const GUID& guid1, const GUID& guid2);

	template <class T> 
	BOOL ArraysMatch(const T& array1, const T& array2, BOOL bOrderSensitive = FALSE)
	{
		int nSize1 = array1.GetSize();
		int nSize2 = array2.GetSize();
		
		if (nSize1 != nSize2)
			return 0;
		
		if (bOrderSensitive)
		{
			for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
			{
				// check for non-equality
				BOOL bMatch = (array1[nItem1] == array2[nItem1]);

				if (!bMatch)
					return FALSE;
			}
			
			return TRUE;
		}
		
		// else order not important
		for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
		{
			BOOL bMatch = FALSE;
			
			// look for matching item
			for (int nItem2 = 0; nItem2 < nSize2 && !bMatch; nItem2++)
				bMatch = (array1[nItem1] == array2[nItem2]);
			
			// no-match found == not the same
			if (!bMatch)
				return FALSE;
		}
		
		return TRUE;
	}

	BOOL ArraysMatch(const CStringArray& array1, const CStringArray& array2, 
					 BOOL bOrderSensitive = FALSE, BOOL bCaseSensitive = FALSE);
	BOOL MatchAny(const CStringArray& array1, const CStringArray& array2, BOOL bCaseSensitive = FALSE);
	CString FormatArray(const CStringArray& array, LPCTSTR szSep = NULL);
	CString FormatArray(const CDWordArray& array, LPCTSTR szSep = NULL);
	int ParseIntoArray(const CString& sText, CStringArray& array, BOOL bAllowEmpty = FALSE, CString sSep = _T(""));
	int Find(const CStringArray& array, LPCTSTR szItem, BOOL bCaseSensitive = FALSE);
	void Trace(const CStringArray& array);
	int RemoveItems(const CStringArray& aItems, CStringArray& aFrom, BOOL bCaseSensitive = FALSE);

	void SortArray(CStringArray& array, BOOL bAscending = TRUE);

	CString GetComputerName();
	CString GetUserName();
	CString GetListSeparator();
	CString GetDecimalSeparator();
	CString GetDefCharset();
	CString GetAM();
	CString GetPM();
	CString GetTimeSeparator();
	CString GetTimeFormat(BOOL bIncSeconds = TRUE);
	CString GetShortDateFormat(BOOL bIncDOW = FALSE);

	int CompareVersions(LPCTSTR szVersion1, LPCTSTR szVersion2);
	int Split(const CString& sText, TCHAR cDelim, CStringArray& aValues);

	double Round(double dValue);
	float Round(float fValue);
	double Atof(const CString& sValue);
	CString Format(double dVal, int nDecPlaces = 2);
	CString Format(int nVal);
	CString FormatCost(double dCost);

	BOOL IsWorkStationLocked();
	BOOL IsScreenSaverActive();

	void ProcessMsgLoop();

	int ParseSearchString(LPCTSTR szLookFor, CStringArray& aWords);
	BOOL FindWord(LPCTSTR szWord, LPCTSTR szText, BOOL bCaseSensitive, BOOL bMatchWholeWord);

	BOOL ModKeysArePressed(DWORD dwKeys); 
	BOOL KeyIsPressed(DWORD dwVirtKey);
	BOOL IsCursorKey(DWORD dwVirtKey, DWORD dwKeys = MKC_ANY);
	BOOL IsCursorKeyPressed(DWORD dwKeys = MKC_ANY);

	BOOL HasFlag(DWORD dwFlags, DWORD dwFlag);
};

#endif // !defined(AFX_MISC_H__4B2FDA3E_63C5_4F52_A139_9512105C3AD4__INCLUDED_)
