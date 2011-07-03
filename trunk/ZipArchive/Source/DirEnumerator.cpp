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

#if defined(_MSC_VER) && _MSC_VER < 1300
	// STL warnings
	#pragma warning(push, 3)
#endif

#include "DirEnumerator.h"
#include "FileFilter.h"

#include <queue>

#if !defined(UNDER_CE)

#if defined __GNUC__ && !defined __MINGW32__
	#include <sys/stat.h>
	#include <dirent.h>
#else
	#include <io.h>
	#ifdef __BORLANDC__
		#ifndef _tfindfirsti64
			#define _tfindfirsti64 __tfindfirsti64
		#endif
		#ifndef _tfindnexti64
			#define _tfindnexti64 __tfindnexti64
		#endif
		#ifndef _tfinddatai64_t
			#define _tfinddatai64_t __tfinddatai64_t
		#endif
	#endif
#endif

#else

static time_t FileTimeToCrtTime(const FILETIME* pftSrc)
{
	FILETIME ftLocal = { 0 };
	if (!::FileTimeToLocalFileTime(pftSrc, &ftLocal))
	{
		return (time_t(-1));
	}

	SYSTEMTIME stTemp = { 0 };
	if (!::FileTimeToSystemTime(&ftLocal, &stTemp))
	{
		return (time_t(-1));
	}

	struct tm tmMake = { 0 };
	tmMake.tm_sec = stTemp.wSecond;
	tmMake.tm_min = stTemp.wMinute;
	tmMake.tm_hour = stTemp.wHour;
	tmMake.tm_mday = stTemp.wDay;
	tmMake.tm_mon = stTemp.wMonth - 1;
	tmMake.tm_year = stTemp.wYear - 1900;
	tmMake.tm_isdst = -1;

	return (mktime(&tmMake));
}

#endif   // UNDER_CE

namespace ZipArchiveLib
{

#if defined __GNUC__ && !defined __MINGW32__
	#define ZIP_ENUMERATOR_FOR_GNUC
#endif


bool CDirEnumerator::Start(CFileFilter& filter)
{
	OnEnumerationBegin();
	std::queue<CZipString> dirs;
	dirs.push(CZipString(m_lpszDirectory));
	bool ret = true;
	do
	{
		m_szCurrentDirectory = dirs.front();
		dirs.pop();
		CZipPathComponent::AppendSeparator(m_szCurrentDirectory);
		EnterDirectory();

#ifdef ZIP_ENUMERATOR_FOR_GNUC
		DIR* dp = opendir(m_szCurrentDirectory);
		if (dp)
		{
			while (true)
			{
				struct dirent* entry = readdir(dp);
				if (!entry)
				{
					break;
				}
				CZipString path(m_szCurrentDirectory + entry->d_name);
#if !defined __APPLE__ && !defined __CYGWIN__
				struct stat64 sStats;
				if (stat64(path, &sStats) == -1)
#else
				struct stat sStats;
				if (stat(path, &sStats) == -1)
#endif
					continue;

				LPCTSTR name = entry->d_name;
				CFileInfo info;
				info.m_uAttributes = sStats.st_mode;

#else
		CZipString szFullFileName = m_szCurrentDirectory + _T("*");

#if !defined(UNDER_CE)
		_tfinddatai64_t ffInfo;
#if (_MSC_VER > 1200)
		intptr_t hFile;
#else
		long hFile;
#endif
		if ((hFile = _tfindfirsti64((LPTSTR)(LPCTSTR)szFullFileName, &ffInfo)) != -1L)
#else
		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFile = INVALID_HANDLE_VALUE;
		if ((hFile = ::FindFirstFile(szFullFileName, &wfd)) != INVALID_HANDLE_VALUE)
#endif   // UNDER_CE
		{
			do
			{
#if !defined(UNDER_CE)
				LPCTSTR name = ffInfo.name;
				CFileInfo info;
				info.m_uAttributes = ffInfo.attrib;
#else
				LPCTSTR name = wfd.cFileName;
				CFileInfo info;
				info.m_uAttributes = wfd.dwFileAttributes != FILE_ATTRIBUTE_NORMAL ? wfd.dwFileAttributes : 0;
#endif   // UNDER_CE
#endif   // ZIP_ENUMERATOR_FOR_GNUC
				bool isDir;
				if (ZipPlatform::IsDirectory(info.m_uAttributes))
				{
					if (!m_bRecursive || IsDots(name))
					{
						continue;
					}
					isDir = true;
				}
				else
				{
					isDir = false;
				}

#ifdef ZIP_ENUMERATOR_FOR_GNUC
				info.m_uSize  = (ZIP_FILE_USIZE)sStats.st_size;
				info.m_uCreateTime = sStats.st_ctime;
				info.m_uModTime = sStats.st_mtime;
				info.m_uAccessTime = sStats.st_atime;
#elif !defined(UNDER_CE)
				info.m_uSize = (ZIP_FILE_USIZE)ffInfo.size;
				info.m_uCreateTime = ffInfo.time_create;
				info.m_uModTime = ffInfo.time_write;
				info.m_uAccessTime = ffInfo.time_access;
				CZipString path(m_szCurrentDirectory + ffInfo.name);
#else
				info.m_uSize = (ZIP_FILE_USIZE)(wfd.nFileSizeHigh * MAXDWORD + 1) + wfd.nFileSizeLow;
				info.m_uCreateTime = FileTimeToCrtTime(&wfd.ftCreationTime);
				info.m_uModTime = FileTimeToCrtTime(&wfd.ftLastWriteTime);
				info.m_uAccessTime = FileTimeToCrtTime(&wfd.ftLastAccessTime);
				CZipString path(m_szCurrentDirectory + wfd.cFileName);
#endif

				if (isDir)
				{
					bool bAllow;
					if (filter.HandlesFile(info))
					{
						bAllow = filter.Evaluate(path, name, info) && Process(path, info);
					}
					else
						// examine directory, if the filter cannot decide
					{
						bAllow = true;
					}

					if (bAllow)
					{
						dirs.push(path);
					}
				}
				else
				{
					bool bAllow;
					if (filter.HandlesFile(info))
					{
						bAllow = filter.Evaluate(path, name, info);
					}
					else
						// skip file, if the filter cannot decide
					{
						bAllow = false;
					}

					if (bAllow && !Process(path, info))
					{
						ret = false;
						break;
					}
				}

#ifdef ZIP_ENUMERATOR_FOR_GNUC
			}
			closedir(dp);
		}
#elif !defined(UNDER_CE)
			}
			while (_tfindnexti64(hFile, &ffInfo) == 0L);
			_findclose(hFile);
		}
#else
			}
			while (::FindNextFile(hFile, &wfd) != 0);
			::FindClose(hFile);
		}
#endif
		ExitDirectory();
	}
	while (!dirs.empty() && ret);
	OnEnumerationEnd(ret);
	return ret;
}

bool CDirEnumerator::IsDots(LPCTSTR lpszName)
{
	CZipString name(lpszName);
	return name.Compare(_T(".")) == 0 || name.Compare(_T("..")) == 0;
}

} // namespace

#if defined(_MSC_VER) && _MSC_VER < 1300
	// STL warnings
	#pragma warning(pop)
#endif

