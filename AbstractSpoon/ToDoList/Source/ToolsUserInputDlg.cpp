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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// ToolsUserInputDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToolsUserInputDlg.h"

#include "../../Common/FileEdit.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolsUserInputDlg dialog

const UINT LABEL_X = 7;
const UINT ITEM_X = 70;
const UINT LABEL_Y = 7; // just the start pos
const UINT SPACING = 6; 
const UINT BTN_CX = 50;
const UINT BTN_CY = 14;

CToolsUserInputDlg::CToolsUserInputDlg(const CToolsCmdlineParser& tcp) : CRuntimeDlg()
{
	//{{AFX_DATA_INIT(CToolsUserInputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// process the user input items and save them off
	CCLArgArray aArgs;
	int nArgCount = tcp.GetArguments(aArgs);
	UINT nCtrlID = 1;
	int nMaxWidth = 0;
	
	CPreferences prefs;

	if (nArgCount)
	{
		for (int nArg = 0; nArg < nArgCount; nArg++)
		{
			TUINPUTITEM tuii; // we fill in specifics as we go along

			tuii.pCtrl = NULL;
			tuii.sName = aArgs[nArg].sName;
			tuii.sName.MakeLower();

			CLA_TYPE nType = aArgs[nArg].nType;
	
			switch (nType)
			{
			case CLAT_USERFILE:
				tuii.pCtrl = new CFileEdit;
				tuii.nStyle = ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP;
				tuii.sizeDLU.cx = 130;
				tuii.sizeDLU.cy = 14;
				break;

			case CLAT_USERFOLDER:
				tuii.pCtrl = new CFileEdit(FES_FOLDERS);
				tuii.nStyle = ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP;
				tuii.sizeDLU.cx = 130;
				tuii.sizeDLU.cy = 14;
				break;

			case CLAT_USERTEXT:
				tuii.pCtrl = new CEdit;
				tuii.nStyle = ES_AUTOHSCROLL | ES_LEFT | WS_TABSTOP;
				tuii.sizeDLU.cx = 130;
				tuii.sizeDLU.cy = 14;
				break;

			case CLAT_USERDATE:
				tuii.pCtrl = new CDateTimeCtrl;
				tuii.nStyle = DTS_RIGHTALIGN | WS_TABSTOP;
				tuii.sizeDLU.cx = 70;
				tuii.sizeDLU.cy = 13;
				break;
			}
			
			if (tuii.pCtrl)
			{
				if (!tuii.sName.IsEmpty())
				{
					tuii.nType = aArgs[nArg].nType;
					tuii.nCtrlID = nCtrlID++;
					tuii.sLabel = aArgs[nArg].sLabel;

					tuii.sDefValue = aArgs[nArg].sDefValue;

					// if sDefValue is empty see if we previously saved a value for this control 
					if (tuii.sDefValue.IsEmpty())
						tuii.sDefValue = prefs.GetProfileString(_T("Tools\\UserInput"), tuii.sLabel);

					m_aInputItems.Add(tuii);
				}
				else
					delete tuii.pCtrl;
			}
		}
	}

	// now add the controls to CRuntimeDlg
	int nNumCtrls = m_aInputItems.GetSize();

	if (nNumCtrls)
	{
		int nYPos = LABEL_Y + SPACING;

		for (int nCtrl = 0; nCtrl < nNumCtrls; nCtrl++)
		{
			TUINPUTITEM& tuii = m_aInputItems[nCtrl];
			
			if (AddRCControl(tuii.pCtrl, tuii.sDefValue, tuii.nStyle, WS_EX_CLIENTEDGE, ITEM_X, nYPos, tuii.sizeDLU.cx, tuii.sizeDLU.cy, tuii.nCtrlID))
			{
				// add label
				AddRCControl(_T("LTEXT"), NULL, tuii.sLabel, SS_CENTERIMAGE, 0, 0, nYPos, ITEM_X - LABEL_X - 3, tuii.sizeDLU.cy, 0);

				nYPos += (tuii.sizeDLU.cy + SPACING);

				if (nMaxWidth < (int)(ITEM_X + tuii.sizeDLU.cx))
					nMaxWidth = (ITEM_X + tuii.sizeDLU.cx);
			}
		}

		// add ok and cancel buttons at the bottom right with a preceding line
		nYPos += SPACING;
		AddRCControl(_T("CONTROL"), _T("static"), NULL, SS_ETCHEDFRAME, 0, 0, nYPos, nMaxWidth - LABEL_X, 1, 0);
		nYPos += SPACING * 2;

		AddRCControl(_T("PUSHBUTTON"), NULL, _T("OK"), 0, 0, nMaxWidth - 2 * BTN_CX - SPACING, nYPos, BTN_CX, BTN_CY, IDOK);
		AddRCControl(_T("PUSHBUTTON"), NULL, _T("Cancel"), 0, 0, nMaxWidth - BTN_CX, nYPos, BTN_CX, BTN_CY, IDCANCEL);
	}
}

CToolsUserInputDlg::~CToolsUserInputDlg()
{
	// cleanup dynamically allocated items
	int nCtrl = m_aInputItems.GetSize();

	while (nCtrl--)
		delete m_aInputItems[nCtrl].pCtrl;
}

void CToolsUserInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolsUserInputDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolsUserInputDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CToolsUserInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolsUserInputDlg message handlers

void CToolsUserInputDlg::OnOK()
{
	CRuntimeDlg::OnOK();

	// now we dynamically extract the window text of the items and map to name
	CPreferences prefs;
	int nCtrl = m_aInputItems.GetSize();

	while (nCtrl--)
	{
		TUINPUTITEM& tuii = m_aInputItems[nCtrl];

		CString sResult;
		tuii.pCtrl->GetWindowText(sResult);

		// save to registry
		AfxGetApp()->WriteProfileString(_T("Tools\\UserInput"), tuii.sLabel, sResult);

		switch (tuii.nType)
		{
		case CLAT_USERFOLDER:
			// make sure folders are terminated
			sResult.TrimRight();

			if (sResult.Right(1) != _T("\\"))
				sResult += _T('\\');
			break;

		case CLAT_USERDATE:
			// make sure dates are formatted to ISO standards ie yyyy-mm-dd
			{
				SYSTEMTIME sysTime;

				if (GDT_VALID == tuii.pCtrl->SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime))
				{
					COleDateTime date(sysTime);
					sResult = date.Format(_T("%Y-%m-%d"));
				}
				else
					ASSERT(0);
			}
			break;
		}

		m_mapResults[tuii.sName] = sResult;
	}
}

BOOL CToolsUserInputDlg::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();

	// userdate default values need a bit more work
	int nCtrl = m_aInputItems.GetSize();

	while (nCtrl--)
	{
		TUINPUTITEM& tuii = m_aInputItems[nCtrl];

		switch (tuii.nType)
		{
		case CLAT_USERDATE:
			if (!tuii.sDefValue.IsEmpty())
			{
				// parse the date to ISO standards ie yyyy-mm-dd
				SYSTEMTIME sysTime;
				ZeroMemory(&sysTime, sizeof(sysTime));
//fabio_2005
#if _MSC_VER >= 1400
				int nRes = _stscanf_s(tuii.sDefValue, _T("%d-%d-%d"), &sysTime.wYear, &sysTime.wMonth, &sysTime.wDay);
#else
				int nRes = _stscanf(tuii.sDefValue, _T("%d-%d-%d"), &sysTime.wYear, &sysTime.wMonth, &sysTime.wDay);
#endif

				if (nRes == 3)
					tuii.pCtrl->SendMessage(DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM) &sysTime);
			}
			break;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CToolsUserInputDlg::GetResult(LPCTSTR szItemName)
{
	CString sItem(szItemName);
	sItem.MakeLower();

	CString sResult;
	m_mapResults.Lookup(sItem, sResult);

	return sResult;
}
