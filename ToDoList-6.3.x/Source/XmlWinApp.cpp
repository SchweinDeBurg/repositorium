// XmlWinApp.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XmlWinApp implements functions to emulate the CWinApp profile functions,
//     using the CXmlProfile class.
//
// History
//     Version 1.0 - 2008 July 2
//     - Initial public release
//
// Public APIs:
//           NAME                              DESCRIPTION
//     ---------------------  -------------------------------------------------
//     GetProfileBinary()     Retrieves binary data from an entry within a 
//                            specified section of the profile
//     WriteProfileBinary()   Writes binary data into an entry within a 
//                            specified section of the profile
//     GetProfileInt()        Retrieves an integer value from an entry within a 
//                            specified section of the profile
//     WriteProfileInt()      Writes an integer value into an entry within a 
//                            specified section of the profile
//     GetProfileString()     Retrieves a string from an entry within a 
//                            specified section of the profile
//     WriteProfileString()   Writes a string into an entry within a 
//                            specified section of the profile
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlWinApp.h"
#include "XMLProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)		// disable bogus deprecation warning

IMPLEMENT_DYNAMIC(CXmlWinApp, CWinApp)

//=============================================================================
BEGIN_MESSAGE_MAP(CXmlWinApp, CWinApp)
//=============================================================================
	//{{AFX_MSG_MAP(CXmlWinApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
CXmlWinApp::CXmlWinApp(LPCTSTR lpszAppName /*= NULL*/) : CWinApp(lpszAppName)
//=============================================================================
{
	TRACE(_T("in CXmlWinApp::CXmlWinApp\n"));
	m_strXmlProfileName = _T("");
	TCHAR szPath[_MAX_PATH*2];
	VERIFY(::GetModuleFileName(m_hInstance, szPath, sizeof(szPath)/sizeof(TCHAR)-2));

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_tsplitpath(szPath, drive, dir, fname, ext);

	m_strXmlProfileName = fname;
	m_strXmlProfileName += _T(".xml");
	m_XmlProfile.Load(m_strXmlProfileName);
}

//=============================================================================
CXmlWinApp::~CXmlWinApp()
//=============================================================================
{
	TRACE(_T("in CXmlWinApp::~CXmlWinApp\n"));
	m_XmlProfile.Dump();
	m_XmlProfile.Close(TRUE);
}


//=============================================================================
//=============================================================================
//
// The following functions emulate the MFC CWinApp profile functions.
//
//=============================================================================
//=============================================================================


//=============================================================================
BOOL CXmlWinApp::GetProfileBinary(LPCTSTR lpszSection, 
								  LPCTSTR lpszEntry,
								  LPBYTE *ppData, 
								  UINT *pBytes)
//=============================================================================
{
	return m_XmlProfile.GetProfileBinary(lpszSection, lpszEntry,
				ppData, pBytes);
}

//=============================================================================
BOOL CXmlWinApp::WriteProfileBinary(LPCTSTR lpszSection, 
									LPCTSTR lpszEntry,	
									LPBYTE pData, 
									UINT nBytes)
//=============================================================================
{
	return m_XmlProfile.WriteProfileBinary(lpszSection, lpszEntry,
				pData, nBytes);
}

//=============================================================================
UINT CXmlWinApp::GetProfileInt(LPCTSTR lpszSection, 
							   LPCTSTR lpszEntry, 
							   int nDefault)
//=============================================================================
{
	return m_XmlProfile.GetProfileInt(lpszSection, lpszEntry, nDefault);
}

//=============================================================================
BOOL CXmlWinApp::WriteProfileInt(LPCTSTR lpszSection, 
								 LPCTSTR lpszEntry, 
								 int nValue)
//=============================================================================
{
	return m_XmlProfile.WriteProfileInt(lpszSection, lpszEntry, nValue);
}

//=============================================================================
CString CXmlWinApp::GetProfileString(LPCTSTR lpszSection, 
									 LPCTSTR lpszEntry,
									 LPCTSTR lpszDefault /*= NULL*/)
//=============================================================================
{
	return m_XmlProfile.GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

//=============================================================================
BOOL CXmlWinApp::WriteProfileString(LPCTSTR lpszSection, 
									LPCTSTR lpszEntry,	
									LPCTSTR lpszValue)
//=============================================================================
{
	return m_XmlProfile.WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

