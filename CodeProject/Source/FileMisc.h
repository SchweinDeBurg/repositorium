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

#ifndef _MISCFILE_FUNCTIONS_H_
#define _MISCFILE_FUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>

class CFileBackup
{
public:
	CFileBackup(const CString& sFile = _T(""), 
				const CString& sFolder = _T(""), 
				BOOL bTimeStamp = FALSE, 
				const CString& sExt = _T(".bak"));
	~CFileBackup();

	BOOL MakeBackup(const CString& sFile, 
					const CString& sFolder = _T(""), 
					BOOL bTimeStamp = FALSE, 
					const CString& sExt = _T(".bak"));
	BOOL RestoreBackup();

	static CString BuildBackupPath(const CString& sFile,
					const CString& sFolder = _T(""),
					BOOL bTimeStamp = FALSE,
					const CString& sExt = _T(".bak"));

protected:
	CString m_sFile, m_sBackup;
};

class CTempFileBackup : public CFileBackup
{
public:
	CTempFileBackup(const CString& sFile = _T(""), 
					const CString& sFolder = _T(""), 
					BOOL bTimeStamp = FALSE, 
					const CString& sExt = _T(".bak"));	
	~CTempFileBackup(); // auto deletes backup file

};

namespace FileMisc
{
	CString FormatGetLastError(DWORD dwLastErr = -1);

	void TerminatePath(CString& sPath);
	void UnterminatePath(CString& sPath);

	time_t GetLastModified(const TCHAR* szPath);
	bool GetLastModified(const TCHAR* szPath, SYSTEMTIME& sysTime, bool bLocalTime = true); // files only
	bool ResetLastModified(const TCHAR* szPath); // resets to current time
	double GetFileSize(const TCHAR* szPath);

	bool RemoveFolder(const TCHAR* szFolder, 
					  HANDLE hTerminate = NULL, 
					  BOOL bProcessMsgLoop = TRUE);

	bool DeleteFolderContents(const TCHAR* szFolder, 
							  BOOL bIncludeSubFolders, 
							  const TCHAR* szFileMask, 
							  HANDLE hTerminate = NULL, 
							  BOOL bProcessMsgLoop = TRUE);

	double GetFolderSize(const TCHAR* szFolder, 
						 BOOL bIncludeSubFolders = TRUE, 
						 const TCHAR* szFileMask = NULL, 
						 HANDLE hTerminate = NULL, 
						 BOOL bProcessMsgLoop = TRUE);

	bool CreateFolder(const TCHAR* szFolder);
	bool CreateFolderFromFilePath(const TCHAR* szFilePath);
	bool FolderExists(const TCHAR* szFolder);
	bool FileExists(const TCHAR* szFile);
	bool FolderFromFilePathExists(const TCHAR* szFilePath);
	bool PathHasWildcard(const TCHAR* szFilePath);

	CString GetCwd();

	int FindFiles(const CString& sFolder, CStringArray& aFiles, LPCTSTR szPattern = _T("*.*"));

	CString& ValidateFilename(CString& sFilename, LPCTSTR szReplace = _T(""));
	CString& ValidateFilepath(CString& sFilepath, LPCTSTR szReplace = _T(""));
	void ReplaceExtension(CString& szFilePath, const TCHAR* szExt);
	
	CString GetTempFolder();
	CString GetTempFileName(LPCTSTR szPrefix, UINT uUnique = 0);
	CString GetTempFileName(LPCTSTR szFilename, LPCTSTR szExt);
	BOOL IsTempFile(LPCTSTR szFilename);

	bool SaveFile(const TCHAR* szPathname, const TCHAR* szText, int nLen = -1);
	bool LoadFile(const TCHAR* szPathname, CString& sText);

	bool ExtractResource(UINT nID, LPCTSTR szType, const CString& sTempFilePath, HINSTANCE hInst = NULL);
	bool ExtractResource(LPCTSTR szModulePath, UINT nID, LPCTSTR szType, const CString& sTempFilePath);

	CString GetModuleFileName(HMODULE hMod = NULL);
	CString GetModuleFolder(HMODULE hMod = NULL);

	void SplitPath(const TCHAR* szPath, CString* pDrive, CString* pDir = NULL, CString* pFName = NULL,
		CString* pExt = NULL);
	CString& MakePath(CString& sPath, const TCHAR* szDrive, const TCHAR* szDir = NULL, const TCHAR* szFName = NULL,
		const TCHAR* szExt = NULL);
	
	CString GetFullPath(const CString& sFilePath);
	BOOL IsSameFile(const CString& sFilePath1, const CString& sFilePath2);

	CString GetFolderFromFilePath(const TCHAR* szFilePath);
	CString GetFileNameFromPath(const TCHAR* szFilepath, BOOL bIncExtension = TRUE);

	// will delete the source folder on success
	bool MoveFolder(const TCHAR* szSrcFolder, 
					const TCHAR* szDestFolder, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	bool CopyFolder(const TCHAR* szSrcFolder, 
					const TCHAR* szDestFolder, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	// will delete the source folder only if file mask was "*.*"
	bool MoveFolder(const TCHAR* szSrcFolder, 
					const TCHAR* szDestFolder, 
					BOOL bIncludeSubFolders, 
					const TCHAR* szFileMask, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	bool CopyFolder(const TCHAR* szSrcFolder, 
					const TCHAR* szDestFolder, 
					BOOL bIncludeSubFolders, 
					const TCHAR* szFileMask, 
					HANDLE hTerminate = NULL, 
					BOOL bProcessMsgLoop = TRUE);

	// append a line of text to a text file
	bool AppendLineToFile(LPCTSTR szPathname, LPCTSTR szLine);

	DWORD Run(HWND hwnd, LPCTSTR lpFile, LPCTSTR lpDirectory = NULL, int nShowCmd = SW_SHOW);

}

#endif // _MISCFILE_FUNCTIONS_H_
