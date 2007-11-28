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
#include "ZipCompressor.h"
#include "DeflateCompressor.h"

using namespace ZipArchiveLib;

CZipCompressor* CZipCompressor::CreateCompressor(WORD uMethod, CZipStorage* pStorage, CZipAutoBuffer* pBuffer)
{
	if (uMethod == methodStore || uMethod == methodDeflate)
		return new CDeflateCompressor(pStorage, pBuffer);
	return NULL;
}

void CZipCompressor::UpdateFileCrc(const void *pBuffer, DWORD uSize)
{
	m_pFile->m_uCrc32 = zarch_crc32(m_pFile->m_uCrc32, (zarch_Bytef*)pBuffer, uSize);
}

void CZipCompressor::UpdateCrc(const void *pBuffer, DWORD uSize)
{
	m_uCrc32 = zarch_crc32(m_uCrc32, (zarch_Bytef*)pBuffer, uSize);
}