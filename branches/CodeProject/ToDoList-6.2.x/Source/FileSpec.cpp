/*
 *	$Header: /common/FileSpec.cpp 3     8/05/01 10:38p Admin $
 *
 *	$History: FileSpec.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Admin        Date: 8/05/01    Time: 10:38p
 * Updated in $/common
 * Fixes to the /msg crash bug.  Changed the trigger point for Autostrut
 * and auto scores.  Added auto scores.
 * 
 * *****************  Version 2  *****************
 * User: Admin        Date: 12/04/01   Time: 11:40p
 * Updated in $/common
 * Fixed some resource leaks.  Added code to update the server list via
 * Sir Prober.
 * 
 * *****************  Version 1  *****************
 * User: Admin        Date: 28/03/01   Time: 8:42p
 * Created in $/common
 */
#include "stdafx.h"
#include <stdlib.h>
#include <io.h>
#include <sys\stat.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <lm.h>
#include <atlconv.h>

#include "FileSpec.h"

#if defined(__INTEL_COMPILER)
// remark #174: expression has no effect
#pragma warning(disable: 174)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #593: variable was set but never used
#pragma warning(disable: 593)
#endif	// __INTEL_COMPILER

#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "mpr.lib")

#ifdef countof
#undef countof
#endif

#define countof(x) (sizeof(x) / sizeof((x)[0]))

CFileSpec::CFileSpec(FS_BUILTINS eSpec)
{
	Initialise(eSpec);
}

CFileSpec::CFileSpec(FS_BUILTINS eSpec, LPCTSTR szFilename)
{
	ASSERT(szFilename);
	ASSERT(AfxIsValidString(szFilename));

	Initialise(eSpec);
	SetFileName(szFilename);
}

CFileSpec::CFileSpec(LPCTSTR szSpec, LPCTSTR szFilename)
{
	ASSERT(szFilename);
	ASSERT(AfxIsValidString(szFilename));

	SetFullSpec(szSpec);
	SetFileName(szFilename);
}

CFileSpec::CFileSpec(LPCTSTR szFilename)
{
	ASSERT(szFilename);
	ASSERT(AfxIsValidString(szFilename));

	SetFullSpec(szFilename);
}

void CFileSpec::Initialise(FS_BUILTINS eSpec)
{
	TCHAR tszPath[_MAX_PATH],
		  *ptsz;

	switch (eSpec)
	{
	case FS_EMPTY:								//	Nothing
		break;

	case FS_APP:								//	Full application path and name
		GetModuleFileName(NULL, tszPath, countof(tszPath));
		SetFullSpec(LPCTSTR(tszPath));
		break;

	case FS_APPDIR:								//	Application folder
		GetModuleFileName(NULL, tszPath, countof(tszPath));
		SetFullSpec(LPCTSTR(tszPath));
		m_csExtension = _T("");
		m_csFilename = _T("");
		break;

	case FS_WINDIR:								//	Windows folder
		GetWindowsDirectory(tszPath, countof(tszPath));
		PathAddBackslash(tszPath);
		SetFullSpec(LPCTSTR(tszPath));
		break;

	case FS_SYSDIR:								//	System folder
		GetSystemDirectory(tszPath, countof(tszPath));
		PathAddBackslash(tszPath);
		SetFullSpec(LPCTSTR(tszPath));
		break;

	case FS_TMPDIR:								//	Temporary folder
		GetTempPath(countof(tszPath), tszPath);
		SetFullSpec(LPCTSTR(tszPath));
		break;

	case FS_DESKTOP:							//	Desktop folder
		GetShellFolder(CSIDL_DESKTOP);
		break;

	case FS_FAVOURITES:							//	Favourites folder
		GetShellFolder(CSIDL_FAVORITES);
		break;

	case FS_TEMPNAME:
		GetTempPath(countof(tszPath), tszPath);
#ifdef _UNICODE
		_tcscpy(tszPath, ptsz = _wtempnam(tszPath, _T("~")));
#else
		_tcscpy(tszPath, ptsz = _tempnam(tszPath, _T("~")));
#endif
		SetFullSpec(LPCTSTR(tszPath));
		free(ptsz);
		break;

	case FS_MEDIA:
		GetWindowsDirectory(tszPath, countof(tszPath));
		PathAddBackslash(tszPath);
		_tcscat(tszPath, _T("media\\"));
		SetFullSpec(LPCTSTR(tszPath));
		break;

	case FS_CURRDIR:
		GetCurrentDirectory(countof(tszPath), tszPath);
		PathAddBackslash(tszPath);
		SetFullSpec(LPCTSTR(tszPath));
		break;

	default:
#ifdef _DEBUG
		TRACE(_T("Invalid initialisation spec for CFileSpec, %d\n"), eSpec);
#endif
		ASSERT(NULL);
	}
}

void CFileSpec::GetShellFolder(int iFolder)
{
	LPITEMIDLIST pItemList;
	LPMALLOC	 pMalloc;

	SHGetMalloc(&pMalloc);

	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, iFolder, &pItemList)))
	{
		TCHAR tszPath[_MAX_PATH];

		if (SHGetPathFromIDList(pItemList, tszPath))
			SetFullSpec(tszPath);

		pMalloc->Free(pItemList);
	}
}

//	Operations
BOOL CFileSpec::LoadArchive(CObject *pObj) const
{
	CFile file;
	BOOL  bStatus = FALSE;

	ASSERT(pObj);
	ASSERT_VALID(pObj);
	ASSERT_KINDOF(CObject, pObj);
	ASSERT(pObj->IsSerializable());

	if (Exists())
	{
		try
		{
			if (file.Open(GetFullSpec(), CFile::modeRead | CFile::typeBinary | CFile::shareExclusive))
			{
				CArchive ar(&file, CArchive::load);

				pObj->Serialize(ar);
				ar.Close();
				file.Close();
				bStatus = TRUE;
			}
		}
		catch(CException *e)
		{
			e->Delete();
		}
	}

	return bStatus;
}

BOOL CFileSpec::SaveArchive(CObject *pObj) const
{
	CFile file;
	BOOL  bStatus = FALSE;

	ASSERT(pObj);
	ASSERT_VALID(pObj);
	ASSERT_KINDOF(CObject, pObj);
	ASSERT(pObj->IsSerializable());

	WriteAble();

	try
	{
		if (file.Open(GetFullSpec(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareExclusive))
		{
			CArchive ar(&file, CArchive::store);

			pObj->Serialize(ar);
			ar.Close();
			file.Close();
			bStatus = TRUE;
		}
	}
	catch(CException *e)
	{
		e->Delete();
	}

	ReadOnly();
	return bStatus;
}

BOOL CFileSpec::IsUNCPath() const
{
	return IsUNCPath(m_csDrive);
}

BOOL CFileSpec::IsUNCPath(LPCTSTR szPath) const
{
	ASSERT(szPath);
	ASSERT(AfxIsValidString(szPath));

	if (_tcslen(szPath) < 2)
		return FALSE;
	
	return (szPath[0] == '\\' && szPath[1] == '\\');
}

BOOL CFileSpec::Exists() const
{
#ifdef _UNICODE
	return _waccess(GetFullSpec(), 0) == 0;
#else
	return _access(GetFullSpec(), 0) == 0;
#endif
}

void CFileSpec::WriteAble() const
{
#ifdef _UNICODE
	if (Exists())
		_wchmod(GetFullSpec(), _S_IWRITE | _S_IREAD);
#else
	if (Exists())
		_chmod(GetFullSpec(), _S_IWRITE | _S_IREAD);
#endif
}

void CFileSpec::ReadOnly() const
{
#ifdef _UNICODE
	if (Exists())
		_wchmod(GetFullSpec(), _S_IREAD);
#else
	if (Exists())
		_chmod(GetFullSpec(), _S_IREAD);
#endif
}

//	Access functions
const CString CFileSpec::GetFileName() const
{
	return m_csFilename + m_csExtension;
}

void CFileSpec::SetFileName(LPCTSTR szSpec)
{
	ASSERT(szSpec);
	ASSERT(AfxIsValidString(szSpec));

	TCHAR tszFilename[_MAX_FNAME],
		  tszExtension[_MAX_EXT];

	_tsplitpath(szSpec, NULL, NULL, tszFilename, tszExtension);
	m_csFilename = tszFilename;
	m_csExtension = tszExtension;
}

const CString	CFileSpec::FullPathNoExtension() const
{
	return m_csDrive + m_csPath + m_csFilename;
}

const CString CFileSpec::GetFolder() const
{
	return m_csDrive + m_csPath;
}

const CString CFileSpec::GetFullSpec() const
{
	return m_csDrive + m_csPath + m_csFilename + m_csExtension;
}

void CFileSpec::SetFullSpec(FS_BUILTINS eSpec)
{
	Initialise(eSpec);
}

void CFileSpec::SetFullSpec(LPCTSTR szSpec)
{
	ASSERT(szSpec);
	ASSERT(AfxIsValidString(szSpec));

	TCHAR	tszDrive[_MAX_DRIVE],
			tszPath[_MAX_PATH],
			tszFilename[_MAX_FNAME],
			tszExtension[_MAX_EXT];
	LPCTSTR ptsz = szSpec;
	int		i = 4;

	if (IsUNCPath(szSpec))
	{
		//	UNC Paths are of the form \\server\share\path\filename[.ext]
		//	For the purpose if this code we treat the server/share portion
		//	as being logically equivalent to a drive and the rest of the 
		//	path then works the same way as for the older style path.
		m_csDrive = _T("");

		//	Brute force copy of the server/share part of the string
		while (i && *ptsz)
		{
			if (*ptsz == '\\')
				i--;

			if (i)
			{
				m_csDrive += *ptsz;

				ptsz++;
			}
		}

		_tsplitpath(ptsz, tszDrive, tszPath, tszFilename, tszExtension);
		m_csPath = tszPath;
		m_csFilename = tszFilename;
		m_csExtension = tszExtension;
	}
	else
	{

		_tsplitpath(ptsz, tszDrive, tszPath, tszFilename, tszExtension);
		m_csDrive = tszDrive;
		m_csPath = tszPath;
		m_csFilename = tszFilename;
		m_csExtension = tszExtension;
	}
}

const CString CFileSpec::ConvertToUNCPath() const
{
	USES_CONVERSION;

	CString csPath = GetFullSpec();

	if (IsUNCPath(csPath))
		return csPath;

	if (csPath[1] == ':')
	{
		// Fully qualified pathname including a drive letter, check if it's a mapped drive
		UINT uiDriveType = GetDriveType(m_csDrive);
		
		if (uiDriveType & DRIVE_REMOTE)
		{
			//	Yup - it's mapped so convert to a UNC path...
			TCHAR				tszTemp[_MAX_PATH];
			UNIVERSAL_NAME_INFO *uncName = (UNIVERSAL_NAME_INFO *) tszTemp;
			DWORD				dwSize = _MAX_PATH;
			DWORD				dwRet = WNetGetUniversalName(m_csDrive, REMOTE_NAME_INFO_LEVEL, uncName, &dwSize);
			CString				csDBShare;
			
			if (dwRet == NO_ERROR)
				return uncName->lpUniversalName + m_csPath + m_csFilename + m_csExtension;
		}
		else
		{
			//	It's a local drive so search for a share to it...
			NET_API_STATUS	res;
			PSHARE_INFO_502 BufPtr,
							p;
			DWORD			er = 0,
							tr = 0,
							resume = 0,
							i;
			int				iBestMatch = 0;
			CString			csTemp,
							csTempDrive,
							csBestMatch;
			
			do
			{
				res = NetShareEnum(NULL, 502, (LPBYTE *) &BufPtr, DWORD(-1), &er, &tr, &resume);

				//
				// If the call succeeds,
				//
				if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA)
				{
					csTempDrive = GetFolder();
					csTempDrive.MakeLower();
					p = BufPtr;

					//
					// Loop through the entries;
					//  print retrieved data.
					//
					for (i = 1; i <= er; i++)
					{
						if (p->shi502_type == STYPE_DISKTREE)
						{
							csTemp = W2A((LPWSTR) p->shi502_path);
							csTemp.MakeLower();

							if (csTempDrive.Find(csTemp) == 0)
							{
								//	We found a match
								if (iBestMatch < csTemp.GetLength())
								{
									iBestMatch = csTemp.GetLength();
									csBestMatch = W2A((LPWSTR) p->shi502_netname);
								}
							}
						}

						p++;
					}

					//
					// Free the allocated buffer.
					//
					NetApiBufferFree(BufPtr);

					if (iBestMatch)
					{
						TCHAR tszComputerName[MAX_COMPUTERNAME_LENGTH + 1];
						DWORD dwBufLen = countof(tszComputerName);

						csTemp = GetFolder();
						csTemp = csTemp.Right(csTemp.GetLength() - iBestMatch + 1);
						GetComputerName(tszComputerName, &dwBufLen);
						csPath.Format(_T("\\\\%s\\%s%s%s%s"), tszComputerName, csBestMatch, csTemp, m_csFilename, m_csExtension);
					}
				}
				else 
					TRACE(_T("Error: %ld\n"), res);

				// Continue to call NetShareEnum while 
				//  there are more entries. 
				// 
			} while (res == ERROR_MORE_DATA); // end do
		}
	}

	return csPath;
}
