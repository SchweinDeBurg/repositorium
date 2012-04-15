////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2012 by Artpol Software - Tadeusz Dracz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// For the licensing details refer to the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --min-conditional-indent=0
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - implemented support for the Windows Mobile/CE tragets
// - added possibility to seamless usage in the ATL-based projects
////////////////////////////////////////////////////////////////////////////////

#ifndef ZIPARCHIVE_ZIPSTRING_DOT_H
	#error Do not include this file directly. Include ZipString.h instead
#endif


#include "stdafx.h"
#include "ZipExport.h"

typedef ATL::CString CZipString;

/**
	A pointer type to point to one of: Collate, CollateNoCase, Compare, CompareNoCase.
*/
typedef int (CZipString::*ZIPSTRINGCOMPARE)(LPCTSTR) const;

/**
	Returns a pointer to a method in the CZipString structure,
	used to compare elements depending on the arguments.
*/
ZIP_API ZIPSTRINGCOMPARE GetCZipStrCompFunc(bool bCaseSensitive, bool bCollate = true);
