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

#ifndef ZIPARCHIVE_ZIPFILEMAPPING_DOT_H
	#error Do not include this file directly. Include ZipFileMapping.h instead
#endif

#include "ZipFile.h"
namespace ZipArchiveLib
{
	struct CZipFileMapping
	{
		CZipFileMapping()
		{
			m_hFileMap = NULL;
			m_pFileMap = NULL;
		}
		bool CreateMapping(CZipFile* pFile)
		{
			if (!pFile)
			{
				return false;
			}
			m_hFileMap = CreateFileMapping((*pFile), NULL, PAGE_READWRITE, 0, 0, _T("ZipArchive Mapping File"));
			if (!m_hFileMap)
			{
				return false;
			}
			// Get pointer to memory representing file
			m_pFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
			return (m_pFileMap != NULL);
		}
		void RemoveMapping()
		{
			if (m_pFileMap)
			{
				UnmapViewOfFile(m_pFileMap);
				m_pFileMap = NULL;
			}
			if (m_hFileMap)
			{
				CloseHandle(m_hFileMap);
				m_hFileMap = NULL;
			}

		}
		~CZipFileMapping()
		{
			RemoveMapping();
		}
		char* GetMappedMemory()
		{
			return reinterpret_cast<char*>(m_pFileMap);
		}
	protected:
		HANDLE m_hFileMap;
		LPVOID m_pFileMap;

	};
}
