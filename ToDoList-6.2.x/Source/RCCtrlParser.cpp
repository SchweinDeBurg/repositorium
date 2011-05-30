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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// RCCtrlParser.cpp: implementation of the CRCCtrlParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RCCtrlParser.h"
#include "WinStyles.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

enum RCTYPE
{
	AUTO3STATE,
	AUTOCHECKBOX,
	AUTORADIOBUTTON,
	CHECKBOX,
	COMBOBOX,
	CONTROL,
	CTEXT,
	DEFPUSHBUTTON,
	EDITTEXT,
	GROUPBOX,
	ICON,
	LISTBOX,
	LTEXT,
	PUSHBOX,
	PUSHBUTTON,
	RADIOBUTTON,
	RTEXT,
	SCROLLBAR,
	STATE3,
};

struct RCCTRLTYPE
{
	LPCTSTR szType;
	RCTYPE nType;
	LPCTSTR szClass;
	DWORD dwDefStyle;
};

#define MAKERCTYPE(type) _T(#type), (RCTYPE)type

static RCCTRLTYPE RCCTRLTYPES[] =
{
	{ MAKERCTYPE(AUTO3STATE), _T("button"), BS_AUTO3STATE | WS_TABSTOP },
	{ MAKERCTYPE(AUTOCHECKBOX), _T("button"), BS_AUTOCHECKBOX | WS_TABSTOP },
	{ MAKERCTYPE(AUTORADIOBUTTON), _T("button"), BS_AUTORADIOBUTTON | WS_TABSTOP },
	{ MAKERCTYPE(CHECKBOX), _T("button"), BS_CHECKBOX | WS_TABSTOP },
	{ MAKERCTYPE(COMBOBOX), _T("combobox"), CBS_SIMPLE | WS_TABSTOP },
	{ MAKERCTYPE(CONTROL), NULL, 0 },
	{ MAKERCTYPE(CTEXT), _T("static"), SS_CENTER | WS_GROUP },
	{ MAKERCTYPE(DEFPUSHBUTTON), _T("button"), BS_DEFPUSHBUTTON | WS_TABSTOP },
	{ MAKERCTYPE(EDITTEXT), _T("edit"), ES_LEFT | WS_BORDER | WS_TABSTOP },
	{ MAKERCTYPE(GROUPBOX), _T("button"), BS_GROUPBOX },
	{ MAKERCTYPE(ICON), _T("static"), SS_ICON },
	{ MAKERCTYPE(LISTBOX), _T("listbox"), LBS_NOTIFY | WS_BORDER },
	{ MAKERCTYPE(LTEXT), _T("static"), SS_LEFT | WS_GROUP },
	{ MAKERCTYPE(PUSHBOX), _T("button"), /*BS_PUSHBOX | */WS_TABSTOP },
	{ MAKERCTYPE(PUSHBUTTON), _T("button"), BS_PUSHBUTTON | WS_TABSTOP },
	{ MAKERCTYPE(RADIOBUTTON), _T("button"), BS_RADIOBUTTON | WS_TABSTOP },
	{ MAKERCTYPE(RTEXT), _T("static"), SS_RIGHT | WS_GROUP },
	{ MAKERCTYPE(SCROLLBAR), _T("scrollbar"), SBS_HORZ },
	{ MAKERCTYPE(STATE3), _T("button"), BS_3STATE | WS_TABSTOP }
};

const int NUMRCTYPES = sizeof(RCCTRLTYPES) / sizeof(RCCTRLTYPE);

/////////////////////////////////////////////////////////////////////////////

// list of classes requiring WS_EX_CLIENTEDGE by default

static LPCTSTR szCtrlsWantingClientEdge[] =
{
	_T("Edit"),
	_T("ComboBox"),
	_T("ComboLBox"),
	_T("ListBox"),
	_T("msctls_hotkey32"),
	_T("SysListView32"),
	_T("SysTreeView32"),
	_T("Richedit"),
	_T("SysDateTimePick32"),
	_T("SysIPAddress32"),
	_T("SysPager32"),
	_T("ComboBoxEx32")
	//"Button",
	//"Static",
	//"Scrollbar",
	//"toolbarwindow32",
	//"msctls_updown32",
	//"msctls_progress32",
	//"msctls_trackbar32",
	//"SysTabControl32",
	//"SysAnimate32",
	//"SysMonthCal32",
	//"msctls_statusbar32",
	//"ReBarWindow32",
	//"SysHeader32",
	//"tooltips_class32",
};

/////////////////////////////////////////////////////////////////////////////

// see ParseRCStyles() for what this is about
#define WS_NOTVISIBLE WS_POPUP

/////////////////////////////////////////////////////////////////////////////

CRCCtrlParser::CRCCtrlParser(LPCTSTR szRCControls)
{
	if (szRCControls)
	{
		ParseRCControls(szRCControls);
	}
}

CRCCtrlParser::~CRCCtrlParser()
{
}

int CRCCtrlParser::ParseRCControls(const CString& sRCControls)
{
	m_lstControls.RemoveAll();

	// parse the text control by control
	int nStart = 0, nEnd = -1;

	do
	{
		nEnd = FindNextRCControl(sRCControls, nStart ? nStart + 1 : 0);
		CString sRCCtrl = nEnd != -1 ? sRCControls.Mid(nStart, nEnd - nStart - 1) : sRCControls.Mid(nStart);

		sRCCtrl.TrimLeft();
		sRCCtrl.TrimRight();

		AddRCControl(sRCCtrl);

		nStart = nEnd;
	}
	while (nEnd != -1);

	return m_lstControls.GetCount();
}

int CRCCtrlParser::GetRCControls(CRTCtrlList& lstControls) const
{
	lstControls.AddTail((CRTCtrlList*)&m_lstControls);

	return m_lstControls.GetCount();
}

BOOL CRCCtrlParser::AddRCControl(const CString& sRCCtrl)
{
	CString sCaption;
	UINT uID = 0;
	CString sClass;
	DWORD dwStyle = 0;
	POINT pos = { 0, 0 };
	SIZE size = { 0, 0 };
	DWORD dwExStyle = 0;
	UINT nIconID;

	int nType = ParseRCControl(sRCCtrl, sCaption, uID, sClass, dwStyle, pos, size, dwExStyle, nIconID);

	if (nType == -1)
	{
		return FALSE;
	}

	RTCONTROL rtc(NULL, sClass, sCaption, dwStyle, dwExStyle, CRect(pos, size), uID, TRUE);
	rtc.m_nIconID = nIconID;

	return (NULL != m_lstControls.AddTail(rtc));
}

int CRCCtrlParser::FindNextRCControl(const CString& sRCControls, int nStart)
{
	int nType = NUMRCTYPES, nFirstType = -1, nFirstFind = -1;

	while (nType--)
	{
		int nFind = sRCControls.Find(RCCTRLTYPES[nType].szType, nStart);

		if (nFind != -1 && (nFind < nFirstFind || nFirstType == -1) && (nFind == 0 || isspace(sRCControls[nFind - 1])))
		{
			nFirstType = RCCTRLTYPES[nType].nType;
			nFirstFind = nFind;
		}
	}

	return nFirstFind;
}

int CRCCtrlParser::ParseRCControl(const CString& sRCCtrl, CString& sCaption, UINT& uID, CString& sClass,
	DWORD& dwStyle, POINT& pos, SIZE& size, DWORD& dwExStyle, UINT& nIconID)
{
	int nIndex = GetRCControlTypeIndex(sRCCtrl);

	if (nIndex == -1)
	{
		return -1;
	}

	CString sID, sStyle, sX, sY, sCx, sCy, sExStyle, sIconID;

	switch (RCCTRLTYPES[nIndex].nType)
	{
		// 'CONTROL' definitions always take the same form:

		// caption, id, class, style, x, y, cx, cy, exstyle
	case CONTROL:
		{
			CString* pItems[] = { &sCaption, &sID, &sClass, &sStyle, &sX, &sY, &sCx, &sCy, &sExStyle };
			ExtractRCItems(sRCCtrl, RCCTRLTYPES[nIndex].szType, pItems, sizeof(pItems) / sizeof(CString*));
		}
		break;

		// older window control definitions have 3 forms:

		// 1. caption, id, x, y, cx, cy, style, exstyle
	case AUTO3STATE:
	case AUTOCHECKBOX:
	case AUTORADIOBUTTON:
	case CHECKBOX:
	case CTEXT:
	case DEFPUSHBUTTON:
	case GROUPBOX:
	case LTEXT:
	case PUSHBOX:
	case PUSHBUTTON:
	case RADIOBUTTON:
	case RTEXT:
	case STATE3:
		{
			CString* pItems[] = { &sCaption, &sID, &sX, &sY, &sCx, &sCy, &sStyle, &sExStyle };
			ExtractRCItems(sRCCtrl, RCCTRLTYPES[nIndex].szType, pItems, sizeof(pItems) / sizeof(CString*));
		}
		break;

		// 2. id, x, y, cx, cy, style, exstyle
	case SCROLLBAR:
	case COMBOBOX:
	case EDITTEXT:
	case LISTBOX:
		{
			CString* pItems[] = { &sID, &sX, &sY, &sCx, &sCy, &sStyle, &sExStyle };
			ExtractRCItems(sRCCtrl, RCCTRLTYPES[nIndex].szType, pItems, sizeof(pItems) / sizeof(CString*));
		}
		break;

		// 3. iconid, id, x, y, cx, cy
	case ICON:
		{
			CString* pItems[] = { &sIconID, &sID, &sX, &sY, &sCx, &sCy };
			ExtractRCItems(sRCCtrl, RCCTRLTYPES[nIndex].szType, pItems, sizeof(pItems) / sizeof(CString*));
		}
		break;

	default:
		nIndex = -1;
	}

	// global settings
	if (nIndex != -1)
	{
		// class names for predefined classes
		if (sClass.IsEmpty())
		{
			sClass = RCCTRLTYPES[nIndex].szClass;
		}

		// styles
		dwStyle = ParseRCStyles(sClass, sStyle);
		dwExStyle = ParseRCExStyles(sClass, sExStyle);

		// handle invisibility
		BOOL bVisible = !(dwStyle & WS_NOTVISIBLE);
		dwStyle &= ~WS_NOTVISIBLE;

		// default styles if no styles specified
		if (!dwStyle)
		{
			dwStyle = RCCTRLTYPES[nIndex].dwDefStyle;
		}

		dwStyle |= WS_CHILD | (bVisible ? WS_VISIBLE : 0); // always

		// add client-edge depending on class name
		if (CtrlWantsClientEdge(sClass))
		{
			dwExStyle |= WS_EX_CLIENTEDGE;
		}

		uID = _ttoi(sID);
		pos.x = _ttoi(sX);
		pos.y = _ttoi(sY);
		size.cx = _ttoi(sCx);
		size.cy = _ttoi(sCy);
		nIconID = _ttoi(sIconID);
	}

	return nIndex;
}

void CRCCtrlParser::ExtractRCItems(const CString& sRCCtrl, LPCTSTR szType, CString* pItems[], int nMaxItems)
{
	CString sTemp(sRCCtrl);

	sTemp = sTemp.Mid(lstrlen(szType));
	sTemp.TrimLeft();

	int nItem = 0;

	while (nItem < nMaxItems)
	{
		// if the first char is a double quote then we need to ignore
		// commas till we find the end quote
		int nStart = 0;

		if (sTemp[0] == _T('\"'))
		{
			nStart = sTemp.Find(_T('\"'), 1);
		}

		int nFind = sTemp.Find(_T(','), nStart);

		if (nFind == -1)
		{
			*pItems[nItem] = sTemp;
			pItems[nItem]->Replace(_T("\""), _T(""));

			break;
		}

		// else
		*pItems[nItem] = sTemp.Left(nFind);
		pItems[nItem]->Replace(_T("\""), _T(""));

		sTemp = sTemp.Mid(nFind + 1);
		sTemp.TrimLeft();

		nItem++;
	}
}

int CRCCtrlParser::GetRCControlTypeIndex(const CString& sRCCtrl)
{
	int nType = NUMRCTYPES;

	while (nType--)
	{
		if (0 == sRCCtrl.Find(RCCTRLTYPES[nType].szType))
		{
			return nType;
		}
	}

	return -1;
}

DWORD CRCCtrlParser::ParseRCStyles(LPCTSTR /*szClass*/, const CString& sStyle)
{
	CStringArray aStyles;
	DWORD dwStyle = 0;

	if (ExtractRCStyles(sStyle, aStyles))
	{
		int nStyle = aStyles.GetSize();

		while (nStyle--)
		{
			// note: invisible controls are handled via 'NOT WS_VISIBLE'
			// so we must handle this explicitly via a hack
			// we can reuse WS_POPUP because this will _never_ appear for
			// a child control (i hope)
			if (CString(aStyles[nStyle]).CompareNoCase(_T("NOT WS_VISIBLE")) == 0)
			{
				dwStyle |= WS_NOTVISIBLE;
			}
			else
			{
				dwStyle |= LookupWndStyle(aStyles[nStyle]);
				dwStyle |= LookupCtrlStyle(aStyles[nStyle]);
			}
		}
	}

	return dwStyle;
}

DWORD CRCCtrlParser::ParseRCExStyles(LPCTSTR /*szClass*/, const CString& sExStyle)
{
	CStringArray aExStyles;
	DWORD dwExStyle = 0;

	if (ExtractRCStyles(sExStyle, aExStyles))
	{
		int nExStyle = aExStyles.GetSize();

		while (nExStyle--)
		{
			dwExStyle |= LookupWndExStyle(aExStyles[nExStyle]);
			dwExStyle |= LookupCtrlStyle(aExStyles[nExStyle]);
		}
	}

	return dwExStyle;
}

int CRCCtrlParser::ExtractRCStyles(const CString& sStyles, CStringArray& aStyles)
{
	// styles are delimetered by '|'
	CString sTemp(sStyles);
	sTemp.TrimLeft();

	do
	{
		int nFind = sTemp.Find(_T('|'));

		if (nFind == -1)
		{
			sTemp.TrimRight();

			if (sTemp.GetLength())
			{
				aStyles.Add(sTemp);
			}
			break;
		}
		else
		{
			CString sStyle = sTemp.Left(nFind);
			sStyle.TrimRight();

			sTemp = sTemp.Mid(nFind + 1);
			sTemp.TrimLeft();

			if (sStyle.GetLength())
			{
				aStyles.Add(sStyle);
			}
		}

	}
	while (TRUE);

	return aStyles.GetSize();
}

BOOL CRCCtrlParser::CtrlWantsClientEdge(LPCTSTR szClass)
{
	int nClass = sizeof(szCtrlsWantingClientEdge) / sizeof(LPCTSTR);
	CString sClass(szClass);

	while (nClass--)
	{
		if (sClass.CompareNoCase(szCtrlsWantingClientEdge[nClass]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE; // no match
}

DWORD CRCCtrlParser::GetDefaultStyles(LPCTSTR szRCType)
{
	int nType = NUMRCTYPES;

	while (nType--)
	{
		if (_tcsicmp(RCCTRLTYPES[nType].szType, szRCType) == 0)
		{
			return RCCTRLTYPES[nType].dwDefStyle;
		}
	}

	return 0;
}

BOOL CRCCtrlParser::GetClassName(LPCTSTR szRCType, CString& sClass)
{
	int nType = NUMRCTYPES;

	while (nType--)
	{
		if (_tcsicmp(RCCTRLTYPES[nType].szType, szRCType) == 0)
		{
			sClass = RCCTRLTYPES[nType].szClass;
			return TRUE;
		}
	}

	return FALSE;
}
