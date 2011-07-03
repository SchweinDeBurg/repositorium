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

//******************************************************************************
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
//******************************************************************************

#include "stdafx.h"
#include "Private/ZipCryptograph.h"
#include "ZipCrc32Cryptograph.h"

CZipCryptograph* CZipCryptograph::CreateCryptograph(int iEncryptionMethod)
{
	if (iEncryptionMethod == encNone)
	{
		return NULL;
	}
	return new CZipCrc32Cryptograph();
}


DWORD CZipCryptograph::GetEncryptedInfoSize(int iEncryptionMethod)
{
	if (iEncryptionMethod != encNone)
	{
		if (iEncryptionMethod == encStandard)
		{
			return CZipCrc32Cryptograph::GetEncryptedInfoSizeBeforeData() + CZipCrc32Cryptograph::GetEncryptedInfoSizeAfterData();
		}
	}
	return 0;
}

DWORD CZipCryptograph::GetEncryptedInfoSizeBeforeData(int iEncryptionMethod)
{
	if (iEncryptionMethod != encNone)
	{
		if (iEncryptionMethod == encStandard)
		{
			return CZipCrc32Cryptograph::GetEncryptedInfoSizeBeforeData();
		}
	}
	return 0;
}


DWORD CZipCryptograph::GetEncryptedInfoSizeAfterData(int iEncryptionMethod)
{
	if (iEncryptionMethod != encNone)
	{
		if (iEncryptionMethod == encStandard)
		{
			return CZipCrc32Cryptograph::GetEncryptedInfoSizeAfterData();
		}
	}
	return 0;
}
