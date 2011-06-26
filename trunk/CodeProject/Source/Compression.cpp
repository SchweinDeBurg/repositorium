// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// Compression.cpp: implementation of the CCompression class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Compression.h"

#include "../../zlib/Source/zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL Compression::Compress(const unsigned char* pContentSrc, int nLenSrc, unsigned char*& pContentDest, int& nLenDest)
{
	ULONG lDest = (int)(nLenSrc * 1.001) + 12; // zlib requirements
	pContentDest = new unsigned char[lDest + 4]; // extra DWORD for storing the source length

	// leave the first DWORD free
	int nRet = compress(pContentDest + 4, &lDest, pContentSrc, nLenSrc);

	if (nRet != Z_OK)
	{
		// cleanup
		delete [] pContentDest;
		nLenDest = 0;
	}
	else
	{
		nLenDest = lDest + 4;

		// encode the source length into the first 4 bytes
		((DWORD*)pContentDest)[0] = nLenSrc;
	}

	return (nRet == Z_OK);
}

BOOL Compression::Decompress(const unsigned char* pContentSrc, int nLenSrc, unsigned char*& pContentDest,
	int& nLenDest)
{
	// get the source length from the first 4 bytes
	ULONG lDest = ((DWORD*)pContentSrc)[0];

	// sanity check
	if (lDest < (ULONG)nLenSrc || lDest > (ULONG)(1000 * nLenSrc))
	{
		return FALSE;
	}

	pContentDest = new unsigned char[lDest];

	int nRet = uncompress(pContentDest, &lDest, pContentSrc + 4, nLenSrc - 4);

	if (nRet != Z_OK)
	{
		delete [] pContentDest;
		nLenDest = 0;
	}
	else
	{
		nLenDest = lDest;
	}

	return (nRet == Z_OK);
}
