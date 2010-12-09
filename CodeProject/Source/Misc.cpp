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
// - merged with ToDoList version 6.1 sources
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// Misc.cpp: implementation of the CMisc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Misc.h"

#include <lmcons.h>
#include <math.h>
#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void Misc::CopyTexttoClipboard(const CString& sText, HWND hwnd)
{
	if (!::OpenClipboard(hwnd))
	{
		return;
	}

	::EmptyClipboard();

	// Allocate a global memory object for the text.
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (sText.GetLength() + 1) * sizeof(TCHAR));

	if (!hglbCopy)
	{
		CloseClipboard();
		return;
	}

	// Lock the handle and copy the text to the buffer.
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);

	memcpy(lptstrCopy, (LPVOID)(LPCTSTR)sText, sText.GetLength() * sizeof(TCHAR));

	lptstrCopy[sText.GetLength()] = (TCHAR) 0;    // null character
	GlobalUnlock(hglbCopy);

	// Place the handle on the clipboard.
	::SetClipboardData(CF_TEXT, hglbCopy);

	::CloseClipboard();
}

CString Misc::GetClipboardText(HWND hwnd)
{
	if (!::OpenClipboard(hwnd))
	{
		return _T("");
	}

	HANDLE hData = ::GetClipboardData(CF_TEXT);
	TCHAR* buffer = (TCHAR*)GlobalLock(hData);

	CString sText(buffer);

	::GlobalUnlock(hData);
	::CloseClipboard();

	return sText;
}

BOOL Misc::IsGuid(LPCTSTR szGuid)
{
	GUID guid;

	return GuidFromString(szGuid, guid);
}

BOOL Misc::GuidFromString(LPCTSTR szGuid, GUID& guid)
{
#if defined(UNICODE) || defined(_UNICODE)
	RPC_STATUS rpcs = UuidFromString((RPC_WSTR)szGuid, &guid);
#else
	RPC_STATUS rpcs = UuidFromString((RPC_CSTR)szGuid, &guid);
#endif   // UNICODE || _UNICODE

	if (rpcs != RPC_S_OK)
	{
		NullGuid(guid);
		return FALSE;
	}

	return TRUE;
}

BOOL Misc::GuidToString(const GUID& guid, CString& sGuid)
{
#if defined(UNICODE) || defined(_UNICODE)
	RPC_WSTR pszGuid;
#else
	RPC_CSTR pszGuid;
#endif   // UNICODE || _UNICODE

	if (RPC_S_OK == UuidToString((GUID*)&guid, &pszGuid))
	{
		sGuid = CString((LPCTSTR)pszGuid);
	}
	else
	{
		sGuid.Empty();
	}

	RpcStringFree(&pszGuid);

	return !sGuid.IsEmpty();
}

BOOL Misc::GuidIsNull(const GUID& guid)
{
	static GUID NULLGUID = { 0 };

	return SameGuids(guid, NULLGUID);
}

BOOL Misc::SameGuids(const GUID& guid1, const GUID& guid2)
{
	return (memcmp(&guid1, &guid2, sizeof(GUID)) == 0);
}

void Misc::NullGuid(GUID& guid)
{
	ZeroMemory(&guid, sizeof(guid));
}

void Misc::ProcessMsgLoop()
{
	MSG msg;

	while (::PeekMessage((LPMSG) &msg, NULL, 0, 0, PM_REMOVE))
	{
		if (::IsDialogMessage(msg.hwnd, (LPMSG)&msg))
		{
			// do nothing - its already been done
		}
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

int Misc::Split(const CString& sText, const CString& sDelim, CStringArray& aValues)
{
	aValues.RemoveAll();

	CString sTextCopy(sText);
	int nFind = sTextCopy.Find(sDelim, 0);

	while (nFind != -1)
	{
		CString sValue = sTextCopy.Left(nFind);
		aValues.Add(sValue);

		// next try
		sTextCopy = sTextCopy.Mid(nFind + sDelim.GetLength());
		nFind = sTextCopy.Find(sDelim, 0);
	}

	// add whatever's left
	if (!sTextCopy.IsEmpty())
	{
		aValues.Add(sTextCopy);
	}

	return aValues.GetSize();
}

int Misc::Split(const CString& sText, TCHAR cDelim, CStringArray& aValues)
{
	return Split(sText, CString(cDelim), aValues);
}

CString Misc::GetComputerName()
{
	static CString sMachine;

	if (sMachine.IsEmpty()) // init first time only
	{
		DWORD LEN = MAX_COMPUTERNAME_LENGTH + 1;

		::GetComputerName(sMachine.GetBuffer(LEN), &LEN);
		sMachine.ReleaseBuffer();
	}

	return sMachine;
}

CString Misc::GetUserName()
{
	static CString sUser;

	if (sUser.IsEmpty()) // init first time only
	{
		DWORD LEN = UNLEN + 1;

		::GetUserName(sUser.GetBuffer(LEN), &LEN);
		sUser.ReleaseBuffer();
	}

	return sUser;
}

CString Misc::GetListSeparator()
{
	static CString sSep;
	const int BUFLEN = 10;

	if (sSep.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, sSep.GetBuffer(BUFLEN), BUFLEN - 1);
		sSep.ReleaseBuffer();

		// Trim extra spaces
		sSep.TrimLeft();
		sSep.TrimRight();

		// If none found, use a comma
		if (!sSep.GetLength())
		{
			sSep = _T(',');
		}
	}

	return sSep;
}

CString Misc::FormatArray(const CStringArray& array, LPCTSTR szSep)
{
	int nCount = array.GetSize();

	if (nCount == 0)
	{
		return _T("");
	}

	CString sSep(szSep);

	if (!szSep)
	{
		sSep = GetListSeparator() + _T(' ');
	}

	CString sText;

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		if (nItem > 0 && array[nItem].GetLength())
		{
			sText += sSep;
		}

		sText += array[nItem];
	}

	return sText;
}

CString Misc::FormatArray(const CDWordArray& array, LPCTSTR szSep)
{
	int nCount = array.GetSize();

	if (nCount == 0)
	{
		return _T("");
	}

	CString sSep(szSep);

	if (!szSep)
	{
		sSep = GetListSeparator() + _T(' ');
	}

	CString sText;

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItem;
		sItem.Format(_T("%ld"), array[nItem]);

		if (nItem > 0)
		{
			sText += sSep;
		}

		sText += sItem;
	}

	return sText;
}

void Misc::Trace(const CStringArray& array)
{
	int nCount = array.GetSize();

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		TRACE(_T("%s, "), array[nItem]);
	}

	TRACE(_T("\n"));
}

int Misc::ParseIntoArray(const CString& sText, CStringArray& array, BOOL bAllowEmpty, CString sSep)
{
	array.RemoveAll();

	if (sSep.IsEmpty())
	{
		sSep = GetListSeparator();
	}

	int nSepPrev = -1;
	int nSep = sText.Find(sSep);

	while (nSep != -1)
	{
		CString sItem = sText.Mid(nSepPrev + 1, nSep - (nSepPrev + 1));
		sItem.TrimLeft();
		sItem.TrimRight();

		if (bAllowEmpty || !sItem.IsEmpty())
		{
			array.Add(sItem);
		}

		nSepPrev = nSep;
		nSep = sText.Find(sSep, nSepPrev + 1);
	}

	// handle whatever's left so long as a separator was found
	// or there is something left
	if (nSepPrev != -1 || !sText.IsEmpty())
	{
		CString sItem = sText.Mid(nSepPrev + 1);
		sItem.TrimLeft();
		sItem.TrimRight();

		if (bAllowEmpty || !sItem.IsEmpty())
		{
			array.Add(sItem);
		}
	}

	return array.GetSize();
}

BOOL Misc::ArraysMatch(const CStringArray& array1, const CStringArray& array2, BOOL bOrderSensitive, BOOL bCaseSensitive)
{
	int nSize1 = array1.GetSize();
	int nSize2 = array2.GetSize();

	if (nSize1 != nSize2)
	{
		return 0;
	}

	if (bOrderSensitive)
	{
		for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
		{
			// check for non-equality
			if (bCaseSensitive)
			{
				if (array1[nItem1] != array2[nItem1])
				{
					return FALSE;
				}
			}
			else
			{
				if (array1[nItem1].CompareNoCase(array2[nItem1]) != 0)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	// else order not important
	for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
	{
		BOOL bMatch = FALSE;

		// look for matching item
		for (int nItem2 = 0; nItem2 < nSize2 && !bMatch; nItem2++)
		{
			if (bCaseSensitive)
			{
				bMatch = (array1[nItem1] == array2[nItem2]);
			}
			else
			{
				bMatch = (array1[nItem1].CompareNoCase(array2[nItem2]) == 0);
			}
		}

		// no-match found == not the same
		if (!bMatch)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Misc::MatchAny(const CStringArray& array1, const CStringArray& array2, BOOL bCaseSensitive)
{
	int nSize1 = array1.GetSize();
	int nSize2 = array2.GetSize();

	for (int nItem1 = 0; nItem1 < nSize1; nItem1++)
	{
		// look for matching item
		for (int nItem2 = 0; nItem2 < nSize2; nItem2++)
		{
			if (bCaseSensitive)
			{
				if (array1[nItem1] == array2[nItem2])
				{
					return TRUE;
				}
			}
			else
			{
				if (array1[nItem1].CompareNoCase(array2[nItem2]) == 0)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

int Misc::Find(const CStringArray& array, LPCTSTR szItem, BOOL bCaseSensitive)
{
	ASSERT(szItem);
	int nSize = array.GetSize();

	for (int nItem = 0; nItem < nSize; nItem++)
	{
		// look for matching item
		const CString& sItem = array[nItem];

		// special case: empty item
		if (sItem.IsEmpty() && !szItem[0])
		{
			return nItem;
		}

		if (bCaseSensitive)
		{
			if (sItem == szItem)
			{
				return nItem;
			}
		}
		else
		{
			if (sItem.CompareNoCase(szItem) == 0)
			{
				return nItem;
			}
		}
	}

	return -1;
}

int Misc::RemoveItems(const CStringArray& aItems, CStringArray& aFrom, BOOL bCaseSensitive)
{
	int nRemoved = 0; // counter
	int nItem = aItems.GetSize();

	while (nItem--)
	{
		int nFind = Find(aFrom, aItems[nItem], bCaseSensitive);

		if (nFind != -1)
		{
			aFrom.RemoveAt(nFind);
			nRemoved++;
		}
	}

	return nRemoved;
}

int Misc::AddUniqueItems(const CStringArray& aItems, CStringArray& aTo, BOOL bCaseSensitive)
{
	int nAdded = 0; // counter
	int nItem = aItems.GetSize();

	while (nItem--)
	{
		if (AddUniqueItem(aItems[nItem], aTo, bCaseSensitive))
		{
			nAdded++;
		}
	}

	return nAdded;
}

BOOL Misc::AddUniqueItem(const CString& sItem, CStringArray& aTo, BOOL bCaseSensitive)
{
	if (sItem.IsEmpty())
	{
		return FALSE;
	}

	int nFind = Find(aTo, sItem, bCaseSensitive);

	if (nFind == -1) // doesn't already exist
	{
		aTo.Add(sItem);
		return TRUE;
	}

	return FALSE; // not added
}

CString Misc::GetAM()
{
	static CString sAM;
	const int BUFLEN = 10;

	if (sAM.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S1159, sAM.GetBuffer(BUFLEN), BUFLEN - 1);
		sAM.ReleaseBuffer();
	}

	return sAM;
}

CString Misc::GetPM()
{
	static CString sPM;
	const int BUFLEN = 10;

	if (sPM.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S2359, sPM.GetBuffer(BUFLEN), BUFLEN - 1);
		sPM.ReleaseBuffer();
	}

	return sPM;
}

CString Misc::GetTimeFormat(BOOL bIncSeconds)
{
	static CString sFormat;
	const int BUFLEN = 100;

	if (sFormat.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, sFormat.GetBuffer(BUFLEN), BUFLEN - 1);
		sFormat.ReleaseBuffer();
	}

	if (!bIncSeconds)
	{
		CString sTemp(sFormat);

		if (!sTemp.Replace(GetTimeSeparator() + _T("ss"), _T("")))
		{
			sTemp.Replace(GetTimeSeparator() + _T("s"), _T(""));
		}

		return sTemp;
	}

	return sFormat;
}

CString Misc::GetTimeSeparator()
{
	static CString sSep;
	const int BUFLEN = 10;

	if (sSep.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIME , sSep.GetBuffer(BUFLEN), BUFLEN - 1);
		sSep.ReleaseBuffer();

		// Trim extra spaces
		sSep.TrimLeft();
		sSep.TrimRight();

		// If none found, use a dot
		if (!sSep.GetLength())
		{
			sSep = ":";
		}
	}

	return sSep;
}

CString Misc::GetShortDateFormat(BOOL bIncDOW)
{
	static CString sFormat;
	const int BUFLEN = 100;

	if (sFormat.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, sFormat.GetBuffer(BUFLEN), BUFLEN - 1);
		sFormat.ReleaseBuffer();
	}

	if (bIncDOW)
	{
		CString sTemp = _T("ddd ") + sFormat;
		return sTemp;
	}

	return sFormat;
}

CString Misc::GetDecimalSeparator()
{
	static CString sSep;
	const int BUFLEN = 10;

	if (sSep.IsEmpty()) // init first time only
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, sSep.GetBuffer(BUFLEN), BUFLEN - 1);
		sSep.ReleaseBuffer();

		// Trim extra spaces
		sSep.TrimLeft();
		sSep.TrimRight();

		// If none found, use a dot
		if (!sSep.GetLength())
		{
			sSep = _T(".");
		}
	}

	return sSep;
}

BOOL Misc::IsMultibyteString(const CString& sText)
{
	int nChar = sText.GetLength();

	while (nChar--)
	{
		if (IsDBCSLeadByte((BYTE)sText[nChar]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CStringA Misc::WideToMultiByte(wchar_t nChar, UINT nCodePage)
{
	char ach[4]; // assuming 3 bytes max in the multibyte encoding

	WideCharToMultiByte(nCodePage, 0, &nChar, 1, ach, sizeof(ach), 0, 0);

	return CStringA(ach);
}

double Misc::Round(double dValue)
{
	if (dValue > 0)
	{
		if ((dValue - (int)dValue) > 0.5)
		{
			return ceil(dValue);
		}
		else
		{
			return floor(dValue);
		}
	}
	else
	{
		if ((dValue - (int)dValue) > -0.5)
		{
			return floor(dValue);
		}
		else
		{
			return ceil(dValue);
		}
	}
}

float Misc::Round(float fValue)
{
	return (float)Round((double)fValue);
}

double Misc::Atof(const CString& sValue)
{
	// needs special care to handle decimal point properly
	// especially since we've no way of knowing how it is encoded.
	// so we assume that if a period is present then it's encoded
	// in 'english' else it's in native format
	TCHAR* szLocale = _tcsdup(_tsetlocale(LC_NUMERIC, NULL));

	if (sValue.Find('.') != -1)
	{
		_tsetlocale(LC_NUMERIC, _T("English"));
	}
	else
	{
		_tsetlocale(LC_NUMERIC, _T(""));
	}

	double dVal = (double)_tstof(sValue);

	// restore locale
	_tsetlocale(LC_NUMERIC, szLocale);
	free(szLocale);

	return dVal;
}

BOOL Misc::IsWorkStationLocked()
{
	// note: we can't call OpenInputDesktop directly because it's not
	// available on win 9x
	typedef HDESK(WINAPI * PFNOPENDESKTOP)(LPSTR lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
	typedef BOOL (WINAPI * PFNCLOSEDESKTOP)(HDESK hDesk);
	typedef BOOL (WINAPI * PFNSWITCHDESKTOP)(HDESK hDesk);

	// load user32.dll once only
	static HMODULE hUser32 = LoadLibrary(_T("user32.dll"));

	if (hUser32)
	{
		static PFNOPENDESKTOP fnOpenDesktop = (PFNOPENDESKTOP)GetProcAddress(hUser32, "OpenDesktopA");
		static PFNCLOSEDESKTOP fnCloseDesktop = (PFNCLOSEDESKTOP)GetProcAddress(hUser32, "CloseDesktop");
		static PFNSWITCHDESKTOP fnSwitchDesktop = (PFNSWITCHDESKTOP)GetProcAddress(hUser32, "SwitchDesktop");

		if (fnOpenDesktop && fnCloseDesktop && fnSwitchDesktop)
		{
			HDESK hDesk = fnOpenDesktop("Default", 0, FALSE, DESKTOP_SWITCHDESKTOP);

			if (hDesk)
			{
				BOOL bLocked = !fnSwitchDesktop(hDesk);

				// cleanup
				fnCloseDesktop(hDesk);

				return bLocked;
			}
		}
	}

	// must be win9x
	return FALSE;
}

#ifndef SPI_GETSCREENSAVERRUNNING
#define SPI_GETSCREENSAVERRUNNING 114
#endif

BOOL Misc::IsScreenSaverActive()
{
	BOOL bSSIsRunning = FALSE;
	::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bSSIsRunning, 0);

	return bSSIsRunning;
}

int Misc::CompareVersions(LPCTSTR szVersion1, LPCTSTR szVersion2)
{
	// if the first character of either string is not a number
	// then fall back on a standard string comparison
	if (!szVersion1 || !isdigit(szVersion1[0]) || !szVersion2 || !isdigit(szVersion2[0]))
	{
		return _tcscmp(szVersion1, szVersion2);
	}

	CStringArray aVer1, aVer2;

	Misc::Split(szVersion1, _T('.'), aVer1);
	Misc::Split(szVersion2, _T('.'), aVer2);

	// compare starting from the front
	for (int nItem = 0; nItem < aVer1.GetSize() && nItem < aVer2.GetSize(); nItem++)
	{
		int nThis = _ttoi(aVer1[nItem]);
		int nOther = _ttoi(aVer2[nItem]);

		if (nThis < nOther)
		{
			return -1;
		}

		else if (nThis > nOther)
		{
			return 1;
		}

		// else try next item
	}

	// if we got here then compare array lengths
	if (aVer1.GetSize() < aVer2.GetSize())
	{
		return -1;
	}

	else if (aVer1.GetSize() > aVer2.GetSize())
	{
		return 1;
	}

	// else
	return 0;
}

CString Misc::GetDefCharset()
{
	CString sDefCharset;
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, sDefCharset.GetBuffer(7), 6);
	sDefCharset.ReleaseBuffer();

	return sDefCharset;
}

BOOL Misc::FindWord(LPCTSTR szWord, LPCTSTR szText, BOOL bCaseSensitive, BOOL bMatchWholeWord)
{
	CString sWord(szWord), sText(szText);

	if (sWord.GetLength() > sText.GetLength())
	{
		return FALSE;
	}

	sWord.TrimLeft();
	sWord.TrimRight();

	if (!bCaseSensitive)
	{
		sWord.MakeUpper();
		sText.MakeUpper();
	}

	int nFind = sText.Find(sWord);

	if (nFind == -1)
	{
		return FALSE;
	}

	else if (bMatchWholeWord) // test whole word
	{
		const CString DELIMS(_T("()-\\/{}[]:;,. ?\"\'"));

		// prior and next chars must be delimeters
		TCHAR cPrevChar = 0, cNextChar = 0;

		// prev
		if (nFind == 0) // word starts at start
		{
			cPrevChar = _T(' ');   // known delim
		}
		else
		{
			cPrevChar = sText[nFind - 1];
		}

		// next
		if ((nFind + sWord.GetLength()) < sText.GetLength())
		{
			cNextChar = sText[nFind + sWord.GetLength()];
		}
		else
		{
			cNextChar = _T(' ');   // known delim
		}

		if (DELIMS.Find(cPrevChar) == -1 || DELIMS.Find(cNextChar) == -1)
		{
			return FALSE;
		}
	}

	return TRUE;
}

int Misc::ParseSearchString(LPCTSTR szLookFor, CStringArray& aWords)
{
	aWords.RemoveAll();

	// parse on spaces unless enclosed in double-quotes
	int nLen = lstrlen(szLookFor);
	BOOL bInQuotes = FALSE, bAddWord = FALSE;
	CString sWord;

	for (int nPos = 0; nPos < nLen; nPos++)
	{
		switch (szLookFor[nPos])
		{
			case _T(' '): // word break
				if (bInQuotes)
				{
					sWord += szLookFor[nPos];
				}
				else
				{
					bAddWord = TRUE;
				}
				break;

			case _T('\"'):
				// whether its the start or end we add the current word
				// and flip bInQuotes
				bInQuotes = !bInQuotes;
				bAddWord = TRUE;
				break;

			default: // everything else
				sWord += szLookFor[nPos];

				// also if its the last char then add it
				bAddWord = (nPos == nLen - 1);
				break;
		}

		if (bAddWord)
		{
			sWord.TrimLeft();
			sWord.TrimRight();

			if (!sWord.IsEmpty())
			{
				aWords.Add(sWord);
			}

			sWord.Empty(); // next word
		}
	}

	return aWords.GetSize();
}

CString Misc::Format(double dVal, int nDecPlaces)
{
	TCHAR* szLocale = _tcsdup(_tsetlocale(LC_NUMERIC, NULL)); // current locale
	_tsetlocale(LC_NUMERIC, _T("")); // local default

	CString sValue;
	sValue.Format(_T("%.*f"), nDecPlaces, dVal);

	// restore locale
	_tsetlocale(LC_NUMERIC, szLocale);
	free(szLocale);

	return sValue;
}

CString Misc::Format(int nVal)
{
	CString sValue;
	sValue.Format(_T("%ld"), nVal);

	return sValue;
}

CString Misc::FormatCost(double dCost)
{
	TCHAR* szLocale = _tcsdup(_tsetlocale(LC_NUMERIC, NULL)); // current locale
	_tsetlocale(LC_NUMERIC, _T("")); // local default

	CString sValue;
	sValue.Format(_T("%.6f"), dCost);

	// restore locale
	_tsetlocale(LC_NUMERIC, szLocale);
	free(szLocale);

	const UINT BUFSIZE = 100;
	TCHAR szCost[BUFSIZE + 1];

	GetCurrencyFormat(NULL, 0, sValue, NULL, szCost, BUFSIZE);
	sValue = szCost;

	return sValue;
}

BOOL Misc::KeyIsPressed(DWORD dwVirtKey)
{
	return (GetKeyState(dwVirtKey) & 0x8000);
}

BOOL Misc::IsCursorKeyPressed(DWORD dwKeys)
{
	if (dwKeys & MKC_LEFTRIGHT)
	{
		if (KeyIsPressed(VK_RIGHT) || KeyIsPressed(VK_LEFT) ||
				KeyIsPressed(VK_HOME) || KeyIsPressed(VK_END))
		{
			return TRUE;
		}
	}

	if (dwKeys & MKC_UPDOWN)
	{
		if (KeyIsPressed(VK_NEXT) || KeyIsPressed(VK_DOWN) ||
				KeyIsPressed(VK_UP) || KeyIsPressed(VK_PRIOR))
		{
			return TRUE;
		}
	}

	// else
	return FALSE;
}

BOOL Misc::IsCursorKey(DWORD dwVirtKey, DWORD dwKeys)
{
	if (dwKeys & MKC_LEFTRIGHT)
	{
		switch (dwVirtKey)
		{
			case VK_RIGHT:
			case VK_LEFT:
			case VK_HOME:
			case VK_END:
				return TRUE;
		}
	}

	if (dwKeys & MKC_UPDOWN)
	{
		switch (dwVirtKey)
		{
			case VK_NEXT:
			case VK_DOWN:
			case VK_UP:
			case VK_PRIOR:
				return TRUE;
		}
	}

	// else
	return FALSE;
}

BOOL Misc::ModKeysArePressed(DWORD dwKeys)
{
	// test for failure
	if (dwKeys & MKS_CTRL)
	{
		if (!KeyIsPressed(VK_CONTROL))
		{
			return FALSE;
		}
	}
	else if (KeyIsPressed(VK_CONTROL))
	{
		return FALSE;
	}

	if (dwKeys & MKS_SHIFT)
	{
		if (!KeyIsPressed(VK_SHIFT))
		{
			return FALSE;
		}
	}
	else if (KeyIsPressed(VK_SHIFT))
	{
		return FALSE;
	}

	if (dwKeys & MKS_ALT)
	{
		if (!KeyIsPressed(VK_MENU))
		{
			return FALSE;
		}
	}
	else if (KeyIsPressed(VK_MENU))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Misc::HasFlag(DWORD dwFlags, DWORD dwFlag)
{
	if ((dwFlags & dwFlag) == dwFlag)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// private method for implementing the bubblesort
BOOL CompareAndSwap(CStringArray& array, int pos, BOOL bAscending)
{
	CString temp;
	int posFirst = pos;
	int posNext = pos + 1;

	CString sFirst = array.GetAt(posFirst);
	CString sNext = array.GetAt(posNext);

	int nCompare = sFirst.CompareNoCase(sNext);

	if ((bAscending && nCompare > 0) || (!bAscending && nCompare < 0))
	{
		array.SetAt(posFirst, sNext);
		array.SetAt(posNext, sFirst);

		return TRUE;

	}
	return FALSE;
}

void Misc::SortArray(CStringArray& array, BOOL bAscending)
{
	BOOL bNotDone = TRUE;

	while (bNotDone)
	{
		bNotDone = FALSE;

		for (int pos = 0; pos < array.GetUpperBound(); pos++)
		{
			bNotDone |= CompareAndSwap(array, pos, bAscending);
		}
	}
}

#pragma comment(lib, "rpcrt4.lib")
