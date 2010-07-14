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

#include "stdafx.h"
#include "FileMisc.h"
#include "FileRegister.h"
#include "Misc.h"

#include <sys/utime.h>
#include <sys/stat.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

///////////////////////////////////////////////////////////////////////////////////////////////////

CTempFileBackup::CTempFileBackup(const CString& sFile, const CString& sFolder, BOOL bTimeStamp, const CString& sExt)
	: CFileBackup(sFile, sFolder, bTimeStamp, sExt)
{
}

CTempFileBackup::~CTempFileBackup()
{
	if (FileMisc::FileExists(m_sBackup))
		::DeleteFile(m_sBackup);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CFileBackup::CFileBackup(const CString& sFile, const CString& sFolder, BOOL bTimeStamp, const CString& sExt)
{
	MakeBackup(sFile, sFolder, bTimeStamp, sExt);
}

CFileBackup::~CFileBackup()
{
}

BOOL CFileBackup::MakeBackup(const CString& sFile, const CString& sFolder, BOOL bTimeStamp, const CString& sExt)
{
	ASSERT (m_sFile.IsEmpty() && m_sBackup.IsEmpty());

	if (!m_sFile.IsEmpty() || !m_sBackup.IsEmpty())
		return FALSE;

	if (!FileMisc::FileExists(sFile))
		return FALSE;

	m_sFile = sFile;
	m_sBackup = BuildBackupPath(sFile, sFolder, bTimeStamp, sExt);
	FileMisc::CreateFolderFromFilePath(m_sBackup);

	BOOL bRes = ::CopyFile(m_sFile, m_sBackup, FALSE);

	if (!bRes)
	{
		TRACE(FileMisc::FormatGetLastError() + '\n');
	}
	else
		ASSERT (FileMisc::FileExists(m_sBackup));

	return bRes;
}

BOOL CFileBackup::RestoreBackup()
{
	ASSERT (!m_sFile.IsEmpty() && !m_sBackup.IsEmpty());

	if (m_sFile.IsEmpty() || m_sBackup.IsEmpty())
		return FALSE;

	return ::CopyFile(m_sBackup, m_sFile, FALSE);
}

CString CFileBackup::BuildBackupPath(const CString& sFile, const CString& sFolder, BOOL bTimeStamp, const CString& sExt)
{
	CString sBackup(sFile);
	sBackup.TrimRight();

	// handle folder
	if (!sFolder.IsEmpty())
	{
		CString sDrive, sPath, sFName, sExt;
		FileMisc::SplitPath(sFile, &sDrive, &sPath, &sFName, &sExt);

		if (::PathIsRelative(sFolder))
		{
			sPath += sFolder;
			FileMisc::MakePath(sBackup, sDrive, sPath, sFName, sExt);
		}
		else
			FileMisc::MakePath(sBackup, NULL, sFolder, sFName, sExt);
	}

	// add timestamp before existing file extension
	CString sFExt;
	FileMisc::SplitPath(sBackup, NULL, NULL, NULL, &sFExt);

	if (bTimeStamp)
	{
		// use ISO date and 24 hour time so that backups can be sorted 
		// by name in date order
		CString sDate = COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d_%H-%M-%S"));
		sFExt = _T(".") + sDate + sFExt;
	}

	// add extension before existing file extension
	if (sExt.IsEmpty())
	{
		// only add a default extension if not copying to another folder or not adding timestamp
		// else we'd overwrite the existing file which wouldn't achieve much
		if (sFolder.IsEmpty() && !bTimeStamp)
			sFExt = _T(".bak") + sFExt;
	}
	else
	{
		if (sExt.Find('.') == -1)
			sFExt = '.' + sExt + sFExt;
		else
			sFExt = sExt + sFExt;
	}

	FileMisc::ReplaceExtension(sBackup, sFExt);

	return sBackup;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CString FileMisc::FormatGetLastError(DWORD dwLastErr)
{
	if (dwLastErr == -1)
		dwLastErr = GetLastError();

	LPTSTR lpMessage;
	DWORD dwErrCode = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,               // no source buffer needed
		dwErrCode,          // error code for this message
		NULL,               // default language ID
		(LPTSTR)&lpMessage, // allocated by fcn
		NULL,               // minimum size of buffer
		NULL);              // no inserts


	CString sError(lpMessage);
	LocalFree(lpMessage);

	return sError;
}

void FileMisc::TerminatePath(CString& sPath)
{
	sPath.TrimRight();

	if (sPath.ReverseFind('\\') != (sPath.GetLength() - 1))
		sPath += '\\';
}

void FileMisc::UnterminatePath(CString& sPath)
{
	sPath.TrimRight();

	int len = sPath.GetLength();

	if (sPath.ReverseFind('\\') == (len - 1))
		sPath = sPath.Left(len - 1);
}

void FileMisc::ReplaceExtension(CString& sFilePath, const TCHAR* szExt)
{
	CString sDrive, sDir, sFile;

	SplitPath(sFilePath, &sDrive, &sDir, &sFile);
	MakePath(sFilePath, sDrive, sDir, sFile, szExt);
}

CString& FileMisc::ValidateFilepath(CString& sFilepath, LPCTSTR szReplace)
{
	sFilepath.TrimLeft();
	sFilepath.TrimRight();

	sFilepath.Replace(_T("/"), szReplace);
	sFilepath.Replace(_T("*"), szReplace);
	sFilepath.Replace(_T("?"), szReplace);
	sFilepath.Replace(_T("\""), szReplace);
	sFilepath.Replace(_T("<"), szReplace);
	sFilepath.Replace(_T(">"), szReplace);
	sFilepath.Replace(_T("|"), szReplace);

	// make sure if a colon exists it is the 2nd pos
	int nColon = sFilepath.ReverseFind(':');

	while (nColon != -1 && nColon != 1)
	{
		// replace the colon
		sFilepath = sFilepath.Left(nColon) + szReplace + sFilepath.Mid(nColon + 1);
		nColon = sFilepath.ReverseFind(':');
	}

	return sFilepath;
}

CString& FileMisc::ValidateFilename(CString& sFilename, LPCTSTR szReplace)
{
	sFilename.Replace(_T("\\"), szReplace);
	sFilename.Replace(_T(":"), szReplace);
	
	return ValidateFilepath(sFilename, szReplace);
}

CString FileMisc::GetFileNameFromPath(const TCHAR* szFilepath, BOOL bIncExtension)
{
	CString sFName, sExt;
	SplitPath(szFilepath, NULL, NULL, &sFName, &sExt);

	if (bIncExtension)
		sFName += sExt;
	
	return sFName;
}

time_t FileMisc::GetLastModified(const TCHAR* szPath)
{
	struct _stat st;

	if (!szPath || _tstat(szPath, &st) != 0)
		return 0;

	// files only
	if ((st.st_mode & _S_IFDIR) == _S_IFDIR)
		return 0;

	return st.st_mtime;
}

bool FileMisc::GetLastModified(const TCHAR* szPath, SYSTEMTIME& sysTime, bool bLocalTime)
{
	ZeroMemory(&sysTime, sizeof(SYSTEMTIME));

	DWORD dwAttr = ::GetFileAttributes(szPath);

	// files only
	if (dwAttr == 0xFFFFFFFF)
		return false;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)szPath, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	FindClose(hFind);

	FILETIME ft = findFileData.ftLastWriteTime;

	if (bLocalTime)
		FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &ft);

	FileTimeToSystemTime(&ft, &sysTime);
	return true;
}

bool FileMisc::ResetLastModified(const TCHAR* szPath)
{
	::SetFileAttributes(szPath, FILE_ATTRIBUTE_NORMAL);

	return (_tutime(szPath, NULL) == 0);
}

int FileMisc::FindFiles(const CString& sFolder, CStringArray& aFiles, LPCTSTR szPattern)
{
	CFileFind ff;
	CString sSearchSpec;

	MakePath(sSearchSpec, NULL, sFolder, szPattern, NULL);

	BOOL bContinue = ff.FindFile(sSearchSpec);
	
	while (bContinue)
	{
		bContinue = ff.FindNextFile();
		
		if (!ff.IsDots())
		{
			if (ff.IsDirectory())
				FindFiles(ff.GetFilePath(), aFiles, szPattern);
			else
				aFiles.Add(ff.GetFilePath());
		}
	}

	return aFiles.GetSize();
}

bool FileMisc::DeleteFolderContents(const TCHAR* szFolder, BOOL bIncludeSubFolders, const TCHAR* szFileMask,
	HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return true;

	// if a file mask has been specified with subfolders we need to do 2 passes on each folder, 
	// one for the files and one for the sub folders
	int nPasses = (bIncludeSubFolders && (szFileMask && lstrlen(szFileMask))) ? 2 : 1;
		
	bool bResult = true;
	bool bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);

	for (int nPass = 0; !bStopped && nPass < nPasses; nPass++)
	{
		CString sSearchSpec(szFolder), sMask(szFileMask);

		if (sMask.IsEmpty() || nPass == 1) // (nPass == 1) == 2nd pass (for folders)
			sMask = "*.*";

		TerminatePath(sSearchSpec);
		sSearchSpec += sMask;

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = NULL;

		if ((hSearch = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (bProcessMsgLoop)
					Misc::ProcessMsgLoop();

				if (finfo.cFileName[0] != '.') 
				{
					CString sItem(szFolder);
					sItem += "\\";
					sItem += finfo.cFileName;

					if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bIncludeSubFolders && (nPass == 1 || nPasses == 1))
						{
							if (DeleteFolderContents(sItem, TRUE, szFileMask, hTerminate, bProcessMsgLoop))
							{
								if (!szFileMask || !lstrlen(szFileMask))
									bResult = (RemoveDirectory(sItem) == TRUE);
							}
						}
					}
					else 
						bResult = (DeleteFile(sItem) == TRUE);
				}

				bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
			} 
			while (!bStopped && bResult && FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
		}
	}

	return (!bStopped && bResult);
}

bool FileMisc::RemoveFolder(const TCHAR* szFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return true;

	if (DeleteFolderContents(szFolder, TRUE, NULL, hTerminate, bProcessMsgLoop))
	{
		::SetFileAttributes(szFolder, FILE_ATTRIBUTE_NORMAL);
		return (RemoveDirectory(szFolder) == TRUE);
	}

	return false;
}

double FileMisc::GetFolderSize(const TCHAR* szFolder, BOOL bIncludeSubFolders, const TCHAR* szFileMask, HANDLE hTerminate,
	BOOL bProcessMsgLoop)
{
	// if the dir does not exists just return
	if (!FolderExists(szFolder))
		return 0;
	
	double dSize = 0;

	WIN32_FIND_DATA finfo;
	CString sSearchSpec(szFolder), sFileMask(szFileMask);

	if (sFileMask.IsEmpty())
		sFileMask = "*.*";

	TerminatePath(sSearchSpec);
	sSearchSpec += sFileMask;

	BOOL bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
	HANDLE h = NULL;
		
	if (!bStopped && (h = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if (bProcessMsgLoop)
				Misc::ProcessMsgLoop();

			if (finfo.cFileName[0] != '.') 
			{
				if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bIncludeSubFolders)
					{
						CString sSubFolder(szFolder);
						sSubFolder += "\\";
						sSubFolder += finfo.cFileName;
						
						dSize += GetFolderSize(sSubFolder, TRUE, sFileMask, hTerminate, bProcessMsgLoop);
					}
				}
				else 
					dSize += (finfo.nFileSizeHigh * ((double)MAXDWORD + 1)) + finfo.nFileSizeLow;
			}

			bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
		}
		while (!bStopped && FindNextFile(h, &finfo));
		
		FindClose(h);
	} 

	return bStopped ? -1 : dSize;
}

bool FileMisc::FolderExists(const TCHAR* szFolder)
{
	// special case
	if (!szFolder || !*szFolder) // cwd
		return TRUE;

	DWORD dwAttrib = GetFileAttributes(szFolder);

	return (dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileMisc::FileExists(const TCHAR* szFile)
{
	DWORD dwAttrib = GetFileAttributes(szFile);

	return (dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileMisc::FolderFromFilePathExists(const TCHAR* szFilePath)
{
	return FolderExists(GetFolderFromFilePath(szFilePath));
}

CString FileMisc::GetCwd()
{
	TCHAR szCwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCwd);

	return szCwd;
}

CString FileMisc::GetModuleFolder(HMODULE hMod) 
{ 
	return GetFolderFromFilePath(GetModuleFileName(hMod)); 
}

CString FileMisc::GetFolderFromFilePath(const TCHAR* szFilePath)
{
	CString sFolder;

	// check if its a folder already
	if (FolderExists(szFilePath))
	{
		sFolder = szFilePath;
	}
	else
	{
		// remove file ending
		CString sDrive, sDir;

		SplitPath(szFilePath, &sDrive, &sDir);
		MakePath(sFolder, sDrive, sDir);
	}

	return sFolder;
}

bool FileMisc::CreateFolderFromFilePath(const TCHAR* szFilePath)
{
	return CreateFolder(GetFolderFromFilePath(szFilePath));
}

bool FileMisc::PathHasWildcard(const TCHAR* szFilePath)
{
	return (_tcschr(szFilePath, _T('?')) || _tcschr(szFilePath, _T('*')));
}

bool FileMisc::CreateFolder(const TCHAR* szFolder)
{
	if (FolderExists(szFolder))
		return true;

	// start from the highest level folder working to the lowest
	CString sFolder, sRemaining(szFolder);
	UnterminatePath(sRemaining);

	bool bDone = false;
	bool bResult = true;

	// pull off the :\ or \\ start
	int nFind = sRemaining.Find(_T(":\\"));

	if (nFind != -1)
	{
		sFolder += sRemaining.Left(nFind + 2);
		sRemaining = sRemaining.Mid(nFind + 2);
	}
	else
	{
		nFind = sRemaining.Find(_T("\\\\"));
		
		if (nFind != -1)
		{
			sFolder += sRemaining.Left(nFind + 2);
			sRemaining = sRemaining.Mid(nFind + 2);
		}
	}

	while (!bDone && bResult)
	{
		nFind = sRemaining.Find(_T('\\'), 1);

		if (nFind == -1)
		{
			bDone = TRUE;
			sFolder += sRemaining;
		}
		else
		{
			sFolder += sRemaining.Left(nFind);
			sRemaining = sRemaining.Mid(nFind);
		}

		if (GetFileAttributes(sFolder) == 0xffffffff && CreateDirectory(sFolder, NULL) != 0)
			bResult = false;
	}

	return bResult;
}

bool FileMisc::MoveFolder(const TCHAR* szSrcFolder, const TCHAR* szDestFolder, BOOL bIncludeSubFolders,
	const TCHAR* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	if (CopyFolder(szSrcFolder, szDestFolder, bIncludeSubFolders, szFileMask, hTerminate, bProcessMsgLoop))
	{
		// don't pass on hTerminate to ensure the operation completes
		DeleteFolderContents(szSrcFolder, bIncludeSubFolders, szFileMask, NULL, bProcessMsgLoop);

		return true;
	}

	return false;
}

bool FileMisc::CopyFolder(const TCHAR* szSrcFolder, const TCHAR* szDestFolder, BOOL bIncludeSubFolders,
	const TCHAR* szFileMask, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	if (!CreateFolder(szDestFolder))
		return false;

	if (!FolderExists(szSrcFolder))
		return false;

	// if a file mask has been specified with subfolders we need to do 2 passes on each folder, 
	// one for the files and one for the sub folders
	int nPasses = (bIncludeSubFolders && (szFileMask && lstrlen(szFileMask))) ? 2 : 1;
		
	bool bResult = true;
	bool bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);

	for (int nPass = 0; !bStopped && nPass < nPasses; nPass++)
	{
		CString sSearchSpec(szSrcFolder), sMask(szFileMask);

		if (sMask.IsEmpty() || nPass == 1) // (nPass == 1) == 2nd pass (for folders)
			sMask = "*.*";

		TerminatePath(sSearchSpec);
		sSearchSpec += sMask;

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = NULL;

		if ((hSearch = FindFirstFile(sSearchSpec, &finfo)) != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (bProcessMsgLoop)
					Misc::ProcessMsgLoop();

				if (finfo.cFileName[0] != '.') 
				{
					CString sSource(szSrcFolder);
					sSource += "\\";
					sSource += finfo.cFileName;
					
					CString sDest(szDestFolder);
					sDest += "\\";
					sDest += finfo.cFileName;
					
					if (finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if ((nPass == 1 || nPasses == 1) && bIncludeSubFolders)
							bResult = CopyFolder(sSource, sDest, hTerminate);
					}
					else if (nPass == 0) // files 
					{
						bResult = (TRUE == CopyFile(sSource, sDest, FALSE));
					}
				}

				bStopped = (WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0);
			}
			while (!bStopped && bResult && FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
		} 
	}

	return (!bStopped && bResult);
}

bool FileMisc::MoveFolder(const TCHAR* szSrcFolder, const TCHAR* szDestFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	return MoveFolder(szSrcFolder, szDestFolder, TRUE, NULL, hTerminate, bProcessMsgLoop);
}

bool FileMisc::CopyFolder(const TCHAR* szSrcFolder, const TCHAR* szDestFolder, HANDLE hTerminate, BOOL bProcessMsgLoop)
{
	return CopyFolder(szSrcFolder, szDestFolder, TRUE, NULL, hTerminate, bProcessMsgLoop);
}

double FileMisc::GetFileSize(const TCHAR* szPath)
{
	HANDLE hFile = ::CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwHighSize = 0;
		DWORD dwLowSize = ::GetFileSize(hFile, &dwHighSize);
		
		::CloseHandle(hFile);
		
		if (dwLowSize != INVALID_FILE_SIZE)
		{
			return (dwHighSize * ((double)MAXDWORD + 1) + dwLowSize);
		}
	}

	// else
	return 0;
}

bool FileMisc::AppendLineToFile(LPCTSTR szPathname, LPCTSTR szLine)
{
	// make sure parent folder exists
	if (!CreateFolderFromFilePath(szPathname))
		return false;

	CStdioFile file;

	if (file.Open(szPathname, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		file.SeekToEnd();
		file.WriteString(szLine);

		if (!_tcsstr(szLine, _T("\n")))
			file.WriteString(_T("\n"));

		return true;
	}

	return false;
}

bool FileMisc::SaveFile(const TCHAR* szPathname, const TCHAR* szText, int nLen)
{
	if (nLen == -1)
		nLen = _tcslen(szText);

	CStdioFile file;

	if (file.Open(szPathname, CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(szText, nLen);
		return true;
	}

	return false;
}

bool FileMisc::LoadFile(const TCHAR* szPathname, CString& sText)
{
	CStdioFile file;

	if (file.Open(szPathname, CFile::modeRead))
	{
		if (file.GetLength())
		{
			// init size
			CString sTemp(_T('\0'), (int)file.GetLength());
			sText = sTemp;

			CString sLine;
			int nPos = 0;

			while (file.ReadString(sLine))
			{
				sText.Insert(nPos, sLine);
				nPos += sLine.GetLength();
			}
		}

		return true;
	}

	return false;
}

BOOL FileMisc::IsTempFile(LPCTSTR szFilename)
{
	CString sFilename(szFilename);
	sFilename.MakeLower();

	CString sTempFolder = GetTempFolder();
	sTempFolder.MakeLower();

	return (sFilename.Find(sTempFolder) == 0);
}

CString FileMisc::GetTempFolder()
{
	TCHAR szTempPath[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempPath))
		return CString(szTempPath);

	// else
	return _T("C:\\Temp");
}

CString FileMisc::GetTempFileName(LPCTSTR szPrefix, UINT uUnique)
{
	TCHAR szTempFile[MAX_PATH], szTempPath[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempPath))
	{
		if (::GetTempFileName(szTempPath, szPrefix, uUnique, szTempFile))
			return szTempFile;
	}

	return _T("");
}

CString FileMisc::GetTempFileName(LPCTSTR szFilename, LPCTSTR szExt)
{
	CString sTempFile;
	TCHAR szTempPath[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempPath))
		MakePath(sTempFile, NULL, szTempPath, szFilename, szExt);

	return sTempFile;
}

DWORD FileMisc::Run(HWND hwnd, LPCTSTR lpFile, LPCTSTR lpDirectory, int nShowCmd)
{
	CString sFile(lpFile), sParams;
	int nHash = sFile.Find(_T('#'));

	if (nHash != -1)
	{
		sParams = sFile.Mid(nHash);
		sFile = sFile.Left(nHash);

		CString sExt;
		SplitPath(sFile, NULL, NULL, NULL, &sExt);

		CString sApp = CFileRegister::GetRegisteredAppPath(sExt);

		if (!sApp.IsEmpty())
		{
			sFile = sApp;
			sParams = lpFile;
		}
		else
		{
			sFile = lpFile;
			sParams.Empty();
		}
	}

	DWORD dwRes = (DWORD)ShellExecute(hwnd, NULL, sFile, sParams, lpDirectory, nShowCmd);

	if (dwRes <= 32) // failure
	{
		// try CreateProcess
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.wShowWindow = (WORD)nShowCmd;
		si.dwFlags = STARTF_USESHOWWINDOW;

		ZeroMemory( &pi, sizeof(pi) );

		// Start the child process.
		if (CreateProcess( NULL,			// No module name (use command line).
							(TCHAR*)lpFile,	// Command line.
							NULL,			// Process handle not inheritable.
							NULL,			// Thread handle not inheritable.
							FALSE,			// Set handle inheritance to FALSE.
							0,				// No creation flags.
							NULL,			// Use parent's environment block.
							lpDirectory,	// starting directory.
							&si,			// Pointer to STARTUPINFO structure.
							&pi ))			// Pointer to PROCESS_INFORMATION structure.
		{
			dwRes = 32; // success
		}

		// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
	return dwRes;
}

bool FileMisc::ExtractResource(UINT nID, LPCTSTR szType, const CString& sTempFilePath, HINSTANCE hInst)
{
	// compare time with that of module from which it was loaded
	CString sTempPath;
	CFileStatus fsRes, fsModule;
	CString sModulePath = GetModuleFileName(hInst);

	if (!CFile::GetStatus(sModulePath, fsModule))
		return FALSE;
	
	// see if the file has been created before
	if (!CFile::GetStatus(sTempFilePath, fsRes) || fsRes.m_mtime < fsModule.m_mtime)
	{
		// Load the resource into memory
		HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(nID), szType);
		
		if (!hRes) 
		{
			TRACE(_T("Couldn't find %s resource %d!\n"), szType, nID);
			return FALSE;
		}
		
		DWORD len = SizeofResource(hInst, hRes);
		
		BYTE* lpRes = (BYTE*)LoadResource(hInst, hRes);
		ASSERT(lpRes);
		
		CFile file;
		
		if (file.Open(sTempFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			file.Write(lpRes, len);
			file.Close();
			FreeResource((HANDLE)lpRes);
		}
		else
		{
			FreeResource((HANDLE)lpRes);
			return FALSE;
		}
	}
	
	return TRUE;
}

CString FileMisc::GetModuleFileName(HMODULE hMod)
{
	CString sModulePath;

	::GetModuleFileName(hMod, sModulePath.GetBuffer(MAX_PATH + 1), MAX_PATH);
	sModulePath.ReleaseBuffer();

	return sModulePath;
}

bool FileMisc::ExtractResource(LPCTSTR szModulePath, UINT nID, LPCTSTR szType, const CString& sTempFilePath)
{
	HMODULE hModule = LoadLibrary(szModulePath);

	if (!hModule)
		return false;

	// else
	return ExtractResource(nID, szType, sTempFilePath, hModule);
}

void FileMisc::SplitPath(const TCHAR* szPath, CString* pDrive, CString* pDir, CString* pFName, CString* pExt)
{
	TCHAR szDrive[_MAX_DRIVE], szFolder[_MAX_DIR], szFile[_MAX_FNAME], szExt[_MAX_EXT];

#if _MSC_VER >= 1400
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szFolder,_MAX_DIR, szFile, _MAX_FNAME, szExt, _MAX_EXT);
#else
	_tsplitpath(szPath, szDrive, szFolder, szFile, szExt);
#endif

	if (pDrive)
		*pDrive = szDrive;

	if (pDir)
		*pDir = szFolder;

	if (pFName)
		*pFName = szFile;

	if (pExt)
		*pExt = szExt;
}

CString& FileMisc::MakePath(CString& sPath, const TCHAR* szDrive, const TCHAR* szDir, const TCHAR* szFName,
	const TCHAR* szExt)
{
	TCHAR szPath[MAX_PATH];

#if _MSC_VER >= 1400
	_tmakepath_s(szPath, MAX_PATH, szDrive, szDir, szFName, szExt);
#else
	_tmakepath(szPath, szDrive, szDir, szFName, szExt);
#endif

	sPath = szPath;

	return sPath;
}

CString FileMisc::GetFullPath(const CString& sFilePath)
{
	TCHAR szFullPath[MAX_PATH+1];

	_tfullpath(szFullPath, sFilePath, MAX_PATH);

	return szFullPath;
}

BOOL FileMisc::IsSameFile(const CString& sFilePath1, const CString& sFilePath2)
{
	CString sFullPath1 = GetFullPath(sFilePath1);
	CString sFullPath2 = GetFullPath(sFilePath2);

	return (sFilePath1.CompareNoCase(sFullPath2) == 0);
}
