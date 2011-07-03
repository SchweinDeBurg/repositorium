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

#include "stdafx.h"
#include "Private/ZipArchive.h"
#include "ZipExtraData.h"
#include "Private/BytesWriter.h"

using namespace ZipArchiveLib;

bool CZipExtraData::Read(char* buffer, WORD uSize)
{
	if (uSize < 4)
	{
		return false;
	}
	CBytesWriter::ReadBytes(m_uHeaderID, buffer);
	m_bHasSize = CZipExtraField::HasSize(m_uHeaderID);
	WORD size;
	if (m_bHasSize)
	{
		CBytesWriter::ReadBytes(size, buffer + 2);
		if (size > (WORD)(uSize - 4))
		{
			return false;
		}
		m_data.Allocate(size);
		memcpy(m_data, buffer + 4, size);
	}
	else
	{
		// The size is not always relevant (in extra headers not conforming to the standard)
		size = (WORD)(uSize - 2);
		m_data.Allocate(size);
		memcpy(m_data, buffer + 2, size);
	}
	return true;
}

WORD CZipExtraData::Write(char* buffer) const
{
	CBytesWriter::WriteBytes(buffer, m_uHeaderID);
	WORD size = (WORD)m_data.GetSize();
	buffer += 2;
	if (m_bHasSize)
	{
		CBytesWriter::WriteBytes(buffer, size);
		buffer += 2;
	}
	memcpy(buffer, m_data, size);
	return (WORD)(size + 4);
}
