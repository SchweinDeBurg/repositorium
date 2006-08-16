////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2006 by Tadeusz Dracz (http://www.artpol-software.com/)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details see the file License.txt
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipCompatibility.h"
#include "ZipPlatform.h"
#include "ZipException.h"
#include "ZipAutoBuffer.h"
#include "ZipFileHeader.h"

#if defined(__INTEL_COMPILER)
// remark #1418: external definition with no prior declaration
#pragma warning(disable: 1418)
// remark #1419: external declaration in primary source file
#pragma warning(disable: 1419)
#endif	// __INTEL_COMPILER

enum iInternalAttr
{
	attROnly	= 0x01,	
	attHidd		= 0x02,
	attSys		= 0x04,
	attDir		= 0x10,
	attArch		= 0x20
};
// *********************** WINDOWS **************************
#ifndef _WIN32
	#define FILE_ATTRIBUTE_READONLY             0x00000001  
	#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
	#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
	#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
	#define FILE_ATTRIBUTE_ARCHIVE              0x00000020 
#endif
// *********************** UINX **************************
#define USER_PERMISSIONS_MASK  0x01C00000
#define EXTRACT_USER_PERMISSIONS(x) ((x & USER_PERMISSIONS_MASK) >> 22)
#define CREATE_USER_PERMISSIONS(x) ((x & 0x0007) << 22)

#define GROUP_PERMISSIONS_MASK 0x00380000
#define EXTRACT_GROUP_PERMISSIONS ((x & GROUP_PERMISSIONS_MASK) >> 19)
#define CREATE_GROUP_PERMISSIONS(x) ((x & 0x0007) << 19)

#define OTHER_PERMISSIONS_MASK  0x00070000
#define EXTRACT_OTHER_PERMISSIONS  ((x & OTHER_PERMISSIONS_MASK) >> 16)
#define CREATE_OTHER_PERMISSIONS(x) ((x & 0x0007) << 16)

#define UNIX_DIRECTORY_ATTRIBUTE 0x40000000
#define UNIX_FILE_ATTRIBUTE 0x80000000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace ZipCompatibility;

typedef DWORD(*conv_func)(DWORD , bool );

DWORD AttrDos(DWORD , bool );
DWORD AttrUnix(DWORD, bool);
DWORD AttrMac(DWORD , bool );

// more to come...
conv_func conv_funcs[11] = {AttrDos,
							NULL,
							NULL,
							AttrUnix,
							NULL,
							NULL,
							AttrDos,
							AttrMac,
							NULL,
							NULL,
							AttrDos
};



DWORD ZipCompatibility::ConvertToSystem(DWORD uAttr, int iFromSystem, int iToSystem)
{
	
	if (iToSystem != iFromSystem && iFromSystem < 11 && iToSystem < 11)
	{
		conv_func p = conv_funcs[iFromSystem], q = conv_funcs[iToSystem];
		if (p && q)
			uAttr = q( p(uAttr, true), false);
	 	else
	 		CZipException::Throw(CZipException::platfNotSupp);
	}
	return uAttr; 
}


DWORD AttrDos(DWORD uAttr, bool )
{
	return uAttr;	
}



DWORD AttrUnix(DWORD uAttr, bool bFrom)
{
	DWORD uNewAttr = 0;
	if (bFrom)
	{
		if (uAttr & UNIX_DIRECTORY_ATTRIBUTE)
			uNewAttr = attDir;

		uAttr = EXTRACT_USER_PERMISSIONS (uAttr);

		// we may set archive attribute if the file hasn't the execute permissions
		// 
		if (!(uAttr & 1))
			uNewAttr |= attArch	;

		if (!(uAttr & 2)) 
		    uNewAttr |= attROnly;

	    if (!(uAttr & 4)) 
		    uNewAttr |= attHidd;
	}
	else
	{

		uNewAttr = 0; // we cannot assume that if the file hasn't the archive attribute set
		
		//then it is executable and set execute permissions

		if (!(uAttr & attHidd)) 
			uNewAttr |= (CREATE_OTHER_PERMISSIONS (4) | 
								  CREATE_GROUP_PERMISSIONS (4))
								  | CREATE_USER_PERMISSIONS (4);

		if (!(uAttr & attROnly))
			uNewAttr |= (CREATE_GROUP_PERMISSIONS (2) |
								  CREATE_USER_PERMISSIONS (2));
		if (uAttr & attDir) 
			uNewAttr |= UNIX_DIRECTORY_ATTRIBUTE | attDir;
		else
			uNewAttr |= UNIX_FILE_ATTRIBUTE;

	}

	return uNewAttr;	
}

DWORD AttrMac(DWORD uAttr, bool )
{
	DWORD uNewAttr  = uAttr & (attDir | attROnly);
// 	if (bFrom)
// 	{
// 		
// 	}
// 	else
// 	{
// 		
// 	}
	return uNewAttr;
}

// ************************************************************************
ZIPINLINE bool ZipCompatibility::IsPlatformSupported(int iCode)
{
	return iCode == zcDosFat || iCode == zcUnix || iCode == zcMacintosh
		|| iCode == zcNtfs || iCode == zcOs2Hpfs;
}


void ZipCompatibility::FileNameUpdate(CZipFileHeader& header, bool bFromZip, bool bOemConversion)
{
	int iSysHeader = header.GetSystemCompatibility();
	int iCurSystem = ZipPlatform::GetSystemID();
	if (bFromZip) 
	{
		if (iCurSystem == zcDosFat)
			SlashBackslashChg(header.m_pszFileName, true);
		if (iSysHeader == zcDosFat && bOemConversion)
			ZipPlatform::AnsiOem(header.m_pszFileName, false);
	}
	else
	{
		if (iSysHeader == zcDosFat && bOemConversion)
		{
			ZipPlatform::AnsiOem(header.m_pszFileName, true);				
		}
		SlashBackslashChg(header.m_pszFileName, false);
	}
}

void ZipCompatibility::SlashBackslashChg(CZipAutoBuffer& buffer, bool bReplaceSlash)
{
	char t1 = '\\' /*backslash*/, t2 = '/', c1, c2;
	if (bReplaceSlash)
	{
		c1 = t1;
		c2 = t2;
	}
	else
	{
		c1 = t2;
		c2 = t1;
	}
	for (DWORD i = 0; i < buffer.GetSize(); i++)
	{
		if (buffer[i] == c2)
			buffer[i] = c1;
	}

}


ZIPINLINE void ZipCompatibility::ReadBytesBigEndian(void* pDestination, const char* pSource, int iCount)
{
	for (int i = 0; i < iCount; i++)
		((char*)pDestination)[i] = pSource[iCount - i - 1];
}

ZIPINLINE void ZipCompatibility::ReadBytesLittleEndian(void* pDestination, const char* pSource, int iCount)
{
	memcpy(pDestination, pSource, iCount);
}


ZIPINLINE void ZipCompatibility::WriteBytesBigEndian(char* pDestination, const void* pSource, int iCount)
{
	for (int i = 0; i < iCount; i++)
		pDestination[i] = ((char*)pSource)[iCount - i - 1];
}

ZIPINLINE void ZipCompatibility::WriteBytesLittleEndian(char* pDestination, const void* pSource, int iCount)
{
	memcpy(pDestination, pSource, iCount);
}

ZIPINLINE bool ZipCompatibility::CompareBytesBigEndian(const char* pBuffer, const void* pBytes, int iCount)
{
	for (int i = 0; i < iCount; i++)
		if (pBuffer[i] != ((char*)pBytes)[iCount - i - 1])
			return false;
	return true;
}

ZIPINLINE bool ZipCompatibility::CompareBytesLittleEndian(const char* pBuffer, const void* pBytes, int iCount)
{
	return memcmp(pBuffer, pBytes, iCount) == 0;
}

ZIPINLINE bool ZipCompatibility::IsBigEndian()
{
	unsigned long endian = 1;
	return (*((unsigned char *)(&endian))) == 0;
}
