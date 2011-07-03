////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2011 by Artpol Software - Tadeusz Dracz
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

/**
* \file ZipMutex.h
*	Includes the ZipArchiveLib::CZipMutex class.
*
*/

#if !defined(ZIPARCHIVE_ZIPMUTEX_DOT_H)
#define ZIPARCHIVE_ZIPMUTEX_DOT_H

#if (_MSC_VER > 1000)
	#pragma once
#endif

#include "_features.h"

#ifdef _ZIP_SYSTEM_LINUX
	#include "ZipMutex_lnx.h"
#else
	#include "ZipMutex_win.h"
#endif


#endif

