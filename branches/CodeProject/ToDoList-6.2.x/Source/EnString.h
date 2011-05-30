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

// EnString.h: interface for the CEnString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_)
#define AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum { ES_END, ES_WORD, ES_PATH, ES_START };

class CEnString : public CString
{
public:
	CEnString();
	CEnString(LPCTSTR lpszFormat, ...);
	CEnString(UINT nFormatID, LPCTSTR szText = NULL);
	CEnString(UINT nFormatID, int nData);
	CEnString(const CString& str);

	~CEnString(); // non-virtual else it messes up LPCTSTR operator

	CSize FormatDC(CDC* pDC, int nWidth, int nStyle = ES_END);
	CSize Draw(CDC* pDC, LPRECT lpRect, int nStyle = ES_END);

protected:
	CSize FormatDCEx(CDC* pDC, int nWidth, int nStyle = ES_END);

};

#endif // !defined(AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_)
