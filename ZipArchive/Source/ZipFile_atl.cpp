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

#include "stdafx.h"
#include "_features.h"

#if defined(_ZIP_IMPL_ATL) && (!defined _ZIP_FILE_IMPLEMENTATION || _ZIP_FILE_IMPLEMENTATION == ZIP_ZFI_DEFAULT)

#include "ZipFile.h"

CZipFile::CZipFile()
{
}

CZipFile::~CZipFile()
{
	Close();
}

bool CZipFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, bool bThrowExc)
{
	DWORD access = 0;
	DWORD temp = nOpenFlags & 3;
	if (temp == modeWrite)
	{
		access = GENERIC_WRITE;
	}
	else if (temp == modeReadWrite)
	{
		access = GENERIC_READ | GENERIC_WRITE;
	}
	else
	{
		access = GENERIC_READ;
	}

	DWORD share = 0;
	temp = nOpenFlags & 0x70;
	if (temp == shareDenyWrite)
	{
		share = FILE_SHARE_READ;
	}
	else if (temp == shareDenyRead)
	{
		share = FILE_SHARE_WRITE;
	}
	else if (temp == shareDenyNone)
	{
		share = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else
	{
		share = 0;
	}

	DWORD create = 0;
	if (nOpenFlags & modeCreate)
	{
		if (nOpenFlags & modeNoTruncate)
		{
			create = OPEN_ALWAYS;
		}
		else
		{
			create = CREATE_ALWAYS;
		}
	}
	else
	{
		create = OPEN_EXISTING;
	}

	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HRESULT hr = ATL::CAtlFile::Create(lpszFileName, access, share, create, FILE_ATTRIBUTE_NORMAL, &sa, NULL);

	if (FAILED(hr))
	{
		if (bThrowExc)
		{
			AtlThrow(hr);
		}
		return (false);
	}
	else {
		m_strFilePath = lpszFileName;
		return (true);
	}
}

#endif
