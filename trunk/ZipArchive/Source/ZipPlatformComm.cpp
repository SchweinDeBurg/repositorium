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
#include "ZipFile.h"
#include "ZipPlatform.h"
#include "Private/ZipException.h"

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

using namespace ZipPlatform;

bool ZipPlatform::ForceDirectory(LPCTSTR lpDirectory)
{
	ASSERT(lpDirectory);
	CZipString szDirectory = lpDirectory;
	szDirectory.TrimRight(CZipPathComponent::m_cSeparator);
	CZipPathComponent zpc(szDirectory);
	if ((zpc.GetFilePath().Compare((LPCTSTR)szDirectory)) == 0 || (FileExists(szDirectory) == -1))
	{
		return true;
	}
	if (!ForceDirectory(zpc.GetFilePath()))
	{
		return false;
	}
	if (!CreateNewDirectory(szDirectory))
	{
		return false;
	}
	return true;
}

bool ZipPlatform::GetFileSize(LPCTSTR lpszFileName, ZIP_SIZE_TYPE& dSize)
{
	CZipFile f;
	if (!f.Open(lpszFileName, CZipFile::modeRead | CZipFile::shareDenyWrite, false))
	{
		return false;
	}
	bool ret;
	try
	{
		ZIP_FILE_USIZE size = f.GetLength();
		// the file may be too large if zip64 is not enabled
		ret = size <= ZIP_SIZE_TYPE(-1);
		if (ret)
		{
			dSize = (ZIP_SIZE_TYPE)size;
		}
	}
#if defined(_ZIP_IMPL_MFC)
	catch (CZipBaseException* e)
	{
		e->Delete();
		ret = false;
	}
#elif defined(_ZIP_IMPL_ATL)
	catch (const CZipBaseException& /*err*/)
	{
		ret = false;
	}
#else
	catch (CZipBaseException e)
	{
		ret = false;
	}
#endif

	try
	{
		f.Close();
	}
#if defined(_ZIP_IMPL_MFC)
	catch (CZipBaseException* e)
	{
		e->Delete();
	}
#elif defined(_ZIP_IMPL_ATL)
	catch (const CZipBaseException& /*err*/)
	{
		ret = false;
	}
#else
	catch (CZipBaseException e)
	{
	}
#endif

	return ret;
}


