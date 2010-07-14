// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// OSVersion.cpp: implementation of the COSVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OSVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COSVersion::COSVersion()
{

}

COSVersion::~COSVersion()
{

}

OSVERSION COSVersion::GetOSVersion()
{
	static OSVERSION nVersion = OSV_UNKNOWN;

	if (nVersion == OSV_UNKNOWN) // first time only
	{
		OSVERSIONINFO vinfo;
		vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
		BOOL rslt = GetVersionEx(&vinfo);
		
		if (rslt)
		{
			switch (vinfo.dwPlatformId)
			{
			case VER_PLATFORM_WIN32_NT:
				switch (vinfo.dwMajorVersion)
				{
				case 3: // nt351
					ASSERT (0); // not supported
					break;
					
				case 4: // nt4
					nVersion = OSV_NT4;
					break;
					
				case 5: // >= w2k
					switch (vinfo.dwMinorVersion)
					{
					case 0: // w2k
						nVersion = OSV_2K;
						break;
						
					case 1: // xp
						nVersion = OSV_XP;
						break;
						
					default: // > xp
						nVersion = OSV_XPP;
						break;
					}
					break;
					
					default: // > xp
						nVersion = OSV_XPP;
						break;
				}
				break;
				
				case VER_PLATFORM_WIN32_WINDOWS:
					ASSERT (vinfo.dwMajorVersion == 4);
					
					switch (vinfo.dwMinorVersion)
					{
					case 0: 
						nVersion = OSV_95;
						break;
						
					case 10:
						nVersion = OSV_98;
						break;
						
					case 90:
						nVersion = OSV_ME;
						break;
						
					default:
						ASSERT (0);
						break;
					}
					break;
					
					default:
						ASSERT (0);
						break;
			}
		}
	}

	return nVersion;
}
