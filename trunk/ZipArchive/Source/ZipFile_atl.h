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

#ifndef ZIPARCHIVE_ZIPFILE_DOT_H
	#error Do not include this file directly. Include ZipFile.h instead
#endif

#if (_MSC_VER > 1000) && defined ZIP_HAS_DLL
	#pragma warning(push)
	#pragma warning(disable: 4275) // non dll-interface used as base for dll-interface class
#endif

#include "ZipAbstractFile.h"
#include "ZipExport.h"

class ZIP_API CZipFile : public CZipAbstractFile, public ATL::CAtlFile
{
public:
	enum OpenModes
	{
		modeRead =         0x00000,
		modeWrite =        0x00001,
		modeReadWrite =    0x00002,
		shareExclusive =   0x00010,
		shareDenyWrite =   0x00020,
		shareDenyRead =    0x00030,
		shareDenyNone =    0x00040,
		modeCreate =       0x01000,
		modeNoTruncate =   0x02000
	};

	void Flush(void)
	{
		ATL::CAtlFile::Flush();
	}

	CZipString GetFilePath(void) const
	{
		return (m_strFilePath);
	}

	ZIP_FILE_USIZE GetPosition(void) const
	{
		ULONGLONG uPos = 0;
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::GetPosition(uPos)));
		return (uPos);
	}

	void SetLength(ZIP_FILE_USIZE nNewLen)
	{
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::SetSize(nNewLen)));
	}

	ZIP_FILE_USIZE Seek(ZIP_FILE_SIZE lOff, int nFrom)
	{
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::Seek(lOff, nFrom)));
		return (GetPosition());
	}

	ZIP_FILE_USIZE GetLength(void) const
	{
		ULONGLONG cbSize = 0;
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::GetSize(cbSize)));
		return (cbSize);
	}

	UINT Read(void* lpBuf, UINT nCount)
	{
		DWORD dwBytesRead = 0;
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::Read(lpBuf, nCount, dwBytesRead)));
		return (dwBytesRead);
	}

	void Write(const void* lpBuf, UINT nCount)
	{
		ATLVERIFY(SUCCEEDED(ATL::CAtlFile::Write(lpBuf, nCount)));
	}

	bool IsClosed(void) const
	{
		return (ATL::CAtlFile::m_h == NULL);
	}


	CZipFile(LPCTSTR lpszFileName, UINT nOpenFlags)
	{
		Open(lpszFileName, nOpenFlags, false);
	}

	void Close(void)
	{
		if (!IsClosed())
		{
			ATL::CAtlFile::Close();
		}
	}

	operator HANDLE(void)
	{
		return (ATL::CAtlFile::m_h);
	}

	CZipFile(void);
	bool Open(LPCTSTR lpszFileName, UINT nOpenFlags, bool bThrowExc);
	virtual ~CZipFile(void);

protected:
	CZipString m_strFilePath;
};

#if (_MSC_VER > 1000) && (defined ZIP_HAS_DLL)
	#pragma warning(pop)
#endif
