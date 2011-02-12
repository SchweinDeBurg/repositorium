// Wizard97.h : main header file for the WIZARD97 application
//

#if !defined(AFX_WIZARD97_H__9D85AAA5_59C4_11D1_A4EC_00C04FD91A9F__INCLUDED_)
#define AFX_WIZARD97_H__9D85AAA5_59C4_11D1_A4EC_00C04FD91A9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWizard97App:
// See Wizard97.cpp for the implementation of this class
//

class CWizard97App : public CWinApp
{
public:
	CWizard97App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizard97App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWizard97App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARD97_H__9D85AAA5_59C4_11D1_A4EC_00C04FD91A9F__INCLUDED_)
