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

// EnString.cpp: implementation of the CEnString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnString::CEnString() : CString()
{
}

CEnString::CEnString(LPCTSTR lpszFormat, ... )
{
	if (lpszFormat)
	{
		ASSERT(AfxIsValidString(lpszFormat));

		va_list argList;
		va_start(argList, lpszFormat);
		CString::FormatV(lpszFormat, argList);
		va_end(argList);
	}
}

CEnString::CEnString(UINT nFormatID, LPCTSTR szText)
{
	if (nFormatID)
	{
		CString strFormat;
		VERIFY(strFormat.LoadString(nFormatID) != 0);

		if (szText && *szText)
			Format(strFormat, szText);
		else
			*this = strFormat;
	}
}

CEnString::CEnString(UINT nFormatID, int nData)
{
	if (nFormatID)
	{
		CString strFormat;
		VERIFY(strFormat.LoadString(nFormatID) != 0);

		Format(strFormat, nData);
	}
}

CEnString::CEnString(const CString& str) : CString(str)
{

}

CEnString::~CEnString()
{

}

CSize CEnString::FormatDC(CDC* pDC, int nWidth, int nStyle)
{
	CRect rect(0, 0, nWidth, 20);
	UINT uFlags = DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING;

	// special case: ES_START
	if (nStyle == ES_START)
		FormatDCEx(pDC, nWidth, nStyle);
	else
	{
		switch (nStyle)
		{
			case ES_END:
				uFlags |= DT_END_ELLIPSIS;
				break;

			case ES_WORD:
				uFlags |= DT_WORD_ELLIPSIS;
				break;

			case ES_PATH:
				uFlags |= DT_PATH_ELLIPSIS;
				break;
		}

		::DrawText(pDC->GetSafeHdc(), GetBuffer(GetLength() + 4), -1, rect, uFlags);
		ReleaseBuffer();

		// if its still too big then do our internal version
		if (rect.Width() > nWidth)
			FormatDCEx(pDC, nWidth, nStyle);
	}

	return pDC->GetTextExtent(*this);
}

CSize CEnString::Draw(CDC* pDC, LPRECT lpRect, int nStyle)
{
	// we must do the format independently of DrawText because we override it
	CEnString sTemp(*this);
	sTemp.FormatDC(pDC, lpRect->right - lpRect->left, nStyle);

	pDC->DrawText(sTemp, lpRect, DT_SINGLELINE);

	return pDC->GetTextExtent(sTemp);
}

CSize CEnString::FormatDCEx(CDC* pDC, int nWidth, int nStyle)
{
	CString sFinalText = *this;

	CSize sizeText = pDC->GetTextExtent(sFinalText);
	CSize sizeEllipsis = pDC->GetTextExtent(_T("..."));

	bool bEndEllipsis = (nStyle == ES_END || nStyle == ES_WORD);
	
	// truncate string if too long adding ellipsis (...)
	if ((sizeText.cx + sizeEllipsis.cx) > nWidth)
	{
		while ((sizeText.cx + sizeEllipsis.cx) > nWidth)
		{
			// truncate another char
			if (bEndEllipsis)
			{
				sFinalText = sFinalText.Left(sFinalText.GetLength() - 1);
				sizeText = pDC->GetTextExtent(CString(_T("...") + sFinalText));
			}
			else
			{
				sFinalText = sFinalText.Mid(1);
				sizeText = pDC->GetTextExtent(CString(sFinalText + _T("...")));
			}

			if (sFinalText.IsEmpty())
				break;
		}
			
		if (bEndEllipsis)
			sFinalText += _T("...");
		else
			sFinalText = _T("...") + sFinalText;
	}

	*this = CEnString(sFinalText);

	return pDC->GetTextExtent(*this);
}
