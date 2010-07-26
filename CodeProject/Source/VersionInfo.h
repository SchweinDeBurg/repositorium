// VersionInfo.h : header file
//

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#ifndef _VERSIONINFO_H_
#define _VERSIONINFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment (lib,"version.lib")

class CVersionInfo
{
public:
	CVersionInfo()
	{
		/*don't instantiate me*/
	}
	~CVersionInfo()
	{
		/*don't instantiate me*/
	}

	static CString GetVersion(const CString& _strDLLFilename);

private:
	static CString ReformatVersionString(const TCHAR* _pszFileVersion, const TCHAR* _pszProductVersion);
};

#endif//_VERSIONINFO_H_
