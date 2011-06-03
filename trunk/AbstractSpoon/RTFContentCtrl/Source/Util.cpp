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
// - adjusted #include's paths
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

#include "StdAfx.h"
#include "Util.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CString LongToString(long newValue)
{
	long lvalue;
	CString str_rtc;
	lvalue = newValue;
	str_rtc.Format(_T("%d"), lvalue);
	return str_rtc;
} //LongToString()

long StringToLong(CString newValue)
{
	long lvalue;
	char* p_str;
	char* p_endstr;
	CString str;
	str = newValue;
	p_str = (LPSTR) LPCTSTR(str);
	lvalue = strtol(p_str, &p_endstr, 10);

	return lvalue;
} //StringToLong()

void StringToArray(const CString& strSrc, CStringArray& trgt)
{
	CString strDelimiter = _T(";");
	CString strElement;
	long lPosition = 0;

	while (lPosition < strSrc.GetLength())
	{
		if ((strDelimiter.Find(strSrc.GetAt(lPosition)) >= 0))
		{
			trgt.Add(strElement);
			strElement = _T("");
		}
		else
		{
			strElement += strSrc.GetAt(lPosition);
		} //if-else is_field_delimiter
		lPosition++;
	}
	trgt.Add(strElement); // add last
}
