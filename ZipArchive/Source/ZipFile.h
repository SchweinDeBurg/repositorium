////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2007 by Artpol Software - Tadeusz Dracz
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

/**
* \file ZipFile.h
*	Includes the CZipFile class.
*
*/


#if !defined(ZIPARCHIVE_ZIPFILE_DOT_H)
#define ZIPARCHIVE_ZIPFILE_DOT_H

#if _MSC_VER > 1000
	#pragma once
#endif

#if defined(__INTEL_COMPILER)
// warning #69: integer conversion resulted in truncation
#pragma warning(disable: 69)
// warning #1125: function is hidden by another
#pragma warning(disable: 1125)
#endif	// __INTEL_COMPILER

#include "_platform.h"

#ifdef ZIP_ARCHIVE_STL
	#include "ZipFile_stl.h"
#else
	#include "ZipFile_mfc.h"
#endif



#endif // !defined(ZIPARCHIVE_ZIPFILE_DOT_H)
