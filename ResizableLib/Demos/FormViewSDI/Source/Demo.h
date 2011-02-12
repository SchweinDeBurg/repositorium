// Demo.h : main header file for the DEMO application
//

#if !defined(AFX_DEMO_H__4807337A_856E_4C64_AA1C_4435C6858F39__INCLUDED_)
#define AFX_DEMO_H__4807337A_856E_4C64_AA1C_4435C6858F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDemoApp:
// See Demo.cpp for the implementation of this class
//

class CDemoApp : public CWinApp
{
public:
	CDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDemoApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMO_H__4807337A_856E_4C64_AA1C_4435C6858F39__INCLUDED_)
