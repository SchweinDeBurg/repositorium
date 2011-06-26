// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - adjusted #include's paths
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// EnBrowserCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "EnBrowserCtrl.h"
//fabio_2005
#if _MSC_VER >= 1300
#	include <comdef.h>
#else
#	include <comutil.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	EBC_NONE,
	EBC_PRINT,
	EBC_PRINTPREVIEW
};

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl

CEnBrowserCtrl::CEnBrowserCtrl():
m_nAction(EBC_NONE)
{
}

CEnBrowserCtrl::~CEnBrowserCtrl()
{
}


BEGIN_MESSAGE_MAP(CEnBrowserCtrl, CWebBrowserCtrl)
	//{{AFX_MSG_MAP(CEnBrowserCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEnBrowserCtrl, CWebBrowserCtrl)
	//{{AFX_EVENTSINK_MAP(CBrowserDlg)
	//}}AFX_EVENTSINK_MAP
	ON_EVENT_REFLECT(CEnBrowserCtrl, 252 /* NavigateComplete2 */, OnNavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnBrowserCtrl message handlers

void CEnBrowserCtrl::Print(LPCTSTR szFile)
{
	if (!szFile || !*szFile)
	{
		SafeExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	else
	{
		COleVariant vFlags(0L), vFrame(_T("")), vData(_T("")), vHeaders(_T(""));

		// navigate to print file
		m_nAction = EBC_PRINT;
		Navigate(szFile, vFlags, vFrame, vData, vHeaders);
	}
}

void CEnBrowserCtrl::PrintPreview(LPCTSTR szFile)
{
	if (!szFile || !*szFile)
	{
		SafeExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	else
	{
		COleVariant vFlags(0L), vFrame(_T("")), vData(_T("")), vHeaders(_T(""));

		// navigate to print file
		m_nAction = EBC_PRINTPREVIEW;
		Navigate(szFile, vFlags, vFrame, vData, vHeaders);
	}
}

void CEnBrowserCtrl::OnNavigateComplete2(LPDISPATCH /*pDisp*/, VARIANT FAR* URL)
{
	CString sURL;
	_bstr_t sTemp;

	sTemp = *URL;
	sURL = CString(LPCTSTR(sTemp));

	switch (m_nAction)
	{
	case EBC_PRINT:
		SafeExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		break;

	case EBC_PRINTPREVIEW:
		SafeExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		break;
	}

	m_nAction = EBC_NONE;
}

BOOL CEnBrowserCtrl::SafeExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut)
{
	try
	{
		ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
	}
	catch (CException* /*e*/)
	{
		return FALSE;
	}

	return TRUE;
}
