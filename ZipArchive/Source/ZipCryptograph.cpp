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

#include "stdafx.h"
#include "ZipCryptograph.h"
#include "ZipCrc32Cryptograph.h"

CZipCryptograph* CZipCryptograph::CreateCryptograph(int iEncryptionMethod)
{
	return (iEncryptionMethod != encNone ? new CZipCrc32Cryptograph() : NULL);
}
