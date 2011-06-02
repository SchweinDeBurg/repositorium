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
// - merged with ToDoList version 6.1 sources
//*****************************************************************************

// CDriveInfo : returns info about drives attached

#if !defined ( DRIVEINFO_H )
#define DRIVEINFO_H

enum { DRIVE_HOST = 7 };

class CDriveInfo
{
public:
	static float GetFreeBytes(int nDrive); // in MB
	static float GetTotalBytes(int nDrive); // in MB

	static CString GetVolume(int nDrive);
	static CString GetRoot(int nDrive);
	static CString GetFullName(int nDrive);
	static char GetLetter(int nDrive);
	static int GetType(int nDrive);
	static int GetPathType(LPCTSTR szPathName);
	static BOOL IsDriveAvailable(int nDrive);
	static void FormatName(CString& sFileName);
	static int GetDrive(LPCTSTR szPathName);
	static BOOL IsMappedPath(LPCTSTR szPathName);
	static BOOL IsRemotePath(LPCTSTR szPathName, BOOL bAllowFileCheck = TRUE);
	static BOOL IsRemovablePath(LPCTSTR szPathName);
	static BOOL IsFixedPath(LPCTSTR szPathName);
	static int IsReadonlyPath(LPCTSTR szPathName); // -1 = no such path, else TRUE/FALSE
	static DWORD GetSerialNumber(int nDrive);
	static BOOL IsUNCPath(LPCTSTR szPathName);
};

#endif
