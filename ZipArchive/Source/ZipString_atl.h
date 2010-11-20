////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2009 by Artpol Software - Tadeusz Dracz
//
// This source code is licensed as closed source and its use is 
// strictly subject to the terms and conditions of the 
// The ZipArchive Library Commercial License.
// The license is distributed with the source code in the License.txt file.
//
// Web Site: http://www.artpol-software.com
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
typedef int (CZipString::*ZIPSTRINGCOMPARE)( LPCTSTR ) const;

/**
	Returns a pointer to a method in the CZipString structure, 
	used to compare elements depending on the arguments.
*/
ZIP_API ZIPSTRINGCOMPARE GetCZipStrCompFunc(bool bCaseSensitive, bool bCollate = true);
