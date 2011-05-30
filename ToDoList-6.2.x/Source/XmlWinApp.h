// XmlWinApp.h  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
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

#ifndef XMLWINAPP_H
#define XMLWINAPP_H

#include "XMLProfile.h"

//=============================================================================
class CXmlWinApp : public CWinApp
//=============================================================================
{
	DECLARE_DYNAMIC(CXmlWinApp)

public:
// Constructor
	CXmlWinApp(LPCTSTR lpszAppName = NULL);
	virtual ~CXmlWinApp();

// Attributes
public:
	CString		m_strXmlProfileName;
	CXmlProfile	m_XmlProfile;

// Functions to emulate CWinApp APIs
public:
	BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
				LPBYTE* ppData, UINT* pBytes);
	BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
				LPBYTE pData, UINT nBytes);
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
				LPCTSTR lpszDefault = NULL);
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
				LPCTSTR lpszValue);

protected:
	//{{AFX_MSG(CXmlWinApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //XMLWINAPP_H
